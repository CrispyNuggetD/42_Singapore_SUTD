# My 42 course notes (Largely ChatGPT)
## Contents
1. [Bitwise stacking using isdigit returns (Proper)](#1-bitwise-stacking-using-isdigit-returns-proper)
2. [5 Compiler optimisations for Clang (My not-so-secret high perf. obsession!)](#2-five-compiler-optimisations-for-clang-my-not-so-secret-high-perf-obsession)
3. [Would adding things like typedefs, enums, bit flags, make the program more inefficient or the compiler does the trick?](#3-would-adding-things-like-typedefs-enums-bit-flags-make-the-program-more-inefficient-or-the-compiler-does-the-trick)
4. [What are LUTs (Look-Up Table)? Linked to (1)'s code optimisation.](#4-what-are-luts-look-up-table-linked-to-1s-code-optimisation)
5. [LUT optimisation for (1)'s code optimisation, using static const.](#5-lut-optimisation-for-1s-code-optimisation-using-static-const)
6. [How to "just check" for potential conflicts without messing up git repo before pulling.](#6-how-to-just-check-for-potential-conflicts-without-messing-up-git-repo-before-pulling)
7. [Four targets 42 enforces in Makefiles.](#7-four-targets-42-enforces-in-makefiles)
8. [Vim: Opening a file directly to a line number.](#8-vim-opening-a-file-directly-to-a-line-number)
9. [Short note on va_lists](#9-short-note-on-va_lists)
10. [What is a variadic functions in C?](#10-what-is-a-variadic-functions-in-c)


##### [Back to top of contents list](#contents)
## Note to self:
- As much I love going into rabbit holes, sometimes it's way beyond what I can handle (e.g. at Week 2 Core)
- Hence, checklists denotes to learn OR learnt thereafter (when I have time)
- Formatting: 
- [ ] To learn (the below) when I have time
- [x] Learnt!
- Also, for things I'm particularly interested or that they're relevant to me, they get: **(IMPORTANT FOR ME)
  
---

#### 1. Bitwise stacking using isdigit returns (Proper)

---
- Alex senior had a HACK of his Libft isdigit library returning in powers of 2 for bitwise usage.
- However, ChatGPT says that’s secretly cursed because powers of 2 is internal, guaranteed returns are 0, 1.
- Proper way:
 
```c
// First:
enum e_char_flags {
    CF_ALPHA = 1 << 0,  // 0001
    CF_DIGIT = 1 << 1,  // 0010
    CF_SPACE = 1 << 2,  // 0100
    // etc.
};

int get_char_flags(int c)
{
    int flags = 0;

    if (ft_isalpha(c))
        flags |= CF_ALPHA;
    if (ft_isdigit(c))
        flags |= CF_DIGIT;
    if (ft_isspace(c))
        flags |= CF_SPACE;
    return flags;
}
// Then can be used by:
int flags = get_char_flags(c);

if (flags & (CF_ALPHA | CF_DIGIT)) {
    // c is alphanumeric
}
```

- Note to self: (Enums = fancy ints with named constants.)
- That keeps: ft_isalpha, ft_isdigit, etc. clean and spec-compliant (0/1 return)
- All the bitmask magic in a separate abstraction designed for it

#### Extras, maybe near future: (Way beyond what I can handle at Week 2 Core)
- (TL;DR) LUT = Look-Up Table (Pronounced loot).
> A LUT is just a precomputed array that lets you replace a slow calculation with a single array access.
- Simply: store the answers in advance, then index with input, then done.

#### (More details, redirection to what are LUTs)
4\. [What are LUTs (Look-Up Table)? Linked to (1)'s code optimisation.](#4-what-are-luts-look-up-table-linked-to-1s-code-optimisation)
#### (Actual LUT optimisation using static const.)
5\. [LUT optimisation for (1)'s code optimisation, using static const.](#5-lut-optimisation-for-1s-code-optimisation-using-static-const)

##### [Back to contents list](#contents)

---

#### 2. Five Compiler optimisations for Clang (My not-so-secret high perf. obsession!)

---

#### A primer: Five examples of types of available optimization:

1\. Inlining

> Inlining replaces a function call with the body of that function.

- For example:

```scss
foo();
```

- Becomes:

```arduino
do stuff in foo directly here
```
- Effects:
	- No jump hence, fewer branches, hence faster.
	- **But...** it makes the instruction footprint bigger.
	- Lots of inlining → bigger code → possible L1I pressure.
2\. Loop unrolling

> Expands one loop iteration into many operations.

From:
```c
for (i = 0; i < 4; i++)
    sum += a[i];
```

- Unroll 4 times:
```c
sum += a[0];
sum += a[1];
sum += a[2];
sum += a[3];
```

- Effect:

1. fewer jumps
2. fewer branch predictions
3. more chances to vectorise

- **But again...** code becomes bulky, meaning larger instruction footprint, meaning I-cache (L1I) pressure.

3\. Branching / branch predictor

> A branch is any point where execution may jump:

```c
if (x > 0) { ... }
```
So it is essentially:
- CPU tries to guess which path you’ll take (branch prediction).
- If the CPU guesses wrong, it must:

1. flush pipeline
2. re-fetch instructions
3. waste cycles

Optimisation attempts to reduce or predict branches, is using:

1. loop rotation
2. duplication of hot paths
3. branch flattening

4\. Vectorisation

> Transform scalar operations into SIMD (Single Instruction, Multiple Data).

- Scalar:

```c
c[i] = a[i] + b[i];
```

Vectorised (NEON? / AVX?):

```arduino
process 4–16 elements per instruction
```

- This massively speeds up DSP, image processing, ML…
- but requires strict alignment and no aliasing.

5\. Strength reduction

> Replace expensive operations with cheaper ones.

```ini
x = y * 8;   →   x = y << 3
```

or:

```arduino
i * 10 inside loop  → convert to running addition
```

#### **Clang supports the classic GCC-style flags:**
#### 0. O0, No optimization (default)
#### 1. O1, Simple optimizations
- [ ] To learn (the below) when I have time
- constant folding
- remove obvious dead code
- minimal inlining
#### 2. O2, Standard “safe but fast”
- [ ] To learn (the below) when I have time
- larger inlining
- loop optimizations
- vectorization
- strength reduction
- common subexpression elimination
- branch prediction hints

This is the commonly recommended setting for production.

#### 3. O3, Maximum speed, aggressive
- [ ] To learn (the below) when I have time
- unrolling
- more aggressive vectorization
- may increase code size
- may occasionally cause pathological slowdowns

Good for performance tests.

#### 4. Os, Optimize for small code size
- [ ] To learn (the below) when I have time
Good for embedded systems.

#### 5. Ofast
- [ ] To learn (the below) when I have time
- Breaks strict IEEE math rules for speed
- Unsafe unless you really know what you're doing.

Would NOT pass 42 tests where correctness matters.

##### [Back to contents list](#contents)

---

#### 3. Would adding things like typedefs, enums, bit flags, make the program more inefficient or the compiler does the trick?

---
ChatGPT: No. Zero cost. Literally zero.

Reason: These constructs exist only at compile-time.
- typedef disappears entirely
- enum becomes an int
- bitwise OR | stays as a CPU instruction.

- the compiler inlines and folds constants aggressively

After compilation, the CPU sees this:

```c
f = CF_ALPHA | CF_DIGIT;
if (f & (CF_ALPHA | CF_DIGIT)) { ... }
```

as something like this in assembly:

```asm
mov eax, 3    ; CF_ALPHA|CF_DIGIT = 1|2 = 3, constant folded
test eax, 3   ; bit test
jnz ...
```

There is no runtime cost, no inefficiency, no penalty.
These features make code more readable and error-proof without costing anything.

##### [Back to contents list](#contents)

---

#### 4. What are LUTs (Look-Up Table)? Linked to (1)'s code optimisa8. [Vim: Opening a file directly to a line number.](#8-vim-opening-a-file-directly-to-a-line-number)
tion.

---
#### Linked from (1)'s code optimisation.
1\. [Bitwise stacking using isdigit returns (Proper)](#1-bitwise-stacking-using-isdigit-returns-proper)

- (TL;DR) LUT = Look-Up Table (Pronounced loot).
> A LUT is just a precomputed array that lets you replace a slow calculation with a single array access.

- Simply: store the answers in advance, then index with input, then done.
 
#### Simple example (intuition)

- Without LUT, every time I want to check if a character is a digit:
           
```c
if (c >= '0' && c <= '9')
```

- The CPU must evaluate comparisons.
- BUT with LUT, I first create a table for all 256 possible ASCII characters:

```c
char is_digit[256] = { /* I ne#### 6. How to “just check” for potential conflicts without messing up git repo before pulling.ed to fill with 1 or 0s */ }
```

- Which is cool, because checking simply becomes this:

```c
if (is_digit[(unsigned char)c])
```

- So, TIL:
- That’s a single array lookup.
- No logic, no branching, no conditions.
- [ ] To learn (the below) when I have time
- That this is hugely powerful in hot loops, parsers, tokenizers, compilers, and OS code.
#### Why LUTs matter in performance
#### 1. Replace expensive operations
Examples:

- [ ] To learn (the below) when I have time
- trig functions
- bit counting
- character classification
- cryptography S-boxes
- noise functions
- color conversion
- [ ] To learn (the below) when I have time
- DSP filters **(IMPORTANT FOR ME) 
Instead of recomputing a function thousands of times, you store the results.
#### 2. Use O(1) time
- Every lookup is constant time.
               
```c
result = table[x];
```

- Which is as fast as memory access gets.
#### 3. Remove branches (avoids branch misprediction)
- Branch misprediction (~15–20 CPU cycles penalty), vs
- LUT array indexing (~1 cycle or less if cached).
- [ ] To learn (the below) when I have time
- Parsers (JSON, HTML, CSV), compilers, tokenizers all use LUTs for this reason.

##### [Back to contents list](#contents)

---

#### 5. LUT optimisation for (1)'s code optimisation, using static const.

---

#### Linked from (1)’s code optimisation.
1\. [Bitwise stacking using isdigit returns (Proper)](#1-bitwise-stacking-using-isdigit-returns-proper)

From earlier:

```c
t_char_flags ft_char_flags(int c)
```

I can convert it from:

- Slow version (multiple ifs):
  
```c
if (ft_isalpha(c))
    flags |= CF_ALPHA;
if (ft_isdigit(c))
    flags |= CF_DIGIT;
...
```

- [ ] To learn (the below) when I have time
To the Fast version (LUT):

```
static const t_char_flags g_char_lut[256] = {
   /* precomputed flags */
};

// Either:
return g_char_lut[(unsigned char)c];

// OR:
t_char_flags ft_char_flags(int c)
{
    if ((unsigned char)c > 127)
        return CF_NONE; // or however you want to handle
    return g_char_lut[(unsigned char)c];
}
```

- Then ```ft_char_flags``` becomes an O(1) array lookup instead of a bunch of `if`s.
- This turns 5–10 conditional checks into one lookup.
- This is how high-performance lexers (C compilers, JSON parsers, regex engines) do it.

#### ChatGPT: Fun analogy
A LUT is like:

- Instead of calculating square roots every time, instead, you store a table of ```sqrt(n)```
- Instead of calculating sins and cosines, instead, store all values for 0–360°
- Instead of figuring out if a char is alphanumeric, instead, store flags in a table
Your program turns from “thinking” into “just reading”.
- [ ] To learn (the below) when I have time
- If you want, I can show you a real-world LUT example from Clang’s tokenizer or Linux kernel so you see how deeply embedded this idea is in systems programming.

##### [Back to contents list](#contents)

---

#### 6. How to “just check” for potential conflicts without messing up git repo before pulling.

---

```
git fetch
git status
git diff HEAD..origin/main
```

##### [Back to contents list](#contents)

---

#### 7. Four targets 42 enforces in Makefiles.

---

- Rough Makefile:

```make
# Compiler and flags
CC     = cc
CFLAGS = -Wall -Wextra -Werror

# Library name
NAME   = libft.a

# Source files (add more as you implement them)
SRCS   = ft_isalpha.c \
         ft_isdigit.c \
         ft_strlen.c \
         ft_putchar_fd.c

# Object files = same names but .o
OBJS   = $(SRCS:.c=.o)

# Default target
all: $(NAME)

# Build the library from object files
$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)	

# Rule to build any .o from its .c
%.o: %.c libft.h
	$(CC) $(CFLAGS) -c $< -o $@

# Remove .o files
clean:
	rm -f $(OBJS)

# Remove .o and the library
fclean: clean
	rm -f $(NAME)

# Rebuild everything
re: fclean all

.PHONY: all clean fclean re
```

- **42 four targets required**:
    1. all (build all main targets)
        - build the main output (libft.a)
    2. clean (clean workspace (intermediate stuff))
        - delete .o files
    3. fclean (full clean)
        - delete .o + final binary/lib
    4. re (rebuild)
        - full clean + rebuild everything

- **.PHONY - tells make these targets aren’t real files.**

- This is a **target rule**:
 
```make
 $(NAME): $(OBJS)
``` 

- Target:

    - $(NAME) → usually libft.a

- Dependencies:

    - $(OBJS) → list of all .o files (compiled from your .c files)
- Meaning:

> “To build libft.a, you must **first** have all the .o files.
If any .o is newer than libft.a (or libft.a is missing),
**then run** the recipe that **follows**.”

```make
ar rcs $(NAME) $(OBJS)
```

> ar = archiver; it bundles .o files into a .a static library.

> r = replace or add files to archive

> c = create the archive if it doesn’t exists

> s = build index (symbol table) → makes linking faster

```make
%.o: %.c libft.h
```

- This rule says:

> “To build X.o, you need X.c and libft.h.”

- %.o: %.c (pattern rule: how to turn any .c into corresponding .o)
 
    - **$< = first dependency (the .c)**

    - **$@ = target (the .o)**

##### [Back to contents list](#contents)

---

#### 8. Vim: Opening a file directly to a line number.

---

- When opening a file from the terminal, you can specify the line number directly.

```sh
vim +123 filename.txt
```

##### [Back to contents list](#contents)

---

#### 9. Short note on va_lists

---

- va_list feels spooky: it’s not a list, it’s a cursor over raw call-frame memory (or register save areas).

##### [Back to contents list](#contents)

---

#### 10. What is a variadic functions in C?

---

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

---

##### [Back to contents list](#contents)
