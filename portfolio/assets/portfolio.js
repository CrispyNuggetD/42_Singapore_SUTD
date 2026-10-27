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
