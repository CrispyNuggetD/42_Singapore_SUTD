#!/usr/bin/env python3
"""Memory/thread checks and separately labelled startup-failure injection."""
import json, os, pathlib, re, subprocess, tempfile
ROOT=pathlib.Path(__file__).resolve().parents[2]
OUT=pathlib.Path(__file__).resolve().parent
results=[]
def check(name,tool,args,expected=0,exe=None,env=None):
    cmd=['valgrind','--tool='+tool,'--error-exitcode=99']
    if tool=='memcheck':cmd+=['--leak-check=full','--show-leak-kinds=all','--errors-for-leak-kinds=all']
    cmd += [str(exe or ROOT/'philo/philo'),*map(str,args)]
    timeout=False
    with (OUT/(name+'.stdout')).open('w') as log,(OUT/(name+'.txt')).open('w') as error:
        p=subprocess.Popen(cmd,stdout=log,stderr=error,env=env)
        try:rc=p.wait(timeout=25)
        except subprocess.TimeoutExpired:timeout=True;p.kill();rc=p.wait()
    text=(OUT/(name+'.txt')).read_text()
    clean=bool(re.search(r'ERROR SUMMARY: 0 errors',text))
    result=dict(name=name,tool=tool,args=args,result='PASS' if rc==expected and clean and not timeout else 'FAIL',returncode=rc,timeout=timeout,zero_errors=clean,summary=[l for l in text.splitlines() if 'ERROR SUMMARY' in l or 'All heap blocks' in l or 'lost:' in l or 'still reachable:' in l])
    results.append(result);(OUT/'valgrind_results.json').write_text(json.dumps(results,indent=2))
    print(json.dumps(result),flush=True)
check('memcheck_goal','memcheck',[5,10000,100,100,3])
check('memcheck_single_death','memcheck',[1,200,60,60])
check('memcheck_multi_death','memcheck',[4,310,200,100])
check('memcheck_invalid','memcheck',[0,800,200,200],expected=1)
check('helgrind_goal','helgrind',[5,10000,100,100,3])
check('helgrind_death','helgrind',[4,310,200,100])
check('drd_goal','drd',[5,10000,100,100,3])
wrapper=r'''
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
void *__real_malloc(size_t size);
int __real_pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int __real_pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
static int selected(const char *name, int call)
{
    const char *value = getenv(name);
    return value && atoi(value) == call;
}
void *__wrap_malloc(size_t size)
{
    static int call;
    if (selected("PHILO_TEST_MALLOC_FAIL", ++call)) return NULL;
    return __real_malloc(size);
}
int __wrap_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    static int call;
    if (selected("PHILO_TEST_MUTEX_INIT_FAIL", ++call)) return EAGAIN;
    return __real_pthread_mutex_init(mutex, attr);
}
int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*routine)(void *), void *arg)
{
    static int call;
    if (selected("PHILO_TEST_THREAD_CREATE_FAIL", ++call)) return EAGAIN;
    return __real_pthread_create(thread, attr, routine, arg);
}
'''
(OUT/'startup_failure_wrapper.c.txt').write_text(wrapper)
with tempfile.TemporaryDirectory(prefix='philo-failure-check-') as temp:
    tmp=pathlib.Path(temp);(tmp/'wrapper.c').write_text(wrapper)
    names=re.findall(r'\$\(SRC_DIR\)/(\w+\.c)',(ROOT/'philo/Makefile').read_text())
    subprocess.run(['cc','-g','-Wall','-Wextra','-Werror','-pthread','-I'+str(ROOT/'philo/includes'),*[str(ROOT/'philo/src'/n) for n in names],str(tmp/'wrapper.c'),'-Wl,--wrap=malloc','-Wl,--wrap=pthread_mutex_init','-Wl,--wrap=pthread_create','-o',str(tmp/'philo')],check=True)
    for kind,calls in [('MALLOC',[1,2]),('MUTEX_INIT',[1,2,5,10]),('THREAD_CREATE',[1,3])]:
        for call in calls:
            env=os.environ.copy();env['PHILO_TEST_'+kind+'_FAIL']=str(call)
            check(f'failure_{kind.lower()}_{call}','memcheck',[5,10000,100,100,3],expected=1,exe=tmp/'philo',env=env)
print('Valgrind and injected startup-failure checks complete.',flush=True)
