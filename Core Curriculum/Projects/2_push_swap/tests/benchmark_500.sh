#!/usr/bin/env bash
# Run from zsh with: bash tests/benchmark_500.sh
# Three trials; require OK from both checkers and fewer than 5500 moves.
set -eu
cd -- "$(dirname -- "$0")/.."
make all bonus
mkdir -p tests/debug/results
report=$(mktemp -d tests/debug/results/benchmark_500.XXXXXX)
exec > >(tee "$report/report.txt") 2>&1
printf 'Saved inputs, moves and report: %s\n' "$report"
printf 'Commit: '; git rev-parse --short HEAD
sha256sum push_swap checker
status=0
for trial in 1 2 3; do
    # Keep the exact input and solution so failures can be reproduced.
    shuf -i 0-999999 -n 500 > "$report/input_$trial.txt"
    args=$(tr '\n' ' ' < "$report/input_$trial.txt")
    if ! ./push_swap "$args" > "$report/moves_$trial.txt" 2> "$report/stderr_$trial.txt"; then
        printf 'Trial %s: FAIL (solver error)\n' "$trial"
        cat "$report/stderr_$trial.txt"
        status=1
        continue
    fi
    moves=$(wc -l < "$report/moves_$trial.txt")
    reference=ERROR
    bonus=ERROR
    if reference=$(./checker_linux "$args" < "$report/moves_$trial.txt") && \
       bonus=$(./checker "$args" < "$report/moves_$trial.txt") && \
       [ "$reference" = OK ] && [ "$bonus" = OK ] && \
       [ "$moves" -lt 5500 ] && [ ! -s "$report/stderr_$trial.txt" ]; then
        printf 'Trial %s: PASS (%s moves; both checkers OK)\n' "$trial" "$moves"
    else
        printf 'Trial %s: FAIL (%s moves; reference=%s; bonus=%s)\n' \
            "$trial" "$moves" "${reference:-ERROR}" "${bonus:-ERROR}"
        cat "$report/stderr_$trial.txt"
        status=1
    fi
done
if [ "$status" -eq 0 ]; then
    printf 'PASS: all three 500-value trials met the strict highest move band.\n'
else
    printf 'FAIL: at least one trial did not meet the checks.\n'
fi
printf 'Paste this report here: %s/report.txt\n' "$report"
exit "$status"
