from pathlib import Path
import subprocess
import tempfile
import json

out = Path(__file__).resolve().parent
root = out.parents[1]
work = Path(tempfile.mkdtemp(prefix='bonus-cleanup-check-'))
exe = work / 'cleanup_harness'
sources = ['init_bonus.c', 'process_bonus.c', 'cleanup_bonus.c']
subprocess.run(['cc', '-Wall', '-Wextra', '-Werror', '-pthread',
                '-I' + str(root / 'philo_bonus/includes'),
                *[str(root / 'philo_bonus/src' / f) for f in sources],
                '-x', 'c', str(out / 'cleanup_harness.c.txt'),
                '-Wl,--wrap=malloc', '-Wl,--wrap=sem_open',
                '-Wl,--wrap=sem_unlink', '-Wl,--wrap=sem_close',
                '-o', str(exe)], check=True)
cases = ['success', 'allocation_failure', 'forks_open_failure',
         'pair_open_failure', 'printing_open_failure', 'existing_forks_name',
         'existing_pair_name', 'existing_print_name', 'unlink_failure',
         'close_failure']
results = []
for mode, name in enumerate(cases):
    p = subprocess.run([str(exe), str(mode)], capture_output=True, text=True,
                       timeout=5)
    results.append({'case': name, 'exit': p.returncode})
    assert p.returncode == 0, (name, p.returncode, p.stderr)
for mode in [0, 3, 6, 8, 9]:
    p = subprocess.run(['valgrind', '--leak-check=full',
                        '--errors-for-leak-kinds=all', '--error-exitcode=99',
                        str(exe), str(mode)], capture_output=True, text=True,
                       timeout=10)
    (out / f'memcheck_{cases[mode]}.log').write_text(p.stdout + p.stderr)
    results.append({'case': 'memcheck_' + cases[mode], 'exit': p.returncode})
    assert p.returncode == 0, p.stderr
(out / 'results.json').write_text(json.dumps(results, indent=2) + '\n')
print('10 lifecycle cases and 5 Memcheck cases passed.')
