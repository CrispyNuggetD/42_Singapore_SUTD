from pathlib import Path
import subprocess,json,re,collections,time
root=Path(__file__).resolve().parents[2]; out=Path(__file__).resolve().parent; results=[]
cases=[('single','1 800 200 200',3,'death'),*[('five_survival_'+str(i),'5 800 200 200',8,'survive') for i in range(1,4)],*[('five_goal_'+str(i),'5 800 200 200 7',10,'goal') for i in range(1,6)],*[('four_survival_'+str(i),'4 410 200 200',8,'survive') for i in range(1,3)],('four_death','4 310 200 100',3,'death'),('two_death','2 310 200 100',3,'death'),('two_goal','2 800 200 200 7',8,'goal'),('six_goal','6 800 200 200 7',8,'goal'),('three_goal','3 800 200 200 7',8,'goal')]
for name,argline,limit,expected in cases:
 args=argline.split();start=time.monotonic(); timed=False
 try:
  r=subprocess.run(['stdbuf','-oL',str(root/'philo/philo'),*args],capture_output=True,text=True,timeout=limit);code=r.returncode; stdout=r.stdout; stderr=r.stderr
 except subprocess.TimeoutExpired as e:
  timed=True;code=124;stdout=e.stdout or b'';stderr=e.stderr or b''
  if isinstance(stdout,bytes):stdout=stdout.decode(errors='replace')
  if isinstance(stderr,bytes):stderr=stderr.decode(errors='replace')
 (out/(name+'.log')).write_text(stdout)
 (out/(name+'.stderr')).write_text(stderr)
 counts=collections.Counter();completed=collections.Counter(); starts={};deaths=[];bad=[];after=0;late=[]
 for line in stdout.splitlines():
  m=re.fullmatch(r'(\d+) (\d+) (has taken a fork|is eating|is sleeping|is thinking|died)',line)
  if not m:bad.append(line);continue
  t,p,action=int(m[1]),int(m[2]),m[3]
  if deaths:after+=1
  if action=='is eating':counts[p]+=1;starts[p]=t
  if action=='is sleeping':completed[p]+=1
  if action=='died':deaths.append(line);late.append(t-starts.get(p,0)-int(args[1]))
 minimum=min((counts[i] for i in range(1,int(args[0])+1)),default=0)
 ok=(not deaths and timed) if expected=='survive' else (code==0 and len(deaths)==1) if expected=='death' else (code==0 and not deaths and minimum>=int(args[4]))
 ok=ok and not bad and not after and not stderr and all(0<=x<=10 for x in late)
 row=dict(name=name,args=args,expected=expected,exit=code,timed_out=timed,elapsed=round(time.monotonic()-start,3),pass_basic=ok,deaths=deaths,death_timestamp_lateness_ms=late,eating_counts=dict(counts),sleeping_counts=dict(completed),malformed=bad,after_death=after)
 results.append(row);(out/'results.json').write_text(json.dumps(results,indent=2))
 print(name,'PASS' if ok else 'FAIL','exit',code,'deaths',deaths,'minimum meals started',minimum,flush=True)
