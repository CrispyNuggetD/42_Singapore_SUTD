#!/bin/sh

last "$USER" | awk '
# Only process normal login lines where $7 is a time like H:MM or HH:MM
$7 ~ /^[0-9]?[0-9]:[0-9][0-9]$/ {
  day = $4 " " $5 " " $6

  # If we moved to a new day, reset state + buffer
  if (day != cur_day) {
    cur_day = day
    ok = 0
    printed = 0
    nbuf = 0
  }

  # Build exactly the output you wanted (same fields as your print)
  line = $1 " " $4 " " $5 " " $6 " " $7
  for (i=8; i<=13; i++) line = line " " $i

  # Buffer lines until we know the day qualifies
  if (!printed) {
    buf[++nbuf] = line
  }

  # Check if this login was before 12:00
  split($7, t, ":")
  if ((t[1] + 0) < 12) ok = 1

  # If the day qualifies and we haven’t printed it yet, dump the buffer
  if (ok && !printed) {
    for (j=1; j<=nbuf; j++) print buf[j]
    printed = 1
  } else if (printed) {
    # Once we’re printing this day, print subsequent lines immediately
    print line
  }
}'

