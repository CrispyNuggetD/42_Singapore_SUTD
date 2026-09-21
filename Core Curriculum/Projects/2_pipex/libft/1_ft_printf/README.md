*This project has been created as part of the 42 curriculum by hnah.*

> Post-submission update (2026-09-14): printf now supports a configurable output FD for reuse in Pipex. See [Post-submission update: custom FD and Ryker libft](#post-submission-update-custom-fd-and-ryker-libft) for the changes from the submitted implementation.

> Post-submission update (2026-09-21): struct initialization now uses our own `ft_memset`. See [the initialization update](#post-submission-update-explicit-struct-initialization).
# Description

- This project recodes a *somewhat relatively* functional ```printf()``` function as per found in ```<stdio.h>```.
- It differs from most other 42 implementations where I also did both bonuses (all flags, precision)
- I also implemented length modifiers, since it's ambiguous whether it's needed, plus it's fun and I get to learn more! (Most students do not attempt length modifiers.)
- However, it does **NOT** implement the buffer management of the original ```printf()```
- It references "Libft" project which is our own 42-student made glibc/ musl implementation.
- This projects also highlights how a makefile can be used to compile another Libft sub-directory makefile properly.

## About my project

### Preface: TL;DR

- I was inspired by intra ```dthoo```'s poor life choices (```\lighthearted```)
- Hence, I also somehow managed to convince myself to do BOTH bonuses (Although only 1 required)
	- Why?
		- It's more *complete* (albeit *without* buffer management)
		- I *LEARNT* a lot from the process (Considerations for codebase's design choice, etc.)
		- It was painstaking to code, but was *REAL FUN* and totally *WORTH IT* working on it in hindsight
		- Debugging became increasingly easy due to chosen ```Modular``` and ```extensible``` codebase.
- This ```README.md``` documents:
	- What *exactly* are the bonuses
	- What *are* the *edge cases* considered
	- How that lead to *final* programmatic framework.
- My program is extensible and modular in nature, using these 4 modules:
	1. Initialiser / Registration (Function keys -> handlers)
	2. Parser (turn ```%...``` into a spec description)
	3. Dispatcher (choose which handler to call)
	4. Handlers (do the actual formatting/ output)
- 4 main limitations of my ```ft_printf``` is documented near the bottom of this ```README.md```
	1. Positional arguments (```$1```, i.e. first argument)
	2. Other specifiers not required by ```Subject.PDF``` ```(f F e E g G a A)```
	3. Wide characters (```L"hello"```)
	4. Four of the "Less common/ more complicated" length type modifiers ```(j z t L)```
	5. Wildcard dynamic arguments (e.g. "%.*s", 2, "test")

---

#### --> Difference in Implementation (with ```dthoo```): 

Yes, I *apparently* did not copy his work. (*Audience gasps!*)

- So, he had many confusing convuluted functions (to me) that needs many parameters
- I have many ```structs``` and ```modules```, and use ```function pointers```.
	- Whether "too many" depends on your preference and perception
	- But it was very painful to code
		- Maybe I'm new to this
		- Or maybe it's just 'cos ```function pointers```
		- \#cope
- Also, my program:
	- Doesn't uses any ```malloc```
	- Handles length specifiers
	- Handles ```write``` syscall partial printing issues. 

##### :D Cool right? I hope you agree :\(

---

### My ft_printf supports the following input syntax:

```c
%[flags][width][.precision][length]specifier
```

Where the type is one letter.

- An example will be something like ```%-08.2u``` (Just like actual ```printf()```)

## Requirements for Bonus

Let's first find out more about the Bonus segments first which influenced how my project is designed (e.g. Structs, Modularisation, etc.):

### The 7 bonus features (5 flags + Precision specifier + Minimum Field Width)

#### (Class A) 5 Flag characters

1. ```0``` flag (zero pad)

If padding is needed to reach the minimum field width, pad with '0' characters instead of spaces.

2. ```-``` left align (pad on right)

3. ```+``` always show sign for signed conversions

4. space (``` ```) leading space for positive signed conversions

5. ```#``` alternate form (prefix for hex, etc.)

#### (Class B) 2 Precision features (via ```.```)

6. ```.``` + digits (**OR only just** ```.```) specifies precision
7. Precision is “present or not”, and if present it has a number
	- (Number **CAN BE** 0)!

---

### "Challenging Scenarios" of Extra Bonus 

1. Flags can appear in any order, and may repeat.
2. Width can be multiple digits. (Or absent)
3. Precision may be absent OR present with no digits
    - The “zero value with percision zero" rule (Big trap, explained in conflict rule 3)
4. Even with conflict rules (below), width applies to the whole formatted field (sign, spaces, prefixes e.g. ```0x```, digits that can be zero-padded with precision).
5. “Padding location” depends on flags and precision

#### --> The hardest bonus interactions are around numbers

Especially when value is ```0```.

- If precision is specified as 0 and the number is 0, the digit string becomes empty (for many conversions). **Width still applies.**
- Then prefixes/signs interact with emptiness.

We cannot assume numeric code always at least one digit, so “digits building” is designed to allow empty output for the core digits, while still allowing prefix/ padding.

## The 4 conflict rules

#### 1. ```-``` with ```0```

> ```%-05d```

Rule: ```-``` wins, **HENCE**, Zero padding is disabled

- Output: ```42   (spaces on right)```

---

#### 2. ```+``` with space

> ```%+ d```

Rule: ```+``` wins, **HENCE**, Space is ignored

- Output: ```+42```

---

#### 3. Precision with ```0``` (numeric)

> ```%0.5d```

Rule: Precision wins, **AND SO**, '''0''' flag ignored

- Output: ```00042```

#### --> The zeroes come from precision, not the 0 flag.

---

#### 4. ```#``` with ```x/X``` (and ```zero```)

> ```%#x with 0```

Rule: No redundant prefix

- Output: Prints ```0```, not ```0x0```

#### --> **BUT AGAIN: Prefix handling must be conditional**

> ```%#x with 42```

- "Normal" ``0x`` Output: ```0x2a```

## Back to my implementation


### Length Modifiers

The length sub-specifier modifies the length of the data type.

- Simplier, supported, and more common ones (add unsigned for non decimal):
	1. hh (signed char)
	2. h (short int)
	3. l (long int)
	4. ll (long long int)
- Others (UPDATE: Not supported due to way out of scope + needing ```<stdint.h>``` / ```<stddef.h>```):
	1. j (intmax_t/ uintmax_t for non decimal)
	2. z (size_t)
	3. t (ptrdiff_t)
	4. The L (long double)/ none (double) are not relevant as other specifiers not required (f F e E g G a A)

##### --> Implementation of length modifiers

For example (for d/i):

- no length: read int
- l: read long
- ll: read long long
- h: read int, then cast to short (because default argument promotions)
- hh: read int, then cast to signed char

For u/x/X:

- no length: unsigned int
- l: unsigned long
- ll: unsigned long long
- h/hh: read unsigned int, cast down

For c/s/p/%:

- length modifiers are basically irrelevant (or invalid/ignored).

### Structs, typedefs, bitmaps and bitwise, to tackle bonus:

These are the defined 3 ```typedefs``` for tackling data handling

### 1. ```t_handler``` that are (void) function pointers
My program uses the following ```typedef``` to work with ```t\_handler``` that are ```int``` function pointers, each function taking a pointer to a ```t\_context``` ```struct```:

```c
typedef int	(*t_handler)(t_context *context);
```

---

### 2. ```t_spec``` holds input's bonus flags

It also uses the following ```struct``` to contain the input flags data to be parsed:

```c
typedef struct  s_spec
{
    unsigned char   flags;
    int             width;
    int             precision;
    char            conversion;
}                   t_spec;
```
#### --> About the ```flags```

```unsigned char flags``` is used to contain the bits needed for bitmask representation of the flags **as well as** ```precision```.

#### --> ```.``` is not truly a flag it just *introduces* precision.

Unlike ```42's Subject.pdf``` bonus lumping ```.``` with the other flags, in ```printf``` semantics it's *not* really a type (as you can see from input syntax).

However, just like ```42's Subject.pdf```, my implementation combines existence of ```.``` with bitwise flags.

- This prevents needing to define another ```int``` or ```_Bool``` just for ```has_precision```, as ```precision``` is **not** a flag.

---

### 3. ```t_field``` holds output pieces (sign/ prefix/ core/ padding)

While the aforementioned ```t_spec``` de facto holds the “contract / request”:

- Another ```t_field struct``` holds the “invoice / final bill” (exact pieces and lengths)

It is defined as:

```c
typedef struct  s_print
{
	char		sign;
	const char	*core;
	size_t		core_len;
	const char	*prefix;
	size_t		prefix_len;
	size_t		prec_zeros;
	size_t		pad_len;
	char		pad_char;
}				t_print;
```

---

#### --> But why is ```t_spec``` alone not enough for the printer (```ft_printf_printer```)?

```t_spec``` alone does not hold *"final"* info the printer needs. 

- Example:

> %08.3d with value -12

Now, ```t_spec``` already holds: ```width=8```, ```precision=3```, flags include ```0``` and ```.```

But my printer also need to know:

1. sign = ```'-'```
2. digits = ```"12"``` (core)
3. prec_zeros = ```1``` (to make ```012```)
4. pad_len = ```8 - (1 sign + 1 prec_zero + 2 digits) = 4```

#### --> **Furthermore**, complication is to be handled (pad_char must be ```' '``` not ```'0'```)

That's because precision suppresses zero-padding for numerics, which is detailed in a later section covering flags conflict resolution.

- My normalizer bitmasking ```normalise_flags``` function resets the bit to do ```' '``` and not ```'0'```.

---

### One more benefit of using ```t_print```: 
- My design also works for:
	- ```%c``` too (core_len = ```1```)
	- And for the “precision makes core empty” case (```%.0d``` with ```0```) 
		- Because core_len can legitimately be ```0```...!

---

### Why my implementation choice is that of Modularisation

Modularisation is used instead of convuluted bad-habit non-extensible coding:

- Why? Consider:

```c
if (a = condition_a)
{
    do_a(complicated_a_input);
}
else if (b = condition_b)
{
    do_b(complicated_b_input);
}
else if (c = condition_c)
{
    do_c(complicated_c_input);
}
else if (d = condition_d)
{
    do_d(complicated_d_input);
}
else
{
    do_something_dangerous(excluding_everything_else_is_error_prone);
    
}
// Non-extensible code? Difficult to shift condition 'z' else if somewhere into nested loops?
else if (z = condition_z)
{
    do_z(complicated_z_input_does_not_conflict_with_original_else_statement_exclusions);
}
```

Statements. It is error prone to code, to maintain, to extend, and separation of implementation portions if ever needed can cause “branch explosion”

---

### Hence, Modular technique:

Four compartmentalised parts to prevent large refactoring requirements/ logic bugs:

1. Initialiser / Registration (Function keys -> handlers)
2. Parser (turn ```%...``` into a spec description)
3. Dispatcher (choose which handler to call)
4. Handlers (do the actual formatting/output)

An explanation of the modules are as follows:

### 1. Initialiser and Registration

Makes function pointers fixed at compile, not run time.

- Example:

```c
// Initialise keys and also allows dispatcher to fetch those keys
static t_handler	init_get_handlers(unsigned char fn_key)
{
	static t_handler const	handlers[256] = {
	['i'] = ft_printf_d_i,
	['d'] = ft_printf_d_i,
	['%'] = ft_printf_percent,
	(...),
	};

	return (handlers[fn_key]);
}
```

---

### 2. Parser

Reads the format string, and when it sees %..., it builds a small description of what it wants:

- Uses indexing table and function pointers
- Being a contiguous array, memory lookup of pointers and dispatching is O(1) complexity
- Paired lists, albeit arguably clearer, were not used simply as design choice (due to O(n) complexity and I wanted to learn index tables).

Capabilities:

- conversion: c/s/p/d/i/u/x/X/%
- flags, width, precision

Example of parsing:

- (“flags: [precision=1, ```-``` + ```0```], width=5, , precision=3, conversion=```d```”)

---

#### --> How parsing is done (reading and string advancement with pointer-to-pointer)

TL;DR

- The parser takes a ```const char **``` cursor into the format string and advances it as it consumes the conversion specifier.

Or also AKA:

- The parser takes a pointer to pointer (pointer to caller's cursor that shows the format-string of the input string after a ```%``` is detected).
- It advances the caller's cursor AKA the pointed-to pointer (```*ptr```) inside that function hence consuming the flags, etc.

#### --> For the context ```struct```, ```va_list``` is passed as a pointer 
(According to C standards) Why is ```va_list``` passed as a pointer to that list?
- Although we can pass a va_list to another function, but if that function uses va_arg(abc, ...), then the value of abc back in the caller will become indeterminate, meaning it could be garbage and we should not use it afterwards (except for ```va_end```)
- Hence, I need to pass a pointer to it (```va_list *```), so the handler consumes arguments and the updated state persists.

---

### Parsing steps for flags retrieval thereafter

When hit ```%```, do these 5 steps:

#### (A) Flags

1. Read flags: while current char is in ```"-0+ #"```
2. Then read width: while digit and before ```.```
3. Then precision: if ```.``` then mark precision specified; parse digits (or 0 if none)
4. Finally, read the conversion: next char is the specifier

#### (B) Then do **bitwise normalisation**:

- if ```-``` set -> clear ```0```
- if ```+``` set -> clear ```space```
- if precision specified numeric conversion (```is_numeric```, explained **below**) -> clear ```0``` padding behavior
- if character, string or ```%%``` -> also clear ```0``` padding behavior

#### --> ```is_numeric``` to tell if numeric conversions
In real printf behavior, precision suppresses 0 padding only for numeric conversions (e.g., d i u x X, etc.).

- ```is_numeric()``` is a tiny helper that returns true for ```d i u x X``` and thus gives conversion info at normalization time

---

### Difference with conventional "mandatory-only" code

Mandatory-only implementations often:

- Parse % then immediately execute
- No ```structs``` required

Bonus-ready implementations:

- Parse % into a spec struct, normalize conflicts, then execute

---

### 3. Dispatcher

Takes that description and decides which handler function should run.

Example of Dispatching:

- (“conv = ```d```  -> call the integer handler”)

Then, dispatches that identified function with a function call with ```t_context``` (```flags``` + ```input```)

- ```va_list``` stored elsewhere in another ```struct```

---

### 4. Handlers (small, single-purpose functions)

Handler builds ```t_print struct``` fields (members) using ```t_context struct``` and passes to ```ft_printf_printer``` function to format and print the final output string.

- Each handler prints one conversion type and returns “how many chars I wrote"
- Since implementation separates handlers, adding another conversion becomes:
	- add one handler + register it (instead of rewriting parser logic!)

#### --> ```ft_printf_printer``` formats and prints final output

This function, *finally*, prints the final formatted string.

- With the following format:
> \[pad][sign][prefix][precision-zeroes][core][pad]

It does this while first considering:

- Whether the padding is on the **right**, and 
- ```0``` decides whether the **left** padding is zeros (with *some* rules).

#### --> ```ft_printf_printer``` handles partial printing too

- ```write()``` returns ```(size_t)``` length printed, can be use to check for partial printing.
- Implementation can use loops to check total length have been printed, and pointer advancements to print remainder if any, as such:

```c
while (len > 0)
{
	w = write(1, p, len);
	if (w < 0)
		return (-1);
	p += w;
	len -= (size_t)w;
}
```

#### Could ```write()``` ever be partial?

Sometimes, when we request syscall of ```write()``` and have (especially), a long buffer and length to print, some can end up being trimmed/ not fully printed.

##### --> This is learnt from ChatGPT:

Even to stdout, it often writes everything, but it’s not guaranteed. Partial writes commonly happen with:

- pipes or sockets
- non-blocking file descriptors
- signals interrupting syscalls
- full buffers or backpressure

# Limitations

At least these 3 (or more) are not handled:

### 1. Positional arguments

```ft_printf("%1$s", NULL)```

- Mine: 
	- Output:
	- Length: -1
- Actual printf: 
	- Output: (null)
	- Length: 6

#### --> Why not?

- Additional layer of complexitity, argument table layer needed

### 2. Other specifiers (including floats, ```%f```)

Other non-Subject.PDF specifiers not implemented, including:

- (f F e E g G a A)

Example:

```ft_printf("%-f", 42.5)```

- Mine: 
	- Output:
	- Length: -1
- Actual printf: 
	- Output: 42.500000
	- Length: 9

#### --> Why not?

- Specifier not needed, in fact I might as well leave it for live coding

### 3. Wide characters

```ft_printf("%ls", L"hello")```

- Mine: 
	- Output: h
	- Length: 1
- Actual printf: 
	- Output: hello
	- Length: 5

#### --> Why not?

Unnecessary even more addition of something that's of medium-to-high complexity:

- New converter path
- New buffer management
- Careful precision handling
- Likely new helpers/files

### 4. Four of the "Less common/ more complicated" length type modifiers ```(j z t L)```

Only (l ll h hh) are supported.

#### --> Why not the others?

Refactoring and additional complexity required.

# Instructions

1. Open ```terminal```, ```cd``` to desired storage directory, ```git clone``` my repository, ```cd``` into the ft_printf directory.

## Make main.c file to test

1. Uncomment ```int main(void)``` of ```ft_printf.c``` using Vim, Visual Studio Code, or any text-editor (make plain text).
2. Edit printf commands as desired.

```c
/*
 * Running instructions:
 * 2 steps.
 *
 * 1. Change %<current command> to the type of output function gives (first argument, left hand side)
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
 * %x		- Unsigned Integer (lowercase Hexadecimal)
 * %X		- Unsigned Integer (UPPERCASE Hexadecimal)
 * %%		- Prints a percent sign
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
3. Compile the needed .c file using ```cc ft_printf.c libftprintf.a -o output```
4. NOTE: -Wall -Wextra -Werror (42 uses these 3 extra warning flags during compilation).
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
- refactor one branch
- add flags logic (#, +, width, precision)

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

---

## Post-submission update: custom FD and Ryker libft

Date: 2026-09-14. This section documents changes made after submission; the earlier sections describe the original project.

### Why Pipex needed this update

Pipex needs formatted diagnostics on stderr (FD 2) and, for its here-document, output to a selected file. Ordinary `ft_printf(...)` must remain usable without an FD argument and defaults to stdout (FD 1). The wrapper is supplied by the expanded Ryker libft package.

For the archive/package distinction, directory layout and reuse instructions, see the [libft post-submission update](../0_libft/README.md#post-submission-update-expanded-ryker-libft).

### Changes from the submitted printf

| File or function | Change |
| --- | --- |
| `ft_printf.h` | Adds `int fd` to `t_context` and declares `ft_vprintf` and `ft_printf_fd_setting`. |
| `ft_printf_fd_setting.c` | New function-local static FD, initially 1; nonnegative arguments set it, negative arguments retrieve it without changing it. |
| `ft_printf_init_t_context` | Copies the current FD setting into the context after clearing it. |
| `write_guaranteed` and `write_repeat` | Replace `write(1, ...)` with `write(context->fd, ...)`. Existing partial-write and error behavior stays as written. |
| `ft_printf.c` | Moves the existing formatting loop into `ft_vprintf(const char *, va_list *)`; the normal `ft_printf(...)` prepares arguments, calls it, cleans up and returns its result. |
| `error_and_return` | Removed because each variadic entry now performs `va_end` after the shared formatter returns, on both success and failure. |
| Standalone Makefile | Includes `ft_printf_fd_setting.c` in both mandatory and bonus builds. |

The parser, dispatch table, conversion handlers, flag normalization, padding, precision, length modifiers and original null-string/pointer formatting are retained. No wildcard `*` support, new conversions, EINTR retries or output-count overflow features were added. The earlier distilled Ryker formatter was replaced with the small wrapper below.

### Custom Ryker wrapper

The package provides `ryker_ft/printf_fd/ryker_ft_printf_fd.c`:

```c
int ryker_ft_printf_fd(int fd, const char *str, ...);
```

It saves the current FD, sets the requested FD, creates a `va_list`, calls `ft_vprintf`, calls `va_end`, restores the saved FD and returns the formatter's result. Restoration also happens when formatting or writing returns an error. A negative FD is rejected before changing the setting. It restores the previous value, which is not necessarily 1, and does not open or close the descriptor.

```c
#include "ft_printf.h"
#include "ryker_ft.h"

ft_printf("Normal output: %d\n", 42);
ryker_ft_printf_fd(2, "pipex: %s\n", "command not found");
ft_printf("Normal output again\n");
```

### Why the shared function receives a va_list pointer

C cannot forward an existing argument list by passing it as one argument to a function taking `...`. Both public entries therefore call `ft_vprintf` with the address of their locally initialized `va_list`. The formatter passes that pointer onward to the original coordinator and handlers; it does not copy or restart the arguments. Each entry keeps `va_start` and `va_end` together.

```text
ft_printf(...)                 ryker_ft_printf_fd(fd, ...)
    |                              | save FD, set custom FD
    | va_start                     | va_start
    +--------- ft_vprintf ---------+
                   |
          original formatting loop
          parser -> handler -> write(context.fd)
                   |
    +---------- return ------------+
    | va_end                       | va_end, restore saved FD
    | return result                | return result
```

The setting is persistent within a process; direct callers of `ft_printf_fd_setting` must restore it themselves if desired. The Ryker wrapper does that automatically. This shared setting is intended for sequential use and is not thread-safe. Forked processes inherit their own copy. Recompile sources after updating: adding a struct member does not promise compatibility with previously compiled objects.

### Building standalone printf

The standalone project still provides `make` for mandatory and `make bonus` for bonus. Switch using a clean build, for example `make fclean && make bonus`. Its bundled base libft stays independent to avoid pulling the expanded bonus library into a mandatory build.

The package builds the bonus variant. Its printf header uses a relative include for the packaged base libft header; the standalone project keeps its own base-libft include. Package build and copying instructions are documented in the [libft update](../0_libft/README.md#post-submission-update-expanded-ryker-libft).

### Authorship and validation scope

The original formatter remains the basis of this code. AI assisted with the post-submission argument-forwarding refactor, FD-setting function, custom wrapper, build integration and this documentation, under my direction. These additions should not be confused with the original submitted implementation.

Build and Norm checks do not by themselves establish identical behavior on every platform. In particular, the original numeric helpers' `va_list` handling is retained; campus-environment regression testing remains relevant when moving between architectures.

## Post-submission update: explicit struct initialization

Date: 2026-09-21. This is a change after submission, separate from the original
formatter described above.

The print/context initializers now use `ft_memset(ptr, 0, sizeof(*ptr))` instead
of assigning a zero-initialized compound literal. With the campus Clang build,
the old struct assignments generated external `memset` and `memcpy` calls.
Calling our own `ft_memset` directly removes those imports from the Pipex
binaries under the tested build flags. No copy is needed, so `ft_memcpy` is not
used in these initializers. The space padding character and configured output
FD are still assigned after clearing the structs.

This change is synchronized across the standalone printf project and the four
expanded-library copies: master, Pipex, FdF, and push_swap. It does not add format
specifiers or change the intended output/return-value behavior. AI assisted with
the symbol inspection, mechanical edits, and focused regression checks under my
direction. These edits are not claimed to be part of the original submission.

Validation on the campus compiler: the standalone mandatory build passed nine
output/return-value comparisons with standard printf; its bonus build and each
of the four expanded-library copies passed twenty. Cases cover empty/literal
output, characters including NUL, strings, integer limits, hex, and pointers;
bonus checks also cover padding, precision, signs, and alternate form. Both
Pipex builds, FdF, push_swap, and the master library build passed. The changed
helper passes Norminette. These are focused regression checks, not an exhaustive
printf-conformance audit or a claim about the original submitted binary.
