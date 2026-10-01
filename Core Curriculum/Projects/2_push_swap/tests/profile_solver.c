/* Development-only linker wrappers; never part of the submission build. */
#include "push_swap.h"
#include <time.h>
#include <sys/resource.h>
#include <stdio.h>

static double seconds(struct timespec t)
{
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

static void report(t_soln *x, int id, int status,
                   struct timespec wall, struct timespec cpu)
{
    struct timespec end_wall, end_cpu;
    struct rusage usage;
    const t_algo_config *config = algorithm_config(id);
    clock_gettime(CLOCK_MONOTONIC, &end_wall);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);
    getrusage(RUSAGE_SELF, &usage);
    dprintf(4, "%d\t%s\t%d\t%.9f\t%.9f\t%ld\t",
            id, config ? config->name : "unknown", status,
            seconds(end_wall) - seconds(wall), seconds(end_cpu) - seconds(cpu),
            usage.ru_maxrss);
    if (status == SUCCESS)
        dprintf(4, "%.*s", x->step, x->ans[x->cur]);
    dprintf(4, "\n");
}

#define START_TIMER \
    struct timespec wall, cpu; \
    int status; \
    clock_gettime(CLOCK_MONOTONIC, &wall); \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu)

int __real_greedy_reinsertion(t_soln *, t_circle_buf *, t_algorithm);
int __wrap_greedy_reinsertion(t_soln *x, t_circle_buf *stacks, t_algorithm algo)
{
    START_TIMER;
    status = __real_greedy_reinsertion(x, stacks, algo);
    report(x, algo, status, wall, cpu);
    return status;
}

int __real_brute_solve(t_soln *, t_circle_buf *, t_circle_buf *, int);
int __wrap_brute_solve(t_soln *x, t_circle_buf *a, t_circle_buf *b, int n)
{
    START_TIMER;
    status = __real_brute_solve(x, a, b, n);
    report(x, ALGO_BFS, status, wall, cpu);
    return status;
}

int __real_get_precomputed_bfs(t_soln *, t_circle_buf *, int);
int __wrap_get_precomputed_bfs(t_soln *x, t_circle_buf *a, int n)
{
    START_TIMER;
    status = __real_get_precomputed_bfs(x, a, n);
    report(x, ALGO_BFS, status, wall, cpu);
    return status;
}
