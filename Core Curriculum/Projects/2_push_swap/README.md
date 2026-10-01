*This project has been created as part of the 42 curriculum by hnah.*

<a id="top"></a>

# push_swap — studying sorting through stack operations and shortest paths


<a id="preface"></a>

## Preface — why I spent so much time on push_swap

I spent a substantial part of my time at 42 reading **Introduction to Algorithms,
Third Edition**, by Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest and
Clifford Stein (CLRS). It was a great help throughout this project and an
important companion to my study of algorithms.
[Book reference: MIT Press, 2009](https://mitpress.mit.edu/9780262033848/introduction-to-algorithms/).

Some peers described push_swap as one of the simplest projects in their
curriculum. For me, its small problem statement opened up a much larger set of
questions. I enjoy problems where a few precise rules leave room for many
different solutions. I wanted to build a strong algorithm, but also to learn
as much as I could about why an approach works, where it fails, and how to
improve it within real computational limits. That is why this README is long:
it is both project documentation and a record of an extended investigation.

The roughly four months associated with this project included an absence from
school of more than a month for health and personal reasons. Much of that
period was spent reading, watching algorithm explanations and working through
ideas away from school, with limited access to coding tools. The implementation
work was concentrated into a shorter period. This timeline reflects both my
circumstances and the breadth of the learning process, rather than four months
of continuous coding.

What interested me was the difference between sorting an ordinary array and
minimising instructions under push_swap's restricted operations. Rotating a
stack changes its orientation; reaching and moving an element has a cost in
the permitted instruction set. This does not invalidate ordinary sorting
theory, but it changes the cost model and the strategies worth considering.
The move-minimisation task can be viewed as **combinatorial optimisation**, or
as finding a shortest path through a graph of stack configurations.

In discussions with AI, I explored questions about search, lower bounds and
even NP-completeness. These were questions to investigate, not complexity
classifications I established. I had not found a standard treatment of this
exact eleven-operation problem comparable to the textbook treatment of familiar
sorting algorithms. That sense of unfamiliar territory encouraged me to
experiment. I am not claiming that no relevant papers exist, that the problem
forms a new branch of computer science, or that related stack-sorting and
permutation problems are unstudied.

An early starting point was
[Jamie Dawson's *Push_Swap: The least amount of moves with two stacks*](https://medium.com/@jamierobertdawson/push-swap-the-least-amount-of-moves-with-two-stacks-d1e76a71789a),
which helped me understand hard-coded small cases. For five elements, the
article describes moving the top two to B, sorting the remaining three, and
reinserting the two. The related article
[Ulysse Gerkens's *Push Swap in less than 4200 operations*](https://medium.com/@ulysse.gks/push-swap-in-less-than-4200-operations-c292f034f6c0)
links readers to Dawson for these fundamentals; they are by different authors.

I initially read the five-element construction as an optimal solution.
Discussion with AI helped me distinguish an optimal three-element subroutine
from an optimal complete five-element route: choosing which elements to push
and how to return them also matters. Sorting three takes zero moves when
already sorted and at most two otherwise, but that fact alone proves no
five-element optimum. This was a question raised by my reading, not an
optimality theorem claimed or proved by the article.

My original plan was to compare that five-element strategy against exhaustive
BFS across all 120 permutations, then use the exact answers to develop a
stronger hard-coded five-element solver. That ambition helped lead me to BFS
and the wider optimality questions below. I am not claiming that I completed
that specific comparative study: the current precomputed table covers 1–4
elements, while five elements use runtime BFS.

Gerkens jokes that push_swap began intruding into his dreams. I recognised
that rather literally: after long days of coding, I also found myself dreaming
about sorting strategies during sleep and naps. I remember dreaming of four
ideas, although only two stayed clear enough to revisit at school. As I recall,
these involved recursive Turk-style planning and an LDS-based alternative,
including changing which stack carried the ordered sequence. I experimented
with the remembered ideas, but they did not become final solver strategies.
Apparently, even my sleeping brain wanted another candidate algorithm.

The question that connected these explorations was simple:

> Given an initial stack A and an empty B, does a sequence of at most K permitted
> operations exist that leaves A ascending and B empty, counting each of the
> eleven instructions as one operation?

This is the **decision version** of finding a shortest solution. It captures a
question I had already been asking; AI helped put it into this explicit form.
It also identifies the exact problem for which I wanted to find a formal
treatment, rather than just another practical sorting strategy. Our limited
search did not identify an academic paper matching these precise rules and
objective; that does not establish that none exists or that the formulation
is original.

A shortest solution does exist for every valid finite input: for example,
repeatedly rotate the smallest remaining element of A to the top and push it
to B, then push everything back to A. This gives a finite legal solution, and
the nonempty set of attainable instruction counts has a minimum. The harder
question is **how to find that minimum, or decide whether it is at most K,
within an affordable amount of time and memory**.

Wanting an optimal answer led me to breadth-first search. Its shortest-path
guarantee under unit operation costs then led directly to the practical problem
of state-space growth, and from there to heuristics, lookahead and pruning.
Those experiments taught me to separate an exact answer, a safe bound and a
useful estimate. A valid solution within K operations answers yes; failing to
find one with a heuristic does not establish no.

This is the thread running through the investigation below. I would find a
careful study of this exact optimisation problem interesting, but this README
is my record of pursuing the question as a student, not a claim to a new research
result or a settled complexity classification.

My aim is to connect practical performance with careful reasoning: measure
complete solutions, understand time and memory costs, and establish guarantees
where the assumptions permit them. Exact small-state search and safe pruning
offer particular guarantees; a promising heuristic or a good benchmark result
answers a different question. Throughout this README, I try to distinguish
those forms of evidence rather than imply that every mathematical expression
proves an improvement.

This reflects my broader interest in low-level programming, interfaces,
high-performance systems and systems where correctness matters. Areas I would
like to explore include security-critical software, low-latency market-data
processing, and digital signal-processing pipelines close to hardware.
Push_swap is a learning exercise rather than evidence of readiness for those
domains, but it gives me a concrete setting in which to practise making costs,
invariants and trade-offs explicit.

The result is not a claim to the best push_swap implementation. It is a record
of what I built, tested, reconsidered and still want to understand. Readers
looking for the current implementation can begin with [At a glance](#at-a-glance);
readers interested in the investigation can use the [Contents](#contents) and
the [guide to mathematical claims and evidence](#reading-the-mathematics).

> Current implementation: small inputs use precomputed answers (1–4) or full-input BFS (5–10); five greedy candidates run through 500 values. Above 500, the existing nonrecursive three-element-seed greedy solver uses heap-backed circular buffers. Archived chunk and seed experiments live in `../backups/`. See [Seed candidate flow](#seed-candidate-flow) for current dispatch and settings. Dated experiments below retain their original configurations.

> Portability: With AI assistance, I fixed a bug in the bundled formatter's shared `va_list` handling that caused the decoded-move debug printer to crash on Apple Silicon. The best-solution scan now considers only generated solutions (`0` through `x->cur`). See the [library portability update](libft/1_ft_printf/README.md#post-submission-update-portable-variadic-argument-consumption) for details and validation. Three generated runs each at 2, 11, 100, and 500 values completed without a crash; sorting correctness and move-count compliance are separate checks.


<a id="at-a-glance"></a>

## At a glance

✅ = implemented. 🚧 = partial or experimental. ❌ = not met or not implemented.
These describe the current code and study tools; they are not evaluation scores.

I have completed the project and passed the listed
[pre-submission checks](#what-i-checked-before-submission). My final solver
combines exact small-input answers with greedy candidate comparison. The
documented trials support my choice of settings; the live evaluation determines
the final score. Earlier experiments, including the archived chunk solver,
remain here as a record of how I reached this implementation.

| Status | Feature | Current behavior |
|---|---|---|
| ✅ | Integer input | Validates signs, integer range and duplicates; supports grouped arguments, long leading zeroes and more than 500 values. |
| ✅ | Rank normalisation | Replaces each distinct value with its position in sorted order. |
| ✅ | Circular-buffer stacks | Uses inline arrays through 500 values and allocated arrays above 500, with wrapping indices. |
| ✅ | Operation implementations | Swap, push, rotate, reverse rotate and combined-operation functions are present, alongside BFS state transformations. |
| ✅ | BFS state indexing | Uses a Lehmer permutation rank plus the A/B split and a visited bitset. |
| 🚧 | Archived chunk solver | Rank-interval extraction and restricted BFS replay are preserved in `../backups/`; they are not in the active build. |
| ✅ | Candidate comparison | Five greedy strategies run through 500 values; inputs of at most 10 also receive an exact candidate. Above 500 uses the existing three-element-seed local candidate. The first shortest generated solution is printed. |
| ✅ | Study tools | Includes a permutation analyser, a reverse-BFS shortest-path analyser and an input generator. |
| ✅ | Saved study data | Reports and trial logs are preserved in Git under [`tests/debug/old_results/`](tests/debug/old_results/). |
| ✅ | Build organisation | Bundled libft, separate source/header directories, ignored build products and incremental builds. |
| ✅ | Subject move requirements | Three current 100-value and three current 500-value trials passed the highest move bands; exact counts and supporting checks are documented below. |
| ✅ | Clean instruction-only output | The active candidate solver prints the selected moves to stdout; debug diagnostics use stderr and descriptor 3. |
| ✅ | Bonus checker implementation | My two-file checker reuses the parser and stack operations; `make bonus` builds it. The supplied Linux checker remains a separate reference. |

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
| ✅ | [Pre-submission checks](#what-i-checked-before-submission) | The listed parser, memory, build, Norm and output checks passed; their scope is documented below. |

[↑ Back to top](#top)

<a id="contents"></a>

## Contents

- [Preface](#preface)
- [At a glance](#at-a-glance)
- [Design choices and edge cases](#design-choices-and-edge-cases)
- [How to read the mathematics and evidence](#reading-the-mathematics)
- [Description](#description)
- [Instructions](#instructions)
- [Rank normalisation](#rank-normalisation)
- [Hybrid storage and recursive malloc](#hybrid-storage-and-recursive-malloc)
- [LIS, LDS and the square-root guarantee](#lis-lds-guarantee)
- [Circular-buffer stacks](#circular-buffer-stacks)
- [Greedy lookahead: who owns each plan?](#greedy-lookahead-who-owns-each-plan)
- [Greedy heuristics and disorder measures](#greedy-heuristics)
- [Safe pruning bounds](#safe-pruning-bounds)
- [BFS and state indexing](#bfs-and-state-indexing)
- [Precomputed BFS tables, pages, heap and stack](#precomputed-bfs-tables)
- [Chunk extraction and the hidden stack](#chunk-extraction-and-the-hidden-stack)
- [Analysis tools and study data](#analysis-tools-and-study-data)
- [Checks and current limitations](#checks-and-current-limitations)
- [Resources and use of AI](#resources)
- [Future research — ideas not implemented](#future-research)

[↑ Back to top](#top)

<a id="reading-the-mathematics"></a>

## How to read the mathematics and evidence

Mathematical notation in this README serves several different purposes.
**An equation is not automatically an optimality proof or a performance bound.**
In particular, the lookahead/batching equations formalise the recursive
algorithms and my design rationale; they do not establish that eight-lookahead/
six-executed beats execute-one or full batching.

| Label | What the reader can conclude | What the reader cannot conclude |
| --- | --- | --- |
| **Definition / specification** | This states a score, metric or algorithm precisely | That the specified score predicts complete sorting cost well |
| **Structural identity / count** | The relationship follows from the stated mechanism and assumptions | That a larger or smaller value necessarily improves performance |
| **Conditional guarantee / bound** | A conclusion holds under the stated assumptions and for the stated objective | That it extends to a different objective, horizon or algorithm |
| **Empirical observation** | The reported inputs and settings produced the recorded result | That it generalises to other workloads |
| **Design rationale / hypothesis** | This is a reason to explore a choice or a possible explanation to test | That its benefit or causal explanation has been demonstrated |

For example, d-e counts the planned insertions left uncommitted; it is not a
bound on sorting error or wasted moves. By contrast, the one-push-per-element
lower bound genuinely supports safe pruning when compared against a budget
for the same objective. The conditional optimal-suffix argument is a valid
property of exact finite-horizon optimisation, but it does not rank policies
that repeatedly search different, moving horizons.

I worked through these explanatory formulations of existing mechanisms and standard
principles in discussion with AI. I use the lookahead/batching notation
to describe the algorithms, not to claim a newly discovered theorem or prove
that one is superior. Formalising a choice after experimentation can clarify what it
does and what must be tested; it does not retroactively prove why a trial won.

**Why did I pursue a choice without a guarantee?** A plausible mechanism, an
affordable computation budget and promising observations gave me a reason to experiment.
Here, partial commitment preserves part of a jointly evaluated plan while
allowing later reconsideration. That trade-off motivated me to test it; correctness
comes from valid stack operations and checking the result, while any claim of
better move counts needs comparative evidence. The rationale is worth testing
even if no setting can be shown to win universally.

<a id="description"></a>

## Description

Push_swap explores sorting with two stacks and a limited vocabulary of moves.
The intended result is an ascending stack A, an empty stack B, and a short list
of instructions that reproduces the sort.

This version is also a study of the search space: how to represent a state,
recognise a state already visited, recover a path, and use small optimal solutions
to investigate larger sorting strategies. The saved reports are reference material
for that investigation, including cases with several equally short solutions.

The main program dispatches inputs of 1–4 values to a precomputed exact table
and 5–10 values to full-input BFS. With the current skip flag set to zero, it
also runs the five greedy candidates; 11–500 values use those greedy candidates
alone. Above 500, it runs the existing three-element-seed local candidate. The first shortest generated solution wins. Archived chunk experiments
and dated benchmarks are distinguished from this active path below.

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
not guarantee a separate report. The ordinary executable's progress diagnostics follow `DEBUG` and use stderr;
the solution dump uses file descriptor 3.

`make clean` removes project and libft objects. `make fclean` also removes built
executables and the library archive. Both preserve study reports and the supplied
checker. Repeated `make` did not relink in my submission-only build checks,
as recorded under [pre-submission checks](#what-i-checked-before-submission).

### Run the development solver

```sh
./push_swap 3 2 1
./push_swap "3 2 1"
./bin/generator 5
```

Start with small inputs. BFS allocation now scales with the input's state count,
but factorial growth still makes the upper end expensive. Greedy lookahead can
also be expensive on large inputs. Stdout contains the selected instructions;
stderr carries progress, and descriptor 3 can capture the solution dump:

```sh
./push_swap 3 2 1 > moves.txt 2> progress.log 3> solutions.log
./checker_linux 3 2 1 < moves.txt
```

The supplied checker is a Linux executable. These commands illustrate validation,
not a claim that every input or submission requirement has passed.

### My GitHub tools and the 42 submission

I keep the development tools on [GitHub](https://github.com/CrispyNuggetD/42_Singapore_SUTD/tree/main/Core%20Curriculum/Projects/2_push_swap).
My local 42 submission repository contains `Makefile`, `README.md`, `src/`,
`includes/` and the bundled `libft/` sources and Makefile. I leave out `tests/`,
`.sh` and `.py` tools, the downloaded `checker_linux`, `.gitignore`, archived
experiments and generated executables, objects and reports. The subject asks
for the C sources, headers and Makefile, and separately requires the README;
it explicitly says development tests do not need to be submitted. This is my
submission layout, rather than a claim that every extra file is forbidden.

The normal and bonus builds do not need those development files.
`make debug`, `make analyse_bfs`, `make analyse_bfs_all_paths` and `make generator`
are GitHub-only development targets whose sources live in `tests/debug/`.
During evaluation I download the supplied Linux checker again as
`./checker_linux` and make it executable. I can also recreate `tests/debug/`
with `mkdir -p` for logs; creating that directory does not make it submission
source. The random runner writes new reports to `tests/debug/results/random_tests/`;
older studies live in `tests/debug/old_results/`.

[manual_eval.txt](tests/manual_eval.txt) is just one copy-pastable terminal
command per line, with no helper functions or scripted verdicts. It includes
100- and 500-value commands for my own manual evaluation. I also ran
[benchmark_500.sh](tests/benchmark_500.sh) as part of the checks below.
[push_swap_eval.txt](tests/push_swap_eval.txt)
is the separate, longer scripted checklist. Both use the current executable
and directory names.

### What I checked before submission

I checked the current layout with diagnostics disabled (`DEBUG = 0`).
`norminette src includes libft` passed. A temporary submission-only copy with
no test tools or downloaded checker built both programs, did not relink on
repeated `make`, preserved executables with `clean`, rebuilt with `re`, and
removed them with `fclean`.

The table below records the inputs I checked and their results. For valid unsorted cases I
saved the solver's moves and replayed them with both my checker and the supplied
Linux checker; stdout contained only moves and stderr was empty.

| Inputs checked | Result |
| --- | --- |
| No arguments; `42`; `0 1 2 3 4 5 6 7 8 9` | Silent success |
| `-2147483648 0 2147483647` | Silent sorted input; both checkers accepted the empty move stream |
| `2147483647 0 -2147483648` | Both checkers returned `OK`; Valgrind reported no errors or leaks |
| `2 1`; every permutation of `0 1 2` | Both checkers returned `OK`; at most three moves |
| `1 5 2 4 3`; every permutation of `0 1 2 3 4` | Both checkers returned `OK`; at most twelve moves; the first case also passed Valgrind |
| `0 one 2`; `1 2 1`; `2147483648`; `-2147483649`; `""`; `--1`; `-0 +0`; `"1-2"` | Both programs printed only `Error` on stderr |
| `1x`; `1 1` under Valgrind | Both programs rejected them without memory errors or leaks |
| Three seeded random 100-value inputs, seed `42` through `push_swap_tester.sh` | Reference checker `OK`; 496, 507 and 511 moves, all below 700 |
| Three random 500-value inputs through `tests/benchmark_500.sh` | Both checkers `OK`; 4811, 4943 and 4969 moves, all below 5500; successful solver exits and empty stderr |

The valid-case suite covered 135 runs, including all three- and five-value
permutations. The error suite covered 16 program/input combinations; eight
Valgrind runs and 18 injected allocation failures with ASan/UBSan also passed.
The precomputed-table test passed all 33 permutations through size four,
including wrapped buffers and a five-value BFS fallback. These results apply
to the tested inputs; they do not imply that every permutation was checked
for larger sizes.

The 500-value script passed all three trials. I kept the exact inputs, moves
and report in `tests/debug/results/benchmark_500.ulrwcP/`. The report identifies
commit `9214973` and records the executable hashes:

```text
push_swap: 58a4c40fc5fbef76caa6322d898d96da8f7fdbe2a08fc37f59fb72fde6f0c939
checker:   0ee13489947755bfb22f6effa4c5b3ab57b6ac0bfdb7ee67a6576ca2367ab620
```

The three counts average 4907.67 moves; the largest, 4969, is below the strict
5500 boundary. These were sorting and move-count checks, not additional
500-value Valgrind runs. I rely on the separate memory checks listed above for
evidence about memory safety. Generated benchmark reports stay local and are
ignored by Git.

#### Runtime and test-machine context

The 500-value shell script saved moves, not explicit timing fields. I estimated
solver durations from each move file's creation time to its last modification:
the file is created before launching the solver and receives the answer near
completion. These are approximate wall times, not CPU-time measurements.

| 500-value trial | Moves | Approximate solver seconds | Approximate duration |
| --- | ---: | ---: | --- |
| 1 | 4811 | 163.79 | 2 min 44 sec |
| 2 | 4943 | 224.02 | 3 min 44 sec |
| 3 | 4969 | 247.50 | 4 min 7 sec |
| Mean | 4907.67 | 211.77 | 3 min 32 sec |

From the benchmark directory's creation to the report's last update, the whole
three-trial test took about 635.36 seconds (10 min 35 sec), including the small
checker/report overhead after the directory was created. The build ran before
that directory was created and is not included in this estimate.

The earlier Python runner did record total solver wall time for the three
100-value trials: 85.153702, 82.470882 and 80.064915 seconds, averaging
82.563166 seconds (about 1 min 23 sec). Those records are in the local session
`tests/debug/results/random_tests/20261002_033534_output/`. It did not record
individual candidate timings. Older 10-value logs exist, including a historical
56-run session averaging 48.015 seconds, but those belong to an earlier
executable/configuration and are not current-version timing evidence. The
candidate regression checks through size 10 did not save timing measurements.

The test PC reports the following hardware and software:

| Item | Test environment |
| --- | --- |
| CPU | Intel Core i7-12700, 12th generation; 12 cores and 20 logical CPUs |
| Reported CPU frequency range | 800–4900 MHz; actual frequencies during the tests were not logged |
| Memory | 15.31 GiB usable RAM reported by Linux; nominal 16 GB class |
| OS | Ubuntu 22.04.5 LTS, x86_64; Linux 5.15.0-190-generic |
| Compiler | Ubuntu Clang 12.0.1 (`cc`) |
| Build flags | `-Wall -Wextra -Werror`; no explicit optimization flag |
| Python | 3.10.12 |
| Solver diagnostics | `DEBUG=0` for the documented benchmark runs |

The solver creates no worker threads and runs its candidates sequentially.
A solver run therefore uses one execution thread rather than all 20 logical
CPUs. The OS may move that thread between cores; CPU affinity, background load,
CPU frequencies and peak RAM usage were not recorded for these older runs.
Installed RAM is capacity, not measured solver consumption. The checker runs
after the solver; parallel compilation is separate from sorting runtime.
These details make the times useful as a rough replication reference, not a
promise of identical runtime on another machine.

My Python runner records candidate wall and CPU times plus a
process-wide peak RSS sample at candidate completion, without changing the
submission executable. This peak can include earlier candidates' memory usage;
it is not memory owned by the named candidate. To collect new comparable
measurements, run `bash push_swap_tester.sh -n 3 --size 100 --seed 42` or use
`--size 500` for a new 500-value session. The original timing fields cannot be
retroactively split into per-algorithm durations.

I passed the listed pre-submission checks, including the build, Norm,
small-input, error and memory checks above. The tested 100- and 500-value
inputs met the highest move bands. The local subject allows at most 5500
moves; the evaluation mirror used by my checklist says fewer than 5500,
and all three 500-value trials meet that stricter boundary. These results
document what I tested; the live evaluation determines the mandatory score
and bonus eligibility.

The final header settings are my best-supported tested compromise between
move count, runtime and memory use within my available computational resources.
I reached them through a variety of inputs and repeated trials, rather than
choosing a depth after one successful run. The approximately three-and-a-half-minute
average for the three documented 500-value trials is a runtime I consider
practical for my 42 evaluation, with acceptable move counts. By “current best”,
I mean the overall trade-off I have had the resources to test and substantiate,
not a proven optimum, an inherent limit of the algorithm, or the fastest
possible configuration. The [configuration table](#seed-candidate-flow)
records those final settings.

#### What the first profiled 100-value session showed

I stopped the profiled session `20261002_043555_output` after 24 successful
100-value runs. All winning streams passed the reference checker. Winners
averaged 499.29 moves, ranging from 456 to 525. The saved seed was
`2459232882432727273`, with `DEBUG=0`, continuation depth 12 and execution
limit 10, and opening depth 12 with batch execution limit 5.

| Candidate | Mean moves | Mean wall time | Mean CPU time | Chosen wins |
| --- | ---: | ---: | ---: | ---: |
| Three-element seed + local greedy | 573.88 | 0.000618 s | 0.000616 s | 0 |
| Circular LIS + local greedy | 539.13 | 0.004047 s | 0.004044 s | 1 |
| Circular LIS + lookahead | 509.42 | 29.737392 s | 29.735572 s | 13 |
| Circular LIS + opening-one lookahead | 510.88 | 34.277833 s | 34.275884 s | 5 |
| Circular LIS + opening-batch lookahead | 521.21 | 36.103273 s | 36.101130 s | 5 |

This helped me see the tradeoff more clearly than the earlier total-runtime measurements.
The plain LIS local candidate averaged about 40 more moves than the winning
answer, but took roughly four milliseconds. Every candidate's move count stayed
below 700 in this sample. Ordinary LIS lookahead had the lowest average move
count among individual candidates and won most often. Opening-batch was worse
on average than the other lookahead variants here, but still won five inputs,
so its contribution cannot be judged from its mean alone. These observations
do not establish the same ranking for 500 values, and I have not changed the
algorithm settings based on them.

Average total solver wall time was 100.125525 seconds. Subtracting the candidate
wall times left about 0.002364 seconds per run for launch, parsing,
initialization, instrumentation output and final output combined. CPU times
closely matched wall times. Almost all measured solver runtime was therefore
spent computing the lookahead candidates. This difference is not a measurement
of all Python overhead: input generation, reference checking and report writing
between solver invocations are outside the solver runtime field.

The ordinary submission executable does not contain the FD 4 profiler; the
Python runner separately links its temporary instrumented executable.
With `DEBUG=0`, existing FD 3 solution diagnostics are suppressed, while the
runner explicitly opens FD 4 for profiling records. These are local file
descriptors, not network destinations. With `DEBUG=1`, FD 3 is normally closed
unless I redirect it, so those writes fail and the dump is lost; stderr progress
still appears. If the evaluator agrees to diagnostic output, I can use:

```sh
./push_swap 3 2 1 3>&2
./push_swap 3 2 1 3>debug
```

The first sends the solution dump to stderr, normally the terminal; the second
saves it to a file. The filename does not need a `.txt` extension. Both keep
stdout reserved for instructions. Diagnostic output can affect runtime, so the
measurements above use `DEBUG=0`.

### My bonus checker

I ended up needing only two bonus `.c` files: [main_checker_bonus.c](src/bonus/main_checker_bonus.c)
and [checker_util_bonus.c](src/bonus/checker_util_bonus.c). I keep the reader
and move dispatcher together in the utility file. Most of the machinery
was already there. I needed to read and execute someone else's moves, then
check the result; I did not need another sorting algorithm.

```sh
make bonus
./push_swap 3 1 2 | ./checker 3 1 2
printf 'sa\n' | ./checker 2 1
```

I start with the same input in A and an empty B. `parse_input()` validates the
integers, checks overflow and duplicates, and replaces the values with ranks.
Ranking preserves their order, so it works for checking as well as sorting.
The first argument remains the top of A. With no arguments, I exit silently
without reading stdin.

My reader applies each instruction as it arrives, in its original order.
Once it reaches EOF, I print `OK` if A is sorted and B is empty, or `KO` if the
valid moves did not finish the job. Invalid arguments, malformed instructions,
or reader failures produce `Error` on stderr instead. A failed sort is still
a valid execution, so `KO` uses the normal success exit status.
Even if the input starts sorted, I still read the moves: they could scramble
it, leave something in B, or contain an error.

The move dispatcher matches the whole instruction, including its newline and
terminator. I reject extra spaces, unknown names and a last instruction with
no newline. Two function-pointer tables handle the existing signatures:
one-stack operations such as `sa`, and two-stack operations such as `ss`.
The matching index chooses the function, and I select A or B for the
one-stack calls. I pass `NULL` as the solution pointer because
`append_move_to_soln()` already treats that as “execute without recording”.
That small convention saved me from duplicating all eleven operations.

I did need to handle one difference: my `pa()` and `pb()` wrappers report an
error if they pop an empty source. In my checker, those are valid no-ops. I check
B before `pa` and A before `pb`, returning success immediately when the source
is empty. Swaps and rotations already handle stacks with fewer than two values.

The circular-buffer helpers let me reuse both inline and heap storage.
`ranks_are_sorted()` now lives in a shared utility file and checks that logical
position `i` contains rank `i`. I also require B to be empty before accepting
the result. Both optional heap buffers are freed after execution or failure.

#### Why my status-returning GNL helped

My custom `ryker_ft_get_next_line()` returns `GNL_LINE`, `GNL_EOF`, or
`GNL_ERROR`, separately from the returned line pointer. That distinction turned
out to be useful here. `GNL_LINE` means I have a move to validate and execute;
`GNL_EOF` means all supplied moves have been read and, because I apply them
immediately, executed. `GNL_ERROR` means I cannot trust that I read the complete
stream, so I report an error instead of checking a potentially partial result.

A straightforward loop around ordinary GNL often looks like this:

```c
line = get_next_line(STDIN_FILENO);
while (line)
{
    /* Validate and apply the move, then free the line. */
    line = get_next_line(STDIN_FILENO);
}
/* Check the final stacks. */
```

The catch is that ordinary GNL returns `NULL` for both EOF and errors.
That loop alone cannot distinguish “finished reading” from “reading failed”.
For example, if a read or allocation fails after a prefix of moves has sorted
A, it could incorrectly print `OK` while unread moves remain. Ordinary GNL can
still be used with an additional reliable error-reporting mechanism; a bare
`NULL` check is what loses the information. My custom result enum makes the
choice explicit, and its cleanup function lets me free any buffered remainder
when I stop early on an invalid instruction. I also free every returned line.
EOF is the end signal, not an empty stack or a special move: a pipe supplies it
when its writer closes, and an interactive run needs EOF from the terminal.

I wrote the checker logic after discussing the design with Codex. Codex supplied
the initial skeleton and build rules, explained function pointers, reviewed my
implementation, helped with formatting and comments, and moved the existing
rank check into shared code. It also ran 107 functional checks, including 84
random instruction-stream comparisons with the reference checker, plus four
Valgrind checks covering valid execution, invalid moves, invalid arguments and
heap storage. Those checks passed, as did the bonus build and Norm checks.
These checks give me evidence about the cases tested; they do not mean I have
tested every possible input. Bonus assessment still depends on the mandatory part
meeting all required benchmarks at the highest score.

### Accepted input formats

My parser (`src/parsing_and_ranking_input/parse_sort_input.c`) reads arguments
from left to right and can read several space-separated integers from each argument. These forms
therefore describe the same initial stack, with 3 at the top:

| Form | Example |
| --- | --- |
| Separate arguments | `./push_swap 3 1 2` |
| One double-quoted list | `./push_swap "3 1 2"` |
| One single-quoted list | `./push_swap '3 1 2'` |
| Mixed grouped and separate arguments | `./push_swap "3 1" 2` |
| Leading, repeated and trailing spaces inside a group | `./push_swap "  3   1  2  "` |

The shell removes the surrounding quotes before my program receives the
argument. Quotes group the input; they are not characters that my parser needs
to strip. I also accept an optional `+` or `-` directly before the digits,
and leading zeroes:

```sh
./push_swap +3 -1 02
```

Values must be distinct after conversion: `2`, `+2` and `02` represent the
same integer, so using more than one of them is a duplicate. My intended range
is signed 32-bit integers on my 42 machine. There is no arbitrary 500-value
input cap. The parser counts tokens before allocating and checks numerical
range rather than digit count, so long strings of leading zeroes are accepted.
Input size is still constrained by available memory, the shell/OS argument
limit, and the implementation's `int` indices (`count + 1` must fit).

Inside a quoted argument, the supported separator is an ordinary ASCII space,
not general whitespace. Do not use commas, tabs, newlines, decimal points or
bracketed list notation. Empty arguments, space-only arguments and signs
without digits are not valid input. Integer-boundary, malformed-input and allocation-failure cases are covered by
the hybrid-storage checks below; those checks are not a complete submission audit.

The bundled Linux checker accepted the grouped and mixed forms in my
AI-assisted checks as well. For a quoted list, pass the same input to both
programs:

```sh
./push_swap "3 1 2" > moves.txt 2> progress.log 3> solutions.log
./checker_linux "3 1 2" < moves.txt
```

For shell-variable input, quoting preserves the whole list as one argument:

```sh
ARG="3 1 2"
./push_swap "$ARG" > moves.txt 2> progress.log 3> solutions.log
./checker_linux "$ARG" < moves.txt
```

### Project layout

| Path | Purpose |
|---|---|
| [`src/`](src/) | Solver, parsing, stack operations and BFS implementation. |
| [`includes/push_swap.h`](includes/push_swap.h) | Shared structures, limits and function declarations. |
| [`libft/`](libft/) | Self-contained library sources used by this project. |
| [`tests/debug/`](tests/debug/) | Analysis and generator sources. |
| [`tests/debug/old_results/`](tests/debug/old_results/) | Study reports and trial logs, kept in Git. |
| [`checker_linux`](checker_linux) | Supplied Linux checker binary. |
| [`notes.md`](../backups/notes.md) | Working questions, ideas and unfinished plans. |
| `obj/`, `bin/`, `push_swap` | Generated build products, ignored by Git. |

The old `DO_NOT_SUBMIT_DEBUG_hidden_bfs.c` now lives in `../backups/` and is not
a dependency of the main build.

[↑ Back to top](#top)

<a id="rank-normalisation"></a>

## Rank normalisation

[`rank_values.c`](src/parsing_and_ranking_input/rank_values.c) counts how many input values are smaller
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

<a id="lis-lds-guarantee"></a>

## LIS, LDS and the square-root guarantee

A subsequence can skip intervening numbers, but keeps their original relative
order. So in `1 8 2 9 3 4`, I can keep `1 2 3 4` as an increasing subsequence.
LIS means longest increasing subsequence; LDS means longest decreasing subsequence.

The **Erdős–Szekeres theorem** says that, for positive integers $r$ and $s$, any
sequence of distinct numbers with length

```math
n \geq (r - 1)(s - 1) + 1
```

contains an increasing subsequence of length at least $r$, **or** a decreasing
subsequence of length at least $s$.
See [the theorem statement in this research paper](https://www.sciencedirect.com/science/article/am/pii/S0195669821001505).

Taking equal thresholds gives the square-root guarantee:

```math
\max\bigl(\mathrm{LIS}(A),\mathrm{LDS}(A)\bigr)
\geq \left\lceil\sqrt{n}\right\rceil.
```

For my 500-number input:

```math
\begin{gathered}
(23 - 1)^2 + 1 = 485 \leq 500 \\
\left\lceil\sqrt{500}\right\rceil = 23.
\end{gathered}
```

So there must be an LIS **or** LDS of at least 23 elements. The catch is
that I don't get to choose which one the theorem guarantees. A completely
descending input has

```math
\begin{gathered}
\mathrm{LIS}(A) = 1 \\
\mathrm{LDS}(A) = 500.
\end{gathered}
```

My implemented circular-LIS preparation keeps an increasing subsequence in A
and pushes the rest to B before greedy reinsertion. This theorem alone doesn't
guarantee that I can keep 23 elements in A: the long subsequence might be
decreasing. It also doesn't promise a particular push_swap move count.

### What LIS length should I expect from random inputs?

For a uniformly random permutation of $n$ distinct ranks, the expected ordinary
LIS length $L_n$ has the asymptotic estimate

```math
\mathbb{E}[L_n] \sim 2\sqrt{n}
\qquad (n \to \infty).
```

| Input size | Leading-order estimate $2\sqrt{n}$ |
| --- | ---: |
| 100 | 20 |
| 500 | 44.7 |

So this gives me a baseline for random testing, not a minimum seed size or an
exact average at 100 or 500 elements. A reverse-sorted input still has ordinary
LIS length 1. See this [research paper discussing the expected-length result](https://math-faculty.net.technion.ac.il/files/2021/05/Law-of-large-numbers-for-increasing-subsequences-of-random-permutations.pdf).

My circular LIS searches all rotations, so its length is at least the ordinary
LIS length for the same input. It may keep more elements, but this formula is
for ordinary LIS; I should measure circular seed lengths rather than treat
$2\sqrt{n}$ as their expected value. Neither estimate guarantees the final move
count, since rotations and reinsertion choices still matter.

### Why keep the longest seed?

What makes a useful seed for my reinsertion algo? Once I've pushed the other
elements to B, the elements kept in A must be circularly ascending. An increasing
subsequence gives me that property. It doesn't need consecutive ranks or a `0`
at the start: greedy reinsertion can fill the gaps later.

Suppose I start with $n$ elements in A and empty B, and keep a seed $S$ of
length $k$. If I push each remaining element to B exactly once, then return
each to A exactly once, the push count is

```math
P(S) = \underbrace{(n-k)}_{\mathrm{pb}}
     + \underbrace{(n-k)}_{\mathrm{pa}}
     = 2(n-k).
```

So every extra element I keep saves exactly two pushes under this strategy:

```math
P(k+1)-P(k)=-2.
```

That's my mathematical reason for starting with LIS: among ordinary increasing
subsequence seeds, choosing the longest one minimises this push count. It is
not a lower bound for every possible push_swap algorithm; other strategies
can use swaps or different transfers.

**Preliminary saving prediction versus a 3-element seed.** Before measuring
results, I can predict the push component. Assuming each non-seed element is
pushed out and back exactly once, keeping $k$ elements instead of 3 saves

```math
\Delta P = 2(n-3)-2(n-k)=2(k-3).
```

Using the ordinary random-LIS estimate $\mathbb{E}[L_n]\sim2\sqrt{n}$ gives a
rough leading-order prediction:

```math
\mathbb{E}[\Delta P] \approx 4\sqrt{n}-6.
```

| Input size | Estimated ordinary LIS length | Predicted pushes saved versus keeping 3 |
| --- | ---: | ---: |
| 100 | 20 | 34 |
| 500 | 44.7 | 83.4 (about 84 if the seed has 45 elements) |

These are preliminary estimates for uniformly random permutations, not test
results or a prediction of the complete solution's move count. Circular LIS may
retain more elements; once I measure its actual length $k$, $2(k-3)$ gives the
exact push difference under the assumptions above. Preparation, rotations and
swaps can increase or decrease the total saving. Measured seed comparisons appear in the dated discussion below; these push-only
estimates do not assume that other costs stay unchanged.

For example, both seeds below have length 9 and require just two pushes:

```text
Input: 0 1 2 8 4 9 10 11 12 13
Keep:  0 1 2 8   9 10 11 12 13   -> move 4 out and back
Keep:  0 1 2   4 9 10 11 12 13   -> move 8 out and back
```

The catch is rotations. If $R(S)$ counts the rotation instructions for
extraction, reinsertion and final alignment (with `rr` or `rrr` counting as one
instruction), this push-and-rotate strategy has total cost

```math
M(S)=2(n-|S|)+R(S).
```

A longer seed reduces the first term, but can change the second. So LIS is a
justified starting heuristic, not proof of the fewest total moves. The current implementation keeps the first longest seed it finds; comparing
rotation costs between equally long seeds remains a possible extension. Circular seed selection can also retain more than an
ordinary LIS: `8 9 0 1 2` is already circularly ascending as a whole.

[↑ Back to top](#top)

<a id="circular-buffer-stacks"></a>

## Circular-buffer stacks

Each stack has an integer array, a capacity, a read index and a write index.
The indices wrap around the array. One spare slot distinguishes a full buffer
from an empty one, so `n` input values use a capacity of `n + 1`.

The circular-buffer helpers implement the underlying movements. The
`stack_operation_*.c` wrappers also append an encoded move to the solution.
The output helper translates those codes back to names such as `sa`, `pb` and
`rra`, one instruction per line. Progress diagnostics use stderr; the detailed
solution dump uses descriptor 3. Neither is part of stdout's instruction stream.

[↑ Back to top](#top)

<a id="bfs-and-state-indexing"></a>

## BFS and state indexing

### Lehmer ranking: give every state an exact address

**Lehmer ranking replaces searching through previously discovered states with
calculating a unique integer ID and checking one bit.** It identifies a
permutation exactly; it does not estimate its distance from sorted order.

The implementation is
[bfs_optimiser_lehmer_rank.c](src/sorting_and_algorithms/brute_force_BFS/bfs_optimiser_lehmer_rank.c).
A state stores A followed by B, both top to bottom, with a split recording the
number of elements in A. The BFS representation uses distinct small local ranks
$0,\ldots,n-1$.

| Component | Example |
| --- | --- |
| A, top first | 2, 0 |
| B, top first | 3, 1 |
| Combined permutation $p$ | $(2,0,3,1)$ |
| Split (number of elements in A) | 2 |

### Why factorials appear

There are $n!$ permutations of $n$ distinct values. In lexicographic order, each
possible first value heads a block of $(n-1)!$ permutations. After fixing it,
each possible second value heads a block of $(n-2)!$, and so on.

Factorials are **block sizes used to calculate an address**. The calculation
does not generate every permutation.

For each position $i$, count the smaller values to its right:

```math
\begin{gathered}
c_{i}=\#\lbrace j:i\lt j\lt n,\ p_{j}\lt p_{i}\rbrace \\
0\le c_{i}\le n-1-i.
\end{gathered}
```

Here, `#` means the number of elements in the set (its cardinality).

These digits form the **Lehmer code**. Their factorial-weighted sum gives the
zero-based permutation rank:

```math
\begin{gathered}
R(p)=\sum_{i=0}^{n-1}c_{i}(n-1-i)! \\
0\le R(p)\lt n!.
\end{gathered}
```

This is a variable-base representation: unlike decimal digits, the allowed digit
range shrinks at each position. Remember $0!=1$; the last digit is always zero.

For $p=(2,0,3,1)$:

| Position | Value | Smaller values to the right | Digit | Weight | Contribution |
| ---: | ---: | --- | ---: | ---: | ---: |
| 0 | 2 | 0, 1 | 2 | $3!=6$ | 12 |
| 1 | 0 | None | 0 | $2!=2$ | 0 |
| 2 | 3 | 1 | 1 | $1!=1$ | 1 |
| 3 | 1 | None | 0 | $0!=1$ | 0 |

```math
\begin{aligned}
R(2,0,3,1)&=2\cdot3!+0\cdot2! \\
&\quad+1\cdot1!+0\cdot0! \\
&=13.
\end{aligned}
```

There are 12 permutations beginning with 0 or 1, plus one earlier permutation
within the chosen prefix: $(2,0,1,3)$. Thus $(2,0,3,1)$ has rank 13.

### Why permutations cannot collide

The factorial blocks do not overlap. To recover the permutation from its code,
start with the sorted unused values, select the value at zero-based index $c_i$,
and remove it:

| Digit | Unused values before selection | Selected value |
| ---: | --- | ---: |
| 2 | 0, 1, 2, 3 | 2 |
| 0 | 0, 1, 3 | 0 |
| 1 | 1, 3 | 3 |
| 0 | 1 | 1 |

This uniquely recovers $(2,0,3,1)$. Ranking is reversible and collision-free for
valid permutations. It is not a hash with possible collisions. The implementation
only needs the ranking direction, not decoding.

### Include the stack split

The same permutation with a different split represents different stacks.
Reserve a block of $n!$ IDs for each split:

```math
\begin{gathered}
\mathrm{ID}(p,s)=s\,n!+R(p) \\
s\in\{0,\ldots,n\}.
\end{gathered}
```

For our example:

```math
\begin{gathered}
n=4,\quad s=2,\quad R=13 \\
\mathrm{ID}=2\cdot24+13=\boxed{61}.
\end{gathered}
```

| Split | Meaning | ID range for $n=4$ |
| ---: | --- | --- |
| 0 | A empty | 0–23 |
| 1 | One element in A | 24–47 |
| 2 | Two elements in A | 48–71 |
| 3 | Three elements in A | 72–95 |
| 4 | B empty | 96–119 |

There are $n+1$ possible splits, so the number of encodable states is:

```math
\begin{gathered}
N=(n+1)n!=(n+1)! \\
0\le\mathrm{ID}\lt N.
\end{gathered}
```

Equivalently, choose which $s$ elements go into A, then order both stacks:

```math
\begin{aligned}
N&=\sum_{s=0}^{n}\binom{n}{s}s!(n-s)! \\
 &=\sum_{s=0}^{n}n! \\
 &=(n+1)!.
\end{aligned}
```

This counts all encodable states; BFS stops when it finds the goal and need not
visit them all.

### From an ID to a visited bit

The BFS stores one visited bit per ID:

```math
\begin{gathered}
\text{byte index}=\left\lfloor\frac{\mathrm{ID}}8\right\rfloor \\
\text{bit offset}=\mathrm{ID}\bmod8.
\end{gathered}
```

For ID 61, that is byte 7, bit 5 (both zero-based).

> **AI-generated diagram:** The Mermaid diagram below was generated by ChatGPT/Codex; I did not draw or write the diagram myself. I worked through this project's algorithm design and process together with AI in earlier discussions. The diagram documents that implementation; it does not imply that I invented Lehmer ranking. See [Use of AI](#use-of-ai) for the scope of AI assistance, including debugging and diagnostic printing tools.

```mermaid
flowchart TD
    S["Candidate: A = [2, 0], B = [3, 1]"] --> P["Permutation [2, 0, 3, 1]: rank 13"]
    S --> T["Split = 2"]
    P --> I["ID = 2 × 24 + 13 = 61"]
    T --> I
    I --> V{"Visited byte 7, bit 5 set?"}
    V -->|"Yes"| X["Skip duplicate"]
    V -->|"No"| Q["Set bit and enqueue with parent and move"]
```

The bit operations used in the BFS are:

```c
/* Check whether this state was already discovered. */
visited[state_id / 8] & (1 << (state_id % 8))

/* Mark it as discovered. */
visited[state_id / 8] |= (1 << (state_id % 8));
```

With unit-cost moves, BFS first discovers a state at minimum depth within its
explored graph. Later routes to the identical state need not enqueue it again:
the available continuations depend on that state, not the route taken to it.
Each discovered node records its parent and incoming move so the final path
can be reconstructed backwards.

### What the optimization saves

The original `same_brute_state` and `brute_state_exists` helpers are preserved
as commented code at the bottom of
[`bfs_verify_node.c`](src/sorting_and_algorithms/brute_force_BFS/bfs_verify_node.c).
That approach still works; it just gets slower as more states are discovered.
If Lehmer indexing feels unfamiliar, read those helpers first to see what it
replaces: scanning earlier nodes and comparing their split and array values.
They are kept for reference and are not compiled or called by the current BFS.

The old duplicate check scanned previously discovered nodes and compared their
arrays. With $V$ stored states and $n$ values, this takes up to $O(Vn)$ work per
candidate. The current nested-loop ranking performs exactly

```math
\frac{n(n-1)}2
```

value comparisons, then one bit lookup.

| Method | Worst-case duplicate-check work per candidate |
| --- | --- |
| Scan previously discovered states | $O(Vn)$ |
| Current Lehmer ranking plus visited bit | $O(n^2)+O(1)$ |

For $n=10$, ranking makes 45 value comparisons regardless of whether BFS has
discovered 100 states or one million. Only the bit lookup is $O(1)$; the complete
rank-and-check operation is $O(n^2)$.

The implementation precomputes factorials in its lookup table. It does not
enumerate $n!$ permutations to compute a rank. The speedup comes from eliminating
the growing scan of stored states. This is separate from greedy branch-and-bound
pruning, which skips branches based on cost.

### Limits: indexing does not remove factorial growth

The visited bitset requires

```math
\left\lceil\frac{(n+1)!}{8}\right\rceil
```

bytes. With the configured maximum of 10 elements, $11!=39,916,800$ states need
4,989,600 bytes (about 4.76 MiB) for visited bits alone. The node array containing
states, parents and moves needs additional, much larger storage. The current
BFS sizes its node array and visited table using `bfs_possible_states(n)` for
the current input, rather than allocating the ten-element maximum on every call. Increasing
the limit also requires checking integer ranges and representation limits.

**Current full-input BFS:** `solve()` checks that B is empty at entry and that
count matches A. For 5–10 values it invokes BFS over all eleven operations,
without the archived hidden-stack restrictions. For 1–4 it uses the precomputed
table. Both produce shortest answers under unit instruction costs; the first
goal reached by FIFO BFS supplies the runtime path.

The BFS helper itself still relies on its caller's empty-B and normalised-rank
contract. A ten-element A above nonempty B in a larger problem is not a supported
entry. The old restricted chunk/seed experiments in `../backups/` are separate
from this active full-input implementation.

[↑ Back to top](#top)

<a id="precomputed-bfs-tables"></a>

## Precomputed BFS tables, pages, heap and stack

My study idea, now demonstrated by the n=1..4 sample below, was to generate shortest
solutions offline with reverse BFS, then use Lehmer ranks to look them up at runtime.
The runtime solver would no longer need the BFS queue and visited table, but
generating the data still needs search time and memory.

### Three ways to store the answers

Let `n` be the number of values, `m` the solution length, and `L` the average
stored solution length. My current nested-loop Lehmer ranking takes O(n²).
The times below cover table indexing and traversal, excluding input parsing,
initial value normalisation, executing stack operations and printing output.

| Technique | Runtime traversal | Approximate table storage for fixed n |
| --- | --- | --- |
| Full packed solution per initial permutation | Rank once, locate the sequence, read m moves: O(n² + m) | n! × L / 2 bytes, plus offsets/lengths or padding |
| Next move only per state | Read a move, apply it, recompute the state ID: O(m × n²) | (n + 1)! / 2 bytes |
| Next move plus next-state ID | Rank once, follow stored indices: O(n² + m) | 4.5 × (n + 1)! bytes with 32-bit IDs and separately packed moves |

The full-solution table only needs the `n!` initial permutations with B empty.
The other two dense tables cover all A/B splits: `(n + 1) × n!` states. After
`pb`, a table covering only initial states would no longer be enough. Reverse
BFS must store a move towards the goal, reducing the remaining distance by one;
reaching the goal ends traversal.

Index hopping is O(1) per move, but recomputing my Lehmer rank is not. Full
solutions can also have better cache locality because their moves are adjacent.
Saving only the next move trades repeated ranking work for compact storage;
adding next-state IDs can cost more memory than storing complete initial-state
solutions. The 4.5-byte estimate assumes IDs fit in 32 bits and separate arrays;
a C struct may add padding. Packing the move and ID together is another option
when their combined bit widths fit the chosen integer type.

For example, at `n = 8`, **assuming** an average full solution of 20 moves:

| Representation | Approximate size |
| --- | ---: |
| Full solutions | 394 KiB, before offsets/lengths or padding |
| Next move only, all splits | 177 KiB |
| Next move + 32-bit next-state ID, all splits | 1.56 MiB |

That average is illustrative, not a measured BFS result.

### Two moves per byte, no homemade WinRAR needed

There are 11 operations, so four bits are enough for one operation. Assigning
codes `1..11` leaves `0` available as an end marker. With the first move in the
high nibble, moves `1, 2, 3, 4, 5, END` become bytes `0x12, 0x34, 0x50`.
Extract move `i` with `(data[i / 2] >> ((1 - i % 2) * 4)) & 15`.
Terminator codes and per-entry alignment add overhead to full-solution storage;
alternatively store lengths. Packed bytes can contain zero, so `strlen` cannot
measure this data.

Even at half a byte per state, factorial growth catches up quickly:

| n | Next-move-only table, all A/B splits |
| ---: | ---: |
| 8 | 177 KiB |
| 9 | 1.73 MiB |
| 10 | 19.0 MiB |
| 11 | 228 MiB |
| 12 | 2.90 GiB |

These are packed data sizes, not C source sizes or total process memory.

### What I learnt about pages, heap and stack

Calling this table "ROM" was misleading. On a typical Linux build, a
`static const` array can live in the executable's read-only data section
(`.rodata`). That is ordinary memory with write protection, not hardware ROM.
`const` alone does not determine placement: a function-local static table has
static storage duration; a normal automatic local array typically uses the
stack, and a `malloc` allocation uses the heap. The C language does not require
these exact OS-level placements.

| Resource | What a hypothetical 10 GB read-only table means |
| --- | --- |
| Disk | The executable contains roughly that much table data |
| Virtual address space | The loader maps an address range for the table |
| Physical RAM | Accessed pages become resident on demand; the entire table need not be resident |
| Stack / heap | No separate full-table copy is needed unless the program explicitly makes one |

If a mapped page is not resident, accessing it causes a page fault. Linux can
bring it in from the executable, or use an already cached copy, then resume the
program. Pages are commonly 4 KiB, though this depends on the system. Linux may
also read ahead. Under memory pressure, unchanged file-backed pages can be
discarded and reloaded from the executable instead of being written to swap.

For a **full-solution** lookup, one short sequence might fit in one page. That
does not mean the whole program needs only 4 KiB: the offset lookup may touch
another page, a sequence can cross a boundary, and code, stacks, buffers and
other data also need memory. The **next-move** techniques can touch a different
page after every move. Scattered uncached accesses can make disk latency matter
far more than the few instructions needed to decode a move.

So a 10 GB executable does not automatically need 10 GB of RAM on a 64-bit
system. Practical obstacles include compiler memory/time for huge initializers,
toolchain addressing and object-size limits, virtual address space, disk and
repository limits, and generating the BFS data in the first place. C does not
universally guarantee support for an object that large. A 32-bit process would
not have enough address space to map the entire 10 GB table at once.

### Authorship and AI assistance for precomputed solutions

I (hnah) proposed the precomputed-solution design, the function prototypes and
responsibilities for `encoded_bfs_data`, `decode_bfs_data` and
`get_precomputed_bfs`, the encoding/decoding mechanism, and how the lookup
would integrate with my existing solution storage and solver. I also proposed
compressing two moves into one byte (`unsigned char`), using four bits per
move. These design decisions and the original function template were mine.

AI assistance supplied and ran the offline Python script
[`generate_precomputed_sample.py`](tests/debug/generate_precomputed_sample.py) to
compute the BFS answers and emit the packed table bytes. AI also supplied the
detailed arithmetic for packing and extracting the nibbles with bit shifts and
masks, and recommended hexadecimal notation to make the two move codes visible
in each byte. The script generates the table data; it does not generate the
whole C implementation or originate its design.

I asked why we could not simply store the actual character/byte values from
the 256 possible values of an eight-bit byte. The explanation was about source
readability: the compiled array already stores those actual byte values, while
hexadecimal shows each four-bit move code as one hex digit. This makes the
packed moves easier to inspect alongside my existing move-code decoding table.
Raw characters can be invisible or require escaping; hexadecimal notation does
not itself provide additional compression.

I subsequently requested `0xNN` array entries instead of `\xNN` string
escapes, and first-move-first ordering: the first move occupies the high (left)
nibble and the next move occupies the low (right) nibble. AI implemented that
change across the generator, table and decoder and ran the verification checks.
For example, `0x91` now represents `rra` followed by `sa`.

### Working sample using my solution template

[`precomputed_ranks_bfs.c`](src/sorting_and_algorithms/exact_hardcoded/precomputed_ranks_bfs.c)
implements the **full-solution** technique for every permutation of 1–4 values.
It follows my original `encoded_bfs_data` / `decode_bfs_data` /
`get_precomputed_bfs` template. The filename was changed to lowercase for Norm.
These are actual generated shortest answers, not placeholder bytes.

There are `1! + 2! + 3! + 4! = 33` entries, each occupying three bytes.
Reverse BFS found a maximum of five moves for these initial states, leaving a
sixth nibble for the zero terminator. The data occupies exactly 99 bytes in a brace-enclosed `unsigned char` array
using `0xNN` integer constants, with no implicit string terminator. Each byte
stores the first move in its high (left) hex digit and the next move in its low
(right) digit. Embedded zero bytes are intentional; this is
binary data, not a string to pass to `strlen`.

`starts = {0, 0, 1, 3, 9}` locates each input size's first entry. For example,
the 24 four-value permutations start after the first nine entries. The address is:

```c
data + (starts[count] + lehmer_rank) * 3
```

This needs a `const unsigned char *`, not `int **`: the pointer identifies the
first packed byte of one contiguous solution. A single `int` cannot hold an
arbitrarily long solution. `x->ans` remains my existing `char **`, with one
allocated answer buffer per algorithm candidate. I cannot replace one of those
owned buffers with a pointer into static data: the representations differ, and
the cleanup code frees those buffers.

The caller uses the existing `new_soln_init` (the initializer I meant by
"new_algo_init"), then calls:

```c
get_precomputed_bfs(x, &stacks[A], count);
```

Its contract is a fresh answer slot, B empty, and distinct normalised ranks
`0..count-1` in A. It records into `x->ans[x->cur]` using
`append_move_to_soln`, which updates both `x->step` and `x->ans_len[x->cur]`.
It does not mutate the stacks. `solve()` now uses it in the existing BFS
candidate slot for 1–4 values; larger inputs retain the previous search path.
No extra candidate allocation is necessary.

The decoder deliberately uses ordinary numbers rather than bit-mask macros:

```c
move = (packed[i / 2] >> ((1 - i % 2) * 4)) & 15;
```

- `i / 2` selects the byte, since each byte contains two moves.
- `i % 2` selects the first or second move.
- `(1 - i % 2) * 4` shifts by four bits for the first move, then zero for the second.
- `15` is binary `1111`, keeping the low four bits after shifting.
- Zero ends the sequence; values `1..11` select my existing move characters
  through `"0123456789AB"`, including `'A'` and `'B'` for `rrb` and `rrr`.

The lookup is O(1) **after** calculating the rank. Calculating the rank is O(n²),
and decoding/storing m moves is O(m), so the complete call is O(n² + m).
Saving m decoded characters into `x` cannot be an O(1) operation.

Reproduce the bytes and run exhaustive sample checks with:

```sh
python3 tests/debug/generate_precomputed_sample.py
python3 tests/test_precomputed_sample.py
norminette src/sorting_and_algorithms/exact_hardcoded/precomputed_ranks_bfs.c
```

The Python generator is an offline study tool, not part of the C build. Tests
compare the C bytes against reverse BFS, check all 33 permutations and optimal
lengths using the supplied checker, exercise wrapped circular buffers through
the direct API, and check a five-value input still uses the fallback correctly.

### How much fits in a 500 MB executable budget?

Here I mean **500,000,000 bytes**, not 500 MiB or 500 MB of resident RAM.
Other executable contents also need some space. For full solutions, the exact
answer depends on their lengths and the chosen table layout. The 19 MiB figure
for n=10 above belongs to the next-move-only technique, not this sample's layout.

**n=10 is safely within the data budget. n=11 needs measurements before I can
claim it fits.** A conservative bound gives a concrete reason: repeatedly
rotate the smallest remaining value to A's top, push it to B, then push all
values back. At size k, reaching any position needs at most `floor(k / 2)`
rotations. This constructs a valid solution in at most
`floor(n² / 4) + 2n` moves, so shortest solutions cannot be longer.

| n | Conservative move bound | Bytes per entry, including zero terminator | Fixed-width full table |
| ---: | ---: | ---: | ---: |
| 10 | 45 | 23 | 83,462,400 bytes |
| 11 | 52 | 27 | 1,077,753,600 bytes |
| 12 | 60 | 31 | 14,849,049,600 bytes |

These are upper-bound layouts, not measured shortest-path maxima. Using a
separate width for each size, all tables from n=1 through n=10 together fit
within 91,485,206 data bytes under this same bound. The sample does **not**
generate these larger tables.

For n=11, even spending the whole 500 MB on that size permits only 12 whole
bytes per fixed-width entry: at most 23 moves plus the terminator. I would
need to establish that every shortest answer fits, or measure the total size
of a variable-length layout including its offsets. The conservative bound
alone cannot decide that. For n=12, a fixed-width table permits only one byte
per entry, which plainly cannot encode every answer with this format.

Increasing the sample's limit is not enough: the data must be regenerated,
entry widths and offsets updated, and ranking/index types checked. My current
BFS state and factorial helpers also have explicit size limits. Generating and
compiling a large table may need far more memory than looking up one answer.
Keeping a huge initializer within the Norm is a separate unresolved design
constraint; this small sample fitting the Norm does not establish that a giant
generated version would.

### Submission considerations

The local push_swap subject forbids global variables. A file-scope `static const`
array is still a file-scope object; neither `const` nor putting it in a `.h`
automatically makes it acceptable. A function-local `static const` table avoids
global scope, but its initializer still has to fit the Norm's formatting and
function-length rules. Multiline macros or obfuscation are not a workaround.
Passing norminette alone does not establish compliance with every review rule.
The documents I checked with AI did not explicitly ban precomputed solutions or specify
a table-size limit; that is not a guarantee that any generated table is suitable
for submission.

**I won't submit a 10 GB file lah.** This was just how a BFS lookup-table idea
turned into me learning about pages, virtual memory, the heap and the stack.
The evaluator should be checking my sorting, not downloading my homemade WinRAR
replacement.

[↑ Back to top](#top)

<a id="chunk-extraction-and-the-hidden-stack"></a>

## Chunk extraction and the hidden stack

**Experiment outcome:** this chunking approach did not work out for my move-count
goal. In my earlier trials, 500 elements took roughly **10,000–12,000 moves**.
My observation was that most moves went into selecting and extracting elements
in increasing order to form the chunks. Finding short BFS routes for the small
chunks did not make up for that selection cost. These are my reported results
for this implementation, not a claim that every chunking algorithm performs
poorly. I am shelving this approach and focusing on full-input BFS for small
inputs and LIS/greedy strategies for larger ones.

The archived development path processed successive rank intervals of up to ten
values. It first moved the selected interval from A to B, then created temporary
stacks containing the active chunk, searched for a solution and replayed that
solution on the real stacks.

[`chunk_optimal_BFS.c`](../backups/chunk_optimal_BFS.c) simulates extraction
routes before executing one. It tries each initial rotation direction and each
point at which to reverse direction after collecting a target value. It chooses
the lowest rotation-plus-push cost among those candidates.

“Optimal” here refers only to that limited family of extraction routes. The code
does not compare arbitrary direction changes or the total future sorting cost.
Likewise, a short BFS solution for one chunk does not prove that the full sequence
meets the subject's move requirements.

The archived hidden search treated the unseen portion of A as a boundary: it
disallowed A rotations when the visible portion was nonempty and disallowed
`sa` when fewer than two visible values were available. This is the experiment behind “hidden BFS”. Its overall
correctness and efficiency still need broader validation.

[↑ Back to top](#top)

<a id="analysis-tools-and-study-data"></a>

## Analysis tools and study data

The two analysis executables accept an `n` from 2 to 7 and write timestamped
reports into their working directory. For a small reverse-BFS study:

```sh
make analyse_bfs_all_paths
mkdir -p tests/debug/old_results
(cd tests/debug/old_results && ../../../bin/bfs_all_paths 3)
```

The reverse analyser builds distances from the sorted goal, uses all eleven
operations, and enumerates shortest paths by following moves that reduce the
remaining distance. The active full-input BFS now also uses all eleven
operations; unlike the reverse analyser, it searches forward from one input.

The permutation analyser can be run with
`(cd tests/debug/old_results && ../../../bin/bfs_analyser 3)` after `make analyse_bfs`.
Its source calls `brute_solve`; historical reports can reflect earlier search
restrictions. An earlier build of `analyse_bfs` failed because its target
omitted required debug-printer symbols. That issue was fixed, and the forward
analyser subsequently rebuilt successfully during the hybrid-storage work.

| Saved material | What to study |
|---|---|
| [`tests/debug/old_results/`](tests/debug/old_results/) | All preserved BFS reports and 500-number trial logs. |
| [All shortest paths, n = 5](tests/debug/old_results/push_swap_bfs_all_paths_n5_2026-08-24_22-02-49.txt) | The report records 120 starting permutations, 720 graph states and a maximum optimal distance of 9. |
| [All shortest paths, n = 7](tests/debug/old_results/push_swap_bfs_all_paths_n7_2026-08-24_22-02-53.txt) | The report records 5,040 starting permutations, 40,320 graph states and a maximum optimal distance of 13. |
| [`notes.md`](../backups/notes.md) | Questions about pattern discovery, heuristics and how small solutions might inform larger cases. |

These numbers describe the saved reports, not a fresh evaluation of the current
executable. The reports are important study data and are **not ignored**. Build
cleanup does not delete them. Larger all-path reports can grow quickly because
one starting permutation may have many equally short solutions.

[↑ Back to top](#top)

<a id="checks-and-current-limitations"></a>

## Checks and current limitations

### Build checks

The following are **historical** checks from the directory reorganisation,
not guarantees about the current targets. They establish build behavior at
that time, not sorting correctness or a subject score.

| Result | Check |
|---|---|
| ✅ | Main executable and all four development tools build with `-Wall -Wextra -Werror`. |
| ✅ | Repeating all build targets leaves executable and archive timestamps unchanged. |
| ✅ | `make clean` removes objects while preserving executables. |
| ✅ | `make fclean` removes build products while preserving the checker and all 17 existing study reports. |
| ✅ | All targets rebuild after cleanup. |
| ✅ | Deleted main and analyser executables are recreated. |

As part of my AI-assisted README review, I had the main executable and reverse
all-path analyser rebuilt successfully. Four small smoke cases (one value, sorted three,
unsorted three and unsorted five) returned `OK` from the supplied checker.
The forward analyser was subsequently rebuilt successfully during the hybrid-storage update.
This limited check is not a final benchmark, Norm audit or full validation.

After parsing and validation, I check the ranks before allocating answer
buffers or running any candidates.
If each logical position `i` contains rank `i`, the input is already sorted.
I just walk through A once: O(n), with the same check for inline and heap
buffers. There is nothing to sort, so I print nothing and use the normal cleanup
path. I still validate the entire input first; duplicates and invalid tokens
must not become successful early exits.

<a id="current-limitations"></a>

### Current limitations

I have completed the implementation and the listed pre-submission checks.
The remaining limits concern the scope of the evidence and the cost of the
algorithms, rather than unfinished versions of the earlier fixes:

- BFS memory still grows factorially, and recursive lookahead remains expensive.
- The documented benchmarks cover the inputs and configurations tested; they
  do not guarantee the same move counts or runtime on every input or machine.
- My candidate regression harness now checks the current candidates, and the
  random runner captures the current configuration macros. Older reports retain
  only the metadata recorded at the time.
- The listed build, Norm and memory checks passed. The live evaluation still
  assesses the submission against the full project requirements.

My final settings reflect the trade-off I could substantiate with the resources
and trials available to me. Further tuning remains possible, but is not required
to describe this implementation as complete.

[↑ Back to top](#top)

<a id="resources"></a>

## Resources

- 2swap. [*I Solved Klotski*](https://www.youtube.com/watch?v=YGLNyHd2w10) (also circulated as *Adventures in State Space*). Inspiration for viewing a concrete puzzle as a graph of configurations and moves, and for my proposed exact push_swap state-graph visualisation described under future research.
- Jamie Dawson. [*Push_Swap: The least amount of moves with two stacks*](https://medium.com/@jamierobertdawson/push-swap-the-least-amount-of-moves-with-two-stacks-d1e76a71789a), 11 May 2019. An early foundation for my understanding of hard-coded small cases and the five-element optimality question described in the preface.
- Ulysse Gerkens. [*Push Swap in less than 4200 operations*](https://medium.com/@ulysse.gks/push-swap-in-less-than-4200-operations-c292f034f6c0), 1 August 2023. A related implementation article that links to Dawson's small-case explanation; its reported performance belongs to that author's implementation.

- [aaax8 — push_swap](https://github.com/aaax8/push_swap) and its [Japanese technical report](https://github.com/aaax8/push_swap/blob/main/docs/push_swap_report.qmd). I came across this repository through Slack in September 2026 while working on this project. Its discussion of beam search for initial solutions and Iterated Greedy destruction/reconstruction inspired me to consider alternative candidates, lookahead and pruning. It influenced my thinking, but I did not implement its beam search or Iterated Greedy methods.

- Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest and Clifford Stein. [*Introduction to Algorithms*, third edition](https://mitpress.mit.edu/9780262033848/introduction-to-algorithms/). MIT Press, 2009. ISBN 978-0-262-03384-8. A major reading reference during my time at 42 and a substantial help to this project; see the [preface](#preface).

- [aleksify — pushswap-research](https://github.com/aleksify/pushswap-research) explores move-sequence optimisation and BFS-based superoptimisation. Its **More Thoughts** section proposes bounded lookahead with beam search or Monte Carlo Tree Search and discusses the difficulty of scoring intermediate stack states. I found this useful inspiration for testing lookahead in greedy reinsertion, but those proposed approaches are not benchmark evidence that two-insertion lookahead, circular-LDS preparation, or their combination will improve my solver.
- [A. Yigit Ogun — Push Swap: A journey to find most efficient sorting algorithm](https://medium.com/@ayogun/push-swap-c1f5d2d41e97) introduces the Turk algorithm. I used this as a reference for my greedy reinsertion approach: both choose transfers by move cost, but mine applies that choice when returning elements from B into circularly sorted A.
- [Working notes](../backups/notes.md) and [saved study reports](tests/debug/old_results/) document the investigation and examples.
- [Bundled libft documentation](libft/README.md) describes the shared library.
- I followed the structure of my [Pipex README](../2_pipex/README.md): feature status, design explanations, reproducible commands and explicit limitations.
- Harvard CS50 lectures by David J. Malan, peer discussions and debugging references contributed to the broader learning process recorded in the previous README.

### Use of AI

I use ChatGPT/Codex for explanations, alternative approaches and tradeoffs, and
help when I am stuck. I bring questions, ideas and deductions into the discussion
and work through the reasoning with AI. This project's algorithm design and
process were developed through that collaboration; established methods such as
Lehmer ranking are not my invention.

AI generated the Lehmer-ranking Mermaid diagram and helped write the mathematical
explanation in this README. I did not create that diagram myself. Its attribution
is also placed beside the diagram so readers do not mistake it for unaided work.

AI assistance also includes the debugging and diagnostic printing tools, test
harnesses and experimental comparisons, mechanical editing, file organisation,
build checks, and documentation. These supporting tools help me inspect behaviour
and test ideas; their output is not proof that the solver is correct or ready
for evaluation.

For Norm compliance, Codex helped refactor my existing BFS code
for the 25-line function limit, five-local-variable limit, typedef naming and
formatting rules. This mainly meant splitting existing work into named functions,
grouping search variables into a struct, moving helpers into focused files, and
updating declarations and Makefile paths. It was not a request to "vibe code" a
new BFS algorithm: the queue, visited bitset, move order, parent links and path
reconstruction came from the existing implementation. I reviewed the changes
through discussion so I can understand and explain them during evaluation.

AI did write the refactoring edits, so "no code was generated" would be too
broad a claim. The distinction is that these edits reorganised existing logic
for readability and Norm compliance rather than replacing it with an unexplained
new solution. In the direct BFS regression check, 159 inputs produced exactly
the same move sequences before and after the refactor. That is evidence for
those cases, not a proof covering every possible input.

For hybrid storage, I asked Codex to implement the agreed
inline/heap split, parser/storage changes, reuse of the existing local greedy solver above 500 and
regression checks. This work includes new code, beyond the earlier mechanical
BFS refactor. I remain responsible for understanding and explaining that code.

I also use AI as an editorial assistant for this README. I bring my questions,
scattered notes, conversations, experiments and sometimes rather tangled
explanations; AI helps collate and paraphrase them into a coherent, readable
account. This write-up grew through those discussions and revisions, rather
than from a single request to generate a README. I remain responsible for
checking that it reflects what I meant and what I actually implemented.
Where AI contributed explanations, mathematical derivations, diagrams or
generated tools, I identify that assistance separately; editorial help does
not make every technical contribution solely mine.

My aim is to understand and explain the implementation, rather than present an
unexplained generated solution as my own. This follows the distinction described
in my [Pipex README's Use of AI section](../2_pipex/README.md#use-of-ai): the
learning discussion and my own reasoning are distinguished from AI-assisted
mechanical work, testing and documentation.

[↑ Back to top](#top)

## Seed candidate flow

`solve()` dispatches the exact candidate first when the input has at most
`BRUTE_MAX_N` values (currently 10). It then loops from `ALGO_THREE_LOCAL` to
`ALGO_COUNT`, unless `SKIP_OTHER_ALGO_AFTER_BFS` is enabled (currently 0).
Above 500, it dispatches directly to the existing `ALGO_THREE_LOCAL` strategy before this candidate loop.
The enum and `src/sorting_and_algorithms/algorithm_config.c` define six entries:

| Enum | Preparation | Search policy |
| --- | --- | --- |
| `ALGO_BFS` | None | Precomputed 1–4; full-input BFS 5–10; absent above 10 |
| `ALGO_THREE_LOCAL` | Three elements | Local greedy |
| `ALGO_LIS_LOCAL` | Circular LIS | Local greedy |
| `ALGO_LIS_LOOKAHEAD` | Circular LIS | Repeated lookahead with partial execution |
| `ALGO_LIS_OPENING_ONE` | Circular LIS | Special opening search, execute one, then repeated lookahead |
| `ALGO_LIS_OPENING_BATCH` | Circular LIS | Special opening search, execute a batch, then repeated lookahead |

I run the three-element local candidate before circular LIS local. Equal-length
answers keep the first candidate, so this order can change the chosen move
sequence in a tie without changing its length.

There are six generated candidates for small inputs with the skip flag off,
and five for 11–500 values. Above 500 has one local greedy answer. Algorithm IDs and generated solution-slot indices
are different when the exact candidate is absent. Three-element seed plus
lookahead is not an active enum entry.

My final, best-supported tested settings in `includes/push_swap.h` are:

| Phase | Lookahead depth | Execution limit |
| --- | ---: | ---: |
| Normal continuation, total input size <= 100 | 12 | 10 |
| Normal continuation, total input size 101–500 | 7 | 5 |
| Special opening | 12 | 1 for OPENING_ONE; 5 for OPENING_BATCH |

Above 500 values, I use local greedy without lookahead. Within the lookahead
path, the size threshold uses **A plus B**, not the shrinking length of B.
Actual saved paths and executed batches are capped by the remaining work. The opening
variants currently continue with lookahead because their `use_lookahead` field
is 1; older opening-then-local benchmarks are historical experiments.

Each candidate starts from fresh stack copies and its own answer slot. The first
shortest generated answer wins ties. `t_seed_mode` chooses preparation;
`t_algorithm` and `t_algo_config` choose the whole strategy. Internal depth zero
selects local greedy; a positive depth searches candidate insertions.

| Stage | File | Responsibility |
| --- | --- | --- |
| Candidate loop | `src/sorting_and_algorithms/solve.c` | Reset stacks and solution for each generated candidate |
| Preparation | `greedy_prepare.c` | Keep three values or a circular LIS |
| Composition | `greedy_stages.c` | Prepare, search/execute batches, then align A |
| Execution | `greedy_execute.c` | Share rotations, finish residual rotations, then push |

Archived seed-search prototypes are in
[backups/BFS WIP backup](../backups/BFS%20WIP%20backup/README.md).
They are not built; the separate full-input BFS under `src/` is built.

**Candidate regression harness:** `tests/test_seed_candidates.py` now builds a
temporary diagnostic executable and reads every recorded candidate from
descriptor 3. It checks sorted-input silence, replays each candidate, and checks
that stdout is the first shortest answer. The updated run passed 173 inputs,
including all permutations through five values and selected arrangements
through 100. Its default run stops at 100; `--include-500` explicitly adds
500-value cases. The diagnostic build leaves my submission settings alone.


## Greedy lookahead: who owns each plan?

**Inspiration and implementation boundary.** One of the later references I
encountered, through Slack in September 2026, was
[aaax8's push_swap report](https://github.com/aaax8/push_swap/blob/main/docs/push_swap_report.qmd).
Reading its beam-search and solution-improvement discussion encouraged me to
explore alternative candidates rather than remain committed to a single plan,
and informed my thinking about lookahead and reducing search work.

My implementation here is recursive lookahead with **branch-and-bound pruning**,
not beam search. Beam search retains a limited set of candidates according to
a score; the pruning described below discards a branch when a valid lower bound
cannot improve the current budget. The shared motivation is to spend search
effort usefully, but the mechanisms and guarantees differ. I did not implement
the referenced solver's beam search or Iterated Greedy destruction/reconstruction.
I am crediting the influence on my thinking, rather than claiming a direct
implementation of that author's method.


Depth counts complete B-to-A insertions, including rotations and `pa`, not
individual instructions. The selector searches a horizon and saves its winning
sequence; `greedy_insert_batch` executes a prefix up to the configured limit,
then the next search starts from the resulting real state. See the settings
above. Depth-three/execute-one examples below describe earlier experiments,
not the current default.

### Same variable name, different storage

Recursive calls own separate local variables. A parent pauses while its child
runs; copying a winning path copies values, not pointers to expired child locals.

| Variable | Where it lives | What it holds |
| --- | --- | --- |
| `best` | `greedy_insert_batch` | Winning `t_greedy_path` to execute |
| `candidate_path` | Each bounded selector call | Current insertion followed by its winning continuation |
| `search.best_path` | Search struct passed by value | Pointer to that caller's output path |
| `copies[2]` | Each simulated continuation | Independent hypothetical A/B buffers |
| `child_path` | Each continuation helper call | Child's saved winning continuation |

`append_child_path` copies the child plans after `plans[0]`. When a candidate
wins, `*search.best_path = candidate_path` copies the entire path struct to the
caller's storage. The child receives `&child_path`, not the root's output
address, so it cannot overwrite the root winner through that parameter.

### Follow one depth-three branch

A root candidate X is simulated on copied stacks. The depth-two child selects
Y and its depth-one continuation Z. The child returns its score and saved path
[Y, Z]; the parent forms [X, Y, Z]. Siblings start from the same parent state.
The selected path's individual `.cost` fields remain immediate insertion costs;
the returned integer scores the entire searched horizon.

Simulations call `greedy_execute_plan(NULL, ...)`, changing only copies and
recording no real moves. After selection, the executor applies the first
`min(execute_limit, best.length)` saved plans on the real stacks. Execute-one
is a supported policy, but the current normal limits are 10 or 5.

If B empties, the leaf returns final alignment cost, even at depth zero.
Otherwise depth zero returns zero: stop looking, not sorted. At depth one with
more than one B element, the immediate cost suffices without simulation; with
one element, simulation is needed to price final alignment.

No allocated search tree is needed. Each branch's local copies and bounded path
arrays are reused as recursion unwinds. Search storage follows active recursion
depth rather than retaining every explored branch; the number of candidate
branches can still be enormous. A lower horizon score does not guarantee a
shorter complete sort. Error/pruning return contracts are described in the
[branch-and-bound section](#optimization-technique-branch-and-bound-pruning).


### Hybrid storage and recursive malloc

I use 500 as the threshold for choosing storage and solver strategies, rather
than as a parser rejection limit. I chose hybrid storage to preserve my recursive
search's cheap, independent struct copies:

| Input size | A/B storage | Solver |
| --- | --- | --- |
| Up to 500 | Existing `int buf[501]` inside each cbuf | Existing exact/greedy candidates and lookahead |
| Above 500 | Two `malloc` arrays, each `count + 1` integers | Existing three-element seed + local greedy |

The spare element distinguishes an empty circular buffer from a full one.
`cbuf_data()` chooses inline or allocated storage. It deliberately does not
store a pointer to the struct's own inline array: after `copies[A] = *a`, such
a pointer would still point at the parent's array! Inline copies remain
independent. Heap buffers belong to the original stacks, and the large solver
mutates them directly. The candidate-copy entry point rejects large buffers.
The greedy entry point allows them only for `ALGO_THREE_LOCAL`, which does
not recurse or use LIS.
A temporary pair of cbuf structs borrows the two heap arrays for that one pass;
it copies the final indices back and never frees the arrays.

I considered using malloc for everything. Allocating the original stacks once is
fine; what I wanted to avoid was allocating two fresh buffers at every simulated
branch. A correct heap clone would still copy the values, then add two allocations
and two frees, plus allocation-failure handling, for every branch it simulates.
Replacing an embedded array with a pointer does not make a struct assignment
deep-copy its contents.

For 500 inputs, the configured normal horizon is **7 insertions**, and the
special opening horizon is **12**, not 500 nested levels. With B still nonempty,
the depth-one shortcut skips the last simulation, so these searches can have
6 or 11 simultaneously active pairs of copied stacks. Each pair is roughly
4 KB, plus path data and helper call frames. The arrays are still copied on
the stack; I am not claiming that this copying is free.

The number of branches matters much more than those modest depths. As an
illustration, suppose preparation leaves **450 elements in B** and nothing is
pruned. A depth-d search has `P(450, d)` possible ordered insertion prefixes.
Two mallocs at every simulated prefix would give approximately
`2 * sum(P(450, k), k=1..d-1)` allocation calls for one search:

| Horizon | Unpruned depth-d prefixes | Hypothetical allocation calls |
| --- | ---: | ---: |
| 8 | 1.58 × 10²¹ | 7.15 × 10¹⁸ |
| 12 | 5.95 × 10³¹ | 2.72 × 10²⁹ |

These are deliberately unpruned estimates, **not measured call counts**.
Actual B size and branch-and-bound pruning change the work enormously, and
searches repeat as batches are executed. More practically, one million
simulated branches would mean two million mallocs and two million frees in
that design. I cannot infer the actual branch count from elapsed time alone.

With an earlier configuration, I observed full runs comparing the enabled
algorithms on 500 values taking **about 10 minutes**, using simulated stack
copies on the stack rather than heap clones. With my final settings, the three
documented 500-value trials averaged **about 3½ minutes per run**. These describe
different configurations, not a measured stack-versus-heap comparison. Search
cost still motivates me to avoid adding per-branch allocator work. Other parts of the existing
program already use malloc. A reusable scratch pool per depth could also avoid
per-branch allocation, but would require a different ownership design; keeping
the inline path preserves the implementation I currently understand.

The parser now makes two iterative passes: validate/count, then fill allocated
or inline storage and check duplicates. It does not recurse once per input
number. Recorded answers also grow with malloc/copy/free when needed;
`INITIAL_SOLUTION_CAPACITY` is their initial capacity, not a hard 10,000-move limit.
This occasional output growth is separate from simulated search branches,
which pass `NULL` for the answer and do not allocate move storage.

Large inputs reuse `ALGO_THREE_LOCAL`: keep three values in A, sort that seed,
then use the existing cheapest-insertion loop. This avoids the fixed-size LIS
arrays and recursive lookahead without adding a sorting algorithm. Local
candidate/target scans can still be expensive; this is not a promise of fast
sorting for enormous inputs. Rank conversion and duplicate checks remain O(n²).
Allocation failures report `Error` and clean up; this is not a
promise to accept inputs beyond available resources.

I use the commands below to check my hybrid-storage implementation. They cover
small-input regressions, large grouped inputs, instruction replay, answer growth
and allocation failures. The random-500 full recursive benchmark is separate;
these tests use sorted 499/500-value cases to check the inline boundary quickly.

```sh
python3 tests/test_precomputed_sample.py
python3 tests/test_hybrid_storage.py
python3 tests/test_hybrid_allocations.py
```

### Debug printers and the DEBUG flag

Set `DEBUG` to `0`, `1`, `2`, `3` or `4` in `includes/push_swap.h`, then run `make`.
The header is the source of truth for these printers; `make debug` does not
force this flag on. Progress and most diagnostics go to stderr; recorded solution dumps use FD 3.
With `DEBUG=1`, stderr displays an updating status bar. For plain event logs,
use `DEBUG=2` or higher and
`./push_swap 5 2 0 4 1 3 2> trace.log 3> solutions.log`.
The chosen solution goes to stdout independently of the debug level.

All printers live under `src/printers` (diagnostics are in `debugging_stderr/`), with at most five functions per file:

- `solution_printers.c`: actual best-solution output, independent of DEBUG.
- `debug_tools.c`: integer arrays, circular buffers and BFS memory estimates.
- `debug_solutions.c`: original ranks and recorded solution details.
- `debug_lookahead.c`: TRY, RETURN, STOP, EXECUTE and LIS-length diagnostics.
- `debug_search_progress.c`: per-search depth, candidate and trial counters.
- `debug_pass_progress.c`: cumulative work across all insertions in a seed pass.
- `debug_search_result.c`: result events and cached percentage thresholds.
- `debug_status_bar.c`: bounded status-line construction and one write per redraw.
- `debug_bfs.c`: BFS progress, allocation and chunk-result diagnostics.
- `debug_chunks.c`: extraction-route and total-move diagnostics.

Debug printing does not choose plans or execute moves. Algorithm functions call
these printers, which return immediately when DEBUG is zero. Required input
errors still print `Error` to stderr regardless of DEBUG.


### Lookahead progress levels

The lookahead printers follow the header's DEBUG level:

- `0`: no diagnostics.
- `1`: an updating stderr status line and encoded solution dumps on FD 3.
- `2`: root-candidate start/completion logs, executed insertions and decoded
  solution dumps.
- `3`: candidates and return/stop events at every recursive level.
- `4`: the above plus rank, immediate cost, continuation cost and combined cost.

The live level-1 line contains `A` (one-based algorithm slot / `ALGO_COUNT`),
`D` (reported insertion level / effective horizon), `E` (root candidates
completed in this search), `I` (real insertions completed in this pass), and
`eval` (evaluated candidates in this search). Algorithm slots include the
BFS slot even when that candidate does not run for a large input. The current
line does not display the internally retained root winner or its score.

The two percentages measure different work. E can restart at each search;
I is monotonic within an algorithm pass and advances only after a successful
real insertion. Simulated pushes and pruned descendants do not advance I.
Neither percentage estimates proximity to an optimal sorting solution.

Level 1 refreshes at each 1,048,576 candidate-result events, at root-search
completion and after a real insertion. These forced refreshes can occur at
the same insertion percentage. Rendering uses a bounded 512-byte stack buffer
and one stderr write, with `\r` and ANSI clear-line characters; levels 2–4 are
more suitable for plain event logs. No timer or extra thread is involved.

At DEBUG 2–4, a field such as `depth=2/3 candidate=4/9` means the fourth
logical B candidate at insertion level two of a three-insertion horizon.
Candidate positions are one-based indices, not ranks. START reports requested
and effective depths; the latter is capped by the remaining B length.

The detailed `covered=X/Y remaining=Z` fields count evaluated or safely
skipped candidate insertions across all levels of one search. For B length
$b$ and effective depth $d$, the unpruned tree contains:

```math
T(b,d)=\sum_{k=1}^{d}\frac{b!}{(b-k)!}.
```

For example, $T(5,3)=5+20+60=85$. These counts reset when a new search starts,
which may follow several real insertions. They do not count emitted moves or
predict work for the whole sort. A parent trial completes after its children
return; safely pruned descendants count as skipped coverage. Detailed search
totals exceeding counter capacity are marked with `+`, rather than wrapped.

`count_pass()` accumulates only searches actually started. It does not predict
future batches, so the pass total is unknown upfront. For local greedy, the
one-level diagnostic horizon counts candidate evaluations without enabling
lookahead: with $b$ initial elements in B, the total is $b(b+1)/2$.

After final alignment, the completion line reports cumulative `covered` and
`skipped`, `inserted=X/Y`, and `final_moves=N DONE`. Here Y is B's length
after seed preparation; N includes preparation and alignment moves. Empty B
can finish at `inserted=0/0`. Saturated counters are not exact work totals.
The diagnostic state assumes one synchronous search at a time and does not
decide which path wins.

<a id="greedy-heuristics"></a>

### Heuristics in my greedy solver do not require A*

**Definitions / specifications:** The formulas below describe immediate costs,
the current horizon score and a possible alternative tail score. They do not
prove a complete-solution performance advantage.

A heuristic is a way to guide a decision or estimate future work; A* is one
particular search algorithm that can use one. My local greedy choice is a
heuristic for the complete sort, even though its immediate insertion cost is
calculated exactly for the rotation routes it considers. My lookahead searches
insertion choices depth-first with branch-and-bound. It does not use A*'s
priority queue.

For a candidate at logical index j in B and its insertion target at index i
in A, let a and b be the current stack lengths. Define forward and reverse
rotation distances, with zero reverse distance when already at the top:

```math
r_A=i,\qquad r_B=j,\qquad
q_A=(a-i)\bmod a,\qquad q_B=(b-j)\bmod b.
```

For nonempty stacks, the four-route insertion cost used by
`greedy_plan_candidate` is:

```math
c(S,j)=1+\min\left\{
\max(r_A,r_B),\;
r_A+q_B,\;
q_A+r_B,\;
\max(q_A,q_B)
\right\}.
```

The 1 pays for `pa`. Same-direction rotations overlap through `rr` or
`rrr`, hence the maximum; opposite directions are performed separately,
hence the sum. If A is empty, its rotation cost is zero instead of evaluating
a modulo-zero expression. Local greedy chooses the first candidate attaining
the smallest immediate cost. That does not prove the cheapest complete sort.

For a searched path P containing k insertions, the current horizon score is:

```math
J(P)=\sum_{t=0}^{k-1} c(S_t,j_t)+
\begin{cases}
a_{\mathrm{final}}(S_k), & B_k=\varnothing,\\
0, & \text{depth cutoff with }B_k\ne\varnothing.
\end{cases}
```

Here S_t is the state before insertion t, and a_final is the rotation cost
to bring A's minimum to the top after B empties. The zero at an unfinished
cutoff means that future work is ignored, not that it is free. A possible
extension is to replace that zero by an estimated remaining cost:

```math
J_{\mathrm{estimate}}(P)
=\sum_{t=0}^{k-1}c(S_t,j_t)+\widehat h(S_k).
```

This tail estimate is a possible experiment, not an implemented feature.
It can rank candidates without being safe for pruning. A greedy completion
on copied stacks gives an achievable remaining cost (an upper bound on the
best possible completion), not a lower bound. Such a rollout can provide a
complete candidate solution, but must not be substituted into the existing
lower-bound pruning test.

### Why sortedness or entropy does not identify the best insertion

My question about "entropy" referred to the separate two-person 42 subject.
The publicly available [group subject, version 1.0](https://github.com/Mourey/pushswap/blob/main/en.subject.pdf)
calls it **disorder**. Section VI.3.2 (printed pages 10-11) prescribes the
fraction of inverted pairs in initial A, measured before any moves:

```math
D(A)=\frac{\#\{(i,j):0\le i<j<n,\ A_i>A_j\}}{n(n-1)/2},
\qquad n\ge 2.
```

Sorted input has D=0; reverse-sorted input has D=1. For fewer than two
elements, an implementation needs a zero-pair guard (returning zero is the
natural convention); the subject's displayed pseudocode leaves that edge case
implicit. This is a prescribed inversion-based measure, not a free choice of
entropy formula. Circular LIS can be an additional experimental feature but
does not replace this required metric. These group requirements are background
for the discussion, not requirements of my individual project.

A disorder metric and a remaining-operation-cost estimate serve different
purposes. Inversion count is order-sensitive but is not an operation-distance
bound here:
`[1, 2, ..., 499, 0]` has 499 inversions and needs only one `rra`,
whereas `[1, 0, 2, ..., 499]` has one inversion and needs one `sa`
(with B empty in both examples).

During reinsertion, A is already circularly ascending. Each candidate has one
correct insertion gap preserving that invariant, rather than every A position
being an alternative target. Every valid insertion increases A's size by one
and decreases B's size by one. Those progress measures cannot distinguish
candidates. What differs is the resulting stack orientation, B's remaining
order, future shared-rotation opportunities and final alignment cost.

For my own experiments, a possible circular-LIS disorder score for one
nonempty stack is:

```math
D_{\mathrm{cLIS}}(A)=1-\frac{L_{\mathrm{cLIS}}(A)}{|A|}.
```

Here L_cLIS is the maximum ordinary increasing-subsequence length over all
rotations of A. This proposed score is zero for a circularly ascending stack.
It is a different measure from the group subject's required disorder
function and cannot replace it; it is not an admissible move-count bound. In my reinsertion phase it stays
zero before and after every valid insertion, so it cannot by itself rank the
candidates. It also ignores B and the final orientation of A.

Candidate ordering and pruning serve different purposes. A promising score
can tell me which branch to examine first; it cannot alone justify discarding
the others. Keeping only the best few estimated candidates would change the
search into a heuristic restriction and could discard the winning continuation.
Reordering also changes which equal-cost path wins under first-minimum ties
unless I explicitly preserve the original tie order.

### Optimization technique: branch-and-bound pruning

`greedy_choose_plan_lookahead` starts an exclusive budget of `INT_MAX`.
`t_greedy_search` groups depth, budget and the output-path pointer so helpers
stay within four arguments. The struct is passed by value: siblings never
share a mutable budget. Only the output pointer refers to the caller's saved path.

The search is depth-first. With no initial incumbent, it first prices a path to
the depth limit (or completion). As calls return, actual continuation costs
establish local budgets; alternative branches can then tighten them. The first
root candidate returns its best continuation score before the next root
candidate is compared against it. Budgets are not arbitrary constants: each
child receives the parent's remaining allowance after the current insertion.
An inherited allowance can also prune a subtree before it finds its own path.

Before simulating an insertion, `greedy_branch_cost` checks:

```math
\text{candidate cost} + \min(d-1, |B|-1) \geq \text{budget}.
```

Every remaining insertion costs at least one `pa`. If this lower bound reaches
the budget, the branch cannot improve it, including under the first-minimum tie
rule. Otherwise its child receives `budget - candidate.cost`. A completed path
below the bound updates the local budget and output plan. An inherited budget
is a threshold, not proof that a plan with that cost exists.

For example, with a known total of 10, an insertion costing 3 gives its child
budget 7. If that child considers a move costing 6 with one insertion still
required, it skips that branch: even 6 + 1 cannot beat 7. Final alignment remains
part of completed-state costs; a horizon cutoff has cost zero.

Return contracts: nonnegative means a real cost below the budget, `-1` means
error, and `GREEDY_PRUNED` (`-2`) means no path beat the bound. A wholly pruned
search leaves its output plan untouched. Pruning preserves the exhaustive
search's selected path and score for the same horizon; it does not make
that horizon globally optimal.

Diagnostics show `covered = evaluated + skipped` against the original exhaustive
trial total. The candidate whose bound is checked counts as evaluated; only its
unvisited descendants count as skipped. Skipping a subtree advances progress in
one jump. The detailed search coverage and skipped counters reset at each search.
The final `covered` and `skipped` totals accumulate across the algorithm pass;
subtract skipped from coverage to get actual evaluations (unless counters
saturate). Completion still reports final recorded moves after alignment.



<a id="safe-pruning-bounds"></a>

### What makes a pruning bound safe?

**Conditional guarantees / bounds:** Unlike the batching identities, the bounds
in this section justify eliminating branches. They require a lower bound and
a budget for the same objective; the current horizon guarantee is not global
optimality of the complete sort.

A lower bound must not exceed the remaining cost of any feasible completion
for the objective being searched. For a complete sort, every element currently
in B needs a `pa`, so a simple valid bound is:

```math
h_{\mathrm{push}}(S)=|B|.
```

If g moves have already been spent and U is the cost of a known complete
solution from the same starting state, the branch cannot strictly improve it
when:

```math
g+h_{\mathrm{push}}(S)\ge U.
```

Equality may be pruned when retaining the first minimum, but not when trying
to enumerate all equally optimal answers. This can eliminate some candidates
with a guarantee without identifying the best candidate in advance.

My current lookahead budget measures only its finite horizon, plus alignment
when B empties. It is not necessarily the cost of a complete sorting solution.
Therefore its matching lower bound before executing a candidate of cost c is:

```math
L_d=c+\min(d-1,|B|-1).
```

This is the bound in the preceding implementation section: only insertions
remaining inside the horizon are counted. Using the full remaining B count
against the existing horizon budget would compare different objectives and
could prune incorrectly. A new terminal score would also require reviewing
the bounds and budget semantics.

Safe pruning preserves the best score for the defined horizon and allowed
insertion plans; it does not prove a globally shortest push_swap solution.
For two proven lower bounds on the same remaining objective, taking their
maximum is safe:

```math
h(S)=\max\{h_1(S),h_2(S)\}.
```

Adding bounds needs a separate proof that costs are not counted twice.
Likewise, summing today's individual insertion costs is not automatically
a lower bound: later rotations and pushes change all subsequent costs.

Exact small-state tables can inspire pattern-database heuristics, but simply
deleting most elements and consulting my precomputed sorting table is not
automatically admissible. A valid abstraction must preserve or relax the
effects and costs of the real moves; deleting elements changes which positions
a swap or rotation acts on. This is a possible research direction, not part of
the current solver.

### Earlier batching experiment: initial decision on five inputs

I tested saving the best multi-depth insertion sequence and executing several
insertions before searching again, instead of executing only its first insertion.
Here, one insertion includes its rotations and final `pa`; it is not one
individual push_swap operation. All variants used the same circular LIS seed,
candidate costs, branch-and-bound pruning and first-minimum tie rule.

On the same five shuffled 100-element inputs:

| Input | Depth 7, execute 1 | Depth 7, execute 3 | Depth 7, execute 7 | Depth 8, execute 3 |
| --- | ---: | ---: | ---: | ---: |
| 1 | 396 | 464 | 480 | 455 |
| 2 | 414 | 415 | 376 | 410 |
| 3 | 389 | 437 | 410 | 412 |
| 4 | 438 | 478 | 442 | 388 |
| 5 | 454 | 464 | 480 | 436 |
| **Average** | **418.2** | **451.6** | **437.6** | **420.2** |

Counts include reinsertion and final alignment, but exclude the identical LIS
preparation cost. Every completed run sorted correctly. These were isolated
experiments, not changes to the configured solver.

At depth seven, executing three was about 2.5x faster than executing one in
their paired test, but used 8.0% more moves on average. Executing all seven was
about 6.8x faster in its paired test, but also had a worse average move count.
The depth-eight/execute-three variant averaged two more moves than the
depth-seven/execute-one baseline. Timing ratios are experiment-specific.

**My decision at the time was to retain execution of one insertion after multi-depth evaluation.**
It produced the lowest average move count in this small sample. Batching was
initially discarded as the default strategy, although it sometimes won on individual
inputs and reduced search time; these results do not prove one-at-a-time
execution is universally better.

Replanning after each insertion moves the horizon one insertion further into
the future. Committing a whole batch saves searches, but delays reconsideration
using that additional information. Both approaches still ignore unfinished
work beyond the depth cutoff, so neither guarantees the best complete sort.

## Discussion / discoveries

**Evidence status:** In the discussion below, I separate recorded experiments,
possible explanations and structural properties of recursive planning. Its
horizon equations specify how the variants differ; they do not prove which
variant produces the fewest complete sorting operations. See the
[mathematics reading guide](#reading-the-mathematics).

### More lookahead, plan switching and partial commitment

This discussion records my earlier eight-lookahead/six-executed experiments
and the reasoning they prompted. My final configuration uses depth 12 /
execute 10 through 100 values and depth 7 / execute 5 for 101–500 values,
with separate opening settings shown in [Seed candidate flow](#seed-candidate-flow).
I keep the earlier dialogue and equations because they explain my investigation;
eight/six is no longer my current preferred setting.

*I shared this real conversation with AI and used its help to paraphrase it,
remove profanity and make the wording suitable for school. This is not a
verbatim transcript. The observations and hypotheses come from my conversation
with my friend; AI helped draft the analysis and diagram below.*

**Earlier conversation**

> **Me:** With deeper lookahead but only one insertion executed each time, the
> final result got worse. How can more information lead to a worse outcome?
> Perhaps my scoring does not capture the future well enough.
>
> **Friend:** That seems possible. Could the extra information be distracting
> from what matters to the final result?

**Follow-up**

> **Me:** I have three possible explanations:
>
> 1. The search still did not look far enough ahead and chose a path that was
>    worse overall.
> 2. Replanning after every insertion may cause repeated changes of plan.
>    Perhaps that hurts performance, although I have not established it.
> 3. Executing an entire planned batch delays the opportunity to reconsider a
>    poor path.
>
> My current best setting in these trials looks ahead eight insertions,
> executes six, then searches again.

In this exchange, "moves" means **candidate insertions**, each including its
rotations and final `pa`, rather than individual push_swap instructions.
"Then recurse" means search again from the resulting real state; the search
itself uses recursion. Depth 8 / execute 6 was the best setting I reported
from those earlier live trials, not a demonstrated universal optimum.
I did not include new benchmark logs, a sample size or runtime measurements
with this exchange. The earlier five-input batching experiment remains evidence about that earlier sample,
not a permanent decision against batching.

*I used AI to generate this conceptual diagram of the planning choices.
It does not show measured outcomes or reproduce the chat.*

```mermaid
flowchart TD
    P["Search eight insertions ahead"] --> E1["Execute one"]
    P --> E6["Execute six"]
    P --> E8["Execute eight"]
    E1 --> R1["Replan sooner"]
    E6 --> R6["Retain most of the plan"]
    E8 --> R8["Retain the full plan"]
    R1 --> T["Trade-off: search effort and when to reconsider"]
    R6 --> T
    R8 --> T
    T --> M["Measure complete move counts and runtime"]
```

#### Assessment of the explanations

| Idea | Assessment | Limitation |
| --- | --- | --- |
| More lookahead can produce a worse complete solution | Correct for a truncated score with an unscored tail | It is not evidence that accurate information is intrinsically harmful |
| The horizon was still too short | Plausible mechanism: costs beyond the cutoff can reverse the preference | Increasing depth again need not fix it; no useful depth threshold has been established |
| Executing one insertion causes harmful plan switching | Plausible hypothesis worth logging | A changed plan is not itself wasted work or proof of harm |
| Full commitment prevents correction | Correct that it delays replanning beyond the old horizon | It might preserve a good sequence instead; neither policy always wins |
| Depth 8 / execute 6 is a useful compromise | Supported by the observations I described | Needs paired, repeatable tests before generalising |

I interpret my friend's "irrelevance" suggestion as a mismatch
between the score and the goal. The extra simulated costs are real, relevant
operation costs. However, they cover only a prefix and can change which path
looks best while omitting the expensive consequence just beyond the cutoff.
There is no contradiction in having more simulated information but choosing a
worse complete route with that incomplete decision rule.

For illustration only, suppose two hypothetical paths have these costs:

| Path | First insertion | First two insertions, total | Unscored tail after two | Complete total |
| --- | ---: | ---: | ---: | ---: |
| X | 2 | 10 | 1 | 11 |
| Y | 3 | 4 | 20 | 24 |

A one-insertion comparison prefers X, whereas a two-insertion comparison prefers
Y. Both comparisons correctly minimise the cost they actually measure. The
complete totals favour X. These numbers illustrate the mechanism; they are not
a measured push_swap trace, and do not establish the cause of my observed run.

#### What "thrashing" would need to mean here

After executing one insertion of an eight-insertion plan, the old plan has
seven unexecuted insertions left. A fresh depth-eight search sees one insertion
further ahead. Its winner may therefore differ from the old suffix.

That is a moving-horizon decision, not necessarily a malfunction. No operations
from the discarded hypothetical suffix were emitted, so discarding it incurs
no direct move cost. Moreover, during reinsertion every executed `pa` reduces
B's length; the solver cannot literally cycle back to the same complete A/B
state while using only these rotation-and-`pa` insertion plans.

Harmful switching would mean the changed insertion order produces more rotation
work or a worse eventual continuation. Alternating rotation directions alone
does not prove waste: different targets can legitimately need opposite routes.
To support this theory, I need to compare the old suffix with the replacement
from the **same post-insertion state**, then measure complete continuations.

There is a useful control experiment. Under identical deterministic transition
rules and candidate choices, the suffix of an optimal depth-eight path remains
optimal for the remaining **seven-step objective**. A strictly better seven-step
suffix would also improve the original eight-step path. Searching eight steps
again changes the objective by extending the horizon; searching seven does not.
Equal-cost alternatives may still differ because of tie-breaking.

#### My follow-up: why depth eight and execute six?

This is my rationale from repeated live testing, computational limits and
intuition, rather than a conclusion from one isolated input. I have not
provided a controlled benchmark dataset for the runs described here.
I changed both lookahead depth and execution count, so I cannot yet separate
their individual effects or their interaction.

My hypothesis is that the earlier preference for executing one insertion was
conditional on the tested horizon, input distribution, preparation and code
version. A shallow horizon can favour an apparently cheap prefix whose larger
cost lies beyond the cutoff. Increasing depth may change that behaviour enough
to change which execution batch works best. This is a plausible interaction,
not proof that insufficient depth caused the earlier outcome or that deeper
search necessarily makes batching better.

**I need to distinguish two earlier experiments.** Opening-only lookahead followed
by local greedy is different from repeatedly replanning and executing one
insertion. I have not included the separate school AI conversation here, so I
cannot establish from these records which early experiment prompted my decision. The
recorded batching table compares depths 7 and 8 on five 100-element inputs;
the separate depth-3 example uses 500 elements. Neither tests depth-eight/
execute-six. These records describe particular samples, not a contradiction of
my newer observation or a universal argument against partial batching.

**Input size and effective horizon.** Eight insertions are 8% of 100 initial
elements but only 1.6% of 500. For the reinsertion search, the more relevant
denominator is the number currently remaining in B, after seed preparation:

```math
\rho(S)=\frac{\min(d,|B|)}{|B|},
\qquad |B|>0.
```

**Structural coverage measure:** Here d counts insertions, not individual emitted instructions. For example,
depth 8 covers 4% of the remaining insertions when B has 200 elements.
This ratio describes horizon coverage, not predictive accuracy or the fraction
of final operation cost known. More remaining candidates leave a longer
unexamined continuation and more possible choices, but do not prove that each
early decision has a larger effect. Input structure and stack orientation
matter too; equal coverage ratios do not imply equal search quality.

**Why eight?** On my school Intel i7 machine, which I described as having 20 cores,
depth 8 was a practical limit for the workloads I was testing. The search is
single-threaded, so one logical CPU being fully busy does not use the whole
machine's parallel capacity. Without pruning, with b candidates and depth d,
the number of leaf paths for d no greater than b is:

```math
P(b,d)=\frac{b!}{(b-d)!},
\qquad
P(b,d+1)=P(b,d)(b-d).
```

**Exact unpruned count, not a timing guarantee:** This explains why one more
layer can be costly. Actual runtime depends on
pruning, candidate ordering and per-node work; the unpruned count does not
predict the measured slowdown.

**Why six?** I used execution count e = d - 2 as an intuitive compromise:
keep most of the evaluated plan, then reconsider before executing its final
two insertions. This leaves two already-evaluated insertions uncommitted:

```math
r=d-e,\qquad
(d,e)=(8,6)\Rightarrow r=2,\qquad
(d,e)=(8,7)\Rightarrow r=1.
```

Those uncommitted insertions influence selection of the prefix. They are not
two guaranteed corrective moves, a reserve of computational bandwidth, or a
proof that a costly path can be escaped. Replanning creates a fresh horizon
from the new state; it does not undo the six insertions already emitted.
Executing seven likewise has no special mathematical failure at the seventh
insertion. A poor commitment could occur earlier, and the old eighth insertion
has already contributed to the score used to select the seventh.

I suspect execute-seven was less favourable, but cannot presently distinguish
a remembered result from intuition. I therefore treat this as a hypothesis to test,
not a measurement I can report. Depth-eight/execute-six was a reasonable configuration
to try under my compute budget, not a derived optimum or a universal
"d minus two" rule.

To test this, I would vary depth and execution count separately on the
same inputs and seed states, with a fixed code version and deterministic ties.
For example, I could compare execution 1, 6, 7 and 8 at depth 8, and compare nearby
depths at fixed execution 1 or 6. I would repeat this across input sizes and distributions,
recording remaining B length, complete move counts, correctness and runtime.
A small manual grid can answer this; automated hyperparameter tuning is
optional and does not require a supercomputer. Any tuning still needs a
separate validation set and an explicit runtime budget.


#### Mathematical interpretation: planning depth and commitment length

**Structural specification and design rationale, not a performance theorem.**
The following notation describes my execute-one, partial-batch and full-batch
variants from the recursive-planning point of view. The identities are exact
under their stated indexing assumptions; their performance implications are
hypotheses. Neither the formulas nor the research analogy prove a preferred
execution fraction.

In discussion with AI, I asked whether my intuition had a recognised connection
to other fields. AI suggested **receding-horizon planning**, particularly
multistep model predictive control (MPC), where planning depth and the number
of actions applied before replanning are separate design choices.
[Grune, Pannek, Seehafer and Worthmann (2010)](https://arxiv.org/abs/1006.2529)
study the effects of these horizons on performance under explicit
controllability assumptions. This provides a conceptual connection, not a
theorem that proves my push_swap configuration optimal. Those assumptions and
performance bounds have not been established for my solver. The connection
recognises the design question; it does not validate my preferred parameter
values or explain the cause of my measured results.

For a selected plan of d insertions, insertion j has d-j subsequent insertions
included in its evaluation. If I execute its first e insertions, the minimum
evaluated continuation among them is:

```math
\ell_j=d-j,\qquad
\ell_{\min}=\min_{1\le j\le e}(d-j)=d-e.
```

At depth eight, executing one, six, seven or eight gives minimum continuation
lengths of seven, two, one or zero respectively. This counts insertions along
the selected plan; it is not a count of all possible consequences or a guarantee
of decision quality. Near completion, the actual remaining insertion count
caps the horizon and final alignment is scored.

If my heuristic design requirement is "include at least r subsequent insertions
for every committed insertion", then:

```math
d-e\ge r
\quad\Longleftrightarrow\quad
e\le d-r.
```

**Conditional design constraint:** Choosing the largest batch satisfying my
chosen minimum continuation margin gives e=d-r. Thus the practical depth limit
d=8 and a chosen margin r=2 imply e=6. This justifies six **conditional on my
chosen two-insertion margin**; it does not prove that two is best. Executing
one satisfies a larger margin, but margin alone does not order complete-solution
quality.

#### Why frequent replanning can help or hurt

After one insertion from a depth-eight plan, its old suffix contains seven
insertions. Searching eight again extends the horizon. The effect is not
"diluted by one-seventh": insertions have unequal costs and selecting the
lowest-scoring path is a discrete decision. Even a small score change can
switch the winner when two alternatives are close.

For an illustration, consider two possible continuations from the same state:

| Continuation | Best seven-insertion prefix cost | Added cost of an eighth insertion | Eight-insertion total |
| --- | ---: | ---: | ---: |
| X | 7 | 20 | 27 |
| Y | 8 | 1 | 9 |

The newly included insertion reverses the preference. These are hypothetical
planning costs, not a measured push_swap trace. They demonstrate why the number
of added layers does not bound their influence. Costs beyond those eight
insertions remain unscored, so this reversal alone says nothing about which
continuation has the cheaper complete finish.

On a fixed continuation with insertion costs c_t, extending depth from d to
d+k accounts for this additional cost:

```math
R_d=\sum_{t=d+1}^{b}c_t,\qquad
R_d-R_{d+k}=\sum_{t=d+1}^{d+k}c_t.
```

**Accounting identity on a fixed path:** Here b is the number of remaining
insertions on that continuation; any final
alignment cost is separate and cancels in this difference. There is no general
inverse-depth or one-seventh law for remaining cost or estimation error.
If deeper search chooses a different continuation, even the fixed-path
comparison no longer describes the change in the final answer.

Execute-one does optimise the freshly extended finite-horizon objective at
every replan, but that is not the same as minimising the complete sort.
Executing six may preserve a useful sequence that repeated short-horizon
decisions would abandon; alternatively, execute-one may discover a better
route sooner. Both are possible. Search runtime is therefore not the only
reason to compare them: complete move count can also change in either direction.

"Thrashing" remains a hypothesis about harmful repeated plan switching, not an
established diagnosis. Discarded simulated moves were never emitted, and each
real insertion reduces B, so the solver does not cycle through identical full
states during reinsertion. A useful test logs plan changes and compares their
complete continuations from the same state; switching alone is not evidence
of harm. Nor does a smaller margin prove harm.

**Conditional optimal-suffix property:** With identical deterministic dynamics,
permitted plans and terminal scoring, reoptimising the old **remaining seven-step objective** cannot strictly improve
an exactly optimal seven-step suffix; otherwise the old eight-step plan was
not optimal. Equal-score alternatives can still change under tie-breaking.
The fresh **eight-step objective** is different. This distinction explains why
the suffix argument does not prove that repeated execute-one must win.

For a focused test, I would hold inputs, preparation, tie rules and code fixed while
varying execution length at depth eight, and record correctness, complete move
count, runtime and search work. My repeated live tests motivate partial
commitment, but the best execution fraction and the cause of any improvement
remain empirical questions. I am explaining the rationale here, not introducing
a solver change.



#### What this discussion establishes, and what remains open

| Statement | Status |
| --- | --- |
| Executing e insertions from a depth-d plan leaves d-e uncommitted | Structural identity, when the full horizon is available |
| Replanning at depth d after e executions extends the old horizon by e insertion positions | Structural identity, while enough work remains; element choices may change |
| Requiring at least r evaluated subsequent insertions gives e <= d-r | Consequence of a chosen design requirement, not proof that r is beneficial |
| An exactly optimal plan has an optimal suffix for its unchanged remaining objective | Conditional guarantee; ties may choose a different equally good suffix |
| A valid lower bound reaching the incumbent budget cannot improve that objective | Conditional pruning guarantee; does not establish complete-sort optimality |
| Frequent replanning causes harmful plan switching in my runs | Unconfirmed causal hypothesis |
| Eight/six beats eight/one or eight/eight in general | Not established |
| The published MPC guarantees apply to this solver | Not established |

My reason for pursuing eight/six at that stage was practical and experimental:
depth eight was affordable on my tested workload, partial commitment had promising
results I observed, and leaving two planned insertions uncommitted was an
intentional design choice. The mathematics makes that choice precise and
exposes its assumptions. It does not supply missing benchmark evidence,
establish the best margin, or turn the intuition into a new theorem.

### Opening lookahead versus continuing to look ahead (30 September 2026)

The historical sample recorded here, session `20260930_175350`, contains 11 checked
100-element inputs from master seed `10666114425917339200`, using one executable
version. The session was interrupted; these figures include completed runs only.

| Strategy | Average moves | Best of the four on how many inputs? |
| --- | ---: | ---: |
| Circular LIS + local greedy | 539.273 | 2 |
| 3-element seed + local greedy | 575.091 | 0 |
| Circular LIS + repeated lookahead | **516.727** | **7** |
| Circular LIS + opening lookahead + local greedy | 543.273 | 2 |

So opening lookahead followed by local greedy was inferior **on average in this
sample**, as I expected when abandoning future information after the opening.
But it is not useless: it beat LIS local on 6 of 11 inputs, lost on 5, and won
against all four strategies on 2 inputs. Its losses outweighed its gains. This
also fits the earlier saved example below where a single lookahead choice helped.

My working explanation / things to test next:

1. Future information helps when the score predicts the remaining sorting cost
   well enough to avoid a bad continuation. A deeper horizon exposes more moves,
   but does not guarantee a better complete solution: the unsearched tail still
   matters, and my cutoff currently scores that tail as zero.
2. I do not have to execute the entire saved path before looking again. Executing
   a prefix and replanning lets the next search see beyond the previous cutoff.
   Here I mean executing fewer **insertions from the winning path**, not executing
   competing branches on the real stacks. This may reveal a poor continuation,
   but does not undo insertions already executed or guarantee fewer total moves.
3. Circular LIS seems useful as expected: its local variant averaged 35.818 fewer
   moves than the 3-element seed with local reinsertion in this sample. Keeping
   more elements avoids push pairs, although rotations still affect the total.
4. Across my trials, the 3-element seed can sometimes outperform LIS. In this
   sample it beat LIS local twice, but never won overall. Circular LIS +
   repeated lookahead had the best average here; that is an observation,
   not a claim that it wins every input or every input size.

Keeping alternatives still pays: selecting the best recorded solution averaged
508.727 moves, versus 516.727 for always choosing repeated LIS lookahead. The
whole solver averaged 82.045 seconds per input (43.364–132.692 seconds); the logs
do not split runtime by algorithm, so they cannot tell me which variant consumed
how much of that time. Saturated trial counters are not exact search-work totals.

After that experiment, I compared algorithm 3's repeated lookahead against
two opening variants: algorithm 4 searches deeper once, executes one insertion,
then continues with repeated lookahead; algorithm 5 executes an opening batch
before that same continuation. Both use `use_lookahead = 1` in the final solver.
The older opening-then-local results above do **not** benchmark these variants;
the later profiled 100-value session does. My final settings are listed in the
seed-flow configuration table above; the normal size threshold uses total A+B
length, not remaining B length. These are tested configuration choices, not
fixed properties of the algorithms.


### Depth 3 can lose to local greedy — on the same input

On this [saved 500-rank input](tests/debug/old_results/2026-09-29_lookahead_horizon_input.txt),
I got these results with the same circular LIS preparation:

| Insertion strategy | Total moves, including preparation and final alignment |
| --- | ---: |
| Local greedy throughout | 4,979 |
| Depth-3 lookahead throughout, searching again after each insertion | 5,363 |
| Execute lookahead's first insertion, then use local greedy for the rest | **4,716** |

That last row surprised me. Lookahead's first choice actually helped: it saved
263 moves compared with local greedy throughout. But continuing to use lookahead
ended up costing 647 more moves than switching to local after that first choice.
The mixed strategy was a diagnostic experiment, not another configured algorithm.

At the first insertion, both choices immediately cost 5 moves. Local chose rank
451; lookahead chose rank 460. The best three-insertion total starting with the
local choice was 10, while the lookahead choice scored 9. So lookahead preferred
the cheaper short path, as intended.

The catch is that my depth cutoff returns zero for the unexamined continuation.
That means "ignore the rest", not "the rest is free". In that historical
repeated-lookahead experiment, every real insertion started a fresh depth-3 search. These results show that a useful first choice does not
make repeated short-horizon decisions produce a better complete solution. They
do not prove that deeper lookahead always loses, or rule out every possible bug.

Branch-and-bound pruning reproduced all three existing algorithms' original
move sequences exactly on this input. The pruned run took about 2.13 seconds on
the machine used for that test. The latest depth-3 comparison I reported was approximately
**20 minutes without pruning versus 2 seconds with pruning** (roughly 600x).
These are approximate observations for that test, not a controlled benchmark
or a guaranteed speedup on other inputs or machines.
For LIS lookahead, it skipped 11,077,063,485 of 11,080,556,820 potential candidate
evaluations. That improves search runtime without changing what the score favours.

The input was recovered from the terminal's complete move logs by reversing each
solution from sorted ranks. All three recorded solutions recovered the same
input. To rerun the currently configured algorithms:

```bash
make
ARG=$(cat tests/debug/old_results/2026-09-29_lookahead_horizon_input.txt)
./push_swap $ARG
```

A possible next experiment is a local-greedy rollout at the cutoff: estimate the
remaining cost by finishing a copied state with local greedy instead of returning
zero. That is not implemented here; it would trade extra computation for a score
that considers a complete solution.

<a id="future-research"></a>

## Future research — ideas not implemented

**Status: proposed experiments, not completed work or performance guarantees.**
These directions record my design ideas and discussions with AI, including my
earlier interest in weighted heuristics from machine learning. AI helped explain
related techniques and edit this discussion. No Optuna study, learned scoring
model, beam-search solver, or recursive multiple-run representation described
below has been implemented here. Existing BFS, exact tables and chunk experiments
are separate work.

### Tune planning depth, execution count and heuristic weights

I originally considered **Optuna** for tuning a floating-point weighted cost
function. The same tool could tune integer lookahead depth d and execution count
e, subject to 1 <= e <= d. Here depth counts insertions, not emitted instructions.

Optuna would call an objective function wrapping my existing Python benchmark
runner. A trial chooses one configuration, runs it on a fixed input suite,
checks correctness and returns a numerical result. A sampler such as TPE uses
previous trial results to suggest subsequent configurations; grid and random
search are alternatives. Trials can run sequentially or in parallel. Parallel
workers do not make the underlying single-threaded solver parallel.

The distinction from launching several Python runners is the coordination:
parameter selection, a persistent trial history and comparison against an
explicit objective. The runner still performs the actual measurements. Optional
early stopping of unpromising trials requires meaningful intermediate results;
it is not the solver's mathematically safe branch-and-bound pruning.

**Proposed benchmark specification, not a theorem:** for configuration theta and
fixed tuning inputs x_i, one possible objective is mean complete emitted moves:

```math
\theta=(d,e,\mathbf{w}),\qquad 1\le e\le d,
\qquad
J(\theta)=\frac{1}{N}\sum_{i=1}^{N}M(x_i;\theta).
```

Here M includes preparation, reinsertion and final alignment; weights are relevant
only if the proposed weighted score is implemented. Correctness is mandatory,
and runtime needs a stated budget. Invalid runs and timeouts must be recorded
rather than silently omitted. Different input sizes should be reported
separately, or their contribution to a combined objective chosen explicitly.

For a fair study, I would freeze the code, inputs, seeds and tie rules; compare depth
and execution count separately as well as jointly; then evaluate selected
settings on held-out inputs. I would record move-count spread, worst observed count and
runtime alongside the mean. Parallel trials need isolated builds if parameters
are compiled in, and CPU contention must not distort timing comparisons.

Optuna does not derive the universally best depth or prove eight/six superior.
It finds promising configurations empirically within the supplied search space,
data and compute budget. A small exhaustive grid is already useful for d and e;
adaptive tuning becomes more attractive when adding many weights.
See the official [Optuna sampler documentation](https://optuna.readthedocs.io/en/stable/reference/samplers/index.html)
and [sampling and trial-pruning explanation](https://optuna.readthedocs.io/en/stable/tutorial/10_key_features/003_efficient_optimization_algorithms.html).

### A weighted score and adaptive decisions

My proposed linear-algebra formulation combines features of a state or candidate
continuation using floating-point weights:

```math
H_{\mathbf{w}}(S)=\mathbf{w}^{\mathsf T}\boldsymbol{\phi}(S)
=\sum_{j=1}^{k}w_j\phi_j(S).
```

**Definition of a proposed score:** features could describe rotation work,
shared-rotation opportunities, disorder in B, increasing-run structure, or
estimated remaining insertion work. Feature scales must be made comparable.
Because A is already circularly increasing during current reinsertion,
circular-LIS disorder of A alone provides little discrimination there.
A lower disorder score is not necessarily a shorter route: useful operations
can temporarily make a state appear less sorted.

This score could order candidates or estimate the unsearched tail. It is not
automatically an admissible lower bound and must not replace the current safe
pruning bound without a separate justification. Hand-chosen weights are a
heuristic; fitting them from data would add a learning or tuning step.

Another unimplemented idea is **online adaptive weighting or strategy
switching**, using observed progress to change weights, depth or execution
length. Offline Optuna tuning and online adaptation are different mechanisms.
PID appeared in earlier AI discussions, but PID is feedback control, not itself
machine learning. Applying it would require a defined feedback signal, target
and adjustable quantity; no suitable controller or benefit has been established
for this solver. A simple explicit switching rule is a more concrete experiment
than claiming PID already explains the design.

### Learning to sort — an earlier joke and a possible experiment

I originally joked with friends about using machine learning or deep learning
to solve push_swap. [BrainTickle Experiments — *I evolved a sorting algorithm
instead of writing one*](https://www.youtube.com/watch?v=5veWaFjDe6s)
provides an illustrative proof of concept for evolving sorting behaviour,
although it does **not** demonstrate a push_swap solver.

**What the source reports:** the creator's video description says a small neural
network learns through evolution using left, right and swap controls. It reports
sorting by generation eight, with behaviour identified as gnome sort. Later
experiments add stopping, returning and longer-distance swapping, ultimately
producing behaviour identified as comb sort. These are the creator's reported
demonstrations; I have not independently reproduced them.

**My proposed connection, not an implementation:** I would like to investigate
whether a model could use a representation of the two stacks to choose the next permitted
push_swap operation, with learning or evolutionary selection guided by a chosen
performance objective. This records my earlier idea; the video's additional
controls are not extra operations available in push_swap. Evolutionary search,
reinforcement learning and deep learning were possibilities I considered, not
interchangeable names for a method verified in this video.

State encoding, model/weight storage, correctness, termination and emitted move
counts would all need investigation. Training cost and the cost of running a
trained model would need separate analysis; learning does not itself establish
a useful complexity bound or compliance with the project's requirements.
This remained an exploratory idea rather than part of my current implementation.

*I used AI to help summarise the creator's accessible description and explain
my proposed connection. I did not have a transcript for this check, so I have
not described a fitness formula, neural-network architecture or training
procedure beyond what that description supplies.*

### Alternative representations and search strategies

| Proposed direction | Motivation | Limitation to investigate |
| --- | --- | --- |
| Beam search | Retain a bounded number of promising partial plans at each layer, potentially permitting greater depth | Discarded paths may contain the best complete solution; quality depends on scoring and beam width |
| Recursive logical sub-stacks | Represent multiple increasing sequences in recursive structs instead of requiring one circularly increasing A | These are logical groups within the actual two stacks, not extra legal stacks; boundaries, rotations and eventual merging need new invariants and cost accounting |
| Bidirectional search / meet in the middle | Search from both endpoints for exact small-state solving or local replacement problems | Memory, predecessor generation and a correct meeting/stopping rule still matter |
| A* or pattern-database guidance | Use an estimate of remaining moves to guide exact search | Optimality requires the relevant admissibility and graph-search conditions; a plausible disorder score alone does not supply them |

BFS is **already implemented** in this project; extending its reach or combining
it with these techniques is the future direction. The credited
[aleksify research](https://github.com/aleksify/pushswap-research) discusses A*,
meet-in-the-middle and heuristic lookahead. Its current README also describes
implemented bidirectional local re-optimisation, so these are not uniformly
unimplemented in that author's work. They remain unimplemented extensions here.

### Two-way Turk-style planning

Another idea I seriously considered, but did not implement, was a two-way
Turk-style planner. Instead of restricting the reinsertion phase to B-to-A
transfers, it would consider candidates in both directions: elements of A
moving to B and elements of B moving to A. I envisaged retaining increasing
and decreasing sequences across the stacks, possibly using LIS/LDS ideas,
and recursively comparing transfer plans before eventually returning B to A.

The intended question was which ranks should temporarily belong in each stack,
and when a transfer in either direction would help the eventual sort. This
remains a design sketch: the ordering rules, scoring and termination conditions
were not fully worked out. Allowing both `pa` and `pb` removes the current
reinsertion phase's simple guarantee that every insertion reduces B, so a
future implementation would need to prevent unproductive back-and-forth
transfers. Considering both directions means comparing alternative next actions,
not executing two pushes simultaneously.

This is different from bidirectional BFS: that searches from start and goal
towards a meeting point. My proposed two-way Turk variant would plan transfers
between the two stacks within one evolving sorting state. Neither this proposal
nor the dream anecdote establishes correctness or improved move counts.

### Visualise the exact state graph

[2swap's Klotski video](https://www.youtube.com/watch?v=YGLNyHd2w10) helped me
see how exhaustive exploration and graph representations can illuminate a
concrete problem with simple movement rules. In the opening transcript
(approximately 0:07–0:42), the creator describes the puzzle's graph structure,
represents a configuration as a node, and explains that a move leads to another
node. That connection between a playable puzzle and a mathematical state space
was an inspiration for this project.

I wanted to visualise the exact push_swap state graph for small inputs:
each node would represent both stacks, and each edge a permitted operation.
My question was whether the arrangement of shortest paths to the sorted state
reveals recurring patterns worth investigating. This would visualise the
search space itself, rather than animate just one sorting sequence.

I did not have time to implement that visualisation. The existing exact-search
code and saved reports are separate from this proposed graphical exploration;
I am not claiming that I found such patterns or derived a pruning rule from
them. The video supplies the inspiration; applying it to push_swap records my
own proposed direction. I used a screenshot of the opening transcript when
discussing this with AI. I am citing that excerpt, rather than claiming to have
reviewed the full transcript for this explanation.

### Learn from exact small-state patterns

My exact-search studies and discussions with AI motivated questions about repeated move patterns and
relationships between search layers. The credited aleksify project also studies
reductions, equal-length paths reaching the same state, and growth between BFS
layers. Similar observations motivate investigation; they do not establish a
universal recurrence or justify extrapolating small-state behaviour to 500
elements.

A possible next step is to extract recurring subpatterns and verify their
equivalence under explicit stack-size and state conditions. Verified reductions,
duplicate-state handling or a sound canonicalisation could eliminate redundant
search. A correlation or common pattern alone cannot justify safe pruning.
Exact solutions for particular small states are not automatically universal
replacement rules for arbitrary larger stacks.

I also wanted to use **observed move frequencies** in exact solutions to guide
which paths to explore first, possibly through a queue mixing bounded deeper
exploration with broader exploration. This would be a frequency-guided,
potentially randomised search policy, not a completed BFS or DFS improvement.
The counting convention matters: one selected shortest path per state and all
shortest paths can produce different frequencies because of ties.

Ordering candidates within each BFS depth preserves its layer order; exploring
deeper states early changes that policy. Permanently dropping uncommon moves
can lose completeness or optimality. Small-state move frequencies may also
transfer poorly to large inputs or to insertion-level decisions. I would
therefore need to compare these ideas with the existing search under controlled
conditions before claiming that they improve it.

[↑ Back to top](#top)

## Repeated random tests

The Bash entry point uses `tests/run_random_tests.py` (Python 3 standard library)
for seeded generation, checking, logs and resume. It builds once with `make`.
No C solver changes or generator executable are required by this runner.

```bash
# 100 successful tests, 100 numbers each (the default size).
./push_swap_tester.sh -n 100

# Run until Ctrl-C, using 500 numbers per input.
./push_swap_tester.sh --size 500

# Also show the full solution debug dump on stderr; it is still saved.
./push_swap_tester.sh -n 100 --show-solutions

# Reproducible master seed; every attempt has its own generation ID.
./push_swap_tester.sh -n 100 --size 100 --seed 42
```

The runner prints its session directory and summary path. To resume a session:

```bash
# Replace this example directory with the session path printed by your run.
./push_swap_tester.sh --resume tests/debug/results/random_tests/YYYYMMDD_HHMMSS_output -n 100
```

On resume, `-n 100` means **100 additional successful tests**; omitting `-n`
continues until Ctrl-C. Seed and input size come from the saved session.
The generation ID advances for duplicate attempts too. Inputs are permutations
of `0..size-1`; SHA-256 hashes use normalised ranks, so different integer values
with identical relative order would deduplicate. Rotations remain distinct.
Small input sizes stop when every unique permutation has been tested.

Files live in the git-ignored `tests/debug/results/random_tests/` directory:

- `YYYYMMDD_HHMMSS_output_summary.md`: Markdown summary with averages/minimum/maximum tables, atomically replaced
  after every successful run. Includes master seed, next generation ID, move and
  runtime averages/min/max, per-algorithm move statistics and detail filenames.
- `YYYYMMDD_HHMMSS_output_000001.md`, etc.: Markdown reports with a ten-run overview table and one section per test.
  Each section includes the ranked input,
  seed, generation ID, rank hash, binary hash, captured settings, checker result,
  complete winning moves and full solution-debug dump in collapsible details.
  The current ten-run file is atomically refreshed after each success.

Thus 100 successful tests produce **one summary plus ten detail files**.
FD 1 is captured and passed to `checker_linux`; FD 2 remains visible for
progress; FD 3 captures the solution dump. `--show-solutions` also prints that
saved dump to stderr after the solver finishes. It does not change the C printer.

Only a zero-exit solver producing valid moves and a checker result of `OK` is
committed to the success log or averages. On failure the runner saves a separate
`*_failed.md` with the input and diagnostics, then stops. Ctrl-C terminates the
active solver/checker process group; completed records remain saved, and resume
retries the uncommitted input. Saved Markdown reports recover a stale summary after an interruption; atomic
replacement keeps the current batch intact. A session lock prevents concurrent writers.

Exact inputs remain replayable even if a Python version changes shuffle details.
Each session builds and snapshots an executable; its runs use that snapshot
and record its hash. Resuming after rebuilding is allowed, so summary averages
may span multiple binaries (listed in the summary). The metadata parser records the per-size and opening depth/limit macros,
`DEBUG`, the opening-lookahead switch, BFS skip switch and relevant capacities.
Older reports retain the settings that were actually captured at the time.


Variable-size sessions are available for comparing algorithms across input
sizes. `-random` chooses each size from a separately domain-separated SHA-256
seed derived from the master seed and generation ID; the existing deterministic
permutation generator then shuffles that many ranks. `-loop` cycles through
all sizes from `-min` to `-max`, inclusively, and wraps around. The default
bounds are 2 and 500; a larger maximum such as 600 is supported by the solver's
existing heap-backed path. Algorithms unavailable at a size produce no rows.

```sh
bash push_swap_tester.sh -random -max 500 --seed 42 -n 100
bash push_swap_tester.sh -loop -min 2 -max 600 --seed 42
```

The size modes and bounds are saved for resume. Variable-size sessions retain
repeated permutations, especially at small sizes, so the loop's size sequence
is not disrupted by deduplication. Fixed-size sessions retain their existing
unique-permutation behavior. The comparison table groups by input size as well
as executable and settings, and the summary includes a winning-result table by
size. Per-run rows support size-versus-time plots with separate algorithm lines.
SD and SEM are sample summaries, not confidence guarantees; repeated inputs
and changing machine load should be considered when interpreting them.
`python3 tests/test_size_modes.py` tests generation and reporting without
building or running a solver.

The runner now links a temporary profiling executable from the current solver
objects and `tests/profile_solver.c`. It wraps the three candidate entry points
with monotonic wall and process CPU clocks, then saves their encoded moves on
descriptor 4. This works with `DEBUG=0`; I do not need to change submission
settings or parse the timing of progress messages. The regular `push_swap`
executable is left alone. Sorted inputs have no algorithm rows, and skipped
candidates are absent rather than assigned zero time.

The summary includes an algorithm comparison table grouped by executable hash
and settings, plus one row per candidate per run. I can use those rows as a
later analysis dump without reopening the detailed batches. They include each
instruction count (`rr` and `rrr` included), forward/reverse/shared rotations,
wall and CPU time, process peak RSS, chosen wins, tied best results and the
strict top-band result. The comparison includes mean, sample standard deviation,
coefficient of variation, quartiles, median, P95, range and standard error,
plus average extra moves versus the best candidate. A single sample has no
sample SD or SEM. Missing measurements in older logs stay missing.

These timings cover candidate execution, excluding candidate initialization and
the wrapper's reporting. Total solver time includes process startup, parsing,
initialization and output, so I do not expect the two totals to match exactly.
The profiling build adds reporting overhead to total time. RSS is a process-wide
high-water mark at candidate completion, not each algorithm's private memory;
later algorithms inherit earlier peaks. The move counts describe emitted
instructions and can include no-ops. SD and percentiles describe this sample's
variability; they do not promise future results or establish asymptotic
complexity from one input size. The reference checker validates the winning
stream, not every candidate separately. `python3 tests/test_runner_profiles.py`
checks the instrumentation, statistics, Markdown round-trip and old-log parsing.

New sessions contain only Markdown reports: 100 tests still means 11 `.md` files.
The seed, hash and resume metadata are ordinary readable table rows; no hidden
JSON state file is needed. `--resume` also accepts older TXT/JSON sessions. Their
existing files are preserved, while new or updated reports use Markdown.
