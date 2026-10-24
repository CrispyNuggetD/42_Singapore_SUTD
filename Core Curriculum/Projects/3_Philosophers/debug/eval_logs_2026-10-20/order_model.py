# Abstract interleaving model: no clock/death/sleep; fork ownership and
# per-attempt order snapshot follow the current program. Release/count
# combined here is an allowed execution with no interleaving between them.
import random,json
from pathlib import Path
rng=random.Random(42);n=5
for trial in range(10000):
 stages=[0]*n; orders=[None]*n;owners=[None]*n;total=0;meals=[0]*n;trace=[]
 for step in range(300):
  enabled=[]
  for i,s in enumerate(stages):
   if s in (0,3) or (s in (1,2) and owners[orders[i][s-1]] is None):enabled.append(i)
  if not enabled:
   data=dict(note='Abstract possible interleaving, not a measured runtime trace. Timing and death monitor omitted.',total=total,meals=meals,orders=orders,owners=owners,trace=trace)
   Path(__file__).resolve().with_name('order_counterexample.json').write_text(json.dumps(data,indent=2))
   print('Found circular wait: total',total,'meals',meals,'orders',orders,'owners',owners,'steps',len(trace));raise SystemExit
  i=rng.choice(enabled);s=stages[i]
  if s==0:
   odd=(i+1)%2;a=(i+odd)%n;b=(i+1-odd)%n
   flip=(total//n)%2
   orders[i]=(b,a) if flip else (a,b);stages[i]=1
   event=f'P{i+1} snapshots total={total}; order F{orders[i][0]+1}->F{orders[i][1]+1}'
  elif s in (1,2):
   f=orders[i][s-1];owners[f]=i;stages[i]+=1;event=f'P{i+1} acquires F{f+1}'
  else:
   a,b=orders[i];owners[a]=None;owners[b]=None;total+=1;meals[i]+=1;stages[i]=0
   event=f'P{i+1} finishes and releases both; total={total}'
  trace.append(event)
print('No counterexample found within search limit; not a proof.')
