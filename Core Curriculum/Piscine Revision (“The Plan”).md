# (ChatGPT Assisted Revision)

##  The Roadmap Forward (where we’re heading)
- You requested that we go chronologically up the Piscine topics → toward libft.
- The order we will follow over multiple sessions:

#### Phase 1 — Shell & C Basics
* Shell commands
* vim
* gcc
* write()
* main()
* operations

#### Phase 2 — Essential C Concepts
* variables, types
* pointers
* arrays
* pointer arithmetic
* functions
* string manipulation
* ASCII mechanics
* splitting memory vs stack vs heap
* address-of and dereference
* the const keyword
* static vs automatic lifetime

#### Phase 3 — malloc
* when to malloc
* why malloc returns void* 
* double malloc (e.g. 2D arrays or char * * )
* free
* memory leaks
* segmentation faults
* common patterns used in libft (ft_split, ft_strtrim)

#### Phase 4 — Headers
* what is a .h
* include guards
* how to structure libft.h
* recompile dependencies
 
#### #### Phase 4.5 (Optional) — Some compiling assembly-level detail
*  Build order
*  object files
*  linking explained in assembly-level detail 

#### Phase 5 — Makefiles
* why Makefiles exist
* what “targets” are
* CC, CFLAGS, RM variables
* clean, fclean, re rules

#### Phase 6 — libft
* reimplementing libc
* ft_strlen
* ft_strlcpy / strlcat
* ft_atoi
* linked lists
* memory functions (ft_memcpy, ft_memmove, etc.)
* bonus files

#### Phase 7 — Everything after libft
* ft_printf
* get_next_line
* push_swap
* how to think algorithmically (to avoid brute force)