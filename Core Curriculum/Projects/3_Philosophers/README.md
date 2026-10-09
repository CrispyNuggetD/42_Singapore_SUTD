*This project has been created as part of the 42 curriculum by hnah.*

# Philosophers

## Description

A C project about threads, mutexes, and the dining philosophers problem.
The mandatory program will model philosophers eating, sleeping, and thinking,
while protecting shared forks and monitoring starvation.

This is an initial scaffold only. `philo/src/main.c` contains an empty main
function; argument parsing and the simulation are not implemented yet.
The mandatory sources, header, and Makefile are in `philo/`, as required by
subject version 13.0. Libft is not used.

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

The current empty executable does not process arguments or run a simulation.

## Learning notes: coming from Pipex

I did Pipex, so `fork()`, processes, pipes, and `waitpid()` are familiar.
Philo introduces threads and shared memory. These are my planning notes;
the simulation is still not implemented.

### Threads

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

### Mutexes

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

### Semaphores

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

### Deadlock and starvation

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

### How I plan to build it

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

### Do I need a static variable?

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

## Resources

- Philosophers subject, version 13.0 (provided locally).
- Local manual pages: `man pthread_create`, `man pthread_mutex_init`,
  `man pthread_join`, `man gettimeofday`, and `man usleep`.
- AI assistance was used to prepare the directory structure, Makefiles,
  header imports, empty main function, shell project setting, and ignore rules.
  It was also used to explain the Makefiles and discuss threads, mutexes,
  semaphores, static variables, and an implementation plan in relation to
  my completed Pipex project. Those explanations were adapted into the
  learning notes and the theoretical background appendix, including standalone
  thread and mutex examples. No simulation implementation has been generated.

## Appendix: theoretical background

These are lecture notes on what I learnt in this project, coming from
Push_swap and Pipex. Push_swap taught me to model state with structs,
pointers, and arrays. Pipex introduced independently running processes.
Philo combines those ideas: several execution paths access the same data,
and I have to coordinate those accesses.

The examples below are separate learning exercises or illustrative snippets,
not a complete simulation or a statement that these features are implemented.

### A process contains threads

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

### Concurrency versus parallelism

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

### Creating and joining a thread

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

### Why the argument is `void *`

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
pthread_create(&philos[i].thread, NULL, routine, &philos[i]);
```

Check the return value in the actual implementation. The new thread can
start immediately, so its required data must already be initialized.

### Mutex lifecycle and critical sections

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

### Why counter++ needs synchronization

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

### Fork pointers: * versus **

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
philos[i].left_fork = &forks[i];
philos[i].right_fork = &forks[(i + 1) % count];
```

Then use the stored pointer directly:

```c
pthread_mutex_lock(philos[i].left_fork);
```

There is no extra `&` because `left_fork` is already a mutex pointer.
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

### Fork locks versus meal locks

A fork mutex stays locked for the whole time the philosopher holds that
fork, including eating. A meal mutex protects brief accesses to meal data:

```c
/* Philosopher */
pthread_mutex_lock(&philo->meal_mutex);
philo->last_meal = now_ms();
pthread_mutex_unlock(&philo->meal_mutex);

/* Monitor */
pthread_mutex_lock(&philo->meal_mutex);
last_meal = philo->last_meal;
pthread_mutex_unlock(&philo->meal_mutex);
```

`now_ms()` is a helper I write, not a library function. Do not hold the meal
mutex throughout eating: the monitor still needs to inspect meal data.
The stop flag also needs synchronized reads and writes.

For a reliable death decision, coordinate checking the deadline and updating
the stop state with meal updates; a copied timestamp can become stale if the
philosopher starts another meal before the monitor acts on it. Establish a
consistent lock order whenever an operation needs multiple mutexes.

### Deadlock is different from starvation

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

### Measuring time and sleeping

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

### What mutexes do beneath the surface

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

### Exercises before the full simulation

1. Start two threads with separate IDs and join them.
2. Increment one shared counter from two threads under a mutex.
3. Let two threads compete for one mutex, briefly sleeping while holding it.

These isolate argument lifetime, shared memory, and locking. The full
project then combines fork ownership, meal timing, monitoring, logging,
and shutdown without data races or deadlock.

### Thread handles are not the threads themselves

On this Linux system, the system header defines `pthread_t` as an alias
for `unsigned long int`. That is a type definition, not a variable.
Its representation is platform-dependent, so I treat it as an opaque
thread handle rather than a number to calculate myself.

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
`pthread_t thread` member, so a separate handle array is unnecessary:

```c
pthread_create(&philos[i].thread, NULL, routine, &philos[i]);
pthread_join(philos[i].thread, NULL);
```

`thread_t` is not the POSIX type used here; the name is `pthread_t`.

### Passing a pointer does not give a thread private memory

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

### Copy the mutex pointer, not the mutex object

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
philo_a.right_fork = &forks[1];
philo_b.left_fork = &forks[1];
```

```text
philo_a.right_fork --+
                    +--> same mutex: forks[1]
philo_b.left_fork ---+
```

Copying a mutex pointer is fine. Copying an initialized mutex with
assignment or `memcpy()` is not. This also applies indirectly when copying
a struct containing an initialized mutex:

```c
philos[1] = philos[0]; /* Also copies meal_mutex: do not use that copy. */
```

Initialize the structs in their final storage, then initialize each mutex
there. Keep that storage alive until all threads using it have finished.
