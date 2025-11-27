# My 42 course notes (Largely ChatGPT)
## Contents
1. [Bitwise stacking using isdigit returns (Proper)](#1-bitwise-stacking-using-isdigit-returns-proper)
2. [5 Compiler optimisations for Clang (My not-so-secret high perf. obsession!)](#2-five-compiler-optimisations-for-clang-my-not-so-secret-high-perf-obsession)
3. [Would adding things like typedefs, enums, bit flags, make the program more inefficient or the compiler does the trick?](#3-would-adding-things-like-typedefs-enums-bit-flags-make-the-program-more-inefficient-or-the-compiler-does-the-trick)
4. [What are LUTs (Look-Up Table)? Linked to (1)'s code optimisation.](#4-what-are-luts-look-up-table-linked-to-1s-code-optimisation)
5. [LUT optimisation for (1)'s code optimisation, using static const.](#5-lut-optimisation-for-1s-code-optimisation-using-static-const)

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

#### (More details, redirection to Contents: 4)
4. [What are LUTs (Look-Up Table)? Linked to (1)'s code optimisation.](#4-what-are-luts-look-up-table-linked-to-1s-code-optimisation)
#### (Actual LUT optimisation using static const.)
5. [LUT optimisation for (1)'s code optimisation, using static const.](#5-lut-optimisation-for-1s-code-optimisation-using-static-const)

##### [Back to contents list](#contents)
---
#### 2. Five Compiler optimisations for Clang (My not-so-secret high perf. obsession!)
---
What optimizations are available?
- Clang supports the classic GCC-style flags:
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
- bitwise OR | stays as a CPU instruction
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
#### 4. What are LUTs (Look-Up Table)? Linked to (1)'s code optimisation.
---
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
char is_digit[256] = { /* I need to fill with 1 or 0s */ }
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
- Every lookup is constant time:
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
