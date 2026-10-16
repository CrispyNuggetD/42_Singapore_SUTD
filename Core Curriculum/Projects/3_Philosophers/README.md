*This project has been created as part of the 42 curriculum by hnah.*

# Philosophers

## Contents

1. [Description](#description)
2. [Instructions](#instructions)
3. [Resources](#resources)
4. [Appendix: my learning points](#appendix-my-learning-points)

## Description

A C project about threads, mutexes, and the dining philosophers problem.
The mandatory program will model philosophers eating, sleeping, and thinking,
while protecting shared forks and monitoring starvation.

This is a work in progress. Argument parsing, resource initialization,
and a thread scaffold exist. Meal setup and waiting are being developed;
the full eating cycle and monitoring are not implemented yet.
The mandatory sources, header, and Makefile are in `philo/`, as required by
subject version 13.0. Libft is not used.

This README is long because I learned way more than I expected from Philo lol.
The project description, build instructions, and resources come first; the
thread, pointer, mutex, and time rabbit holes are in the linked learning
appendix below.

## Instructions

Build from this folder with `make`, or use `make -C philo`.
The compiler uses `cc -Wall -Wextra -Werror -pthread`.
The executable is `philo/philo`.

`make clean` removes object files, `make fclean` also removes the executable,
and `make re` rebuilds. The root Makefile forwards these commands to `philo/`.

The eventual command format is:

```sh
./philo/philo number_of_philosophers time_to_die time_to_eat time_to_sleep \
    [number_of_times_each_philosopher_must_eat]
```

The current executable parses arguments and starts greeting threads; it does
not yet run the dining philosophers simulation.

## Resources

- Philosophers subject, version 13.0 (provided locally).
- Local manual pages: `man pthread_create`, `man pthread_mutex_init`,
  `man pthread_join`, `man gettimeofday`, and `man usleep`.
- AI assistance helped explain concepts and write these learning notes.
  It also helped implement argument parsing, initialization, the thread
  greeting scaffold, and cleanup. The full simulation is not implemented.

## Appendix: my learning points

Push_swap taught me to model state with structs, pointers, and arrays.
Pipex introduced processes, `fork()`, pipes, and `waitpid()`. In Philo I am
learning how threads share memory and how to coordinate their access to it.

These notes are separate from the mandatory project description and build
instructions above. Examples may be exercises or plans rather than implemented
simulation features. The numbered links jump to each learning point.

<a id="learning-points"></a>

1. [Threads](#learning-point-1)
2. [Mutexes](#learning-point-2)
3. [Semaphores](#learning-point-3)
4. [Deadlock and starvation](#learning-point-4)
5. [How I plan to build it](#learning-point-5)
6. [Do I need a static variable?](#learning-point-6)
7. [A process contains threads](#learning-point-7)
8. [Concurrency versus parallelism](#learning-point-8)
9. [Creating and joining a thread](#learning-point-9)
10. [Why I call `pthread_create()` for each philosopher](#learning-point-10)
11. [Why the argument is `void *`](#learning-point-11)
12. [Mutex lifecycle and critical sections](#learning-point-12)
13. [Why counter++ needs synchronization](#learning-point-13)
14. [Fork pointers: * versus **](#learning-point-14)
15. [Fork locks versus meal locks](#learning-point-15)
16. [Deadlock is different from starvation](#learning-point-16)
17. [Measuring time and sleeping](#learning-point-17)
18. [How my simulation structs fit together](#learning-point-18)
19. [Reading the manual: mutex initialization can fail](#learning-point-19)
20. [What mutexes do beneath the surface](#learning-point-20)
21. [Exercises before the full simulation](#learning-point-21)
22. [Thread handles are not the threads themselves](#learning-point-22)
23. [Passing a pointer does not give a thread private memory](#learning-point-23)
24. [Copy the mutex pointer, not the mutex object](#learning-point-24)
25. [NULL arguments: defaults, omitted inputs, and ignored outputs](#learning-point-25)
26. [Why I saw volatile in Minishell, but it is not a Philo lock](#learning-point-26)
27. [What atomic means](#learning-point-27)
28. [What the non-default pthread settings actually configure](#learning-point-28)
29. [Recursive mutex use case and the caller-held-lock alternative](#learning-point-29)
30. [Main as monitor versus a separate monitor thread](#learning-point-30)
31. [Why I use separate stop and printing locks](#learning-point-31)

<a id="learning-point-1"></a>

### 1. Threads

In Pipex, `fork()` created separate processes. Changing an ordinary variable
in the child did not change the parent's copy.

Threads run inside the same process and share its memory. Each thread has
its own stack for local variables and function calls, but if I pass two
threads a pointer to the same struct, both can access it.

| What I used in Pipex | What I will use in mandatory Philo |
| --- | --- |
| `fork()` to create a process | `pthread_create()` to create a thread |
| Separate memory spaces | Shared memory inside one process |
| Pipes for communication | Pointers to shared structs |
| `waitpid()` to wait for children | `pthread_join()` to wait for threads |

Each philosopher gets their own thread and repeats:

```text
take two forks -> eat -> release forks -> sleep -> think -> repeat
```

The OS decides when threads run. I cannot assume philosopher 1 runs before
philosopher 2 just because I created them in that order.

[Back to appendix index](#learning-points)

<a id="learning-point-2"></a>

### 2. Mutexes

A mutex is a lock: one thread can hold it at a time. The name means
"mutual exclusion". For mandatory Philo, each fork has its own mutex.
Locking that mutex means holding the fork; unlocking means putting it down.
If someone already holds it, another thread trying to lock it waits.

Mutexes also protect shared data. A philosopher updates their `last_meal`,
while the monitor reads it. Those accesses need synchronization to avoid
a data race, which the subject forbids.

```text
philosopher: lock -> update last_meal -> unlock
monitor:    lock -> read last_meal   -> unlock
```

Both must use the same mutex. Just putting a mutex in the struct does not
protect anything lol; the accesses actually have to follow the locking rule.

[Back to appendix index](#learning-points)

<a id="learning-point-3"></a>

### 3. Semaphores

A semaphore keeps a count of available resources. For example, a fork pool
could start with a count of 5:

- `sem_wait()` takes one permit and decreases the count. If none are
  available, it waits.
- `sem_post()` returns one permit and increases the count.

A mutex belongs to the thread that locked it, which must unlock it.
A semaphore does not have that same ownership rule.

The bonus uses a separate process per philosopher, so `fork()` comes back.
All forks are in a shared pool represented by a semaphore. Mandatory uses
threads and mutexes; semaphores are not in its allowed function list.

[Back to appendix index](#learning-points)

<a id="learning-point-4"></a>

### 4. Deadlock and starvation

If everyone takes their left fork first, they could all end up holding one
fork and waiting for their neighbour's fork. Nobody can eat or release
their forks. That is deadlock.

One approach is to number the forks and always lock the lower-numbered
fork before the higher-numbered fork. This prevents circular waiting.
It does not guarantee fairness: a philosopher could still keep missing
chances to eat. That is starvation.

One philosopher needs special handling. There is only one fork, so they
can take it but cannot eat. Trying to lock the same mutex twice is not
the solution; they wait until they die.

[Back to appendix index](#learning-points)

<a id="learning-point-5"></a>

### 5. How I plan to build it

Keep the data in structs and pass pointers to the threads:

| Shared simulation data | Each philosopher's data |
| --- | --- |
| Number of philosophers and timing arguments | ID and thread handle |
| Simulation start time | Pointers to their two forks |
| Fork mutex array | Last meal time and meal count |
| Stop flag and its mutex | Mutex protecting meal information |
| Output mutex and philosopher array | Pointer to the shared simulation |

The main thread can monitor the philosophers after creating their threads.
It checks whether `current_time - last_meal >= time_to_die`, and whether
everyone has completed the optional meal quota.

The death timer starts at the **start** of the last meal, not the end.
Before the first meal, it starts at the simulation start. The subject also
requires the death message within 10 ms of the actual death.

Printing needs coordination too: messages must not overlap, and normal
state messages should not slip out after the death message. Protect the
shared stop flag as well as the meal data.

My steps are:

1. Parse and validate arguments.
2. Define and initialize structs and mutexes.
3. Create threads, run a small routine, and join them.
4. Add fork acquisition and the eating, sleeping, thinking cycle.
5. Add protected meal tracking and death monitoring.
6. Add coordinated stopping, sleeps that can respond to stopping, and cleanup.

At the end, join the threads before destroying the mutexes and freeing
the data they use. Also handle partial initialization or thread creation
failures properly.

[Back to appendix index](#learning-points)

<a id="learning-point-6"></a>

### 6. Do I need a static variable?

My friends used one, but no, the subject does not require it.

A `static` variable inside a function keeps its value between calls.
Threads calling that function share that variable; it is not a separate
copy for each thread. Concurrent access can still need a mutex.

A file-scope `static` variable is still a global variable, even though
only that source file can name it. The subject forbids globals.

A `static` function is different: it just makes the function private to
its source file. It does not store shared simulation state.

I can keep things like the start time and stop flag in the simulation
struct instead. That makes it easier to see what is shared and which
mutex protects it.

[Back to appendix index](#learning-points)

<a id="learning-point-7"></a>

### 7. A process contains threads

A process is a running program with resources such as an address space and
open file descriptors. A thread is an execution path inside that process.
Each thread has its own instruction position, CPU registers, stack, and
scheduling state. Threads in the same process share its address space.

```text
One philo process
|
+-- Shared data: simulation, philosopher array, fork mutex array
|
+-- Main thread: monitors philosophers
+-- Philosopher thread 1: runs its routine
+-- Philosopher thread 2: runs its routine
+-- Philosopher thread 3: runs its routine
```

After `fork()` in Pipex, ordinary memory changes in the child did not change
that memory in the parent. With threads, passing the same pointer lets them
access the same object.

Each thread has its own stack, but this does not make stack objects
inaccessible to other threads. I can pass the address of a local variable
to a thread, provided the variable stays alive until that thread finishes
using it. Shared access still needs synchronization where appropriate.

[Back to appendix index](#learning-points)

<a id="learning-point-8"></a>

### 8. Concurrency versus parallelism

Threading does not require multiple CPU cores.

On one core, the scheduler can switch between threads:

```text
time ->
core 0: [philo 1][philo 2][main][philo 1][philo 3]
```

This is concurrency: their progress overlaps in time. On several cores,
threads can also execute simultaneously, which is parallelism:

```text
time ->
core 0: [philo 1........][main...........]
core 1: [philo 2........................]
core 2: [philo 3........................]
```

On Linux, POSIX threads are scheduled by the kernel. The scheduler assigns
runnable threads to available logical CPUs. Creating 200 threads does not
create 200 CPU cores. A logical CPU may also be a hardware thread sharing
resources with another logical CPU on the same physical core.

Threads sleeping or blocked on a mutex generally do not need to keep
executing instructions. In this simulation, eating means holding two forks
while time passes, not keeping a CPU busy with an eating calculation.

[Back to appendix index](#learning-points)

<a id="learning-point-9"></a>

### 9. Creating and joining a thread

A thread routine takes a `void *` argument and returns a `void *`:

```c
#include <pthread.h>
#include <stdio.h>

typedef struct s_worker
{
	int	id;
}	t_worker;

void	*routine(void *argument)
{
	t_worker	*worker;

	worker = argument;
	printf("Hello from worker %d\n", worker->id);
	return (NULL);
}

int	main(void)
{
	pthread_t	thread;
	t_worker	worker;

	worker.id = 1;
	if (pthread_create(&thread, NULL, routine, &worker) != 0)
		return (1);
	printf("Hello from main\n");
	if (pthread_join(thread, NULL) != 0)
		return (1);
	return (0);
}
```

Save that example separately as `demo.c` and compile it with:

```sh
cc -Wall -Wextra -Werror -pthread demo.c -o demo
```

| Argument to `pthread_create()` | Meaning |
| --- | --- |
| `&thread` | Where to store the new thread's handle |
| `NULL` | Default thread attributes |
| `routine` | Function the thread starts executing |
| `&worker` | Pointer passed to that function |

The API passes the pointer value; it does not copy the worker struct.
Either greeting can appear first. The new thread might execute before
main reaches its next statement.

`pthread_join()` waits for a joinable thread to finish. In this example,
it also keeps main's `worker` object alive while the thread uses it.
Returning from `main` terminates the process, including its other threads.
`pthread_detach()` is an alternative lifecycle choice: a detached thread's
resources are reclaimed when it finishes, and it cannot subsequently be
joined. Detaching does not make shared data safe to free early.

[Back to appendix index](#learning-points)

<a id="learning-point-10"></a>

### 10. Why I call `pthread_create()` for each philosopher

I already initialized an array of `t_philo` records in
`sim.resources.philos`. I need one thread per philosopher, and I pass each
thread its own existing record. This is what `start_philos()` currently does:

```c
res = &sim->resources;
while (res->threads_to_join < sim->config.count)
{
    next_philo = &res->philos[res->threads_to_join];
    if (pthread_create(&next_philo->thread_handle, NULL,
            philo_routine, next_philo) != 0)
        return (ERR_THREAD_CREATE);
    res->threads_to_join++;
}
```

`res` points to `sim->resources`; `next_philo` points to one record already
in that array. Neither pointer creates or copies a philosopher. I could
write the call without `next_philo`, but I would repeat the longer array
expression in both arguments:

```c
pthread_create(&res->philos[i].thread_handle, NULL,
    philo_routine, &res->philos[i]);
```

The two pointer arguments have different jobs:

| Argument | Meaning |
| --- | --- |
| `&next_philo->thread_handle` | Where `pthread_create()` stores the new thread's `pthread_t` handle |
| `next_philo` | The whole philosopher record passed to `philo_routine()` |

The handle lets cleanup join this thread later. The record lets the routine
read this philosopher's `id` and forks and follow its `sim` pointer to
shared settings. Passing only `sim` would give every routine the same
pointer without saying which philosopher it represents.

`sim->config.count` is the planned number of philosophers. The
`threads_to_join` counter starts at zero, selects the next record, and
increments only after `pthread_create()` succeeds. If creation fails
partway through, cleanup joins exactly the threads that started. Cleanup
then decrements the counter after each join. My routine currently prints
a greeting; the eating cycle is still to be written.

[Back to appendix index](#learning-points)

<a id="learning-point-11"></a>

### 11. Why the argument is `void *`

`void *` is a generic object pointer. The routine converts it back to the
appropriate pointer type. C permits this assignment without an explicit cast:

```c
void	*routine(void *argument)
{
	t_philo	*philo;

	philo = argument;
	/* Use philo->id, philo->sim, etc. */
	return (NULL);
}
```

There is only one argument slot, so a struct packages everything the thread
needs. A philosopher's pointer to the simulation gives access to shared data.

Do not pass every thread the address of the same changing loop variable:

```c
/* Wrong: all threads receive the same address. */
while (i < count)
{
	pthread_create(&threads[i], NULL, routine, &i);
	i++;
}
```

Instead, initialize separate philosopher structs before launching their
threads, then pass each thread its own element:

```c
pthread_create(&philos[i].thread_handle, NULL, routine, &philos[i]);
```

Check the return value in the actual implementation. The new thread can
start immediately, so its required data must already be initialized.

[Back to appendix index](#learning-points)

<a id="learning-point-12"></a>

### 12. Mutex lifecycle and critical sections

A mutex implements mutual exclusion. The protected region is called a
critical section:

```c
pthread_mutex_t	lock;

pthread_mutex_init(&lock, NULL);
pthread_mutex_lock(&lock);
/* Access protected data here. */
pthread_mutex_unlock(&lock);
pthread_mutex_destroy(&lock);
```

This snippet illustrates the order; actual code must handle operation
failures. Destroy the mutex only after no thread can still use it.
Do not copy an initialized mutex with assignment or `memcpy()`.

```text
Thread A                         Thread B
lock succeeds
                                 tries to lock
                                 waits...
access protected data
unlock
                                 lock succeeds
                                 access protected data
                                 unlock
```

The mutex does not know which variable it protects. All relevant accesses
must follow the agreed locking rule. A thread that ignores the mutex can
still access the variable and cause a race.

I initially checked every lock/unlock return because "it returns an error
code, so I should check it," lol. Then I learnt that with my successfully
initialized default mutexes, valid ownership, and correct lifetime, locking
normally succeeds or waits. A busy mutex does not make `pthread_mutex_lock()`
return an error. Those checks do not prevent deadlock or reliably catch
misuse such as touching an uninitialized or destroyed mutex.

So some of my error plumbing is more than this configuration needs. I kept
it as a starting template for explicit error propagation if I change the
mutex configuration later. That does not make it automatically future-proof:
a robust mutex can return `EOWNERDEAD` while granting ownership, so treating
every nonzero result as "I did not acquire the lock" would be wrong. I would
have to review the handling when changing the mutex type. See the
[POSIX lock documentation](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_mutex_lock.html).

The useful distinction I learnt: checks for initialization/resource failures
and releasing held locks on later errors still matter. Lock/unlock checks
are a separate design choice based on the mutex configuration and what the
caller guarantees. Writing more checks is not the same as preventing more
bugs; I need to understand what each operation can actually report.

I also reused my Push_swap `write_guaranteed()` idea for error messages.
Here it takes a file descriptor directly, without linking libft or using
my printf context. It keeps writing the remaining bytes after a partial
write, and returns `ERR_WRITE` if `write()` fails or makes no progress.
The name describes finishing partial writes; it cannot guarantee that a
broken output stream will work. If printing an error itself fails, I return
`ERR_WRITE` rather than recursively trying to print another error there.

[Back to appendix index](#learning-points)

<a id="learning-point-13"></a>

### 13. Why counter++ needs synchronization

An increment conceptually reads a value, calculates a new value, and writes
it back. Two threads could both read 0 and both write 1. More fundamentally,
unsynchronized conflicting accesses to an ordinary C object are a data race
and cause undefined behavior. A lost increment is only one possible symptom.

Here is a complete protected counter exercise:

```c
#include <pthread.h>
#include <stdio.h>

typedef struct s_shared
{
	int				counter;
	pthread_mutex_t	lock;
}	t_shared;

void	*increment(void *argument)
{
	t_shared	*shared;
	int			i;

	shared = argument;
	i = 0;
	while (i < 100000)
	{
		pthread_mutex_lock(&shared->lock);
		shared->counter++;
		pthread_mutex_unlock(&shared->lock);
		i++;
	}
	return (NULL);
}

int	main(void)
{
	t_shared	shared;
	pthread_t	first;
	pthread_t	second;

	shared.counter = 0;
	if (pthread_mutex_init(&shared.lock, NULL) != 0)
		return (1);
	if (pthread_create(&first, NULL, increment, &shared) != 0)
	{
		pthread_mutex_destroy(&shared.lock);
		return (1);
	}
	if (pthread_create(&second, NULL, increment, &shared) != 0)
	{
		pthread_join(first, NULL);
		pthread_mutex_destroy(&shared.lock);
		return (1);
	}
	pthread_join(first, NULL);
	pthread_join(second, NULL);
	printf("%d\n", shared.counter);
	pthread_mutex_destroy(&shared.lock);
	return (0);
}
```

Assuming the mutex and join operations succeed, this prints `200000`.
Main reads the counter after both joins, when neither worker can modify it.
The example handles initialization and creation failures; a full application
also needs an error policy for the remaining pthread operations.

[Back to appendix index](#learning-points)

<a id="learning-point-14"></a>

### 14. Fork pointers: * versus **

Threading does not require a pointer to a pointer. The type depends on the
memory layout. A flat array of fork mutexes only needs one pointer:

```c
pthread_mutex_t	*forks;

forks = malloc(sizeof(*forks) * count);
if (forks == NULL)
	return (1);
```

Initialize each element before use:

```c
i = 0;
while (i < count)
{
	pthread_mutex_init(&forks[i], NULL);
	i++;
}
```

If initialization fails partway through, destroy only the mutexes already
initialized and free the allocation. The snippet omits that cleanup to show
the array layout:

```text
forks -> [mutex 0][mutex 1][mutex 2][mutex 3]
```

`forks[i]` is a mutex object, so pass its address to lock it:

```c
pthread_mutex_lock(&forks[i]);
```

A philosopher can store pointers into that array:

```c
philos[i].left_fork_lock = &forks[i];
philos[i].right_fork_lock = &forks[(i + 1) % count];
```

Then use the stored pointer directly:

```c
pthread_mutex_lock(philos[i].left_fork_lock);
```

There is no extra `&` because `left_fork_lock` is already a mutex pointer.
For three philosophers, using array indices rather than displayed IDs:

```text
philo 0: forks 0 and 1
philo 1: forks 1 and 2
philo 2: forks 2 and 0
```

Neighbours refer to the same mutex object. Copying mutexes into separate
philosopher structs would not represent sharing the same fork.

A `pthread_mutex_t **forks` could instead point to an array of mutex pointers:

```text
forks -> [pointer][pointer][pointer]
             |        |        |
             v        v        v
           mutex    mutex    mutex
```

In that layout, `forks[i]` is already a pointer, so the call would be
`pthread_mutex_lock(forks[i])`. This is possible but unnecessary for a flat
array. Also, declaring `**forks` alone allocates neither the pointer array
nor the mutexes it would point to.

[Back to appendix index](#learning-points)

<a id="learning-point-15"></a>

### 15. Fork locks versus meal locks

A fork mutex stays locked for the whole time the philosopher holds that
fork, including eating. A meal mutex protects brief accesses to meal data:

```c
/* Philosopher */
pthread_mutex_lock(&philo->meal_state_lock);
philo->last_meal = now_ms();
pthread_mutex_unlock(&philo->meal_state_lock);

/* Monitor */
pthread_mutex_lock(&philo->meal_state_lock);
last_meal = philo->last_meal;
pthread_mutex_unlock(&philo->meal_state_lock);
```

`now_ms()` is a helper I write, not a library function. Do not hold the meal
mutex throughout eating: the monitor still needs to inspect meal data.
The stop flag also needs synchronized reads and writes.

For a reliable death decision, coordinate checking the deadline and updating
the stop state with meal updates; a copied timestamp can become stale if the
philosopher starts another meal before the monitor acts on it. Establish a
consistent lock order whenever an operation needs multiple mutexes.

[Back to appendix index](#learning-points)

<a id="learning-point-16"></a>

### 16. Deadlock is different from starvation

If everyone locks their left fork and then waits for their right fork,
a circular chain of waiting can form. This is deadlock.

Always acquiring forks by ascending fork index prevents this circular wait:

```text
needs forks 0 and 1 -> lock 0, then 1
needs forks 1 and 2 -> lock 1, then 2
needs forks 2 and 0 -> lock 0, then 2
```

Use indices to define the order. Relational comparisons between pointers to
unrelated allocations are not a valid general ordering strategy in C.
Preventing deadlock does not guarantee fairness or prevent starvation.

With one philosopher, the two pointers from the earlier mapping refer to
the same sole fork. Handle that case separately: take one fork, wait for
death, and release it rather than locking the same mutex twice.

[Back to appendix index](#learning-points)

<a id="learning-point-17"></a>

### 17. Measuring time and sleeping

How `gettimeofday()` works: Here I need milliseconds to measure philosopher timings.

The project arguments use milliseconds, while `usleep()` takes microseconds.
A basic conversion helper using `gettimeofday()` looks like this:

```c
long	now_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
		return (-1);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
}
```

I wrote the return formula as `tv.tv_sec * 1000L + tv.tv_usec / 1000`
because `gettimeofday()` splits one timestamp into two parts, while my
Philo timings need a single value in milliseconds:

| Part of the formula | What it contains | Why this conversion? |
| --- | --- | --- |
| `tv.tv_sec * 1000L` | Whole seconds since the Unix epoch | Multiply by 1,000: each second contains 1,000 milliseconds |
| `tv.tv_usec / 1000` | Remaining microseconds within the current second | Divide by 1,000: each millisecond contains 1,000 microseconds |
| `+` | The two converted parts | Add them to get the complete timestamp in milliseconds |

The `L` in `1000L` makes that integer literal a `long`; it is not a cast.
A cast would look like `(long)value`. Using `1000L` makes the multiplication
use at least `long` integer arithmetic, rather than relying on an `int`
literal. The exact resulting type also depends on the type of `tv_sec`.

For example, if `tv_sec` is `2` and `tv_usec` is `500000`, the formula means
`2 * 1000 + 500000 / 1000`, which gives `2500` milliseconds. I multiply the
seconds part but divide the microseconds part.

The division is integer division, so `500999 / 1000` gives `500`, not
`500.999`. This discards the fraction of a millisecond rather than rounding
it. The returned value is a timestamp; subtracting `sim->start_time` gives
elapsed milliseconds since my simulation started.

In `gettimeofday(&tv, NULL)`, `&tv` gives the function a place to store
seconds and microseconds since the Unix epoch. The second argument is an
old timezone-output parameter. `NULL` means no timezone output object is
provided; it does not mean "use my default timezone". It does not affect
the timestamp stored in `tv`.

So we need to keep `NULL` as the second argument for this project. Historically, some
systems accepted `&tz`, where `tz` was a `struct timezone`, to receive
system-wide timezone information. That interface is obsolete and its
behavior is not portable; GNU systems do not support it as a useful
source of timezone information. Obsolete means outdated, not necessarily
removed: the function can remain available while an old feature is no
longer useful. See the [GNU C Library time documentation](https://www.sourceware.org/glibc/manual/2.41/html_node/Getting-the-Time.html)
and [Gnulib portability notes](https://www.gnu.org/software/gnulib/manual/html_node/gettimeofday.html).

| Purpose | Function or call | Allowed in mandatory Philo? |
| --- | --- | --- |
| Get the current timestamp | `gettimeofday(&tv, NULL)` | Yes; convert the result to milliseconds |
| Measure elapsed time with a monotonic clock | `clock_gettime(CLOCK_MONOTONIC, &ts)` | No; useful outside this project |
| Convert a timestamp to local date/time | `localtime()` or `localtime_r()` | No; useful outside this project |

For Philo, use `gettimeofday(&tv, NULL)` and subtract the start timestamp
to calculate elapsed milliseconds.

The caller must handle `-1` rather than using it as a timestamp. This uses
`long` for the project's 64-bit Linux environment; do not assume it has
that width on every platform.

```c
timestamp = now_ms() - sim->start_time;
usleep(1000); /* Request approximately 1 millisecond. */
```

Sleeping does not guarantee execution resumes at exactly the requested time.
The scheduler may run the thread later. A long sleep also delays noticing a
stop request, so a simulation can use short sleeps while checking elapsed
time and the protected stop flag. An empty clock-checking loop wastes CPU.

The death countdown starts at the start of the last meal, or at simulation
start before the first meal. Eating does not suspend the countdown: a
philosopher can die while eating if that deadline is reached.

`gettimeofday()` measures wall-clock time, which can be affected by system
clock adjustments. General applications often use a monotonic clock for
elapsed time; this subject's mandatory function list allows `gettimeofday()`.

[Back to appendix index](#learning-points)

<a id="learning-point-18"></a>

### 18. How my simulation structs fit together

`t_sim` holds three groups so I can distinguish settings from state and
owned resources without guessing which function last touched a field.

| Group in `t_sim` | Contents | Functions that use it |
| --- | --- | --- |
| `config` (`t_config`) | Validated count, durations, and meal goal | `parse_input()` writes it; setup and threads read it |
| `runtime` (`t_runtime`) | Start timestamp, stop state, and print mutex | `init_sim()` sets the timestamp; stop/logging setup is still unfinished |
| `resources` (`t_resources`) | Fork/philosopher arrays and successful initialization/thread counters | Setup and thread creation update it; `cleanup_resources()` releases it |

For example, `sim.config.count` is a setting,
`sim.runtime.start_time` is state, and `sim.resources.philos` is an owned
array. Inside resource helpers, `res = &sim->resources` is a short pointer
alias to that same group; it does not copy it or allocate another object.

`init_sim()` reads the start time, allocates forks and philosophers,
initializes the fork mutexes, then initializes philosopher records and
meal mutexes. Cleanup uses the counters to handle partial initialization.

My realization: "what do I clean up if something errors here???" is exactly
what `t_resources` helps me answer lol. It holds the arrays I own and the
counts of successful setup steps. I do not need to guess how far a failed
function got or pass a growing list of unrelated pointers to cleanup.

| Resource information | What cleanup can do with it |
| --- | --- |
| `threads_to_join` and each `thread_handle` | Join the threads that successfully started |
| `meals_init_count` | Destroy that many initialized meal mutexes |
| `forks_init_count` | Destroy that many initialized fork mutexes |
| `philos` and `forks` | Free the allocated arrays after joining and destruction succeed |

The function name and parameter now say what it operates on:

```c
int cleanup_resources(t_resources *res, t_runtime *run, t_status status);

/* In main, after successful work or an earlier error: */
return (cleanup_resources(&sim.resources, &sim.runtime, status));
```

I pass `&sim.resources` because cleanup needs the address of my actual
resource group. One `*` is enough: it lets the function update counters
and set `res->philos` and `res->all_forks_mutexes` to `NULL`. A `t_resources **` would
be useful if the function needed to replace a caller's `t_resources *`
variable; that is not what this call does.

Cleanup joins threads before destroying mutexes and freeing the arrays.
It decreases counters only after each operation succeeds. If joining or
mutex destruction fails, it reports the error and retains the arrays.
The existing `status` tells it whether to report an earlier setup error
or return success after normal cleanup. As I add more owned resources,
I must also add their initialization tracking and cleanup handling.

[Back to appendix index](#learning-points)

<a id="learning-point-19"></a>

### 19. Reading the manual: mutex initialization can fail

`pthread_mutex_init(&mutex, NULL)` initializes the object at `&mutex`
with default attributes. It returns an `int`, even if my wrapper used to
return `void`. Ignoring that integer would hide a possible failure.

| Return value | Meaning | My wrapper's result |
| --- | --- | --- |
| `0` | The mutex was initialized successfully | Continue; return `STATUS_OK` after all philosophers are initialized |
| Nonzero error number | Initialization failed | Stop and return `ERR_MUTEX_INIT` |

The [POSIX manual](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_mutex_destroy.html)
includes resource failures such as `EAGAIN` and `ENOMEM`. Pthread functions
return their error number directly; I should check `!= 0`, rather than
assuming a `-1` return or looking in `errno`. My project enum groups these
failures under one message: "Error: Mutex initialization failed."

```c
if (pthread_mutex_init(&sim->resources.philos[i].meal_state_lock, NULL) != 0)
    return (ERR_MUTEX_INIT);
```

That is why `init_philos()` returns `t_status` now. In `main`, I save that
status and report failure before creating any philosopher threads:

```c
status = init_philos(&sim);
if (status != STATUS_OK)
    return (print_return_error(status) != STATUS_OK);
```

This check reports failure; it does not perform cleanup. If initialization
fails at index `i`, the earlier meal mutexes at indexes `0` through `i - 1`
remain initialized. Later cleanup work must track those successful
initializations and destroy only initialized mutexes. The philosopher
array and fork array must already be allocated, and fork mutexes initialized,
before calling this helper.

[Back to appendix index](#learning-points)

<a id="learning-point-20"></a>

### 20. What mutexes do beneath the surface

Mutex synchronization also establishes memory ordering and visibility.
Compilers and CPUs may reorder operations where permitted, and CPU cores
have caches. If one thread modifies protected data and unlocks a mutex,
a subsequent successful acquisition of that mutex lets another thread
observe those modifications through the synchronization relationship.

Neither of these replaces synchronization:

```c
volatile int	stop;
usleep(1000); /* Hoping another thread finished writing is not a lock. */
```

`volatile` does not make ordinary shared accesses thread-safe. Sleeping does
not establish a synchronization relationship.

On Linux, mutex implementations commonly use atomic operations for an
uncontended lock and a kernel mechanism called a futex when they need to
park a waiting thread. The pthread API handles those details for me.

[Back to appendix index](#learning-points)

<a id="learning-point-21"></a>

### 21. Exercises before the full simulation

1. Start two threads with separate IDs and join them.
2. Increment one shared counter from two threads under a mutex.
3. Let two threads compete for one mutex, briefly sleeping while holding it.

These isolate argument lifetime, shared memory, and locking. The full
project then combines fork ownership, meal timing, monitoring, logging,
and shutdown without data races or deadlock.

[Back to appendix index](#learning-points)

<a id="learning-point-22"></a>

### 22. Thread handles are not the threads themselves

I learnt from AI that a *handle* is apparently a technical term for a
value an API gives me so I can refer to something it manages later. For
threads, `pthread_create()` stores that value in a `pthread_t` variable;
I pass it to `pthread_join()` to wait for that same thread. It is not the
philosopher's `id`, a loop index, or necessarily a pointer to the thread.

On this Linux system, the system header defines `pthread_t` as an alias
for `unsigned long int`. That is a type definition, not a variable. Its
representation can differ on other systems, so I treat the handle as an
opaque value: I store and pass it back, but do not calculate a new one.

```c
pthread_t	*threads;

threads = malloc(sizeof(*threads) * count);
```

This allocates storage for handles; it does not start any threads.
`pthread_create()` writes a handle into that storage, and `pthread_join()`
uses the handle to identify the thread to wait for:

```c
pthread_create(&threads[i], NULL, routine, &philos[i]);
pthread_join(threads[i], NULL);
```

These are illustrative calls; actual code must check their results and
handle allocation failure. My philosopher struct already has a
`pthread_t thread_handle` member, so a separate handle array is unnecessary:

```c
pthread_create(&philos[i].thread_handle, NULL, routine, &philos[i]);
pthread_join(philos[i].thread_handle, NULL);
```

`thread_t` is not the POSIX type used here; the name is `pthread_t`.

[Back to appendix index](#learning-points)

<a id="learning-point-23"></a>

### 23. Passing a pointer does not give a thread private memory

The wrong loop-argument example passes `&i` to every thread. All arguments
point to one loop counter, not to separate philosopher objects:

```text
thread 0 argument --+
thread 1 argument --+--> one variable: i
thread 2 argument --+
```

Main increments `i` while the threads may read it. Even if no worker writes
anything, this can cause a data race. A worker may also read a later value
than the index at which it was created, including the loop's final value.

Passing `&philos[i]` instead gives each thread a different array element:

```text
thread 0 argument ----> philos[0]
thread 1 argument ----> philos[1]
thread 2 argument ----> philos[2]
```

Changing `philos[2].id` changes that element's member, not every
philosopher's ID. However, the threads still share the address space.
The argument selects their starting object; it does not restrict access.

If a thread receives `&philos[2]`, it can reach the previous element:

```c
t_philo	*philo;
t_philo	*previous;

philo = argument;      /* In this example, &philos[2]. */
previous = philo - 1;  /* Points to philos[1]. */
```

Pointer arithmetic moves by whole objects: subtracting 1 moves by one
`t_philo`, not one byte. It must stay within the same array or its
one-past-the-end position, which cannot be dereferenced. Subtracting 1
from `&philos[0]` goes outside those bounds and is invalid.

The shared simulation pointer can also give access to another philosopher:

```c
philo->sim->philos[1]
```

Having a valid pointer does not make concurrent access safe. Reading or
modifying another philosopher's changing meal data still requires the
mutex that protects it. Immutable data initialized before thread creation
can be read without adding a mutex for every read.

[Back to appendix index](#learning-points)

<a id="learning-point-24"></a>

### 24. Copy the mutex pointer, not the mutex object

A mutex contains synchronization state. Its implementation can track lock
state, ownership, and waiting threads. Copying its bytes does not create
a valid new synchronization object, and using that copy is undefined:

```c
pthread_mutex_t	a;
pthread_mutex_t	b;

pthread_mutex_init(&a, NULL);
b = a; /* Invalid way to create a usable second mutex. */
```

For two independent locks, initialize each object separately:

```c
pthread_mutex_init(&a, NULL);
pthread_mutex_init(&b, NULL);
```

For two philosophers sharing one fork, store pointers to the same object:

```c
philo_a.right_fork_lock = &forks[1];
philo_b.left_fork_lock = &forks[1];
```

```text
philo_a.right_fork_lock --+
                    +--> same mutex: forks[1]
philo_b.left_fork_lock ---+
```

Copying a mutex pointer is fine. Copying an initialized mutex with
assignment or `memcpy()` is not. This also applies indirectly when copying
a struct containing an initialized mutex:

```c
philos[1] = philos[0]; /* Also copies meal_state_lock: do not use that copy. */
```

Initialize the structs in their final storage, then initialize each mutex
there. Keep that storage alive until all threads using it have finished.

[Back to appendix index](#learning-points)

<a id="learning-point-25"></a>

### 25. NULL arguments: defaults, omitted inputs, and ignored outputs

`NULL` is a null pointer constant: it represents a pointer that does not
point to an object or function. It does not universally mean "default".
The function's contract decides whether a particular argument may be NULL
and what that means. Dereferencing a null pointer is undefined behavior.

| Example | Meaning of this particular NULL |
| --- | --- |
| `pthread_create(&thread, NULL, routine, &philo)` | Use default thread attributes |
| `pthread_mutex_init(&mutex, NULL)` | Use default mutex attributes |
| `pthread_join(thread, NULL)` | Wait, but do not collect the routine's returned pointer |
| `pthread_create(&thread, NULL, routine, NULL)` | Also pass a null argument pointer to the routine |
| `return (NULL)` in a thread routine | Return no result object |
| `gettimeofday(&tv, NULL)` | Store the time in `tv`; provide no timezone output object |

#### Default thread attributes

```c
pthread_create(&thread, NULL, routine, &philo);
```

The second parameter is `const pthread_attr_t *attr`. An attributes object
configures properties such as stack size, scheduling settings, and whether
the thread starts joinable or detached. NULL requests the default attributes;
the thread is joinable by default. Exact stack sizes depend on the environment.
It does not mean "use no stack" or "skip creating the thread".

Custom attributes normally involve `pthread_attr_init()` and related
setters. Those functions are not in this subject's allowed list, so the
project can use NULL here. This is a deliberate API option, not an omitted
argument: C still requires all four arguments.

The fourth parameter is different: it is the pointer delivered to the
routine. If I pass NULL there, the routine must not dereference it:

```c
void	*routine(void *argument)
{
	(void)argument; /* This example needs no input. */
	return (NULL);
}

/* Two NULLs, with different meanings. */
pthread_create(&thread, NULL, routine, NULL);
```

Passing NULL as the routine itself is not a request for a default routine.
Passing NULL instead of `&thread` is not an option to ignore the handle.
These parameters require valid values.

#### Default mutex attributes

```c
pthread_mutex_init(&mutex, NULL);
```

The second parameter is `const pthread_mutexattr_t *attr`. NULL selects
default attributes, including a process-private mutex and the default mutex
type. It does not make the mutex pointer itself NULL: `&mutex` still refers
to the actual object being initialized.

Do not assume the default mutex supports recursive locking. Our code must
not lock a mutex it already holds, unlock someone else's mutex, or unlock
an unlocked mutex. Custom mutex attributes use other APIs not listed for
mandatory Philo.

#### Ignoring a thread's returned pointer

```c
pthread_join(thread, NULL);
```

The second parameter is `void **retval`: an optional address where the API
can store the routine's returned pointer. NULL means I do not want that
output. Joining still waits for the thread and performs the join operation;
it does not detach, cancel, or kill the thread.

To collect a result instead:

```c
void	*result;
int		error;

error = pthread_join(thread, &result);
if (error != 0)
	return (1);
/* result now contains the pointer returned by the routine. */
```

This explains the pointer-to-pointer: `result` is a pointer, and the
function needs its address to change it. The integer returned by
`pthread_join()` is the API's success/error code; it is separate from the
worker's returned pointer.

A routine can return its input pointer when that object remains alive:

```c
void	*routine(void *argument)
{
	return (argument);
}
```

Do not return the address of a routine's ordinary local variable: its
lifetime ends when the routine returns. If returning heap-allocated data,
agree on who frees it. Joining does not automatically free such data,
even when the result argument is NULL.

#### Reading a prototype instead of guessing

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine)(void *), void *arg);
int pthread_mutex_init(pthread_mutex_t *mutex,
	const pthread_mutexattr_t *attr);
int pthread_join(pthread_t thread, void **retval);
```

The prototype tells me the types. The manual tells me which pointers are
optional and what NULL means for each. The local references for this section
are `man pthread_create`, `man pthread_mutex_init`, and `man pthread_join`.
A pointer parameter alone does not imply that NULL is accepted.

[Back to appendix index](#learning-points)

<a id="learning-point-26"></a>

### 26. Why I saw volatile in Minishell, but it is not a Philo lock

I read other Minishell teams' code and saw `volatile` used for signal
handling. That made me wonder whether it could also protect Philo's stop
flag. These are different situations.

`volatile` tells the compiler that accesses to an object are observable
and its value may change outside the ordinary execution path. For example,
it prevents the compiler from treating repeated volatile reads as one
cached value. It does not make an operation atomic, establish cross-thread
memory ordering, or make conflicting thread accesses safe.

A common signal-handling declaration is:

```c
volatile sig_atomic_t	g_signal;
```

`sig_atomic_t` comes from `<signal.h>`. It is an integer type that can be
accessed as an atomic entity in the presence of asynchronous interrupts.
A minimal signal handler can record the signal number:

```c
/* Signal-handling illustration, not code for mandatory Philo. */
#include <signal.h>

volatile sig_atomic_t	g_signal;

void	handle_signal(int signal_number)
{
	g_signal = signal_number;
}
```

The handler can interrupt normal execution, so ordinary assumptions about
when a variable changes do not apply. `volatile sig_atomic_t` is the usual
combination for a simple flag written by the handler and read by normal
code. This does not make arbitrary handler work safe, nor does it make a
read-modify-write operation such as `g_signal++` safe. Signal handlers also
have restrictions on which functions they can call; `printf()` and mutex
locking are not appropriate substitutes for simply recording a flag.

This example explains a pattern I saw; it is not a claim that every
Minishell team's signal handling is correct. A bare `volatile int` is not
the same portable signal-handling guarantee as `volatile sig_atomic_t`.

For Philo, a monitor thread and philosopher threads execute independently:

```c
volatile int	stop;

/* Monitor thread */
stop = 1;

/* Philosopher thread */
if (stop)
	return (NULL);
```

Those unsynchronized accesses can race. `volatile` does not fix that,
even if an individual integer load or store happens to be indivisible on
this CPU. `sig_atomic_t` is not a replacement for thread synchronization
either. Hardware atomicity alone does not satisfy C's thread memory rules.

The proposed Philo design uses the same mutex for every concurrent read
and write of the stop flag:

```c
/* Monitor thread */
pthread_mutex_lock(&sim->stop_mutex);
sim->stop = 1;
pthread_mutex_unlock(&sim->stop_mutex);

/* Philosopher thread: read a snapshot under the same lock. */
pthread_mutex_lock(&sim->stop_mutex);
stopped = sim->stop;
pthread_mutex_unlock(&sim->stop_mutex);
if (stopped)
	return (NULL);
```

The local `stopped` snapshot can be used after unlocking, but the shared
flag may change afterward. Operations such as deciding whether to print
need their own coordinated protocol with stopping; a snapshot alone does
not prevent a message from slipping out after death.

C atomic types are another general tool for thread-safe flags, but this
project's proposed implementation uses the allowed pthread mutex API.
The signal example uses a global to illustrate the Minishell pattern;
Philo's subject forbids globals, so its state stays in the simulation struct.

[Back to appendix index](#learning-points)

<a id="learning-point-27"></a>

### 27. What atomic means

An atomic operation is indivisible with respect to other relevant operations:
other threads cannot observe it half-completed. This does not mean the CPU
runs only that thread, or that the operation necessarily takes one machine
instruction. The guarantee is about how the operation can be observed.

An ordinary increment is not guaranteed atomic:

```c
counter++;
```

Conceptually it reads the old value, adds one, and writes the new value.
Two threads can both read 0 and both write 1. In C, unsynchronized
conflicting accesses to an ordinary shared object also cause a data race
and undefined behavior; that interleaving is only an illustration.

With the mutex approach used in this project:

```c
pthread_mutex_lock(&lock);
counter++;
pthread_mutex_unlock(&lock);
```

Only one cooperating thread enters that critical section at a time.
The increment is protected against other accesses using the same lock;
it does not magically become an atomic C object, and an unlocked access
can still cause a race.

C also provides atomic objects through `<stdatomic.h>`. This separate
learning example shows an atomic read-modify-write operation:

```c
#include <stdatomic.h>

atomic_int	counter;

atomic_init(&counter, 0); /* Initialize before threads use it. */
atomic_fetch_add(&counter, 1); /* Add one as a single atomic operation. */
```

If two threads each perform that addition once, the value becomes 2,
assuming no other modifications. The operations do not lose an increment.
These default C atomic operations also provide sequentially consistent
ordering. Other ordering modes exist, but atomicity and ordering are
separate ideas; choosing weaker ordering requires additional reasoning.
This example is theoretical background, not a proposal to add atomic APIs
to the subject's allowed function list.

Even with an atomic counter, two separate operations are not automatically
one indivisible transaction:

```c
if (atomic_load(&counter) > 0)
	atomic_fetch_sub(&counter, 1);
```

Two threads could both observe 1 and both subtract, leaving -1. Each
operation is atomic, but the combined check-and-subtract is not. A mutex
around the complete decision and update, or an appropriate atomic algorithm,
is needed when those steps must act as one unit.

An atomic operation may use special CPU instructions. Some atomic types
or operations require implementation-provided locks instead: atomic does
not necessarily mean lock-free. Linux mutex implementations themselves use
atomic operations internally to coordinate ownership of the lock.

For my Philo design, mutexes protect shared meal data, the stop flag,
and compound decisions. `volatile` does not provide these guarantees.
The earlier `sig_atomic_t` discussion concerns simple signal-handler
accesses, not a general guarantee for synchronization between threads.

[Back to appendix index](#learning-points)

<a id="learning-point-28"></a>

### 28. What the non-default pthread settings actually configure

`pthread_t` is a thread handle, not its settings object. The settings types
are `pthread_attr_t` for thread creation and `pthread_mutexattr_t` for mutex
initialization. The second argument of each creation/initialization call
can point to the corresponding initialized settings object instead of NULL.

The attribute initialization and setter functions discussed below are not
in subject v13.0's allowed function lists for mandatory or bonus. They are
background knowledge; this project uses default attributes. A default
joinable thread can later be detached with the allowed `pthread_detach()`,
although joining is more convenient for this proposed cleanup design.

#### Thread attributes

| Setting | What it changes | Relevance to Philo |
| --- | --- | --- |
| Detach state | Joinable threads can be joined; detached threads cannot and release their thread resources automatically when finished | Default joinable threads let main wait before freeing shared data |
| Stack size | Space available for the thread's call stack | Default is sufficient for these small routines; avoid large local arrays and deep recursion |
| Stack address | Uses caller-provided memory for the thread stack | Unnecessary; introduces alignment and lifetime responsibilities |
| Guard size | A protected region intended to detect stack overflow | Keep the implementation's default; caller-provided stacks need special care |
| Scheduling inheritance | Inherit the creator's scheduling policy/parameters, or use explicitly configured ones | Default inheritance is sufficient |
| Scheduling policy | Chooses policies such as ordinary scheduling, FIFO, or round-robin real-time scheduling | Do not use real-time scheduling to compensate for simulation bugs; permissions and platform support vary |
| Scheduling priority | Priority under the selected scheduling policy | Ordinary Linux scheduling does not provide arbitrary real-time priorities through this field |
| Contention scope | Whether scheduling competition is system-wide or within a process | Platform-dependent; Linux supports system scope, not process scope |

Linux also has a non-portable affinity attribute API to restrict which CPUs
a thread may run on. Pinning threads is unnecessary for Philo, and its API
is not allowed by the subject. Creating threads does not assign each one a
permanent CPU core.

The following illustrates configuring a detached thread in a general program,
not permitted project code. Error handling is omitted here to show the calls:

```c
pthread_attr_t	attributes;
pthread_t		thread;

pthread_attr_init(&attributes);
pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
pthread_create(&thread, &attributes, routine, argument);
pthread_attr_destroy(&attributes);
```

Destroying the attributes object after creation does not destroy the thread:
the object describes creation settings. A detached thread still requires a
safe shared-data lifetime protocol; it cannot be joined afterward.

#### Mutex attributes

| Setting | What it changes | Relevance to Philo |
| --- | --- | --- |
| Type: default | Implementation-defined default mutex behavior within POSIX rules | Use disciplined lock/unlock ownership; do not assume recursive or error-checking behavior |
| Type: normal | Locking again from the owner deadlocks; incorrect unlocks have undefined behavior | Fork locks should never rely on double-locking |
| Type: error-checking | Detects certain misuse, such as self-locking or unlocking without ownership, through error returns | Useful in general debugging, but the attribute setter is not allowed here |
| Type: recursive | Owner may lock repeatedly; each successful lock needs a matching unlock | Does not fix the dining-philosophers deadlock between different threads |
| Process sharing | Process-private, or usable between processes when placed in suitable shared memory | Mandatory shares one process; bonus uses semaphores rather than shared mutexes |
| Robustness | A robust mutex can report that its previous owner died while holding it | Requires recovery logic; unnecessary for the proposed design |
| Priority protocol | Priority inheritance or priority ceilings can address certain scheduling problems | Advanced real-time concerns; not needed here |

For a robust mutex, acquiring it after owner death may return `EOWNERDEAD`
while also giving the caller ownership. The caller must repair protected
state and mark it consistent using additional APIs. It is not an automatic
"recover from any crash" feature.

A process-shared mutex is not created merely by copying its bytes after
`fork()`: the mutex must reside in memory genuinely shared between the
processes and be initialized appropriately.

For example, a general program could select an error-checking mutex:

```c
pthread_mutexattr_t	attributes;
pthread_mutex_t		mutex;

pthread_mutexattr_init(&attributes);
pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_ERRORCHECK);
pthread_mutex_init(&mutex, &attributes);
pthread_mutexattr_destroy(&attributes);
```

Again, these attribute APIs are background examples outside the project's
allowed list. With Philo's defaults, correct ownership, consistent lock
ordering, and return-value handling remain my responsibility.

#### Other NULL positions are not settings

`pthread_join(thread, NULL)` has no attributes parameter. Its NULL means
"do not collect the routine's returned pointer". Passing `&result` collects
that pointer instead. Likewise, the fourth argument of `pthread_create()`
is the routine's input pointer, not a configuration object.

[Back to appendix index](#learning-points)

<a id="learning-point-29"></a>

### 29. Recursive mutex use case and the caller-held-lock alternative

A recursive mutex can help when a function already holding a lock calls
another function that also acquires the same lock. For example, a document
API might have both an append-text operation and an append-paragraph operation:

```c
/* Illustration: this nesting requires a recursive mutex. */
void	append_text(t_doc *doc)
{
	pthread_mutex_lock(&doc->mutex);
	/* Modify the document. */
	pthread_mutex_unlock(&doc->mutex);
}

void	append_paragraph(t_doc *doc)
{
	pthread_mutex_lock(&doc->mutex);
	append_text(doc);
	append_text(doc);
	pthread_mutex_unlock(&doc->mutex);
}
```

With a normal mutex, the inner lock waits for a mutex already held by the
same thread. That thread cannot reach the outer unlock, so it deadlocks.
A recursive mutex instead counts the owner's successful acquisitions:

```text
outer lock:    count 1
inner lock:    count 2
inner unlock:  count 1 -- still owned by this thread
outer unlock:  count 0 -- another thread can acquire it
```

Each lock needs a matching unlock. Other threads cannot acquire the mutex
until the owner releases its final acquisition. This only handles repeated
locking by the same owner; it does not fix circular waiting between threads.

An alternative is to separate the actual work from the public operation
that acquires the lock:

```c
/* Caller must already hold doc->mutex. This helper does not lock. */
static void	append_text_locked(t_doc *doc)
{
	/* Modify the document. */
	(void)doc; /* Placeholder until the actual work is written. */
}

void	append_text(t_doc *doc)
{
	pthread_mutex_lock(&doc->mutex);
	append_text_locked(doc);
	pthread_mutex_unlock(&doc->mutex);
}

void	append_paragraph(t_doc *doc)
{
	pthread_mutex_lock(&doc->mutex);
	append_text_locked(doc);
	append_text_locked(doc);
	pthread_mutex_unlock(&doc->mutex);
}
```

Now each top-level operation locks only once. The helper runs inside the
caller's existing critical section, so its work is still protected without
acquiring the mutex again. Keeping the lock across both append operations
also prevents another cooperating thread from modifying the document between
them. Unlocking before each nested public call would lose that guarantee.

The helper's contract matters: calling `append_text_locked()` without holding
the mutex would leave its accesses unprotected. `static` limits which source
file can call it, but does not enforce ownership or provide synchronization.
The `_locked` suffix documents the precondition; it is not a C language feature.
These snippets illustrate structure, with operation error handling omitted.

For Philo, the same pattern can be useful for internal helpers that update
protected state while their caller already owns the relevant mutex. It does
not mean moving all locks to main: the thread performing the operation acquires
the lock before calling the helper and releases it afterward. Fork ownership
still lasts through eating. Recursive locking cannot turn the sole fork in
the one-philosopher case into two forks, and recursive attribute setters are
not in this subject's allowed list.

[Back to appendix index](#learning-points)

<a id="learning-point-30"></a>

### 30. Main as monitor versus a separate monitor thread

I thought a "monitor" had to be another thread, but main is already a
thread lol. After it creates the philosopher threads, it can run the
monitor loop itself. Creating another monitor thread is another valid
design; the difference is where the monitoring function runs.

| Design choice | Main monitors (my current design) | Separate monitor thread |
| --- | --- | --- |
| Execution paths for N philosophers | N philosopher threads plus main | N philosopher threads, a monitor thread, and main |
| What main does | Starts philosophers, monitors, then joins and cleans up | Starts philosophers and monitor, then coordinates joins and cleanup |
| Main advantage | Uses the existing main thread; fewer handles and failure paths to manage | Leaves main available for other orchestration while monitoring runs independently |
| Main tradeoff | Main cannot block on another task while it is responsible for death checks | Extra thread creation, stack resources, a handle, and another join to manage |
| Startup failures | Handle partial philosopher creation | Also handle monitor creation failing after philosophers have started |
| Shared-data protection | Lock meal data, stopping, and logging as appropriate | Needs the same protections; a separate monitor does not remove races |
| Death-detection timing | Depends on scan duration, polling pauses, lock contention, and scheduling | Depends on those same factors; an extra thread does not guarantee faster detection |

For my implementation of this project, I chose main as the monitor because
it is the better fit: once philosopher threads are started, main has no
other job that needs to run concurrently with monitoring. Joining and cleanup
come afterward. Creating a separate monitor just to leave main waiting would
add a thread, a handle, a creation-failure path, and a join without adding a
capability I need. Main is already there, so I give it the monitoring work lol.
That makes this design simpler and less redundant for my program; it is a
design choice, not a requirement that every Philo implementation use main:

```text
main: initialize -> create philosophers -> monitor -> join -> cleanup
philosopher threads: run their routines concurrently
```

With a separate monitor, a possible arrangement is:

```text
main: initialize -> create philosophers -> create monitor -> join -> cleanup
monitor thread: check death/quota -> publish stopping -> return
philosopher threads: run their routines concurrently
```

Either design needs a stop request on startup/runtime errors so joins do
not wait forever for routines that have no reason to finish. All threads
that might touch the shared objects must finish and be joined before I
destroy those objects. The extra monitor's creation can also fail from
resource shortages, as documented in the
[pthread_create manual](https://www.man7.org/linux/man-pages/man3/pthread_create.3.html).

Neither design creates a dedicated printing thread. A philosopher calls
`log_philo_state()` for its own messages; the thread running the monitor
calls it for death. They execute the same logger on their own threads,
and `printing_lock` lets one through at a time. A state struct stores data;
it does not execute the monitor or replace a thread.

These are design tradeoffs, not a claim that either layout automatically
satisfies the subject's timing requirements. My complete lifecycle and
shutdown handling still need to be finished and tested.

[Back to appendix index](#learning-points)

<a id="learning-point-31"></a>

### 31. Why I use separate stop and printing locks

At first glance I thought I only needed one mutex for stopping and printing.
Then I realized my philosophers check stopping much more often than they
print anything lol. During a 200 ms eating wait, `philo_wait()` repeatedly
checks the flag between short sleeps. Those checks do not print messages.
If stopping is requested, the philosopher releases its forks and returns
without printing another normal state message.

One mutex could protect both the flag and output correctly. My two-lock
solution separates a brief flag check from potentially slower output:

| Lock | Work while it is held | Why keep it separate? |
| --- | --- | --- |
| `all_stop_lock` | Copy or update the integer stop flag | Frequent stop checks need only a short critical section |
| `printing_lock` | Coordinate state logging, read the time, and call `printf()` | Output can take longer or block, especially when its destination is slow |

If both jobs shared one mutex, a tiny stop check could queue behind an
output operation. With two, a philosopher can check the stop flag while
another thread holds `printing_lock`, except during that logger's brief
access to `all_stop_lock`. It does not need to wait for the whole output
operation to finish. That is the practical reason for keeping two locks.

"Fast mutex flag" is my intuition, but the mutexes themselves are the same
kind. The difference is the work inside their critical sections. Even the
brief flag check can wait because of contention or scheduling; I have not
benchmarked a speed difference. This design avoids tying that check to
output latency rather than guaranteeing a particular response time.

I would not call the output mutex `printing_block`: "block" could mean a
code block or a blocked thread, and does not tell me its expected duration.
`printing_lock` describes its job. `all_stop_lock` describes the flag it
protects; `stop_flag_lock` would also be clear. I can explain their different
hold times in comments without naming one "fast" and the other "slow".

The logger still acquires locks in the order `printing_lock` then
`all_stop_lock`, releases the stop lock, and completes output before
releasing the print lock. Once death publishes the stop flag, wait loops
can observe it while the death message is being printed. Publishing death
itself can still wait to acquire `printing_lock`; two locks do not make a
blocked output destination harmless or guarantee the death-message deadline.

[Back to appendix index](#learning-points)
