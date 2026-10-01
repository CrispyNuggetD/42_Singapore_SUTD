#!/usr/bin/env python3
"""Exercise both storage paths, long answers and parsing; replay every move."""
import argparse
from collections import deque
from itertools import permutations
from pathlib import Path
import random
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def run(binary, args):
    result = subprocess.run([str(binary), *args], capture_output=True, timeout=60)
    assert b'Sanitizer' not in result.stderr and b'runtime error:' not in result.stderr, result.stderr
    return result


def replay(values, output):
    a, b = deque(values), deque()
    for move in output.decode().splitlines():
        if move in ('sa', 'sb', 'ss'):
            for stack in ([a] if move == 'sa' else [b] if move == 'sb' else [a, b]):
                if len(stack) >= 2:
                    stack[0], stack[1] = stack[1], stack[0]
        elif move in ('pa', 'pb'):
            dest, source = (a, b) if move == 'pa' else (b, a)
            if source:
                dest.appendleft(source.popleft())
        elif move in ('ra', 'rb', 'rr', 'rra', 'rrb', 'rrr'):
            stacks = [a, b] if move in ('rr', 'rrr') else [a] if move.endswith('a') else [b]
            for stack in stacks:
                stack.rotate(1 if move.startswith('rr') and move != 'rr' else -1)
        else:
            raise AssertionError(f'Unknown move: {move}')
    assert not b and list(a) == sorted(values), (len(values), list(a)[:20], len(b))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--binary', type=Path, default=ROOT / 'push_swap')
    parser.add_argument('--baseline', type=Path)
    options = parser.parse_args()
    rng = random.Random(42)
    cases = [list(p) for n in range(1, 6) for p in permutations(range(n))]
    cases += [rng.sample(range(-1000, 1000), n) for n in (6, 7, 11, 12)]
    # Exercise the inline boundary without an expensive random-500 search.
    cases += [list(range(100)), list(range(499)), list(range(500))]
    cases += [rng.sample(range(-100000, 100000), n) for n in (501, 502, 1000, 2000)]
    cases += [list(range(501)), list(range(1000, 0, -1)), [-2147483648, 2147483647, 0]]
    longest = 0
    for values in cases:
        args = list(map(str, values))
        result = run(options.binary, args)
        assert result.returncode == 0, (len(values), result.stderr)
        replay(values, result.stdout)
        if values == sorted(values):
            assert result.stdout == result.stderr == b'', 'Sorted input must be silent'
        longest = max(longest, len(result.stdout.splitlines()))
        if options.baseline and len(values) <= 500 and values != sorted(values):
            before = run(options.baseline, args)
            # Reordered candidates may select another equally short answer.
            assert before.returncode == 0, len(values)
            assert len(before.stdout.splitlines()) == len(result.stdout.splitlines()), len(values)
        if len(values) > 500:
            grouped = run(options.binary, [' '.join(args[:200]), *args[200:]])
            assert grouped.returncode == 0 and grouped.stdout == result.stdout
            quoted = run(options.binary, [' '.join(args)])
            assert quoted.returncode == 0 and quoted.stdout == result.stdout
    valid = [(['000000000000000000000000001', '-0000000000002'], [1, -2]),
             (['  +2147483647  -2147483648  0  '], [2147483647, -2147483648, 0])]
    for args, values in valid:
        result = run(options.binary, args)
        assert result.returncode == 0, result.stderr
        replay(values, result.stdout)
    invalid = [[''], [' '], ['1', ''], ['+'], ['--1'], ['+-1'], ['1,2'],
               ['1\t2'], ['2147483648'], ['-2147483649'], ['9' * 1000],
               ['1', '+01'], ['-0', '0'], [' '.join(map(str, range(501))) + ' 500'],
               [' '.join(map(str, range(501))) + ' x']]
    for args in invalid:
        result = run(options.binary, args)
        assert result.returncode != 0 and result.stdout == b'' and result.stderr == b'Error\n', result
    empty = run(options.binary, [])
    assert empty.returncode == 0 and empty.stdout == empty.stderr == b''
    assert longest > 10000, 'Did not exercise answer growth'
    print(f'PASS: {len(cases)} sorting cases, grouped/quoted large input, integer boundaries, '
          f'{len(invalid)} invalid inputs; longest answer {longest} moves')


if __name__ == '__main__':
    main()
