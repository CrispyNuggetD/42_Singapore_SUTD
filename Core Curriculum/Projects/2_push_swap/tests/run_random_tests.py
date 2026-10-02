#!/usr/bin/env python3
"""Reproducible rank permutations, checker-verified journals, resumable stats."""
import argparse
import datetime as dt
import fcntl
import hashlib
import html
import json
import math
import os
from pathlib import Path
import random
import re
import secrets
import signal
import statistics
import threading
from concurrent.futures import ThreadPoolExecutor
from collections import Counter
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
MOVES = set('sa sb ss pa pb ra rb rr rra rrb rrr'.split())
GENERATOR = 'python-mt19937-shuffle-sha256-v1'



ACTIVE_PROCESSES = set()
PROCESS_LOCK = threading.Lock()
STOP_REQUESTED = threading.Event()


def start_process(*args, **kwargs):
    with PROCESS_LOCK:
        if STOP_REQUESTED.is_set():
            raise InterruptedError('Benchmark stopping')
        process = subprocess.Popen(*args, **kwargs)
        ACTIVE_PROCESSES.add(process)
        return process


def forget_process(process):
    with PROCESS_LOCK:
        ACTIVE_PROCESSES.discard(process)


def stop_workers():
    with PROCESS_LOCK:
        STOP_REQUESTED.set()
        active = list(ACTIVE_PROCESSES)
    for process in active:
        try:
            stop_process(process)
        except ProcessLookupError:
            pass


def physical_cpu_count():
    allowed = os.sched_getaffinity(0)
    cores = set()
    for cpu in allowed:
        folder = Path(f'/sys/devices/system/cpu/cpu{cpu}/topology')
        try:
            cores.add(((folder / 'physical_package_id').read_text().strip(),
                       (folder / 'core_id').read_text().strip()))
        except OSError:
            return len(allowed)
    return len(cores)


def worker_limit(config, settings, requested=None, light=False):
    """Conservative BFS memory estimate; this is a scheduling cap, not a guarantee."""
    maximum = max(1, physical_cpu_count() - 1)
    if requested is not None:
        maximum = requested
    mode = config.get('size_mode', 'fixed')
    low = config['size'] if mode == 'fixed' else config.get('min_size', 2)
    high = config['size'] if mode == 'fixed' else config.get('max_size', 500)
    bfs_n = min(high, settings.get('BRUTE_MAX_N', 10))
    estimate = 128 * 1024 ** 2
    if bfs_n >= max(low, 5):
        # Nodes, visited storage and transient allocation growth headroom.
        estimate = max(estimate, math.factorial(bfs_n + 1) * 48)
    available = None
    try:
        match = re.search(r'^MemAvailable:\s+(\d+)', Path('/proc/meminfo').read_text(), re.M)
        available = int(match.group(1)) * 1024
    except (OSError, AttributeError):
        pass
    cap = max(1, int(available * .6) // estimate) if available else 1
    return (1 if light else min(maximum, cap)), estimate

def rank_hash(values):
    """Equivalent relative orders have identical hashes, irrespective of values."""
    ranks = {value: i for i, value in enumerate(sorted(values))}
    if len(ranks) != len(values):
        raise ValueError('Input contains duplicate values')
    sequence = [ranks[value] for value in values]
    return hashlib.sha256(' '.join(map(str, sequence)).encode()).hexdigest()


def generate(seed, generation, size):
    material = f'{GENERATOR}:{seed}:{generation}'.encode()
    derived = int.from_bytes(hashlib.sha256(material).digest(), 'big')
    values = list(range(size))
    random.Random(derived).shuffle(values)
    return values



def trial_size(config, generation):
    mode = config.get('size_mode', 'fixed')
    if mode == 'fixed':
        return config['size']
    low, high = config['min_size'], config['max_size']
    if mode == 'loop':
        return low + generation % (high - low + 1)
    material = f"size-v1:{config['seed']}:{generation}".encode()
    derived = int.from_bytes(hashlib.sha256(material).digest(), 'big')
    return random.Random(derived).randint(low, high)

def stats(values):
    if not values:
        return {'count': 0, 'average': None, 'min': None, 'max': None}
    return {'count': len(values), 'average': sum(values) / len(values),
            'min': min(values), 'max': max(values)}



def profile_build(destination):
    """Link existing solver objects with development-only timing wrappers."""
    recipe = "profile_objects:\n\t@printf '%s\\n' $(OBJS)\n"
    objects = subprocess.check_output(
        ['make', '-s', '-f', 'Makefile', '-f', '-', 'profile_objects'],
        input=recipe.encode(), cwd=ROOT).decode().splitlines()
    subprocess.run(['cc', '-Wall', '-Wextra', '-Werror', '-Iincludes',
                    'tests/profile_solver.c', *objects, 'libft/libft.a',
                    '-Wl,--wrap=greedy_reinsertion', '-Wl,--wrap=brute_solve',
                    '-Wl,--wrap=get_precomputed_bfs', '-o', str(destination)],
                   cwd=ROOT, check=True)


def move_counts(moves):
    return dict(sorted(Counter(moves).items()))


def parse_profile(text):
    algorithms = []
    codes = dict(zip('123456789AB', 'sa sb ss pa pb ra rb rr rra rrb rrr'.split()))
    for line in text.splitlines():
        ident, name, status, wall, cpu, rss, encoded = line.split('\t')
        moves = [codes[c] for c in encoded]
        algorithms.append(dict(id=int(ident), name=name.strip(), status=int(status), moves=len(moves),
                               seconds=float(wall), cpu_seconds=float(cpu),
                               process_peak_rss_kib=int(rss), counts=move_counts(moves)))
    return algorithms


def quantile(values, fraction):
    ordered = sorted(values)
    offset = (len(ordered) - 1) * fraction
    lo, hi = math.floor(offset), math.ceil(offset)
    return ordered[lo] + (ordered[hi] - ordered[lo]) * (offset - lo)


def distribution(values):
    if not values:
        return ['—'] * 10
    mean = statistics.mean(values)
    sd = statistics.stdev(values) if len(values) > 1 else None
    return [mean, sd if sd is not None else '—',
            sd / mean if sd is not None and mean else '—',
            min(values), quantile(values, .25), statistics.median(values),
            quantile(values, .75), quantile(values, .95), max(values),
            sd / math.sqrt(len(values)) if sd is not None else '—']


def analysis_tables(records, size):
    groups = {}
    raw = []
    headers = ['Run', 'Generation', 'Input size', 'Rank SHA-256', 'Binary SHA-256',
               'Algorithm ID', 'Algorithm', 'Moves', 'Wall seconds', 'CPU seconds',
               'Process peak RSS KiB', 'Chosen', 'Tied best', 'Within top band',
               *sorted(MOVES), 'Shared rotations', 'Forward rotations',
               'Reverse rotations', 'Rotation actions', 'Rotation instructions']
    for record in records:
        size = len(record['input_ranks'])
        candidates = record['algorithms']
        best = min((a['moves'] for a in candidates), default=None)
        chosen = next((a for a in candidates if a['moves'] == best), None)
        for a in candidates:
            key = (record['binary_sha256'], json.dumps(record['settings'], sort_keys=True), size, a['name'])
            groups.setdefault(key, []).append((record, a, a is chosen, a['moves'] == best))
            counts = a.get('counts', {})
            rotations = sum(counts.get(m, 0) for m in ['ra', 'rb', 'rr', 'rra', 'rrb', 'rrr'])
            shared = counts.get('rr', 0) + counts.get('rrr', 0)
            band = a['moves'] < 700 if size == 100 else a['moves'] < 5500 if size == 500 else '—'
            raw.append([record['run_id'], record['generation_id'], size, record['rank_sha256'],
                        record['binary_sha256'], a.get('id', '—'), a['name'], a['moves'],
                        a.get('seconds', '—'), a.get('cpu_seconds', '—'),
                        a.get('process_peak_rss_kib', '—'), int(a is chosen), int(a['moves'] == best), band,
                        *[counts.get(m, 0) if 'counts' in a else '—' for m in sorted(MOVES)],
                        shared if counts else '—',
                        sum(counts.get(m, 0) for m in ['ra', 'rb', 'rr']) if counts else '—',
                        sum(counts.get(m, 0) for m in ['rra', 'rrb', 'rrr']) if counts else '—',
                        rotations + shared if counts else '—', rotations if counts else '—'])
    rows = []
    for (digest, settings, size, name), entries in sorted(groups.items()):
        moves = [a['moves'] for _, a, _, _ in entries]
        timed = [a['seconds'] for _, a, _, _ in entries if 'seconds' in a]
        cpu = [a['cpu_seconds'] for _, a, _, _ in entries if 'cpu_seconds' in a]
        peak = [a['process_peak_rss_kib'] for _, a, _, _ in entries if 'process_peak_rss_kib' in a]
        rows.append([digest, settings, size, name, len(entries), len(timed),
                     sum(chosen for _, _, chosen, _ in entries),
                     sum(tied for _, _, _, tied in entries), *distribution(moves),
                     *distribution(timed), statistics.mean(cpu) if cpu else '—', max(peak) if peak else '—',
                     statistics.mean([a['moves'] - min(c['moves'] for c in r['algorithms'])
                                      for r, a, _, _ in entries]),
                     sum(a['moves'] < (700 if size == 100 else 5500)
                         for _, a, _, _ in entries) / len(entries) if size in (100, 500) else '—',
                     *[statistics.mean([a['counts'].get(move, 0) for _, a, _, _ in entries
                                        if 'counts' in a])
                       if any('counts' in a for _, a, _, _ in entries) else '—'
                       for move in sorted(MOVES)]])
    labels = ['Mean', 'Sample SD', 'CV', 'Min', 'P25', 'Median', 'P75', 'P95', 'Max', 'SEM']
    text = '\n## Algorithm comparison\n\n'
    text += table(['Binary SHA-256', 'Settings JSON', 'Input size', 'Algorithm', 'Runs', 'Timed runs',
                   'Chosen wins', 'Tied best', *['Moves ' + x for x in labels],
                   *['Seconds ' + x for x in labels], 'Mean CPU seconds', 'Max process peak RSS KiB', 'Mean extra moves vs best',
                   'Top-band fraction', *['Mean ' + move for move in sorted(MOVES)]], rows)
    text += '\n## Per-run algorithm data\n\n' + table(headers, raw)
    text += ('\nRows contain only algorithms that actually ran. Comparisons are grouped by binary, settings and input size. '
             'Sample SD measures observed variability; CV is SD/mean; SEM is SD/sqrt(n), not a prediction interval. '
             'Quantiles use linear interpolation. Missing historical measurements are marked —. '
             'Chosen wins use the first minimum, matching solver tie-breaking. '
             'rr and rrr are shared forward/reverse rotations; rotation actions count each shared move twice. '
             'Counts describe emitted instructions, including possible no-ops. '
             'Timing covers candidate execution, excluding instrumentation output and candidate initialization; '
             'total solver time includes process launch, parsing, initialization, reporting and output. '
             'RSS is Linux process-wide peak memory at candidate completion, not memory owned by that algorithm; '
             'later rows inherit earlier peaks. Runtime is measured, asymptotic complexity is not. '
             'All candidate rows belong to runs whose winning output passed the reference checker; '
             'individual candidates are not independently checker-validated by this runner.\n')
    return text

def atomic_text(path, text):
    temporary = path.with_suffix('.tmp')
    with temporary.open('w') as output:
        output.write(text)
        output.flush()
        os.fsync(output.fileno())
    os.replace(temporary, path)


def cell(value):
    return html.escape(str(value), quote=False).replace('|', '&#124;').replace('\n', '&#10;')


def table(headers, rows):
    lines = ['| ' + ' | '.join(headers) + ' |', '| ' + ' | '.join('---' for _ in headers) + ' |']
    lines += ['| ' + ' | '.join(cell(value) for value in row) + ' |' for row in rows]
    return '\n'.join(lines) + '\n'


def fields(text):
    result = {}
    for line in text.splitlines():
        if line.startswith('| ') and line.endswith(' |'):
            parts = [html.unescape(part.strip()) for part in line[2:-2].split(' | ')]
            if len(parts) == 2:
                result[parts[0]] = parts[1]
    return result


def code_section(title, text):
    fence = '`' * max(3, 1 + max((len(m.group()) for m in re.finditer(r'`+', text)), default=0))
    return f'\n### {title}\n\n{fence}text\n{text}\n{fence}\n'


def read_section(text, title):
    match = re.search(r'^### ' + re.escape(title) + r'\n\n(`{3,})text\n(.*?)\n\1\n', text, re.M | re.S)
    return match.group(2) if match else ''


def render_record(record):
    keys = [('Generation ID', 'generation_id'), ('Seed', 'seed'), ('Rank SHA-256', 'rank_sha256'),
            ('Binary SHA-256', 'binary_sha256'), ('Checker', 'checker'), ('Moves', 'move_count'),
            ('Solver seconds', 'solver_seconds'), ('Solver exit', 'solver_exit')]
    rows = [(label, record.get(key, '')) for label, key in keys]
    rows += [(key, value) for key, value in record['settings'].items()]
    rows += [(key, record[key]) for key in ('error', 'stderr_tail', 'checker_stderr') if key in record]
    text = f"\n## Run {record['run_id']}\n\n" + table(['Field', 'Value'], rows)
    text += '\n### Algorithm moves\n\n' + table(['Algorithm', 'Moves'],
             [(a['name'], a['moves']) for a in record['algorithms']])
    text += code_section('Algorithm metrics JSON', json.dumps(record['algorithms'], sort_keys=True))
    text += code_section('Input ranks', ' '.join(map(str, record['input_ranks'])))
    text += code_section('Winning moves', '\n'.join(record['moves']))
    text += '\n<details>\n<summary>Solution debug and completed status</summary>\n'
    text += code_section('Solution debug', record['solution_debug'])
    text += code_section('Completed status', '\n'.join(record['completed_status']))
    return text + '\n</details>\n'


def parse_records(text):
    records = []
    for part in re.split(r'^## Run ', text, flags=re.M)[1:]:
        run_id, body = part.split('\n', 1)
        metadata = fields(body.split('### Algorithm moves', 1)[0])
        algorithms = fields(body.split('### Algorithm moves\n', 1)[1].split('### Input ranks', 1)[0])
        record = {'run_id': int(run_id), 'generation_id': int(metadata['Generation ID']),
                  'seed': int(metadata['Seed']), 'rank_sha256': metadata['Rank SHA-256'],
                  'binary_sha256': metadata['Binary SHA-256'], 'checker': metadata['Checker'],
                  'move_count': int(metadata['Moves']), 'solver_seconds': float(metadata['Solver seconds']),
                  'solver_exit': int(metadata['Solver exit']),
                  'settings': {k: int(metadata[k]) for k in ('LOOKAHEAD_DEPTH', 'EXECUTE_DEPTH', 'DEBUG', 'LOOKAHEAD_DEPTH_100', 'LOOKAHEAD_DEPTH_500', 'LOOKAHEAD_DEPTH_FIRST_MOVE', 'EXECUTE_LIMIT_100', 'EXECUTE_LIMIT_500', 'EXECUTE_LIMIT_FIRST_MOVE', 'ENABLE_OPENING_LOOKAHEAD', 'SKIP_OTHER_ALGO_AFTER_BFS', 'GREEDY_PATH_CAPACITY', 'BRUTE_MAX_N', 'BENCHMARK_WORKERS') if k in metadata},
                  'input_ranks': list(map(int, read_section(body, 'Input ranks').split())),
                  'moves': read_section(body, 'Winning moves').splitlines(),
                  'solution_debug': read_section(body, 'Solution debug'),
                  'completed_status': read_section(body, 'Completed status').splitlines(),
                  'algorithms': [{'name': k, 'moves': int(v)} for k, v in algorithms.items()
                                 if k not in ('Algorithm', '---')]}
        metrics = read_section(body, 'Algorithm metrics JSON')
        if metrics:
            record['algorithms'] = json.loads(metrics)
        record.update({k: metadata[k] for k in ('error', 'stderr_tail', 'checker_stderr') if k in metadata})
        records.append(record)
    return records


def load_records(folder, prefix):
    """Atomic Markdown batches are authoritative; old JSONL logs remain readable."""
    paths = {}
    for extension in ('txt', 'md'):
        for path in folder.glob(f'{prefix}_output_[0-9][0-9][0-9][0-9][0-9][0-9].{extension}'):
            paths[path.stem] = path
    records = []
    for path in sorted(paths.values()):
        if path.suffix == '.md':
            batch = parse_records(path.read_text())
        else:
            batch = []
            for line in path.read_text().splitlines(keepends=True):
                if not line.endswith('\n'):
                    break
                batch.append(json.loads(line))
        for record in batch:
            if record['run_id'] != len(records) + 1 or rank_hash(record['input_ranks']) != record['rank_sha256']:
                raise ValueError(f'Invalid run ID or input hash in {path}')
            records.append(record)
    return records


def save_detail(folder, config, records, record):
    batch = (record['run_id'] - 1) // 10
    selected = records[batch * 10:] + [record]
    text = f"# Test runs {batch * 10 + 1}–{batch * 10 + len(selected)}\n\n"
    text += table(['Run', 'Generation', 'Checker', 'Moves', 'Seconds', 'Rank hash (short)'],
                  [(r['run_id'], r['generation_id'], r['checker'], r['move_count'],
                    r['solver_seconds'], r['rank_sha256'][:12]) for r in selected])
    text += ''.join(render_record(r) for r in selected)
    atomic_text(folder / f"{config['prefix']}_output_{batch+1:06d}.md", text)


def read_config(path):
    if path.suffix == '.txt':
        return json.loads(path.read_text())['config']
    metadata = fields(path.read_text().split('## Averages', 1)[0])
    return {'prefix': metadata['Session prefix'], 'seed': int(metadata['Master seed']),
            'size': int(metadata['Input size']) if metadata['Input size'] != 'variable' else None,
            'size_mode': metadata.get('Size mode', 'fixed'),
            'min_size': int(metadata.get('Minimum size', 2)),
            'max_size': int(metadata.get('Maximum size', 500)),
            'generator': metadata['Generator'],
            'python_version': metadata['Python version']}


def save_summary(path, config, records, status):
    algorithms = {}
    for record in records:
        for algorithm in record['algorithms']:
            algorithms.setdefault(algorithm['name'], []).append(algorithm['moves'])
    text = '# Push_swap test summary\n\n'
    text += table(['Session', 'Value'], [
        ('Status', status), ('Workers for this invocation', config.get('workers', 1)),
        ('Successful runs', len(records)), ('Master seed', config['seed']),
        ('Input size', config['size'] if config.get('size_mode', 'fixed') == 'fixed' else 'variable'),
        ('Size mode', config.get('size_mode', 'fixed')),
        ('Minimum size', config.get('min_size', 2)), ('Maximum size', config.get('max_size', 500)), ('Next generation ID', records[-1]['generation_id']+1 if records else 0),
        ('Updated', dt.datetime.now().astimezone().isoformat()), ('Session prefix', config['prefix']),
        ('Generator', config['generator']), ('Python version', config['python_version'])])
    text += '\n## Averages\n\n'
    rows = []
    measures = [('Winning solution', [r['move_count'] for r in records], 'moves')]
    measures += [(name, values, 'moves') for name, values in algorithms.items()]
    measures += [('Solver runtime', [r['solver_seconds'] for r in records], 'seconds')]
    for name, values, unit in measures:
        result = stats(values)
        rows.append((name, result['count'], f"{result['average']:.3f}" if values else '—',
                     result['min'] if values else '—', result['max'] if values else '—', unit))
    text += table(['Metric / algorithm', 'Runs', 'Average', 'Minimum', 'Maximum', 'Unit'], rows)
    text += '\n## Winning result by input size\n\n'
    size_rows = []
    for size in sorted({len(r['input_ranks']) for r in records}):
        selected = [r for r in records if len(r['input_ranks']) == size]
        size_rows.append([size, len(selected),
                          *distribution([r['move_count'] for r in selected]),
                          *distribution([r['solver_seconds'] for r in selected])])
    labels = ['Mean', 'Sample SD', 'CV', 'Min', 'P25', 'Median', 'P75', 'P95', 'Max', 'SEM']
    text += table(['Input size', 'Runs', *['Moves ' + x for x in labels],
                   *['Total seconds ' + x for x in labels]], size_rows)
    text += analysis_tables(records, config['size'])
    text += '\n## Saved runs\n\n'
    for i in range(0, len(records), 10):
        name = f"{config['prefix']}_output_{i//10+1:06d}"
        extension = '.md' if (path.parent / (name + '.md')).exists() else '.txt'
        text += f'- [Runs {i+1}–{min(i+10, len(records))}]({name}{extension})\n'
    text += '\n## Executable versions\n\n'
    text += table(['Binary SHA-256', 'Runs'], [(digest, sum(r['binary_sha256']==digest for r in records))
                  for digest in sorted({r['binary_sha256'] for r in records})])
    text += '\nAverages include successful checks only. Resumed sessions may include different binaries; '
    text += 'each run saves its exact input, settings, seed and generation ID.\n'
    atomic_text(path, text)


def stop_process(process):
    if process.poll() is None:
        os.killpg(process.pid, signal.SIGTERM)
        try:
            process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.wait()


def run_case(values, show_solutions=False, executable=None, live_progress=True):
    """FD 1 = moves, FD 2 = live progress, FD 3 = saved solution dump."""
    args = list(map(str, values))
    executable = executable or ROOT / 'push_swap'
    with tempfile.TemporaryDirectory(prefix='push_swap_test_') as temporary:
        dump = Path(temporary) / 'solutions.txt'
        metrics_path = Path(temporary) / 'metrics.tsv'
        with tempfile.TemporaryFile() as moves, tempfile.TemporaryFile() as diagnostic:
            command = ['bash', '-c', 'exec 3>"$1" 4>"$2"; shift 2; exec "$@"',
                       'push-swap-runner', str(dump), str(metrics_path), str(executable), *args]
            start = time.monotonic()
            process = start_process(command, stdout=moves, stderr=subprocess.PIPE,
                                       start_new_session=True)
            try:
                while True:
                    chunk = os.read(process.stderr.fileno(), 65536)
                    if not chunk:
                        break
                    diagnostic.write(chunk)
                    if live_progress:
                        sys.stderr.buffer.write(chunk)
                        sys.stderr.buffer.flush()
                code = process.wait()
            except BaseException:
                stop_process(process)
                raise
            finally:
                process.stderr.close()
                forget_process(process)
            elapsed = time.monotonic() - start
            moves.seek(0)
            output = moves.read()
            diagnostic.seek(0)
            debug = diagnostic.read().decode(errors='replace')
        profiled = parse_profile(metrics_path.read_text()) if metrics_path.exists() else []
        solution_dump = dump.read_text(errors='replace') if dump.exists() else ''
    if show_solutions:
        sys.stderr.write(solution_dump)
        sys.stderr.flush()
    decoded = output.decode(errors='replace').splitlines()
    result = {'solver_exit': code, 'solver_seconds': round(elapsed, 6),
              'move_count': len(decoded), 'moves': decoded, 'solution_debug': solution_dump,
              'algorithms': [{'name': name.strip(), 'moves': int(count)} for name, count in
                             re.findall(r'algo=\d+/\d+\s*\(([^\r\n]*)\) final_moves=(\d+) DONE', debug)],
              'completed_status': [line for line in re.sub(r'\x1b\[[0-9;]*[A-Za-z]', '', debug).splitlines()
                                   if 'final_moves=' in line]}
    if profiled:
        result['algorithms'] = profiled
        if code == 0 and (len({a['id'] for a in profiled}) != len(profiled)
                          or min(a['moves'] for a in profiled) != len(decoded)):
            result.update(checker='NOT RUN', error='Profiling events disagree with solver winner')
            return result
    elif values != sorted(values) and code == 0:
        result.update(checker='NOT RUN', error='No algorithm profiling events received')
        return result
    if code != 0 or any(move not in MOVES for move in decoded):
        result.update(checker='NOT RUN', error='Solver failed or printed invalid moves',
                      stderr_tail=debug[-8000:])
        return result
    checker = start_process([str(ROOT / 'checker_linux'), *args],
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, start_new_session=True)
    try:
        answer, errors = checker.communicate(output)
    except BaseException:
        stop_process(checker)
        raise
    finally:
        forget_process(checker)
    result['checker'] = answer.decode(errors='replace').strip()
    if checker.returncode != 0 or answer.strip() != b'OK':
        result.update(error='Checker rejected solution',
                      checker_stderr=errors.decode(errors='replace'))
    return result


def latest_session(base):
    summaries = [summary for folder in base.glob('*_output*') if folder.is_dir()
                 for summary in list(folder.glob('*_output_summary.md'))
                 + list(folder.glob('*_output_summary.txt'))]
    if not summaries:
        raise ValueError('No saved sessions found in ' + str(base))
    return max(summaries, key=lambda path: (path.stat().st_mtime_ns, str(path)))


def arguments():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-n', type=int, help='successful runs to add; omit for Ctrl-C mode')
    load = parser.add_mutually_exclusive_group()
    load.add_argument('-light', '--light', action='store_true', help='one worker; use for isolated timings')
    load.add_argument('-j', '--jobs', type=int, help='requested concurrent runs (RAM cap still applies)')
    parser.add_argument('--size', type=int, help='numbers per test (default: 100)')
    modes = parser.add_mutually_exclusive_group()
    modes.add_argument('-random', '--random', action='store_true', help='deterministic random input size per trial')
    modes.add_argument('-loop', '--loop', action='store_true', help='cycle through sizes from min to max')
    parser.add_argument('-min', '--min', dest='min_size', type=int, help='minimum variable size (default: 2)')
    parser.add_argument('-max', '--max', dest='max_size', type=int, help='maximum variable size (default: 500)')
    parser.add_argument('--seed', type=int, help='master seed (random by default)')
    parser.add_argument('--show-solutions', action='store_true',
                        help='also print the saved solution dump to stderr')
    resume = parser.add_mutually_exclusive_group()
    resume.add_argument('--resume', type=Path, help='existing session directory or summary file')
    resume.add_argument('--resume-latest', action='store_true',
                        help='resume the most recently modified session summary')
    args = parser.parse_args()
    if args.jobs is not None and args.jobs < 1:
        parser.error('--jobs must be positive')
    if args.n is not None and args.n < 1:
        parser.error('-n must be positive')
    if args.size is not None and args.size < 2:
        parser.error('--size must be at least 2')
    if (args.random or args.loop) and args.size is not None:
        parser.error('--size cannot be combined with -random or -loop')
    if not (args.random or args.loop) and (args.min_size is not None or args.max_size is not None):
        parser.error('-min/-max require -random or -loop')
    low = args.min_size if args.min_size is not None else 2
    high = args.max_size if args.max_size is not None else 500
    if low < 2 or high < low:
        parser.error('sizes require 2 <= min <= max')
    if (args.resume or args.resume_latest) and any([args.seed is not None, args.size is not None,
                            args.random, args.loop, args.min_size is not None, args.max_size is not None]):
        parser.error('resume reuses its saved seed and size mode/bounds')
    if args.resume_latest:
        try:
            args.resume = latest_session(ROOT / 'tests/debug/results/random_tests')
        except ValueError as error:
            parser.error(str(error))
    return args


def main():
    args = arguments()
    STOP_REQUESTED.clear()
    if args.resume:
        folder = args.resume.resolve()
        if folder.is_file():
            folder = folder.parent
        summaries = list(folder.glob('*_output_summary.md')) or list(folder.glob('*_output_summary.txt'))
        if len(summaries) != 1:
            raise ValueError('Resume directory must contain exactly one summary')
        summary = summaries[0]
        config = read_config(summary)
        summary = summary.with_suffix('.md')
        if config['generator'] != GENERATOR:
            raise ValueError('Unsupported saved generator version')
    else:
        prefix = dt.datetime.now().strftime('%Y%m%d_%H%M%S')
        base = ROOT / 'tests/debug/results/random_tests'
        base.mkdir(parents=True, exist_ok=True)
        folder = base / f'{prefix}_output'
        suffix = 0
        while folder.exists():
            suffix += 1
            folder = base / f'{prefix}_output_{suffix}'
        folder.mkdir()
        summary = folder / f'{prefix}_output_summary.md'
        config = {'prefix': prefix, 'seed': args.seed if args.seed is not None else secrets.randbits(64),
                  'size': args.size or 100, 'generator': GENERATOR,
                  'size_mode': 'random' if args.random else 'loop' if args.loop else 'fixed',
                  'min_size': args.min_size if args.min_size is not None else 2,
                  'max_size': args.max_size if args.max_size is not None else 500,
                  'python_version': sys.version.split()[0]}
    snapshot_dir = tempfile.TemporaryDirectory(prefix='push_swap_session_')
    lock = os.open(folder, os.O_RDONLY)
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        records = load_records(folder, config['prefix'])
        save_summary(summary, config, records, 'building')
        # Serialize runner builds through snapshot creation across sessions.
        build_key = hashlib.sha256(str(ROOT).encode()).hexdigest()[:16]
        build_lock = Path(tempfile.gettempdir()) / f'push_swap_build_{os.getuid()}_{build_key}.lock'
        with build_lock.open('a') as build_guard:
            fcntl.flock(build_guard, fcntl.LOCK_EX)
            subprocess.run(['make', '-s'], cwd=ROOT, check=True)
            if not os.access(ROOT / 'checker_linux', os.X_OK):
                raise ValueError('checker_linux must be executable')
            executable = Path(snapshot_dir.name) / 'push_swap'
            profile_build(executable)
            executable.chmod(0o700)
            binary_hash = hashlib.sha256(executable.read_bytes()).hexdigest()
        settings = {name: int(value) for name, value in re.findall(
            r'#\s*define\s+(LOOKAHEAD_DEPTH(?:_100|_500|_FIRST_MOVE)?|EXECUTE_(?:DEPTH|LIMIT_100|LIMIT_500|LIMIT_FIRST_MOVE)|DEBUG|ENABLE_OPENING_LOOKAHEAD|SKIP_OTHER_ALGO_AFTER_BFS|GREEDY_PATH_CAPACITY|BRUTE_MAX_N)\s+(\d+)',
            (ROOT / 'includes/push_swap.h').read_text())}
        workers, memory_estimate = worker_limit(config, settings, args.jobs, args.light)
        settings['BENCHMARK_WORKERS'] = workers
        config['workers'] = workers
        seen = {r['rank_sha256'] for r in records}
        generation = records[-1]['generation_id'] + 1 if records else 0
        target = len(records) + args.n if args.n else None
        fixed = config.get('size_mode', 'fixed') == 'fixed'
        permutations = math.factorial(config['size']) if fixed else None
        print(f"Session: {folder}\nSeed: {config['seed']} | mode: {config.get('size_mode', 'fixed')} | size: {config['size']} | saved: {len(records)}", flush=True)
        print(f"Workers: {workers} | estimated memory allowance per worker: {memory_estimate / 1024**2:.0f} MiB", flush=True)
        if workers > 1:
            print('Concurrent throughput mode: timings include resource contention; use --light for isolated comparisons.', flush=True)
        save_summary(summary, config, records, 'running')
        status = 'complete'
        executor = ThreadPoolExecutor(max_workers=workers)
        try:
            while target is None or len(records) < target:
                if fixed and len(seen) == permutations:
                    status = 'all_unique_permutations_tested'
                    break
                count = workers if target is None else min(workers, target - len(records))
                if fixed:
                    count = min(count, permutations - len(seen))
                batch = []
                pending = set()
                while len(batch) < count:
                    size = trial_size(config, generation)
                    values = generate(config['seed'], generation, size)
                    digest = rank_hash(values)
                    generation += 1
                    if fixed and (digest in seen or digest in pending):
                        continue
                    pending.add(digest)
                    record = {'run_id': len(records) + len(batch) + 1,
                              'generation_id': generation - 1, 'seed': config['seed'],
                              'rank_sha256': digest, 'input_ranks': values,
                              'binary_sha256': binary_hash, 'settings': settings}
                    print(f"Run {record['run_id']} | generation {generation-1} | size {size} | ranks {digest[:12]}", flush=True)
                    future = executor.submit(run_case, values, False, executable, workers == 1)
                    batch.append((record, future))
                # Commit in generation order, so interruption/resume stays deterministic.
                for record, future in batch:
                    record.update(future.result())
                    if args.show_solutions:
                        sys.stderr.write(record['solution_debug'])
                    if 'error' in record:
                        atomic_text(folder / f"{config['prefix']}_failed.md", "# Failed test\n" + render_record(record))
                        status = 'failed'
                        print(f"Failed: {record['error']}; input and diagnostics saved.", file=sys.stderr)
                        stop_workers()
                        break
                    save_detail(folder, config, records, record)
                    records.append(record)
                    seen.add(record['rank_sha256'])
                    save_summary(summary, config, records, 'running')
                    print(f"OK | run {record['run_id']} | {record['move_count']} moves | {record['solver_seconds']:.3f}s", flush=True)
                if status == 'failed':
                    break
        except KeyboardInterrupt:
            signal.signal(signal.SIGINT, signal.SIG_IGN)
            stop_workers()
            records = load_records(folder, config['prefix'])
            status = 'interrupted'
            print('Stopped; completed runs saved. Uncommitted inputs will be retried.', flush=True)
        finally:
            stop_workers()
            executor.shutdown(wait=True, cancel_futures=True)
        save_summary(summary, config, records, status)
        print(f'Summary: {summary}', flush=True)
        return 1 if status == 'failed' else 0
    finally:
        os.close(lock)
        snapshot_dir.cleanup()


if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print('\nStopped before testing; any saved session can be resumed.', file=sys.stderr)
        sys.exit(130)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(f'Runner error: {error}', file=sys.stderr)
        sys.exit(1)
