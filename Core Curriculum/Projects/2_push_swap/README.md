*This project has been created as part of the 42 curriculum by hnah.*

<a id="top"></a>

# push_swap — studying sorting through stack operations and shortest paths

<a id="at-a-glance"></a>

## At a glance

✅ = implemented. 🚧 = partial or experimental. ❌ = not met or not implemented.
These describe the current code and study tools; they are not evaluation scores.

**Work in progress: this implementation does not yet meet the subject's move
requirements.** The repository already contains stack operations, a BFS search,
chunk-extraction experiments, and saved analysis data. Those are useful building
blocks, but they do not establish a compliant or efficient final solver.

| Status | Feature | Current behavior |
|---|---|---|
| 🚧 | Integer input | Parses separate arguments and space-separated strings; includes sign, range, duplicate and 500-value checks. Edge cases still need correction and validation. |
| ✅ | Rank normalisation | Replaces each distinct value with its position in sorted order. |
| ✅ | Circular-buffer stacks | Stores A and B in fixed arrays with wrapping read/write indices. |
| ✅ | Operation implementations | Swap, push, rotate, reverse rotate and combined-operation functions are present, alongside BFS state transformations. |
| ✅ | BFS state indexing | Uses a Lehmer permutation rank plus the A/B split and a visited bitset. |
| 🚧 | Chunk solver | Extracts rank intervals and replays a restricted BFS solution for each active chunk. This is the active development path. |
| ✅ | Study tools | Includes a permutation analyser, a reverse-BFS shortest-path analyser and an input generator. |
| ✅ | Saved study data | Reports and trial logs are preserved in Git under [`debug/results/`](debug/results/). |
| ✅ | Build organisation | Bundled libft, separate source/header directories, ignored build products and incremental builds. |
| ❌ | Subject move requirements | Not met yet; no passing 100/500-number benchmark is claimed. |
| ❌ | Clean instruction-only output | The current solver prints progress and route diagnostics to stdout alongside moves. |
| ❌ | Bonus checker implementation | The supplied Linux checker is a reference binary, not a checker written for this project. |

[↑ Back to top](#top)

<a id="design-choices-and-edge-cases"></a>

## Design choices and edge cases

| Status | Choice | Why it matters |
|---|---|---|
| ✅ | [Normalise before searching](#rank-normalisation) | Relative order determines sorting; original integer magnitudes need not appear in BFS states. |
| ✅ | [Circular buffers](#circular-buffer-stacks) | Stack operations reuse fixed storage without allocating nodes for each move. |
| ✅ | [Encode states directly](#bfs-and-state-indexing) | A permutation and split identify both stacks, allowing direct visited-bit lookup. |
| 🚧 | [Protect the hidden part of A](#chunk-extraction-and-the-hidden-stack) | Restricts the active search so a chunk can be considered separately from the rest of the stack. |
| 🚧 | [Compare extraction routes](#chunk-extraction-and-the-hidden-stack) | Tries both initial rotation directions and at most one direction change; this is not a global optimality proof. |
| ✅ | [Reverse BFS for study](#analysis-tools-and-study-data) | Reuses distances from the goal to enumerate shortest solutions for small permutations. |
| ❌ | [Submission edge cases](#current-limitations) | Output, trivial inputs, parser safety, memory handling and compliance still need work. |

[↑ Back to top](#top)

<a id="contents"></a>

## Contents

- [At a glance](#at-a-glance)
- [Design choices and edge cases](#design-choices-and-edge-cases)
- [Description](#description)
- [Instructions](#instructions)
- [Rank normalisation](#rank-normalisation)
- [Circular-buffer stacks](#circular-buffer-stacks)
- [BFS and state indexing](#bfs-and-state-indexing)
- [Chunk extraction and the hidden stack](#chunk-extraction-and-the-hidden-stack)
- [Analysis tools and study data](#analysis-tools-and-study-data)
- [Checks and current limitations](#checks-and-current-limitations)
- [Resources and use of AI](#resources)

[↑ Back to top](#top)

<a id="description"></a>

## Description

Push_swap explores sorting with two stacks and a limited vocabulary of moves.
The intended result is an ascending stack A, an empty stack B, and a short list
of instructions that reproduces the sort.

This version is also a study of the search space: how to represent a state,
recognise a state already visited, recover a path, and use small optimal solutions
to investigate larger sorting strategies. The saved reports are reference material
for that investigation, including cases with several equally short solutions.

The main program currently runs the experimental chunk solver unconditionally.
The small-case alternative in `solve.c` exists, but is bypassed by that early
return. The sections below distinguish the active solver from the analysis tools.

[↑ Back to top](#top)

<a id="instructions"></a>

## Instructions

Run these commands from the project directory. Building requires `make`, a C
compiler available as `cc`, and `ar`. The Makefile builds the bundled `libft/`
automatically; no sibling checkout or extra library copy is needed.

### Build and clean

```sh
make                      # ./push_swap
make debug                # bin/push_swap_debug
make analyse_bfs          # bin/bfs_analyser
make analyse_bfs_all_paths # bin/bfs_all_paths
make generator            # bin/generator
make clean
make fclean
make re
```

`make debug` defines `BFS_DEBUG` and links the logging helper. The logging call
in the old BFS implementation is currently commented out, so this target does
not guarantee a separate report. The ordinary executable already prints diagnostics.

`make clean` removes project and libft objects. `make fclean` also removes built
executables and the library archive. Both preserve study reports and the supplied
checker. Repeated builds leave unchanged executables alone.

### Run the development solver

```sh
./push_swap 3 2 1
./push_swap "3 2 1"
./bin/generator 5
```

Start with small inputs. The current BFS reserves space for the configured maximum
state count even for a small problem; larger searches can be expensive.
Output contains both instructions and diagnostics, so piping it directly to the
checker or counting all stdout lines does not provide a valid subject benchmark.

### Project layout

| Path | Purpose |
|---|---|
| [`src/`](src/) | Solver, parsing, stack operations and BFS implementation. |
| [`includes/push_swap.h`](includes/push_swap.h) | Shared structures, limits and function declarations. |
| [`libft/`](libft/) | Self-contained library sources used by this project. |
| [`debug/`](debug/) | Analysis and generator sources. |
| [`debug/results/`](debug/results/) | Study reports and trial logs, kept in Git. |
| [`tests/checker_linux`](tests/checker_linux) | Supplied Linux checker binary. |
| [`notes.md`](notes.md) | Working questions, ideas and unfinished plans. |
| `obj/`, `bin/`, `push_swap` | Generated build products, ignored by Git. |

The `DO_NOT_SUBMIT` name on `src/DO_NOT_SUBMIT_DEBUG_hidden_bfs.c` reflects its
experimental role; it is still a dependency of the main build.

[↑ Back to top](#top)

<a id="rank-normalisation"></a>

## Rank normalisation

[`rank_values.c`](src/rank_values.c) counts how many input values are smaller
than each value. For distinct inputs, that gives ranks from `0` to `n - 1`:

```text
values:  40  -8  12
ranks:    2   0   1
```

The ordering is preserved, so the same stack moves sort either representation.
The current implementation compares every value with every other value, taking
O(n²) time. Small BFS states store normalised ranks as unsigned bytes; the main
stacks still store integers.

[↑ Back to top](#top)

<a id="circular-buffer-stacks"></a>

## Circular-buffer stacks

Each stack has an integer array, a capacity, a read index and a write index.
The indices wrap around the array. One spare slot distinguishes a full buffer
from an empty one, so `n` input values use a capacity of `n + 1`.

The circular-buffer helpers implement the underlying movements. The
`stack_operation_*.c` wrappers also append an encoded move to the solution.
The output helper translates those codes back to names such as `sa`, `pb` and
`rra`, one instruction per line. The solver's additional diagnostic prints are
what currently prevent the complete output from being instruction-only.

[↑ Back to top](#top)

<a id="bfs-and-state-indexing"></a>

## BFS and state indexing

A BFS state stores one permutation and a split position. The prefix represents
A and the suffix represents B. For `n` distinct ranks, there are `n!`
permutations and `n + 1` possible splits:

```text
possible states = (n + 1) × n! = (n + 1)!
state ID        = split × n! + Lehmer rank(permutation)
```

[`bfs_optimiser_lehmer_rank.c`](src/bfs_optimiser_lehmer_rank.c) computes that ID.
The solver uses it to look up one visited bit per state. Each discovered node
also records its parent and the move used to reach it, allowing the final path
to be reconstructed backwards.

BFS finds a shortest path within the graph it actually explores. The active
solver tries only `sa`, `sb`, `pa`, `pb`, `ra` and `rra`, with additional
restrictions to protect hidden values in A. Its result must not be described as
the globally shortest solution over all eleven operations.

With `BRUTE_MAX_N` set to 10, the configured maximum is `11! = 39,916,800`
states. The main BFS allocates that many nodes for every call. Compact state
encoding helps, but does not remove factorial growth or the current allocation cost.

[↑ Back to top](#top)

<a id="chunk-extraction-and-the-hidden-stack"></a>

## Chunk extraction and the hidden stack

The active development path processes successive rank intervals of up to ten
values. It first moves the selected interval from A to B, then creates temporary
stacks containing the active chunk, searches for a solution and replays that
solution on the real stacks.

[`chunk_extract_optimal.c`](src/chunk_extract_optimal.c) simulates extraction
routes before executing one. It tries each initial rotation direction and each
point at which to reverse direction after collecting a target value. It chooses
the lowest rotation-plus-push cost among those candidates.

“Optimal” here refers only to that limited family of extraction routes. The code
does not compare arbitrary direction changes or the total future sorting cost.
Likewise, a short BFS solution for one chunk does not prove that the full sequence
meets the subject's move requirements.

The search treats the unseen portion of A as a boundary: it disallows A rotations
when the visible portion is nonempty and disallows `sa` when fewer than two visible
values are available. This is the experiment behind “hidden BFS”. Its overall
correctness and efficiency still need broader validation.

[↑ Back to top](#top)

<a id="analysis-tools-and-study-data"></a>

## Analysis tools and study data

The two analysis executables accept an `n` from 2 to 7 and write timestamped
reports into their working directory. For a small reverse-BFS study:

```sh
make analyse_bfs_all_paths
mkdir -p debug/results
(cd debug/results && ../../bin/bfs_all_paths 3)
```

The reverse analyser builds distances from the sorted goal, uses all eleven
operations, and enumerates shortest paths by following moves that reduce the
remaining distance. This differs from the restricted search in the main solver.

The permutation analyser can be run with
`(cd debug/results && ../../bin/bfs_analyser 3)` after `make analyse_bfs`.
It calls the current `brute_solve`, so its results depend on the current search
restrictions. Historical reports can reflect an earlier version of the search.

| Saved material | What to study |
|---|---|
| [`debug/results/`](debug/results/) | All preserved BFS reports and 500-number trial logs. |
| [All shortest paths, n = 5](debug/results/push_swap_bfs_all_paths_n5_2026-08-24_22-02-49.txt) | The report records 120 starting permutations, 720 graph states and a maximum optimal distance of 9. |
| [All shortest paths, n = 7](debug/results/push_swap_bfs_all_paths_n7_2026-08-24_22-02-53.txt) | The report records 5,040 starting permutations, 40,320 graph states and a maximum optimal distance of 13. |
| [`notes.md`](notes.md) | Questions about pattern discovery, heuristics and how small solutions might inform larger cases. |

These numbers describe the saved reports, not a fresh evaluation of the current
executable. The reports are important study data and are **not ignored**. Build
cleanup does not delete them. Larger all-path reports can grow quickly because
one starting permutation may have many equally short solutions.

[↑ Back to top](#top)

<a id="checks-and-current-limitations"></a>

## Checks and current limitations

### Build checks

The following checks passed during the directory reorganisation. They establish
build behavior, not sorting correctness or a subject score.

| Result | Check |
|---|---|
| ✅ | Main executable and all four development tools build with `-Wall -Wextra -Werror`. |
| ✅ | Repeating all build targets leaves executable and archive timestamps unchanged. |
| ✅ | `make clean` removes objects while preserving executables. |
| ✅ | `make fclean` removes build products while preserving the checker and all 17 existing study reports. |
| ✅ | All targets rebuild after cleanup. |
| ✅ | Deleted main and analyser executables are recreated. |

<a id="current-limitations"></a>

### Current limitations

The next milestone is a correct, compliant instruction stream, followed by
measured move-count improvements. Outstanding work includes:

- Meeting the subject's move requirements and recording reproducible benchmarks.
- Removing solver diagnostics from stdout and checking results with the supplied checker.
- Correct handling of trivial inputs: one integer currently prints `Error`, and sorted input has no early exit before the chunk solver.
- Hardening parsing. A non-space character immediately after digits can leave the count unchanged before an array write; long strings of leading zeroes also hit the digit-count limit.
- Completing allocation-failure handling and solution cleanup; no leak-free result is claimed.
- Reducing the fixed maximum-size BFS allocation and validating chunk replay across larger inputs.
- Reviewing Norm, allowed functions and global variables before submission. A successful build is not a compliance check.

[↑ Back to top](#top)

<a id="resources"></a>

## Resources

- [Working notes](notes.md) and [saved study reports](debug/results/) document the investigation and examples.
- [Bundled libft documentation](libft/README.md) describes the shared library.
- [Pipex README](../2_pipex/README.md) provides the structure used here: feature status, design explanations, reproducible commands and explicit limitations.
- Harvard CS50 lectures by David J. Malan, peer discussions and debugging references contributed to the broader learning process recorded in the previous README.

### Use of AI

I use ChatGPT for explanations, alternative approaches and tradeoffs, and help
when I am stuck. My preference is to work through the reasoning and derive an
approach before receiving a complete solution. Questions about computation,
hardware and algorithmic costs are part of that learning process too.

AI also helped organise this project's files, check its build behavior and rewrite
this README against the current source. The feature descriptions distinguish
implemented mechanisms from unfinished requirements; AI assistance is not evidence
that the solver is correct or ready for evaluation.

[↑ Back to top](#top)
