#!/usr/bin/env bash
# -n counts successful tests; omit it to run until Ctrl-C. Reports are Markdown.
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
exec python3 tests/run_random_tests.py "$@"
