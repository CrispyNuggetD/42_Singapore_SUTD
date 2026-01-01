#!/bin/sh
# login_award_7day.sh

set -u

TARGET_HOURS=50
TARGET_MIN=$((TARGET_HOURS * 60))

YEAR="$(date +%Y)"

# Build last 7 ISO dates (oldest -> newest) with NO trailing blank line.
LAST7_DATES="$(i=6; while [ "$i" -ge 0 ]; do
  date -d "$i days ago" +%F
  i=$((i - 1))
done)"

out="$(
  last "$USER" 2>/dev/null | gawk -v TARGET_MIN="$TARGET_MIN" -v YEAR="$YEAR" -v LAST7_DATES="$LAST7_DATES" '
  BEGIN {
    now = systime()

    n = split(LAST7_DATES, D, "\n")
    for (i = 1; i <= n; i++) {
      gsub(/^[ \t\r]+|[ \t\r]+$/, "", D[i])
      if (D[i] != "")
        daymin[D[i]] = 0
    }

    monnum["Jan"]=1; monnum["Feb"]=2; monnum["Mar"]=3; monnum["Apr"]=4
    monnum["May"]=5; monnum["Jun"]=6; monnum["Jul"]=7; monnum["Aug"]=8
    monnum["Sep"]=9; monnum["Oct"]=10; monnum["Nov"]=11; monnum["Dec"]=12
  }

  function epoch_from_parts(mon, dd, hhmm,    y, mm, s, cmd, e) {
    mm = monnum[mon]
    if (mm == 0)
      return 0

    y = YEAR + 0
    s = sprintf("%04d-%02d-%02d %s", y, mm, dd + 0, hhmm)
    cmd = "date -d \"" s "\" +%s"
    cmd | getline e
    close(cmd)
    e += 0

    if (e > now) {
      y = y - 1
      s = sprintf("%04d-%02d-%02d %s", y, mm, dd + 0, hhmm)
      cmd = "date -d \"" s "\" +%s"
      cmd | getline e
      close(cmd)
      e += 0
    }
    return e
  }

  function key_from_epoch(e,    cmd, k) {
    cmd = "date -d @" e " +%F"
    cmd | getline k
    close(cmd)
    return k
  }

  # We only trust lines where $7 is a login time like H:MM or HH:MM
  $7 ~ /^[0-9]?[0-9]:[0-9][0-9]$/ {
    # last fields (from your sample):
    # $4=DOW, $5=Mon, $6=DD, $7=HH:MM
    start = epoch_from_parts($5, $6, $7)
    if (start <= 0)
      next

    key = key_from_epoch(start)
    if (!(key in daymin))
      next

    # explicit duration: (HH:MM)
    if (match($0, /\(([0-9]+):([0-9][0-9])\)$/, t)) {
      mins = (t[1] + 0) * 60 + (t[2] + 0)
      daymin[key] += mins
      next
    }

    # open-ended session
    mins = int((now - start) / 60)
    if (mins < 0)
      mins = 0
    daymin[key] += mins
  }

  END {
    n = split(LAST7_DATES, D, "\n")

    print "Per-day login totals (last 7 days):"
    for (i = 1; i <= n; i++) {
      gsub(/^[ \t\r]+|[ \t\r]+$/, "", D[i])
      if (D[i] == "")
        continue

      cmd = "date -d \"" D[i] "\" +%a"
      cmd | getline wd
      close(cmd)

      h = int(daymin[D[i]] / 60)
      m = daymin[D[i]] % 60
      printf "%s (%s): %dh %02dm\n", D[i], wd, h, m
    }

    roll = 0
    for (i = 1; i <= n; i++) {
      gsub(/^[ \t\r]+|[ \t\r]+$/, "", D[i])
      if (D[i] != "")
        roll += daymin[D[i]]
    }

    roll_h = int(roll / 60)
    roll_m = roll % 60

    avg = int(roll / 7)
    avg_h = int(avg / 60)
    avg_m = avg % 60

    print ""
    printf "Rolling 7-day total: %dh %02dm\n", roll_h, roll_m
    printf "Rolling 7-day average per day: %dh %02dm\n", avg_h, avg_m

    remaining = TARGET_MIN - roll
    if (remaining < 0)
      remaining = 0

    rh = int(remaining / 60)
    rm = remaining % 60

    print ""
    printf "Target (rolling 7 days): %dh 00m\n", int(TARGET_MIN / 60)
    printf "Remaining to reach target: %dh %02dm\n", rh, rm

    if (remaining == 0)
      print "You already hit the 50h rolling-7-days award ✅"
    else
      printf "If you stay logged in from now, you need ~%dh %02dm more.\n", rh, rm

    printf "REM_MIN=%d\n", remaining
  }'
)"

# If awk/date blew up, out might be empty. Fail loudly.
if [ -z "$out" ]; then
  echo "Error: produced no output. Check: gawk installed? GNU date? last output?" >&2
  exit 1
fi

# Print human output (everything except REM_MIN line)
printf "%s\n" "$out" | awk '!/^REM_MIN=/'

# Extract remaining minutes
rem_min="$(printf "%s\n" "$out" | awk -F= '/^REM_MIN=/{print $2; exit}')"
[ -z "$rem_min" ] && rem_min=0

echo ""
if [ "$rem_min" -eq 0 ]; then
  echo "Stay logged in until: now (already achieved)"
else
  finish_time="$(date -d "now + $rem_min minutes" +"%I:%M%p")"
  finish_day="$(date -d "now + $rem_min minutes" +"%F")"
  today="$(date +%F)"

  finish_time="$(printf "%s" "$finish_time" | tr 'APM' 'apm' | sed 's/^0//')"

  if [ "$finish_day" = "$today" ]; then
    echo "Stay logged in until: $finish_time"
  else
    finish_label="$(date -d "now + $rem_min minutes" +"%a %Y-%m-%d")"
    echo "Stay logged in until: $finish_time ($finish_label)"
  fi
fi
