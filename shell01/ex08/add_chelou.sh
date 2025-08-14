#!/bin/sh

base_1="'\\\"?!"
base_2='mrdoc'
base_output='gtaio luSnemf'
# I first put the first 3 bases into variables
# I retried this so many times but it appears the 
# reason it keeps failing is because SOMEHOW
# Moulinette is weird and uses /bin/sh
# instead of using bash like a normal program

# So my code is written in bash syntax with
# curly braces operations that won't work in POSIX sh.

result_1=0
result_2=0
output=""
# Make placeholder variables for the outputs first

BASE_OUTPUT_LEN=$(echo -n "$base_output" | wc -c)
# This gets the length of the output base lah, which is 13

# Convert FT_NBR1 from base_1
len1=$(echo -n "$FT_NBR1" | wc -c)
# So horh, we first get the length of the first input

i=1
# init the loop

# So after getting KO by Moulinette many times,
# I realised I cannot use curly braces like a normal bash script
# So sh script uses square brackets as you will see below

# -le means less than or equals to, so we are looping 
# through the length of the input lorh.

# cut -c means cut the input to get the character at $i position 

# Just in case, we check if Moulinette decides to be funny
# to test inputs with characters not in the base.
# Hopefully that's not the reason why I keep failing bcos
# I will be so done :D

# Then we just do a simple summation in base 5

# Note we need bc (basic calculator) bcos the input is so big
# it's more than 64 bit which sh will overflow

while [ $i -le $len1 ]; do
  char_1=$(echo "$FT_NBR1" | cut -c $i)
  digit_1=$(expr index "$base_1" "$char_1")
  if [ "$digit_1" -eq 0 ]; then
    echo "Error: Invalid character '$char_1' in input FT_NBR1" >&2
    exit 1
  fi
  digit_1_index=$((digit_1 - 1))
  result_1=$(echo "$result_1 * 5 + $digit_1_index" | bc)
  i=$((i + 1))
done


# Here horh, we rinse and repeat for the second input
# Convert FT_NBR2 from base_2
len2=$(echo -n "$FT_NBR2" | wc -c)
i=1
while [ $i -le $len2 ]; do
  char_2=$(echo "$FT_NBR2" | cut -c $i)
  digit_2=$(expr index "$base_2" "$char_2")
  if [ "$digit_2" -eq 0 ]; then
    echo "Error: Invalid character '$char_2' in input FT_NBR2" >&2
    exit 1
  fi
  digit_2_index=$((digit_2 - 1))
  result_2=$(echo "$result_2 * 5 + $digit_2_index" | bc)
  i=$((i + 1))
done

# Simple here, we sum up both
# Sum results
result_3=$(echo "$result_1 + $result_2" | bc)

# First we check if the sum is 0 first, if Moulinette trolls
# Then we do a simple modulo print and repeated division

# Convert result_3 to base_output alphabet
is_zero=$(echo "$result_3 == 0" | bc)
if [ "$is_zero" -eq 1 ]; then
  output=$(echo "$base_output" | cut -c1)
else
  output=""
  while [ "$(echo "$result_3 > 0" | bc)" -eq 1 ]; do
    rem=$(echo "$result_3 % $BASE_OUTPUT_LEN" | bc)
    char=$(echo "$base_output" | cut -c $((rem + 1)))
    output="$char$output"
    result_3=$(echo "$result_3 / $BASE_OUTPUT_LEN" | bc)
  done
fi

echo "$output"
