#!/usr/bin/env python3
"""Inject malloc failures in a temporary ASan/UBSan build; preserve the main build."""
from pathlib import Path
import os
import random
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
WRAPPER = r'''
#include <stdlib.h>
#include <stddef.h>
void *__real_malloc(size_t size);
void *__wrap_malloc(size_t size)
{
    static long calls;
    const char *setting = getenv("FAIL_MALLOC_AT");
    calls++;
    if (setting && calls == strtol(setting, NULL, 10))
        return NULL;
    return __real_malloc(size);
}
'''


def main():
    subprocess.run(['make', '-s', '-j4'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='push_swap_allocations_') as folder:
        path = Path(folder)
        wrapper = path / 'fail_malloc.c'
        wrapper.write_text(WRAPPER)
        binary = path / 'push_swap'
        subprocess.run(['cc', '-Wall', '-Wextra', '-Werror', '-g',
                        '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
                        '-Iincludes', '-Idebug',
                        *map(str, (ROOT / 'src').rglob('*.c')), str(wrapper),
                        'libft/libft.a', '-Wl,--wrap=malloc', '-o', str(binary)],
                       cwd=ROOT, check=True)
        for n, allocations in [(3, 8), (5, 10), (2000, 7)]:
            values = list(range(n, 0, -1))
            if n > 500:
                random.Random(42).shuffle(values)
            for fail in range(1, allocations + 1):
                env = dict(os.environ, FAIL_MALLOC_AT=str(fail),
                           ASAN_OPTIONS='detect_leaks=1')
                result = subprocess.run([str(binary), *map(str, values)],
                                        env=env, capture_output=True, timeout=30)
                assert result.returncode == 1 and not result.stdout, (n, fail, result)
                assert result.stderr.endswith(b'Error\n'), (n, fail, result.stderr)
                assert b'Sanitizer' not in result.stderr, (n, fail, result.stderr)
                assert b'runtime error:' not in result.stderr, (n, fail, result.stderr)
    print('PASS: 25 injected malloc failures; ASan/UBSan/leak detection clean')


if __name__ == '__main__':
    main()
