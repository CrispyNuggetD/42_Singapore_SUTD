*This project has been created as part of the 42 curriculum by hnah.*

# Description

- This project recodes a minimally functional ```printf()``` function as per found in ```<stdio.h>```.
- However, it does **NOT** implement the buffer management of the original ```printf()```
- It references "Libft" project which is our own 42-student made glibc/ musl implementation.
- This projects also highlights how a makefile can be used to compile another Libft sub-directory makefile properly.

## About my project

- Instead of convuluted ```if; else if; else if; else;``` statements, project uses modular technique:

### Modular technique:

Three compartmentalised parts to prevent large refactoring requirements/ logic bugs:

1. Parser
2. Dispatcher
3. Handler

An explanation of the modules are as follows:

#### Parser

Reads the format string, and when it sees %..., it builds a small description of what it wants:

- Uses indexing table and function pointers
- Being a contiguous array, memory lookup of pointers and dispatching is O(1) complexity
- Paired lists, albeit arguably clearer, were not used simply as design choice (due to O(n) complexity and I wanted to learn index tables).

Capabilities:

- conversion: c/s/p/d/i/u/x/X/%
- (optional later) flags, width, precision

#### Dispatcher

Takes that description and decides which handler function should run.

#### Handlers (small, single-purpose functions)

- Each handler prints one conversion type and returns “how many chars I wrote”.
- When these are separate, adding a conversion becomes:
	- add one handler + register it (instead of rewriting parser logic).

# Instructions

1. Open ```terminal```, ```cd``` to desired storage directory, ```git clone``` my repository, ```cd``` into the ft_printf directory.

## Make main.c file to test

Copy-and-paste and save as a "main.c" file using Vim, Visual Studio Code, or any text-editor (make plain text):

```c
#include "ft_printf.h"

int	main(void)
{
	ft_printf("Your output is: %li", ft_strlen("Hello World!"));
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

1. There is a Makefile needed to compile the header file (ft_printf.h) into a library (libftprintf.a) to be used for compilation.

2. Again, cd to ft_printf directory, then run command "make".

3. Compile the needed .c file using ```cc main.c libftprintf.a -o output -Wall -Wextra -Werror```

4. NOTE: -Wall -Wextra -Werror (42 uses these 3 warning flags during compilation).

5. Run program with ./output.

# Resources

## Resources used:

- Harvard CS50 Youtube Series (up till Linked Lists), by David J. Malan.
- Various Stackover Flow forum Q&As, mostly for debugging error codes or finding out best practices.
- Several posts and videos about how NOT to code (inefficiencies, silent bugs).
- Peer-shared web-articles.
- Of course needless to say, my peers also were great help in providing invaluable "irl" advice you can't find online :\)

## Usage of A.I.

- Other than Google search's own generative A.I. responses (mostly for VIM commands like :set nonu)
- I subscribe to ChatGPT Plus, so that's the only coding "advisor" I use.

### Usage of ChatGPT
- I have a Custom Folder "Computer Science", whatever help or nerding out goes inside.
- Things like "What 'is' computation?", or "Why is M-series Chips' designed with so much L1-I Cache?" or "Why is division so expensive", goes inside there.
- I often ask for "many ways to do the same thing", such as 5 types of algos (hardcoding INT_MIN, fixed buffer, recursion...) and the tradeoffs.
- I use it for help with projects, which brings me to:

---

## ChatGPT for coding:

### Summary of A.I. usage


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

### Example of prompts used

---

1. What is a variadic function in C?

2. How might I make me printf extensible?

3. What's a dispatch table and how can I use it?

4. Of course, S.O.S. when I'm stuck/ no idea how to start, asking: "Do NOT give me any code (or solution when debugging), guide me through".

---

## Example response: (I formatted it for clarity)

### What is a variadic functions in C?

You already know fixed-arity functions like::

```c
int add(int a, int b);
```

A variadic function says instead: “I know some arguments for sure… and then I’ll accept however many more you want.”

- The canonical example is printf:

```c
printf("x = %d, y = %f\n", x, y);
```

#### The syntax, stripped to its bones

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

#### How C actually makes this work (important mental model)

C does NOT track:

- How many... 

```
(Blah blah blah)
```
---

### How might I make me printf extensible?

---

#### The mental model

printf work naturally separates into 3 layers:

1. Parser
Reads the format string, and when it sees %..., it builds a small description of what it wants:

- conversion: c/s/p/d/i/u/x/X/%
- (optional later) flags, width, precision

2. Dispatcher
Takes that description and decides which handler function should run.

3. Handlers (small, single-purpose functions)
- Each handler prints one conversion type and returns “how many chars I wrote”.

- When these are separate, adding a conversion becomes:
	- add one handler + register it (instead of rewriting parsing logic).

## The Goal of printf/ writing code (in 42):

“Locality” beats cleverness:

- Extensible doesn’t mean abstract for abstract’s sake. In 42, you want:
- small C files per concern,
- ```static``` helpers in each file,
- minimal global shared state (ideally none),
- and a single clear data flow: ```parse → dispatch → handler → write```.

That stays readable under stress — which is the real goal.

---

### What's a dispatch table and how can I use it?

---

Then why does the indexed table exist at all?

Good question. Three reasons:

1. Algorithmic clarity

Indexed dispatch is O(1) by construction.
Linear scan is O(N).

Even if N is small today, the intent is different.

2. Scalability

In:

- Instruction decoders
- syscalls
- interpreters
- protocol parsers

N can be 100s or 1000s.

Then:

- linear scan becomes expensive
- indexed dispatch stays flat

```printf``` is tiny. CPUs are not.

3. Predictability

Indexed lookup:
- no branches
- no data-dependent control flow

This matters in:
- real-time systems
- kernels
- hot loops

Not your ```ft_printf```, but the idea transfers.

### Bad code (so I don't do it)

```c
if (spec == 'c')
{
    count += print_char(va_arg(args, int));
}
else if (spec == 's')
{
    count += print_string(va_arg(args, char *));
}
else if (spec == 'd' || spec == 'i')
{
    count += print_int(va_arg(args, int));
}
else if (spec == 'u')
{
    count += print_unsigned(va_arg(args, unsigned int));
}
else if (spec == 'x')
{
    count += print_hex(va_arg(args, unsigned int), 0);
}
else if (spec == 'X')
{
    count += print_hex(va_arg(args, unsigned int), 1);
}
else if (spec == '%')
{
    count += write(1, "%", 1);
}
```

### (Of huge relevance) 3. Unrelated cases are now coupled

This code:
- violates “no risk of breaking unrelated cases”

This one is subtle and important. If you:
- reorder conditions
-refactor one branch
-add flags logic (#, +, width, precision)

You are editing a single tightly coupled control structure.


#### Example failure modes:

> %d and %i share logic --> one change affects both\
 %x and %X share helper --> branch logic leaks\
 adding flags introduces nested ifs inside each branch


Now changes are non-local: touching one thing risks others.

That’s the opposite of extensible.

## Contact Details

#### For further queries/ help:

I, (42 intra) hnah (Christopher Hui-Kang Nah) am contactable via:

1. Email: christopher_nah@yahoo.com.sg

2. LinkedIn: linkedin.com/in/crispynugget/

```
42BeyondTheCode
```

##### EOF :D
