(()=>{
  function revealDisclosure(){
    if(location.hash!=='#ai-disclosure')return;
    const panel=document.getElementById('ai-disclosure');if(!panel)return;
    panel.open=true;panel.scrollIntoView({block:'start'});
  }
  revealDisclosure();window.addEventListener('hashchange',revealDisclosure);
})();
(()=>{
  const link=document.querySelector('.back-to-top');if(!link)return;
  const update=()=>{link.hidden=scrollY<420;};
  window.addEventListener('scroll',update,{passive:true});
  window.addEventListener('pageshow',update);update();
})();
(()=>{
  const element=document.getElementById('deadline');if(!element)return;
  const deadline=Date.parse(element.dataset.until),observed=Date.parse(element.dataset.observed);
  if(!Number.isFinite(deadline)||!Number.isFinite(observed))return;
  function render(){const now=Date.now(),left=deadline-now;const days=Math.floor(Math.abs(left)/86400000),hours=Math.floor(Math.abs(left)%86400000/3600000);element.textContent=(left>=0?`${days}d ${hours}h to the published deadline.`:'The published deadline has passed.')+' Based on the last snapshot, not a live eligibility check.'+(now-observed>3*86400000?' Snapshot is over three days old; refresh needed.':'');}
  render();setInterval(render,60000);
})();

(()=>{
  const stamp=document.querySelector('.site-build');if(!stamp)return;
  const status=stamp.querySelector('.build-state'),button=stamp.querySelector('.check-build');
  button.hidden=false;
  async function check(){
    button.disabled=true;status.textContent='Checking published build…';
    try{
      const response=await fetch('build.json?check='+Date.now(),{cache:'no-store',credentials:'omit'});
      if(!response.ok)throw Error('Unavailable');const remote=await response.json();
      if(typeof remote.id!=='string'||!Number.isFinite(Date.parse(remote.built_at)))throw Error('Invalid build');
      if(remote.id===stamp.dataset.buildId)status.textContent='Matches the latest published build ✓';
      else if(Date.parse(remote.built_at)>Date.parse(stamp.dataset.builtAt)){
        status.textContent='New website build available. ';
        const refresh=document.createElement('button');refresh.type='button';refresh.textContent='Refresh now';refresh.addEventListener('click',()=>location.reload());status.append(refresh);
      }else status.textContent='Deployment is updating; check again shortly.';
    }catch{status.textContent='Couldn’t check right now; build details remain above.';}
    finally{button.disabled=false;}
  }
  button.addEventListener('click',check);check();
})();
