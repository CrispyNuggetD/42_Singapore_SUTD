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
            h.render('hours', 'student', NOW, ZONE, data, [(NOW-timedelta(hours=5),NOW)])
        text = output.getvalue()
        self.assertIn('(42 API Verified)', text)
        self.assertIn('Discrepancy', text)
        self.assertIn('Rolling 7-day total: 1h 00m', text)
        self.assertIn('Remaining to reach target: 49h 00m', text)
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
        self.assertIn('2026-10-26: 11:59:00 (42 API Verified)', output.getvalue())


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


if __name__ == '__main__':unittest.main()
