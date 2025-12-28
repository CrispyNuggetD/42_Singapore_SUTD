#!/bin/sh
# login_award_7day.sh
#
# Shows:
# 1) Per-day login totals for the last 7 days (including today, including zero days)
# 2) Rolling 7-day total (guaranteed = sum of the printed 7 days)
# 3) Rolling 7-day average per day
# 4) Remaining time to reach a 50h rolling-7-days target
# 5) “Stay logged in until” clock time (e.g., 8:03pm), with date if it crosses midnight
#
# Notes:
# - Counts sessions with explicit duration (HH:MM) like "(02:34)"
# - Also counts open-ended sessions (e.g., "still logged in", "gone - no logout", etc.) as running until now
# - Attributes time to the LOGIN DAY (does not split sessions across midnight)

TARGET_HOURS=50
TARGET_MIN=$((TARGET_HOURS * 60))

# Build list of last 7 dates in ISO format (oldest -> newest)
dates=""
i=6
while [ "$i" -ge 0 ]; do
  d="$(date -d "$i days ago" +%F)"
  dates="${dates}${d}
"
  i=$((i-1))
done
export LAST7_DATES="$dates"
export TODAY="$(date +%F)"

out="$(
  last "$USER" | awk -v TARGET_MIN="$TARGET_MIN" '
  BEGIN {
    now = systime()

    # Initialize exactly the last 7 ISO dates to 0 so we print zeros too
    n = split(ENVIRON["LAST7_DATES"], D, "\n")
    for (i=1; i<=n; i++) {
      if (D[i] != "") daymin[D[i]] = 0
    }
  }

  function epoch_from_last(dow, mon, dd, hhmm,    cmd, e) {
    cmd = "date -d \"" dow " " mon " " dd " " hhmm "\" +%s"
    cmd | getline e
    close(cmd)
    return e + 0
  }

  function key_from_epoch(e,    cmd, k) {
    cmd = "date -d @" e " +%F"
    cmd | getline k
    close(cmd)
    return k
  }

  # Process lines where $7 is a login time like H:MM or HH:MM
  $7 ~ /^[0-9]?[0-9]:[0-9][0-9]$/ {
    start = epoch_from_last($4, $5, $6, $7)
    if (start <= 0) next

    key = key_from_epoch(start)

    # CRITICAL: only count sessions whose day key is one of our last-7 printed days
    if (!(key in daymin)) next

    # Case 1: explicit duration at end: (HH:MM)
    if (match($0, /\(([0-9]+):([0-9][0-9])\)$/, t)) {
      mins = (t[1] + 0) * 60 + (t[2] + 0)
      daymin[key] += mins
      next
    }

    # Case 2: open-ended / unknown logout (still logged in / gone - no logout / crash / etc.)
    mins = int((now - start) / 60)
    if (mins < 0) mins = 0
    daymin[key] += mins
  }

  END {
    # Per-day totals (chronological)
    n = split(ENVIRON["LAST7_DATES"], D, "\n")
    print "Per-day login totals (last 7 days):"
    for (i=1; i<=n; i++) {
      if (D[i] == "") continue

      cmd = "date -d \"" D[i] "\" +%a"
      cmd | getline wd
      close(cmd)

      h = int(daymin[D[i]] / 60)
      m = daymin[D[i]] % 60
      printf "%s (%s): %dh %02dm\n", D[i], wd, h, m
    }

    # Rolling total = sum of the same 7 printed days (cannot disagree)
    roll = 0
    for (i=1; i<=n; i++) {
      if (D[i] != "") roll += daymin[D[i]]
    }

    roll_h = int(roll / 60)
    roll_m = roll % 60

    # Rolling average per day (over 7 days, including zeros)
    avg = int(roll / 7)
    avg_h = int(avg / 60)
    avg_m = avg % 60

    print ""
    printf "Rolling 7-day total: %dh %02dm\n", roll_h, roll_m
    printf "Rolling 7-day average per day: %dh %02dm\n", avg_h, avg_m

    remaining = TARGET_MIN - roll
    if (remaining < 0) remaining = 0

    rh = int(remaining / 60)
    rm = remaining % 60

    print ""
    printf "Target (rolling 7 days): %dh 00m\n", int(TARGET_MIN / 60)
    printf "Remaining to reach target: %dh %02dm\n", rh, rm

    if (remaining == 0) {
      print "You already hit the 50h rolling-7-days award ✅"
    } else {
      printf "If you stay logged in from now, you need ~%dh %02dm more.\n", rh, rm
    }

    # machine-readable for shell:
    printf "REM_MIN=%d\n", remaining
  }'
)"

# Print human output (everything except REM_MIN line)
printf "%s\n" "$out" | awk '!/^REM_MIN=/'

# Extract remaining minutes
rem_min="$(printf "%s\n" "$out" | awk -F= '/^REM_MIN=/{print $2; exit}')"
[ -z "$rem_min" ] && rem_min=0

# Compute the clock time to stay logged in until
echo ""
if [ "$rem_min" -eq 0 ]; then
  echo "Stay logged in until: now (already achieved)"
else
  finish_time="$(date -d "now + $rem_min minutes" +"%I:%M%p")"
  finish_day="$(date -d "now + $rem_min minutes" +"%F")"
  today="$(date +%F)"

  # Format like 8:03pm (not 08:03PM)
  finish_time="$(printf "%s" "$finish_time" | tr 'APM' 'apm' | sed 's/^0//')"

  if [ "$finish_day" = "$today" ]; then
    echo "Stay logged in until: $finish_time"
  else
    finish_label="$(date -d "now + $rem_min minutes" +"%a %Y-%m-%d")"
    echo "Stay logged in until: $finish_time ($finish_label)"
  fi
fi

