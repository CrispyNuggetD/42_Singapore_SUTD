#!/usr/bin/env python3
"""Bounded sequential checks; reports observations, not an infinite-time proof."""
import collections, hashlib, json, pathlib, re, subprocess, time
ROOT=pathlib.Path(__file__).resolve().parents[2]
OUT=pathlib.Path(__file__).resolve().parent
EXE=ROOT/'philo/philo'
RESULTS=[]
LINE=re.compile(r'^(\d+) (\d+) (has taken a fork|is eating|is sleeping|is thinking|died)$')
def save(): (OUT/'runtime_results.json').write_text(json.dumps(RESULTS,indent=2))
def run(name,args,kind,seconds=5,category='sheet',prefix=None):
    cmd=(prefix or ['stdbuf','-oL'])+[str(EXE),*map(str,args)]
    begin=time.monotonic(); timed_out=False
    with (OUT/(name+'.log')).open('w') as log, (OUT/(name+'.stderr')).open('w') as err:
        p=subprocess.Popen(cmd,stdout=log,stderr=err,cwd=ROOT)
        try: rc=p.wait(timeout=seconds)
        except subprocess.TimeoutExpired:
            timed_out=True;p.kill();rc=p.wait()
    elapsed=time.monotonic()-begin
    output=(OUT/(name+'.log')).read_text(); error=(OUT/(name+'.stderr')).read_text()
    problems=[];rows=[]
    for line in output.splitlines():
        m=LINE.fullmatch(line)
        if not m:problems.append('malformed output: '+line[:100]);continue
        rows.append((int(m[1]),int(m[2]),m[3]))
    deaths=[r for r in rows if r[2]=='died']
    lateness=None
    if kind=='invalid':
        if timed_out or rc==0 or output or not error:problems.append('invalid input was not rejected cleanly')
        counts=[]
    else:
        n,d,e,s=map(int,args[:4]);counts=[0]*n;last={};eaten_at={};fork_logs=collections.Counter();previous=-1
        for stamp,pid,state in rows:
            if not 1<=pid<=n:problems.append('invalid philosopher ID');continue
            if stamp<previous:problems.append('timestamps decreased')
            previous=stamp
            if state=='has taken a fork':fork_logs[pid]+=1
            elif state=='is eating':
                counts[pid-1]+=1
                if fork_logs[pid]<2:problems.append('eating without two fork log entries')
                fork_logs[pid]=0;last[pid]=stamp;eaten_at[pid]=stamp
            elif state=='is sleeping' and pid in eaten_at:
                if stamp-eaten_at[pid]<e:problems.append('logged eating duration shorter than requested')
            elif state=='died':lateness=stamp-(last.get(pid,0)+d)
        if deaths and (len(deaths)!=1 or rows[-1][2]!='died'):problems.append('death not unique and final')
        if error:problems.append('unexpected stderr')
        if kind=='survive' and (not timed_out or deaths):problems.append('did not survive observation')
        if kind=='goal' and (timed_out or rc!=0 or deaths or min(counts)<int(args[4])):problems.append('quota did not complete normally')
        if kind=='death' and (timed_out or rc!=0 or len(deaths)!=1):problems.append('expected one death and clean exit')
        if kind=='death' and lateness is not None and not 0<=lateness<=10:problems.append('logged deadline difference outside 0..10 ms')
        if n==1 and (counts[0] or sum(r[2]=='has taken a fork' for r in rows)!=1):problems.append('single philosopher fork/eating behavior wrong')
    result=dict(name=name,category=category,args=args,kind=kind,result='FAIL' if problems else 'PASS',returncode=rc,timed_out=timed_out,elapsed_s=round(elapsed,3),deaths=deaths,logged_death_lateness_ms=lateness,eating_starts=counts,problems=sorted(set(problems)))
    RESULTS.append(result);save()
    print(json.dumps({k:v for k,v in result.items() if k!='eating_starts'}),flush=True)

invalid=[[],['5','800','200'],['5','800','200','200','7','extra'],['0','800','200','200'],['-1','800','200','200'],['5','0','200','200'],['5','800','200','200','0'],['5','800','200','200','-1'],['five','800','200','200'],['5','800x','200','200'],['5','','200','200'],['5','+','200','200'],['5','2147483648','200','200'],['5','999999999999999999999999','200','200']]
for i,args in enumerate(invalid):run(f'input_invalid_{i:02}',args,'invalid',2,'input')
run('input_plus',['+2','+800','+60','+60','+2'],'goal',3,'input')
run('single',[1,800,200,200],'death',2)
for i in range(1,4):run(f'five_survive_{i}',[5,800,200,200],'survive',10)
for i in range(1,6):run(f'five_goal_{i}',[5,800,200,200,7],'goal',6)
for i in range(1,4):run(f'four_survive_{i}',[4,410,200,200],'survive',10)
for i in range(1,4):run(f'four_die_{i}',[4,310,200,100],'death',2)
for args in ([2,310,200,100],[2,250,150,150],[2,60,120,60]):
    for i in range(1,4):run('two_die_'+'_'.join(map(str,args[1:]))+'_'+str(i),args,'death',2)
run('two_goal',[2,800,200,200,7],'goal',5)
for n in (3,7,9):run(f'odd_{n}_goal',[n,800,200,200,3],'goal',4,'extra')
for n in (199,200):run(f'large_{n}_goal',[n,2000,60,60,3],'goal',5,'extra')
run('five_short_sleep',[5,800,200,100,7],'goal',6,'extra')
run('five_long_sleep',[5,800,200,400,3],'goal',5,'extra')
run('three_long_sleep',[3,800,200,400,3],'goal',5,'extra')
print('Native checks complete.',flush=True)
