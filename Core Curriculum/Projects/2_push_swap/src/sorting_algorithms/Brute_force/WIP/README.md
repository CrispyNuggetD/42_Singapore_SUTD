# BFS seed experiments — WIP

**2026-09-29: all files here are excluded from the active build and solver.**
The active program compares only the three-value and circular-LIS seeds.
The existing experimental chunk BFS in `../BFS/bfs.c` has not been moved.

## File guide

| Files | State | What they do |
| --- | --- | --- |
| `bfs_seed.h` | Implemented, parked | Private declarations and compact search context for the restricted seed BFS. |
| `bfs_seed_state.c` | Implemented, parked | Locally rank A's retained values, allocate queue/visited storage, check ascending A. |
| `bfs_seed_search.c` | Implemented, parked | Search using only `sa`, `sb`, `pa`, `pb`, `ra`, `rra`. |
| `bfs_seed.c` | Implemented, parked | Reconstruct and replay the restricted route, appending to existing solution moves. |
| `bfs_seed_wip.h` / `bfs_seed_wip.c` | Partial scaffold | Proposed bounded search with all eleven moves. Only its goal check is implemented; the search returns `WIP_BFS_NOT_IMPLEMENTED`. |

## How the parked restricted version works

After pushing all but at most ten values to B, `bfs_seed_sort()` searches only the
retained values in A. It temporarily pushes them above the saved B tail and brings
them back into ascending A. It preserves both seed membership and the original B
tail. It does not allow B rotations or combined moves, and duplicate-state removal
prevents replaying model no-ops that could touch hidden values in the real stacks.
Global ranks are normalised locally before entering the compact byte-sized state.

This implementation passed candidate replay checks before being parked, but it is
not the all-eleven-move design requested below. Its queue can reserve roughly
639 MB plus a 5 MB visited bitset for ten values, and searches can take tens of
seconds. It has no configurable node/depth budget.

## Reconnecting later

Do not add the entire WIP directory to the build automatically. Choose an approach
first. To restore the restricted version, explicitly add its three C files to the
Makefile, make this directory's `bfs_seed.h` available to callers, restore a BFS
seed enum entry before `SEED_COUNT` and an algorithm configuration before `ALGO_COUNT`, and add the preparation branch that pushes
until A has at most `BRUTE_MAX_N` values before calling `bfs_seed_sort()`.
It needs an initialised current solution and stack capacity for the whole input.
Update candidate-count assertions in `tests/test_seed_candidates.py` and rerun
route-replay checks after integration. For the bounded version, finish the search
and replay first; its stub must never be treated as a successful sort.

For a syntax-only check without integrating either experiment:

```sh
cc -Wall -Wextra -Werror -Iincludes -fsyntax-only \
  src/sorting_algorithms/Brute_force/WIP/*.c
```

---

# Proposed bounded BFS: all eleven moves

**2026-09-29 — scaffold only. Not included in the Makefile or active solver.**

Start after preparation has pushed all but ten values to B: for a 500-value
input, A has ten and B has 490. Search for a useful seed before greedy reinsertion.

## Goal and permitted changes

The goal is **exactly ten values in A, strictly ascending from top to bottom**.
B has no ordering requirement. The final ten values may differ from the initial
ten: values may move between A and B during the search. Preserve the complete
input multiset across both stacks. A may temporarily have more or fewer than ten.

All eleven operations are intended to be available:
`sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`, `rr`, `rra`, `rrb`, `rrr`.
Combined operations must work when only one participating stack can change.

The goal only inspects A, but simulation and duplicate detection must account for
both stacks. B is not an anonymous wall: `pa` reads its top, and B rotations change
which values can be pulled into the seed. Do not reuse the ten-byte compact BFS
state or its factorial index to represent all 500 values.

## Implemented versus unfinished

- Implemented: `wip_bfs_seed_is_goal()` checks the exact ten-value ascending goal.
- Drafted: a node with both real circular buffers, parent, depth and move;
  caller-supplied node/depth limits; distinct search result statuses.
- Unfinished: `wip_bfs_seed_search()` always returns `WIP_BFS_NOT_IMPLEMENTED`.
  It does not allocate, modify stacks, append moves, or invoke a fallback.

## Implementation checkpoints

1. Validate initial stacks, ten-value A, capacities, and positive limits. Decide
   the default limits after measuring node memory and expansion costs. The draft
   full-buffer node is simple but large; budget memory before allocating.
2. Copy both stacks into a bounded FIFO queue. Simulate all eleven operations on
   private states without appending speculative moves to the real solution.
3. Deduplicate by logical A/B contents and their split. Circular-buffer indices
   or unused buffer slots alone must not make equivalent states distinct. Hash
   collisions need exact comparison; never discard a distinct state on hash alone.
4. Apply the A-only goal check. Enforce node/depth limits before further expansion;
   return `WIP_BFS_LIMIT` when a frontier is truncated, not `WIP_BFS_EXHAUSTED`.
   Check the initial goal before allocating a large queue.
5. Reconstruct a found route, release search memory, then replay it on the real
   stacks and append it after the existing preparation moves. Verify sorted A,
   A length ten, and conservation of all values before greedy reinsertion.
6. Only when ready, integrate with seed preparation. On a search limit, the
   existing restricted seed BFS is a possible fallback from the untouched input.
   Do not wire the placeholder into the active solver yet.

This is breadth-first search with a resource limit, not exhaustive sorting of all
500 values and not beam search. A found route is shortest within completely
explored earlier layers; reaching a limit does not establish that no route exists.
B will subsequently be handled by greedy reinsertion.

## Checks to add when implementation resumes

- Ten sorted values in A succeeds with arbitrary B; nine/eleven or unsorted A fails.
- All eleven simulated moves match real operations, including empty/singleton
  sides of combined moves and wrapped circular buffers.
- A route may exchange seed membership and reorder B while preserving all values.
- Budget exhaustion leaves real stacks and the existing move prefix untouched.
- Found routes replay correctly, including ranks above 255 and the 500-value case.
- Tiny-state searches match an independent BFS for route length and goal behavior.


## Memory diagnostics

`src/debug_tools.c` provides `debug_print_bfs_memory(node_count)`. Both the
original chunk BFS and this parked restricted BFS call it before allocation when
compiled with `BFS_DEBUG` (the project's `make debug` flag). It writes planned
state payload, actual node-table size, visited-bitset size, and their combined
allocation to stderr. Bytes/KB/MB use decimal units and integer truncation; these
are capacity estimates, not measured resident memory. The all-eleven-move scaffold
has a different node layout and must get its own estimate when implemented.
Because seed BFS is parked, the active greedy solver does not emit this report.
