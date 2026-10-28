#!/usr/bin/env python3
"""42 API campus login reports; unavailable API reports are skipped.

No API keys or tokens are stored in the report cache. Overlapping sessions are
merged and split at local midnight for rolling and Monday–Sunday views.
"""
import argparse
from contextlib import redirect_stdout
from datetime import datetime, timedelta, timezone
import io
import json
import math
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import urllib.error
import urllib.parse
import urllib.request
from zoneinfo import ZoneInfo

UTC = timezone.utc
ISO = re.compile(r'\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?(?:Z|[+-]\d{2}:?\d{2})')


class Unavailable(Exception):
    pass


def timestamp(value):
    if not isinstance(value, str):
        raise ValueError('Timestamp missing')
    result = datetime.fromisoformat(value.replace('Z', '+00:00'))
    if result.tzinfo is None:
        raise ValueError('Timestamp needs timezone')
    return result.astimezone(UTC)


def report_zone():
    # Use a named zone, including DST transitions, rather than today's fixed offset.
    name = os.environ.get('LOGIN_HOURS_TIMEZONE') or os.environ.get('TZ')
    if name:
        return ZoneInfo(name.lstrip(':'))
    try:
        resolved = str(Path('/etc/localtime').resolve())
        if '/zoneinfo/' in resolved:
            return ZoneInfo(resolved.split('/zoneinfo/', 1)[1])
        with Path('/etc/localtime').open('rb') as source:
            return ZoneInfo.from_file(source, key='system local time')
    except OSError:
        return UTC


def bounds(now, zone):
    today = now.astimezone(zone).date()
    days = [today - timedelta(days=i) for i in range(6, -1, -1)]
    start = datetime.combine(days[0], datetime.min.time(), zone).astimezone(UTC)
    return days, start


def period_days(now, zone):
    today = now.astimezone(zone).date()
    monday = today - timedelta(days=today.weekday())
    return [monday + timedelta(days=i) for i in range(7)]


def day_label(day, period_start):
    offset = (day - period_start).days
    number = offset + 1 if offset >= 0 else offset
    return f'Day {number} {day:%A} {day.isoformat()}'


def union(intervals, start, end):
    merged = []
    for begin, finish in sorted((max(a, start), min(b, end)) for a, b in intervals if b > start and a < end):
        if finish <= begin:
            continue
        if merged and begin <= merged[-1][1]:
            merged[-1] = (merged[-1][0], max(finish, merged[-1][1]))
        else:
            merged.append((begin, finish))
    return merged


def totals(intervals, days, zone, now):
    start = datetime.combine(days[0], datetime.min.time(), zone).astimezone(UTC)
    merged = union(intervals, start, now)
    values = {}
    for day in days:
        lo = datetime.combine(day, datetime.min.time(), zone).astimezone(UTC)
        hi = datetime.combine(day + timedelta(days=1), datetime.min.time(), zone).astimezone(UTC)
        seconds = sum(max(0, (min(b, hi) - max(a, lo)).total_seconds()) for a, b in merged)
        values[day.isoformat()] = seconds
    return values


class TokenAPI:
    """Optional existing user-local access token; fixed HTTPS provider, no redirects."""
    def __init__(self, token):
        self.token = token
        class NoRedirect(urllib.request.HTTPRedirectHandler):
            def redirect_request(self, req, fp, code, msg, headers, newurl):
                return None
        self.opener = urllib.request.build_opener(NoRedirect())
        self.last = 0

    def get(self, path, **params):
        import time
        time.sleep(max(0, .51 - (time.monotonic() - self.last)))
        self.last = time.monotonic()
        request = urllib.request.Request('https://api.intra.42.fr/v2/' + path + '?' + urllib.parse.urlencode(params),
                                         headers={'Authorization': 'Bearer ' + self.token, 'Accept': 'application/json'})
        with self.opener.open(request, timeout=10) as response:
            raw = response.read(4 * 1024 * 1024 + 1)
        if len(raw) > 4 * 1024 * 1024:
            raise ValueError('API response too large')
        return json.loads(raw)


def api_client():
    token = os.environ.get('FORTYTWO_ACCESS_TOKEN')
    if token:
        return TokenAPI(token)
    clone = Path(os.environ.get('LAN42_CLONE', str(Path.home() / 'Documents/42sg-campus-lan')))
    if (clone / 'campus_lan/campus_info.py').is_file():
        sys.path.insert(0, str(clone))
        # Reuse the API host's own protected settings and cross-process pacing.
        # Clients never receive or copy that configuration.
        from campus_lan.campus_info import API
        return API()
    raise Unavailable('no local API access configured')


def fetch(api, login, start, now):
    records = []
    path = 'users/' + urllib.parse.quote(login, safe='') + '/locations'
    # end_at range includes sessions that began before our seven-day window.
    queries = [{'range[end_at]': start.isoformat() + ',' + now.isoformat()}, {'filter[active]': 'true'}]
    for query in queries:
        complete = False
        for page in range(1, 51):
            batch = api.get(path, **query, **{'page[number]': page, 'page[size]': 100, 'sort': '-begin_at'})
            if not isinstance(batch, list):
                raise ValueError('API returned an invalid page')
            for row in batch:
                if not isinstance(row, dict) or not isinstance(row.get('user'), dict) or row['user'].get('login', '').lower() != login.lower():
                    raise ValueError('API returned another user or an invalid record')
                begin = timestamp(row.get('begin_at'))
                finish = timestamp(row['end_at']) if row.get('end_at') is not None else None
                if begin > now or (finish is not None and (finish < begin or finish > now)):
                    raise ValueError('API returned an invalid interval')
                records.append({'begin_at': begin.isoformat(), 'end_at': finish.isoformat() if finish else None})
            if len(batch) < 100:
                complete = True
                break
        if not complete:
            raise ValueError('API pagination incomplete')
    # Only begin/end are retained; no hosts or other student profile fields.
    return list({(r['begin_at'], r['end_at']): r for r in records}.values())


def cache_file(login):
    return Path(os.environ.get('XDG_CACHE_HOME', str(Path.home() / '.cache'))) / '42-shell' / ('login-hours-' + login + '.json')


def snapshot(login, start, now, max_age=300):
    path = cache_file(login)
    try:
        if path.is_symlink() or path.stat().st_uid != os.getuid() or path.stat().st_mode & 0o077 or path.stat().st_size > 4 * 1024 * 1024:
            raise ValueError('Untrusted cache')
        data = json.loads(path.read_text())
        fetched = timestamp(data['fetched_at'])
        if data['login'] == login and data['start'] == start.isoformat() and 0 <= (now - fetched).total_seconds() < max_age:
            # Revalidate timestamps even in an owner-local cache.
            for row in data['records']:
                begin = timestamp(row['begin_at'])
                finish = timestamp(row['end_at']) if row['end_at'] else None
                if begin > fetched or (finish and (finish < begin or finish > fetched)):
                    raise ValueError('Invalid cache interval')
            return data
    except (OSError, ValueError, KeyError, TypeError):
        pass
    try:
        # Suppress optional API client's pacing chatter, never HTTP bodies/errors.
        with redirect_stdout(io.StringIO()):
            records = fetch(api_client(), login, start, now)
    except Exception:
        # Avoid displaying URLs, tokens or private configuration in exception text.
        raise Unavailable('API unavailable or incomplete; old cached data is not used as verified') from None
    data = {'login': login, 'start': start.isoformat(), 'fetched_at': now.isoformat(), 'records': records}
    temporary = None
    try:
        path.parent.mkdir(parents=True, exist_ok=True, mode=0o700)
        with tempfile.NamedTemporaryFile('w', dir=path.parent, delete=False) as output:
            temporary = Path(output.name)
            json.dump(data, output)
        temporary.replace(path)
    except OSError:
        pass  # A successful live fetch can still be reported without a cache.
    finally:
        if temporary is not None:
            temporary.unlink(missing_ok=True)
    return data


def local_intervals(user, now):
    if sys.platform != 'linux':
        raise Unavailable('Linux last records unavailable on this workstation')
    try:
        result = subprocess.run(['last', '-w', '--time-format', 'iso', user],
                                capture_output=True, text=True, timeout=10, env=dict(os.environ, LC_ALL='C'))
    except (OSError, subprocess.SubprocessError):
        raise Unavailable('local last command unavailable') from None
    if result.returncode or len(result.stdout) > 4 * 1024 * 1024:
        raise Unavailable('local last records could not be read')
    intervals = []
    for line in result.stdout.splitlines():
        if not line.split() or line.split()[0] != user:
            continue
        stamps = ISO.findall(line)
        if not stamps:
            continue
        begin = timestamp(stamps[0])
        if len(stamps) > 1:
            finish = timestamp(stamps[1])
        elif 'still logged in' in line:
            finish = now
        else:
            continue  # Unknown crash/logout time is not assumed to mean still active.
        if begin <= finish:
            intervals.append((begin, finish))
    return intervals


def duration(seconds):
    minutes = max(0, int(seconds // 60))
    return f'{minutes // 60}h {minutes % 60:02d}m'


def discrepancy(api_values, local_values):
    return [day for day in api_values if int(api_values[day] // 60) != int(local_values[day] // 60)]


def render(report, login, now, zone, data, local, reason='', target=None):
    days, start = bounds(now, zone)
    week = period_days(now, zone)
    today = days[-1]
    period_start = week[0]
    local_values = totals(local, days, zone, now) if local is not None else None
    active = []
    if data is not None:
        closed = []
        for row in data['records']:
            begin = timestamp(row['begin_at'])
            if row['end_at'] is None:
                active.append((begin, now))
            else:
                closed.append((begin, timestamp(row['end_at'])))
        values = totals(closed, days, zone, now)
        print(f'{login}: (42 API Verified) — completed API location intervals')
        print('API fetched: ' + timestamp(data['fetched_at']).astimezone(zone).isoformat(timespec='seconds'))
        print('API publication delay is unknown; a fetch timestamp does not verify Intra’s update schedule.')
        if local_values is not None:
            differing = discrepancy(values, local_values)
            if differing:
                print('Discrepancy with local last records (API used for official-data calculation):')
                for day in differing:
                    date = next(d for d in days if d.isoformat() == day)
                    print(f'  {day_label(date, period_start)}: API {duration(values[day])}; local {duration(local_values[day])}')
            else:
                print('Local last totals agree to the minute.')
        else:
            print('Local last comparison unavailable on this workstation.')
        if active:
            extra = sum(totals(closed + active, days, zone, now).values()) - sum(values.values())
            print('Active API session elapsed estimate: ' + duration(extra) + ' (provisional; excluded from verified total).')
            if report == 'hours':
                provisional = sum(values.values()) + extra
                print('Provisional rolling total including active API sessions: ' + duration(provisional))
        intervals = closed + [(a, a) for a, _ in active]
    elif local_values is not None:
        print(f'{login}: local last fallback (not API verified) — {reason}')
        print('Local records cover this computer only and may contain stale open sessions; overlaps are counted once.')
        values, intervals = local_values, local
    else:
        print('Login report unavailable: API and local last data could not be read.')
        return 1
    print('Rolling window: ' + days[0].isoformat() + ' to ' + days[-1].isoformat() + '; timezone ' + str(zone))
    if report == 'before-noon':
        print('Days with a recorded login before 12:00 (last seven calendar dates):')
        qualifying = {}
        for begin, _ in intervals:
            when = begin.astimezone(zone)
            if days[0] <= when.date() <= days[-1] and when.hour < 12:
                qualifying[when.date()] = min(qualifying.get(when.date(), when), when)
        for day, when in sorted(qualifying.items()):
            print(f'  {day_label(day, period_start)}: {when:%H:%M:%S}' + (' (42 API Verified)' if data is not None else ' (local fallback)'))
        if not qualifying:
            print('  No qualifying login in available records.')
    else:
        print('Rolling 7 days (day numbers follow the attendance period):')
        for day in days:
            print(f'  {day_label(day, period_start)}: {duration(values[day.isoformat()])}')
        total = sum(values.values())
        print('Rolling 7-day total: ' + duration(total) + f' ({total / 3600:.2f}h)')
        print('Rolling 7-day average per day: ' + duration(total / 7))
        week_values = totals(closed if data is not None else local, week, zone, now)
        print(f'Attendance period: {week[0].isoformat()} to {week[-1].isoformat()} (Monday–Sunday; {zone})')
        for day in week:
            value = duration(week_values[day.isoformat()]) if day <= today else 'upcoming'
            print(f'  {day_label(day, period_start)}: {value}')
        week_total = sum(week_values.values())
        source = 'API location' if data is not None else 'Local fallback'
        print(source + ' period total so far: ' + duration(week_total) + f' ({week_total / 3600:.2f}h)')
        if target is not None:
            print(f'Weekly policy target (configured locally): {target:g}h')
            print('Estimated remaining from location intervals: ' + duration(max(0, target * 3600 - week_total)))
        else:
            print('Weekly policy target: not configured; set LOGIN_HOURS_WEEKLY_TARGET to the portal policy.')
        print('Totals come from available location intervals; the attendance portal may apply different rules or publication timing.')
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('report', choices=('hours', 'before-noon'))
    parser.add_argument('--refresh', action='store_true', help='Fetch again instead of reusing a five-minute API snapshot')
    parser.add_argument('--workflow', action='store_true', help='Return skip status 77 for dailylogin reporting')
    args = parser.parse_args()
    login = os.environ.get('FORTYTWO_LOGIN') or os.environ.get('MAIL', '').split('@')[0] or os.environ.get('USER', '')
    def skip(reason):
        print('Login report skipped: ' + reason, file=sys.stderr)
        print('Set FORTYTWO_LOGIN to your own 42 login and configure local API access; '
              'or disable loginreports with setupzshrc. Dailylogin can continue.', file=sys.stderr)
        return 77 if args.workflow else 0
    if not re.fullmatch(r'[A-Za-z0-9_-]{1,32}', login):
        return skip('no valid campus login configured.')
    now = datetime.now(UTC)
    try:
        zone = report_zone()
    except (ValueError, KeyError, OSError):
        parser.error('Set LOGIN_HOURS_TIMEZONE to a valid IANA timezone, for example Asia/Singapore')
    _, start = bounds(now, zone)
    data, reason = None, ''
    try:
        data = snapshot(login, start, now, max_age=0 if args.refresh else 300)
    except Unavailable as error:
        # Preserve the established campus hnah workflow. Other accounts and
        # macOS must not mistake workstation login time for campus API hours.
        campus_fallback = (sys.platform == 'linux' and
                           os.environ.get('USER') == 'hnah' and login.lower() == 'hnah')
        if not campus_fallback:
            return skip(str(error) + '; API data is required, so no local hours are substituted.')
        reason = str(error)

    try:
        local = local_intervals(os.environ.get('USER', login), now)
    except Unavailable:
        local = None
    if data is None and local is None:
        return skip('API and campus local login records are unavailable.')
    raw_target = os.environ.get('LOGIN_HOURS_WEEKLY_TARGET', '').strip()
    try:
        target = float(raw_target) if raw_target else (20.0 if login.lower() == 'hnah' else None)
        if target is not None and (not 0 < target <= 168 or not math.isfinite(target)):
            raise ValueError
    except ValueError:
        parser.error('LOGIN_HOURS_WEEKLY_TARGET must be a positive number of hours up to 168')
    return render(args.report, login, now, zone, data, local, reason, target)


if __name__ == '__main__':
    sys.exit(main())
