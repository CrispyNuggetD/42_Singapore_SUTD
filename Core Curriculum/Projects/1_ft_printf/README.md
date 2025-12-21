*This project has been created as part of the 42 curriculum by hnah.*

# Description

-

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

1. What is a variadic functions in C?

2. Is (return None if n < 0 else value) valid C? (Psst: It's not - That's Python).

3. (Sometimes) Clarifying subject.pdf interpretation (I'm not too good at understanding extent required of questions yet, oops)

4. Checking whether I'm over-complicating things (Some are responsibility of Caller, e.g. I'm NOT supposed to think of 'hacking' a prev->next idea, shown below).

5. Checking my flawed assumptions (I type my interpretation of the question and check, sometimes I'm wrong. E.g. Aforementioned hack: memcpy *next to current, link, del and free(next))

6. Of course, S.O.S. when I'm stuck/ no idea how to start, asking: "Do NOT give me any code (or solution when debugging), guide me through".

7. What are the various trade-offs of these ft_itoa algos and which is most efficient (Psst: It's the Syscall interruption that's the slowest anyway).

---

## Example respone: (I formatted it for clarity)

#### What is a variadic functions in C?

You already know fixed-arity functions like::

```c
int add(int a, int b);
```

A variadic function says instead: “I know some arguments for sure… and then I’ll accept however many more you want.”

- The canonical example is printf:

```c
printf("x = %d, y = %f\n", x, y);
```

##### The syntax, stripped to its bones

A variadic function has three defining features:

1. At least one named parameter

2. An ellipsis ... at the end

3. The ```<stdarg.h>``` machinery inside

Prototype shape:

```c
return_type function_name(type fixed_arg, ...);
```

- That ... is not magic values; it’s a promise:

> “More arguments exist on the call stack, but the compiler won’t help you anymore.”

So the burden shifts to you.

##### How C actually makes this work (important mental model)

C does NOT track:

- How many... 

```
(Blah blah blah)
```


---

## Contact Details

##### For further queries/ help:

I, (42 intra) hnah (Christopher Hui-Kang Nah) am contactable via:

1. Email: christopher_nah@yahoo.com.sg

2. LinkedIn: linkedin.com/in/crispynugget/

```
42 BEYOND THE CODE
```

##### EOF :D
