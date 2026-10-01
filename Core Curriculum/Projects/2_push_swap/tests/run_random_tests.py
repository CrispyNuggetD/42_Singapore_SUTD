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
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
MOVES = set('sa sb ss pa pb ra rb rr rra rrb rrr'.split())
GENERATOR = 'python-mt19937-shuffle-sha256-v1'


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


def stats(values):
    if not values:
        return {'count': 0, 'average': None, 'min': None, 'max': None}
    return {'count': len(values), 'average': sum(values) / len(values),
            'min': min(values), 'max': max(values)}


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
                  'settings': {k: int(metadata[k]) for k in ('LOOKAHEAD_DEPTH', 'EXECUTE_DEPTH', 'DEBUG', 'LOOKAHEAD_DEPTH_100', 'LOOKAHEAD_DEPTH_500', 'LOOKAHEAD_DEPTH_FIRST_MOVE', 'EXECUTE_LIMIT_100', 'EXECUTE_LIMIT_500', 'EXECUTE_LIMIT_FIRST_MOVE', 'ENABLE_OPENING_LOOKAHEAD', 'SKIP_OTHER_ALGO_AFTER_BFS', 'GREEDY_PATH_CAPACITY', 'BRUTE_MAX_N') if k in metadata},
                  'input_ranks': list(map(int, read_section(body, 'Input ranks').split())),
                  'moves': read_section(body, 'Winning moves').splitlines(),
                  'solution_debug': read_section(body, 'Solution debug'),
                  'completed_status': read_section(body, 'Completed status').splitlines(),
                  'algorithms': [{'name': k, 'moves': int(v)} for k, v in algorithms.items()
                                 if k not in ('Algorithm', '---')]}
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
            'size': int(metadata['Input size']), 'generator': metadata['Generator'],
            'python_version': metadata['Python version']}


def save_summary(path, config, records, status):
    algorithms = {}
    for record in records:
        for algorithm in record['algorithms']:
            algorithms.setdefault(algorithm['name'], []).append(algorithm['moves'])
    text = '# Push_swap test summary\n\n'
    text += table(['Session', 'Value'], [
        ('Status', status), ('Successful runs', len(records)), ('Master seed', config['seed']),
        ('Input size', config['size']), ('Next generation ID', records[-1]['generation_id']+1 if records else 0),
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


def run_case(values, show_solutions=False, executable=None):
    """FD 1 = moves, FD 2 = live progress, FD 3 = saved solution dump."""
    args = list(map(str, values))
    executable = executable or ROOT / 'push_swap'
    with tempfile.TemporaryDirectory(prefix='push_swap_test_') as temporary:
        dump = Path(temporary) / 'solutions.txt'
        with tempfile.TemporaryFile() as moves, tempfile.TemporaryFile() as diagnostic:
            command = ['bash', '-c', 'exec 3>"$1"; shift; exec "$@"',
                       'push-swap-runner', str(dump), str(executable), *args]
            start = time.monotonic()
            process = subprocess.Popen(command, stdout=moves, stderr=subprocess.PIPE,
                                       start_new_session=True)
            try:
                while True:
                    chunk = os.read(process.stderr.fileno(), 65536)
                    if not chunk:
                        break
                    diagnostic.write(chunk)
                    sys.stderr.buffer.write(chunk)
                    sys.stderr.buffer.flush()
                code = process.wait()
            except BaseException:
                stop_process(process)
                raise
            finally:
                process.stderr.close()
            elapsed = time.monotonic() - start
            moves.seek(0)
            output = moves.read()
            diagnostic.seek(0)
            debug = diagnostic.read().decode(errors='replace')
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
    if code != 0 or any(move not in MOVES for move in decoded):
        result.update(checker='NOT RUN', error='Solver failed or printed invalid moves',
                      stderr_tail=debug[-8000:])
        return result
    checker = subprocess.Popen([str(ROOT / 'checker_linux'), *args],
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, start_new_session=True)
    try:
        answer, errors = checker.communicate(output)
    except BaseException:
        stop_process(checker)
        raise
    result['checker'] = answer.decode(errors='replace').strip()
    if checker.returncode != 0 or answer.strip() != b'OK':
        result.update(error='Checker rejected solution',
                      checker_stderr=errors.decode(errors='replace'))
    return result


def arguments():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('-n', type=int, help='successful runs to add; omit for Ctrl-C mode')
    parser.add_argument('--size', type=int, help='numbers per test (default: 100)')
    parser.add_argument('--seed', type=int, help='master seed (random by default)')
    parser.add_argument('--show-solutions', action='store_true',
                        help='also print the saved solution dump to stderr')
    parser.add_argument('--resume', type=Path, help='existing session directory or summary file')
    args = parser.parse_args()
    if args.n is not None and args.n < 1:
        parser.error('-n must be positive')
    if args.size is not None and not 2 <= args.size <= 500:
        parser.error('--size must be between 2 and 500')
    if args.resume and (args.seed is not None or args.size is not None):
        parser.error('--resume reuses its saved seed and size')
    return args


def main():
    args = arguments()
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
            executable.write_bytes((ROOT / 'push_swap').read_bytes())
            executable.chmod(0o700)
            binary_hash = hashlib.sha256(executable.read_bytes()).hexdigest()
        settings = {name: int(value) for name, value in re.findall(
            r'#\s*define\s+(LOOKAHEAD_DEPTH(?:_100|_500|_FIRST_MOVE)?|EXECUTE_(?:DEPTH|LIMIT_100|LIMIT_500|LIMIT_FIRST_MOVE)|DEBUG|ENABLE_OPENING_LOOKAHEAD|SKIP_OTHER_ALGO_AFTER_BFS|GREEDY_PATH_CAPACITY|BRUTE_MAX_N)\s+(\d+)',
            (ROOT / 'includes/push_swap.h').read_text())}
        seen = {r['rank_sha256'] for r in records}
        generation = records[-1]['generation_id'] + 1 if records else 0
        target = len(records) + args.n if args.n else None
        permutations = math.factorial(config['size'])
        print(f"Session: {folder}\nSeed: {config['seed']} | size: {config['size']} | saved: {len(records)}", flush=True)
        save_summary(summary, config, records, 'running')
        status = 'complete'
        try:
            while target is None or len(records) < target:
                if len(seen) == permutations:
                    status = 'all_unique_permutations_tested'
                    print('All unique permutations have been tested.', flush=True)
                    break
                values = generate(config['seed'], generation, config['size'])
                digest = rank_hash(values)
                generation += 1
                if digest in seen:
                    continue
                print(f"Run {len(records)+1} | generation {generation-1} | ranks {digest[:12]}", flush=True)
                record = {'run_id': len(records)+1, 'generation_id': generation-1,
                          'seed': config['seed'], 'rank_sha256': digest, 'input_ranks': values,
                          'binary_sha256': binary_hash, 'settings': settings}
                record.update(run_case(values, args.show_solutions, executable))
                if 'error' in record:
                    atomic_text(folder / f"{config['prefix']}_failed.md", "# Failed test\n" + render_record(record))
                    status = 'failed'
                    print(f"Failed: {record['error']}; input and diagnostics saved.", file=sys.stderr)
                    break
                save_detail(folder, config, records, record)
                records.append(record)
                seen.add(digest)
                save_summary(summary, config, records, 'running')
                print(f"OK | {record['move_count']} moves | average {stats([r['move_count'] for r in records])['average']:.2f}", flush=True)
        except KeyboardInterrupt:
            signal.signal(signal.SIGINT, signal.SIG_IGN)
            records = load_records(folder, config['prefix'])
            status = 'interrupted'
            print('\nStopped; completed runs saved. Interrupted input will be retried.', flush=True)
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
