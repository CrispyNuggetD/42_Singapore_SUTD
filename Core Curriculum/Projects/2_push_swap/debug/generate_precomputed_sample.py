#!/usr/bin/env python3
"""Offline study tool: print the packed initializer for n=1..4, in Lehmer order."""
from collections import deque
from itertools import permutations

MOVES = 'sa sb ss pa pb ra rb rr rra rrb rrr'.split()
INVERSE = (0, 1, 2, 4, 3, 8, 9, 10, 5, 6, 7)


def apply(state, move):
    a, b = map(list, state)
    if move in ('sa', 'ss') and len(a) > 1:
        a[0], a[1] = a[1], a[0]
    if move in ('sb', 'ss') and len(b) > 1:
        b[0], b[1] = b[1], b[0]
    if move == 'pa' and b:
        a.insert(0, b.pop(0))
    if move == 'pb' and a:
        b.insert(0, a.pop(0))
    if move in ('ra', 'rr') and a:
        a.append(a.pop(0))
    if move in ('rb', 'rr') and b:
        b.append(b.pop(0))
    if move in ('rra', 'rrr') and a:
        a.insert(0, a.pop())
    if move in ('rrb', 'rrr') and b:
        b.insert(0, b.pop())
    return tuple(a), tuple(b)


def solutions(n):
    goal = (tuple(range(n)), ())
    paths = {goal: ()}
    queue = deque([goal])
    while queue:
        state = queue.popleft()
        for code, inverse in enumerate(INVERSE, 1):
            previous = apply(state, MOVES[inverse])
            if previous in paths or apply(previous, MOVES[code - 1]) != state:
                continue
            paths[previous] = (code,) + paths[state]
            queue.append(previous)
    return [paths[(p, ())] for p in permutations(range(n))]


def packed_data():
    data = bytearray()
    for n in range(1, 5):
        for path in solutions(n):
            assert len(path) <= 5
            codes = path + (0,) * (6 - len(path))
            data.extend(codes[i] | (codes[i + 1] << 4) for i in (0, 2, 4))
    return data


if __name__ == '__main__':
    data = packed_data()
    for start in range(0, len(data), 12):
        print('"' + ''.join(f'\\x{byte:02x}' for byte in data[start:start + 12]) + '"')
