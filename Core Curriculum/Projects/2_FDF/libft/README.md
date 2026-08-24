*This project has been created as part of the 42 curriculum by hnah.*

# Description

- This is the first "baby project" in 42 Core Curriculum, after students pass the Piscine entry bootcamp/ exam. 

- It can be included in later projects as helper functions.

- It challenges students with a re-creation (not to be confused with having a "recreation" - It's not. :D) of libc.

- But, practically it's more of a simplfied version - "musl" than "libc".

- It's not really libc as that's 1. Architecture-specific (x86, ARM, RISC-V...), 2. Safety/ Branch prediction tricks, Optimisation, 3. Has assembly..., etc.

## 3 Parts of Libft:

1. Basic musl/ libc functions (Character checking, string/ memory functions, atoi, simple malloc like Calloc implementation).

2. "Additional functions" that are non-libc functions that will be useful for our later projects. (itoa, file descriptor functions, ft_strtrim for trimming strings somehow from BOTH front and back, etc.)

3. Linked lists :D

## The libft.h header file (Library created)

Contains:

1. Basic unix/ C libraries used (```stduni.h``` for writing or ```stdint.h``` for ```INT_MIN/ INT_MAX``` values)

2. Structs for linked list.

3. Prototypes for all the functions that other .c files can "see" to be used as helper functions.

### Struct for the linked list

Defined as follows:

```c
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;
```

This node:

1. Accepts any ```(void *)``` type of content

2. A pointer ```(*next)``` to the next node in the linked list.

### Prototype functions for libft project

- Contains 43 prototypes for each of the functions contained in each .c file.

- t_list type prototypes are for when nodes (as return value) in linked list functions are involved.

- Other local functions are kept in each .c file with static function type to limit linkage.

- Further infomation found in libft.h file.

# Instructions

1. Open ```terminal```, ```cd``` to desired storage directory, ```git clone``` my repository, ```cd``` into the libft directory.

## Make main.c file to test

Copy-and-paste and save as a "main.c" file using text-editor (make plain text):

```c
#include <stdio.h>
#include "libft.h"

int	main(void)
{
	printf("Your output is: %li", ft_strlen("Hello World!"));
}

/*
 * Running instructions:
 * 2 steps.
 *
 * 1. Change %li to the type of output function gives (first argument, left hand side)
 * 
 * Common ones:
 * %c 		- Single Character
 * %s 		- (char *) String (Null terminated, containing '\0' AKA ASCII 0)
 * %i/ %d	- Signed Decimal Integer
 * %u		- Unsigned Integer
 * %li/ %ld	- Long Decimal Integer
 * %lu		- Unsigned Long
 * %zu		- size_t
 * %p		- Pointer Address (Prints in Hexadecimal)
 * %x		- Unsigned Integer (Prints in Hexadecimal)
 *
 * BTW void functions don't "return" anything so
 * you can't "print the output" as is.
 *
 * Chain it with/ Pipe into other functions.
 *
 * 2. Change the function to evaluate (second argument, right side)
 *
 * Choose return type for %return_type based on function.
 */
```

## Terminal commands to compile and run program

1. There is a Makefile needed to compile the header file (libft.h) into a library (libft.a) to be used for compilation.

2. Again, cd to libft directory, then run command "make".

3. Compile the needed .c file using ```cc main.c libft.a -o output -Wall -Wextra -Werror```

4. NOTE: -Wall -Wextra -Werror (42 uses these 3 warning flags during compilation).

5. Run program with ./output.

# Resources


## Resources used:

- Harvard CS50 Youtube Series (up till Linked Lists), by David J. Malan.
- Various Stackover Flow forum Q&As, mostly for debugging error codes or finding out best practices.
- Several posts and videos about how NOT to code (inefficiencies, silent bugs).
- Peer-shared web-articles.
- Of course needless to say, my peers also were great help in providing invaluable "irl" advice you can't find online :)

## Differences between glibc and BSD libc

We were challenged to be curious to explore the differences between glibc and BSD libc. Summary:

1. glibc (GNU C Library) is the dominant libc on most Linux distros.

2. BSD libc (FreeBSD / OpenBSD / NetBSD each ship their own libc as part of the OS “base system”).

Hence:

- glibc (GNU C Library)

1. Lots of ABIs (Application binary interface, which is the: Interface to software defined in terms of in-process, machine code access)

2. Lots of legacy software

3. Lots of extensions, and

4. Strong backward compatibility expectations.

The GNU manual frames it as ISO C + POSIX (and more) for GNU/Linux systems.

Then:

- BSD libc

1. Tends to be tightly integrated with that OS’s kernel + userland conventions, and

2. Often includes BSD APIs by default (because the OS owns the whole stack).

*Cool beans.*

## Usage of A.I.

- Other than Google search's own generative A.I. responses (mostly for VIM commands like :set nonu)
- I subscribe to ChatGPT Plus, so that's the only coding "advisor" I use.

### Usage of ChatGPT
- I have a Custom Folder "Computer Science", whatever help or nerding out goes inside.
- Things like "What 'is' computation?", or "Why is M-series Chips' designed with so much L1-I Cache?" or "Why is division so expensive", goes inside there.
- I often ask for "many ways to do the same thing", such as 5 types of algos (hardcoding INT_MIN, fixed buffer, recursion...) and the tradeoffs.
- I use it for help with projects, which brings me to:

---

### ChatGPT for coding:

#### Summary of A.I. usage


- Lots of A.I. used, but systematic and no vibe-coding.

- Explain till I understand, and (I try my best to) internalise.

- Asks and learns "more than required" with A.I.

- Also partly (along with many MCs) reason why I took 30 days for this project, Libft, instead of Pace 12 - 12 days.

---


### ChatGPT's "Computer Science" - 7 Custom Instructions:

1. I am interested in computing in general rather than just trying to “score/ pass/ get the best answer or algo”, so things like theoretical computing (P=NP) or hardware and engineering (L1 Cache, Assembly) are relevant or interesting too (I.e. trivia can be given but convo should not “stray too far”). 

2. In working on programming problems, especially starting on “new projects” (copy pasting questions/ projects of 42), do NOT give code or model answer immediately. 

3. Assist and guide in computational thinking/ problem solving (including mathematical/ intuition simplification of presented problem). 

4. This will help develop me into a problem solver/ thinker rather than a prompt engineer/ vibe-coder.

5. When “stuck”, clues guiding to “derivation” of the state-of-the-art algo (e.g. sorting/ path-finding algos) can be assisted.

6. Final best algo name and code should only be given “at end” or when “stuck”. 

7. Make sure all (if any), generated code conforms to Norm/ norminette as per version 4 (V4).

---

#### Example of prompts used

---

1. Is the following allowed in 42's Norm: is_negative = (n < 0);?

2. Is (return None if n < 0 else value) valid C? (Psst: It's not - That's Python).

3. (Sometimes) Clarifying subject.pdf interpretation (I'm not too good at understanding extent required of questions yet, oops)

4. Checking whether I'm over-complicating things (Some are responsibility of Caller, e.g. I'm NOT supposed to think of 'hacking' a prev->next idea, shown below).

5. Checking my flawed assumptions (I type my interpretation of the question and check, sometimes I'm wrong. E.g. Aforementioned hack: memcpy *next to current, link, del and free(next))

6. Of course, S.O.S. when I'm stuck/ no idea how to start, asking: "Do NOT give me any code (or solution when debugging), guide me through".

7. What are the various trade-offs of these ft_itoa algos and which is most efficient (Psst: It's the Syscall interruption that's the slowest anyway).

---

## Example response: (I formatted it for clarity)

#### 1. Canonical ft_itoa algorithm (the one ChatGPT recommends for 42)

- Goal:
Convert an int n to a freshly malloc’d C string.

Key constraints:

1. Works for all int values (including INT_MIN)

2. Allocates exact length (no waste)

3. No hardcoded magic constants like 12 if possible

4. Caller must be able to free the result

---

##### Step 0 — Promote to wider type

---

- Problem: INT_MIN cannot be negated in int range.

So we do this conceptually:

- Take input int n

- Create long nb = n;
(now nb can hold +2147483648 safely on usual 64-bit machines)

From here on, all logic is done using nb.

---

##### Step 1 — Determine if negative

---

Algorithm:

1. is_negative = (nb < 0)

If negative:

1. remember this (need one extra char for -)

2. set nb = -nb (safe now, because nb is long)

Now nb is non-negative, even if original was INT_MIN.

---

##### Step 2 — Count digits

---

- We now have a non-negative nb.

Algorithm to count digits:

- If nb == 0, then digit count is 1 (special case)

- Else:

1. count = 0

2. while nb > 0:

3. nb /= 10

4. count++

After this, count = number of decimal strings. Then total length:

1. len = count

2. If is_negative, len++ for ‘-‘

3. Plus one more for ‘\0’ when allocating

So we will malloc(len + 1) bytes:

1. len characters

2. 1 null terminator

(Implementation detail: some people fold “+1 for \0” into len itself; doesn’t matter conceptually.)

---

##### Step 3 — Fill the string from the end

---

- Now we reconstruct the number backwards.

We reset nb again to the absolute value of n (as long):

1. If original n was 0 → handle as special case: string is “0”

2. Else:

Conceptual algorithm:

1. Put the string terminator at the end: str[len] = ‘\0’

2. Start from index i = len - 1

3. While nb > 0:

4. last_digit = nb % 10 (gives 0–9)

5. convert digit to char: ‘0’ + last_digit

6. place at str[i]

7. i—

8. nb /= 10

9. If is_negative, put ‘-‘ at str[0]

---

##### Example with n = -123:

---

nb = -123 → negative → is_negative = 1 → nb = 123

1. digits = 3

2. len = 4 (3 digits + 1 sign)

3. allocate 5 bytes (4 chars + \0)

Fill: Index progression:

1. str[4] = ‘\0’

2. i = 3, nb = 123 → digit = 3 → str[3] = ‘3’

3. nb = 12, i = 2 → digit = 2 → str[2] = ‘2’

4. nb = 1, i = 1 → digit = 1 → str[1] = ‘1’

5. nb = 0 → stop

6. since negative → str[0] = ‘-‘

Result: “-123”.

---

##### ChatGPT's explanation of algorithmic choice/ design

---

##### Why this is “nice”

1. No hardcoded limits

2. Works for all ints

3. Clear conceptual flow

4. Single malloc call, no extra copies

5. (Algorithmic) Complexity is 0 (number of digits)

This is the one I’d call “best” for libft: clear, safe, portable enough, and norm-friendly.

---

#### 2. Alternative Strategy A — All-negative route (no long)

---

- This is conceptually fun: you never take absolute value, so you never overflow on INT_MIN.

Idea:

1. Keep n as negative, and when you extract digits, do:

2. last_digit = -(n % 10) (gives positive 0–9)

3. then n /= 10 (still negative, approaching 0)

Algorithm sketch:

- ```blah blah blah...```

Pros:

1. No need for a long

2. Works for INT_MIN naturally

3. Mathematically neat

Cons:

1. Slightly unintuitive: % on negative numbers is confusing

2. Can be harder to reason about under sleep deprivation

3. If you haven’t internalised negative modulo semantics in C, it’s bug-prone

Still perfectly valid though, and kind of elegant once your brain locks it in.

---

##### 3. Alternative Strategy B — Temporary fixed-size buffer on the stack

---

- This is a hybrid approach: use a local array, fill it, then malloc the exact size and copy.

Conceptual idea:

```
blah blah blah
```

---

## Contact Details

##### For further queries/ help:

I, (42 intra) hnah (Christopher Hui-Kang Nah) am contactable via:

1. Email: christopher_nah@yahoo.com.sg

2. LinkedIn: linkedin.com/in/crispynugget/

```
42BeyondTheCode
```

##### EOF :D
