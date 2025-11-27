# My 42 course notes (Largely ChatGPT)
## Contents
1. [Bitwise stacking using isdigit returns (Proper)](#1-bitwise-stacking-using-isdigit-returns-proper)
2. [Compiler optimisations for Clang (My not-so-secret high perf. obsession!)](#2-compiler-optimisations-for-clang-my-not-so-secret-high-perf-obsession)

---
#### 1. Bitwise stacking using isdigit returns (Proper)
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

---
#### 2. Compiler optimisations for Clang (My not-so-secret high perf. obsession!)
What optimizations are available?
- Clang supports the classic GCC-style flags:
##### 1. O0, No optimization (default)
##### 2. O1, Simple optimizations
- constant folding
- remove obvious dead code
- minimal inlining
##### 3. O2, Standard “safe but fast”
- larger inlining
- loop optimizations
- vectorization
- strength reduction
- common subexpression elimination
- branch prediction hints

This is the commonly recommended setting for production.

##### 4. O3, Maximum speed, aggressive
- unrolling
- more aggressive vectorization
- may increase code size
- may occasionally cause pathological slowdowns

Good for performance tests.

##### 5. Os, Optimize for small code size
Good for embedded systems.

##### 6. Ofast
- Breaks strict IEEE math rules for speed
- Unsafe unless you really know what you're doing.

Would NOT pass 42 tests where correctness matters.

