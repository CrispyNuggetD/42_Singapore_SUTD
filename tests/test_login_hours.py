"""Synthetic login intervals and fake API only; no credentials or network."""
from contextlib import redirect_stdout
from datetime import datetime, timedelta, timezone
import io
import json
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import Mock, patch
from zoneinfo import ZoneInfo
import login_hours as h

UTC = timezone.utc
NOW = datetime(2026, 10, 26, 12, tzinfo=UTC)
ZONE = ZoneInfo('Asia/Singapore')


def row(begin, end, login='student'):
    return dict(begin_at=begin, end_at=end, user={'login': login})


class Durations(unittest.TestCase):
    def test_overlap_midnight_and_window_clipping(self):
        days, start = h.bounds(NOW, ZONE)
        values = h.totals([(start - timedelta(hours=1), start + timedelta(hours=2)),
                           (start + timedelta(hours=1), start + timedelta(hours=3))], days, ZONE, NOW)
        self.assertEqual(sum(values.values()), 3 * 3600)
        midnight = datetime(2026, 10, 25, 16, tzinfo=UTC)
        values = h.totals([(midnight-timedelta(hours=1), midnight+timedelta(hours=2))], days, ZONE, NOW)
        self.assertEqual(values['2026-10-25'], 3600)
        self.assertEqual(values['2026-10-26'], 7200)

    def test_dst_day_is_not_forced_to_twenty_four_hours(self):
        zone = ZoneInfo('America/New_York')
        now = datetime(2026, 11, 2, 12, tzinfo=UTC)
        days, _ = h.bounds(now, zone)
        a = datetime(2026, 11, 1, tzinfo=zone).astimezone(UTC)
        b = datetime(2026, 11, 2, tzinfo=zone).astimezone(UTC)
        self.assertEqual(h.totals([(a,b)], days, zone, now)['2026-11-01'], 25 * 3600)

    def test_official_total_excludes_open_estimate_and_uses_api_despite_difference(self):
        data = dict(fetched_at=NOW.isoformat(), records=[
            dict(begin_at='2026-10-26T01:00:00Z',end_at='2026-10-26T02:00:00Z'),
            dict(begin_at='2026-10-26T03:00:00Z',end_at=None)])
        output = io.StringIO()
        with redirect_stdout(output):
            h.render('hours', 'student', NOW, ZONE, data, [(NOW-timedelta(hours=5),NOW)], target=20)
        text = output.getvalue()
        self.assertIn('(42 API Verified)', text)
        self.assertIn('Discrepancy', text)
        self.assertIn('Rolling 7-day total: 1h 00m', text)
        self.assertIn('Estimated remaining from location intervals: 19h 00m', text)
        self.assertIn('Day 1 Monday 2026-10-26: 1h 00m', text)
        self.assertIn('Weekly policy target (configured locally): 20h', text)
        self.assertIn('9h 00m (provisional', text)

    def test_empty_api_is_a_valid_zero_not_a_fallback(self):
        output = io.StringIO()
        with redirect_stdout(output):
            h.render('hours', 'student', NOW, ZONE, dict(fetched_at=NOW.isoformat(),records=[]), [(NOW-timedelta(hours=3),NOW)])
        self.assertIn('Rolling 7-day total: 0h 00m', output.getvalue())
        self.assertIn('(42 API Verified)', output.getvalue())

    def test_before_noon_checks_local_time_even_for_active_api_login(self):
        data = dict(fetched_at=NOW.isoformat(),records=[dict(begin_at='2026-10-26T03:59:00Z',end_at=None)])
        output = io.StringIO()
        with redirect_stdout(output):
            h.render('before-noon','student',NOW,ZONE,data,None)
        self.assertIn('Day 1 Monday 2026-10-26: 11:59:00 (42 API Verified)', output.getvalue())

    def test_thursday_period_and_negative_rolling_days(self):
        now = datetime(2026, 10, 29, 12, tzinfo=UTC)
        data = dict(fetched_at=now.isoformat(), records=[
            dict(begin_at='2026-10-27T00:00:00Z', end_at='2026-10-27T12:19:12Z')])
        output = io.StringIO()
        with redirect_stdout(output):
            h.render('hours', 'student', now, ZONE, data, None, target=20)
        text = output.getvalue()
        self.assertIn('Day -3 Friday 2026-10-23', text)
        self.assertIn('Day -1 Sunday 2026-10-25', text)
        self.assertIn('Day 4 Thursday 2026-10-29', text)
        self.assertNotIn('Day 0 ', text)
        self.assertIn('Attendance period: 2026-10-26 to 2026-11-01', text)
        self.assertIn('Day 4 Thursday 2026-10-29', text)
        self.assertIn('Day 7 Sunday 2026-11-01: upcoming', text)
        self.assertIn('API location period total so far: 12h 19m (12.32h)', text)


class API(unittest.TestCase):
    def test_completed_range_and_active_queries_cover_cross_window_sessions(self):
        _, start = h.bounds(NOW, ZONE)
        record = row('2026-10-10T00:00:00Z','2026-10-21T00:00:00Z')
        api = Mock()
        api.get.side_effect = [[record],[]]
        rows = h.fetch(api,'student',start,NOW)
        self.assertEqual(len(rows),1)
        self.assertIn('range[end_at]',api.get.call_args_list[0].kwargs)
        self.assertEqual(api.get.call_args_list[1].kwargs['filter[active]'],'true')

    def test_wrong_user_or_incomplete_pagination_rejects_entire_snapshot(self):
        _, start = h.bounds(NOW, ZONE)
        api = Mock()
        api.get.return_value = [row('2026-10-25T00:00:00Z',None,'another-user')]
        with self.assertRaises(ValueError):h.fetch(api,'student',start,NOW)
        api.get.return_value = [row('2026-10-25T00:00:00Z',None)] * 100
        with self.assertRaisesRegex(ValueError,'pagination'):h.fetch(api,'student',start,NOW)

    def test_recent_cache_shared_but_stale_cache_never_becomes_verified_after_failure(self):
        _, start = h.bounds(NOW, ZONE)
        with tempfile.TemporaryDirectory() as tmp, patch.dict(os.environ,{'XDG_CACHE_HOME':tmp}), \
             patch.object(h,'api_client',return_value=Mock()) as client, patch.object(h,'fetch',return_value=[]):
            first = h.snapshot('student',start,NOW)
            self.assertEqual(h.snapshot('student',start,NOW+timedelta(seconds=30)),first)
            self.assertEqual(client.call_count,1)
            self.assertEqual(h.cache_file('student').stat().st_mode & 0o777,0o600)
            with patch.object(h,'fetch',side_effect=RuntimeError('PRIVATE ERROR MUST NOT PRINT')):
                with self.assertRaises(h.Unavailable) as caught:h.snapshot('student',start,NOW+timedelta(minutes=6))
                self.assertNotIn('PRIVATE',str(caught.exception))


class Local(unittest.TestCase):
    def test_last_parsing_does_not_treat_crashes_as_active(self):
        text = '''student pts/1 host 2026-10-26T01:00:00+00:00 - 2026-10-26T02:00:00+00:00 (01:00)
student pts/2 host 2026-10-26T03:00:00+00:00 still logged in
student pts/3 host 2026-10-25T03:00:00+00:00 gone - no logout
other pts/4 host 2026-10-26T01:00:00+00:00 still logged in
'''
        with patch.object(h.sys,'platform','linux'),patch.object(h.subprocess,'run',return_value=Mock(returncode=0,stdout=text)):
            self.assertEqual(len(h.local_intervals('student',NOW)),2)

    def test_fallback_is_explicit_and_never_api_verified(self):
        output = io.StringIO()
        with redirect_stdout(output):
            h.render('hours','student',NOW,ZONE,None,[(NOW-timedelta(hours=2),NOW)],'offline')
        self.assertIn('local last fallback (not API verified)',output.getvalue())
        self.assertNotIn('(42 API Verified)',output.getvalue())
        self.assertIn('Rolling 7-day total: 2h 00m',output.getvalue())


class Entry(unittest.TestCase):
    def test_unavailable_api_skips_without_local_fallback_for_any_user(self):
        from contextlib import redirect_stderr
        for workflow in (False, True):
            output=io.StringIO()
            with self.subTest(workflow=workflow), \
                 patch.dict(os.environ,{'FORTYTWO_LOGIN':'another-student'},clear=True), \
                 patch.object(h.sys,'argv',['login_hours.py','hours']+(['--workflow'] if workflow else [])), \
                 patch.object(h,'snapshot',side_effect=h.Unavailable('API unavailable')), \
                 patch.object(h,'local_intervals') as local, redirect_stderr(output):
                self.assertEqual(h.main(),77 if workflow else 0)
                local.assert_not_called()
                self.assertIn('Login report skipped',output.getvalue())
                self.assertIn('Dailylogin can continue',output.getvalue())

    def test_linux_hnah_retains_explicit_local_fallback(self):
        with patch.dict(os.environ,{'FORTYTWO_LOGIN':'hnah','USER':'hnah'},clear=True), \
             patch.object(h.sys,'platform','linux'), \
             patch.object(h.sys,'argv',['login_hours.py','hours']), \
             patch.object(h,'snapshot',side_effect=h.Unavailable('offline')), \
             patch.object(h,'local_intervals',return_value=[]) as local, \
             patch.object(h,'render',return_value=0) as render:
            self.assertEqual(h.main(),0)
            local.assert_called_once()
            self.assertIsNone(render.call_args.args[4])
            self.assertEqual(render.call_args.args[5],[])
            self.assertEqual(render.call_args.args[6],'offline')
            self.assertEqual(render.call_args.args[7],20)

    def test_mac_hnah_and_other_linux_accounts_do_not_use_local_fallback(self):
        from contextlib import redirect_stderr
        for platform, user, login in [('darwin','crispy','hnah'),('linux','other','hnah'),('linux','hnah','other')]:
            with self.subTest(platform=platform,user=user,login=login), \
                 patch.dict(os.environ,{'FORTYTWO_LOGIN':login,'USER':user},clear=True), \
                 patch.object(h.sys,'platform',platform), \
                 patch.object(h.sys,'argv',['login_hours.py','hours','--workflow']), \
                 patch.object(h,'snapshot',side_effect=h.Unavailable('offline')), \
                 patch.object(h,'local_intervals') as local, redirect_stderr(io.StringIO()):
                self.assertEqual(h.main(),77)
                local.assert_not_called()

    def test_non_owner_login_uses_api_even_on_mac(self):
        with patch.dict(os.environ,{'FORTYTWO_LOGIN':'another-student'},clear=True), \
             patch.object(h.sys,'platform','darwin'), \
             patch.object(h.sys,'argv',['login_hours.py','hours']), \
             patch.object(h,'snapshot',return_value={'records':[]}) as snapshot, \
             patch.object(h,'local_intervals',side_effect=h.Unavailable('not Linux')), \
             patch.object(h,'render',return_value=0) as render:
            self.assertEqual(h.main(),0)
            self.assertEqual(snapshot.call_args.args[0],'another-student')
            self.assertIsNotNone(render.call_args.args[4])


if __name__ == '__main__':unittest.main()
