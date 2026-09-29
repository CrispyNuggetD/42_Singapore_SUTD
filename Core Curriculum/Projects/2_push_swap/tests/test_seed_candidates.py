"""Replay every candidate, then verify stdout is the first shortest candidate.

Run after make: python3 tests/test_seed_candidates.py
Four candidates combine three-value/circular-LIS seeds with local/lookahead
insertion. Build with DEBUG >= 2 for the recorded candidate dump. BFS is in WIP.
"""
import itertools
import random
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MOVES = dict(zip("123456789AB", "sa sb ss pa pb ra rb rr rra rrb rrr".split()))


def replay(values, moves):
    a, b = list(values), []
    for move in moves:
        assert move in MOVES.values(), move
        if move in ("sa", "ss") and len(a) > 1:
            a[0], a[1] = a[1], a[0]
        if move in ("sb", "ss") and len(b) > 1:
            b[0], b[1] = b[1], b[0]
        if move == "pa" and b:
            a.insert(0, b.pop(0))
        if move == "pb" and a:
            b.insert(0, a.pop(0))
        if move in ("ra", "rr") and a:
            a.append(a.pop(0))
        if move in ("rb", "rr") and b:
            b.append(b.pop(0))
        if move in ("rra", "rrr") and a:
            a.insert(0, a.pop())
        if move in ("rrb", "rrr") and b:
            b.insert(0, b.pop())
    assert a == sorted(values) and not b, (values, a, b)


def check(values):
    run = subprocess.run(
        [str(ROOT / "push_swap"), *map(str, values)],
        capture_output=True, text=True, timeout=120, check=True,
    )
    matches = re.findall(r"Stored length: (\d+)\nEncoded      : ([1-9AB]*)\n", run.stderr)
    assert len(matches) == 4, run.stderr[-1000:]
    candidates = []
    for length, encoded in matches:
        assert int(length) == len(encoded)
        moves = [MOVES[c] for c in encoded]
        replay(values, moves)
        candidates.append(moves)
    assert run.stdout.splitlines() == min(candidates, key=len)


def main():
    count = 0
    for size in range(2, 6):
        for values in itertools.permutations(range(size)):
            check(values)
            count += 1
    rng = random.Random(42)
    for size in (6, 7, 8, 9, 10, 11, 100, 500):
        values = list(range(size))
        for arrangement in (values, list(reversed(values)), rng.sample(values, size)):
            check(arrangement)
            count += 1
        print(f"size {size}: all four candidates sort correctly", flush=True)
    print(f"PASS: {count} inputs; every candidate and winner verified")


if __name__ == "__main__":
    main()
