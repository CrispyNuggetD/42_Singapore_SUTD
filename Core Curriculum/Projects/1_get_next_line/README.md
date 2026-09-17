*This project has been created as part of the 42 curriculum by hnah.*

> Post-submission update (2026-09-18): corrected descriptor validation in mandatory and bonus GNL after reviewing reuse in Pipex. See [Post-submission update: file descriptor limits](#post-submission-update-file-descriptor-limits) for the bug, research, and remaining integration work.

# Description

## get_next_line

**get_next_line** is a C function that reads from a **file descriptor (fd)** and returns **one line per call**.

- Repeated calls should return successive lines from the same fd, **until EOF**, then return `NULL` after the last line in that fd is returned. 
- The returned line **includes the `\n`** if one was present in the input (except possibly the last line if the file doesn’t end with `\n`). 
- Must work for both **reading from files** and **standard input**.

This repository contains:
- Mandatory implementation: `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h` 
- Bonus implementation (multi-fd): `get_next_line_bonus.c`, `get_next_line_utils_bonus.c`, `get_next_line_bonus.h` 

# Quick feature list

### Mandatory
- `char *get_next_line(int fd);`
- Returns:
  - A heap-allocated string containing the next line (including `\n` if present)
  - Or `NULL` on EOF (nothing left) or on error 
- Uses only:
  - `read`, `malloc`, `free`.
- For correctness, in this project there are no forbidden functions like `lseek()`, no global variables, no libft usage. 

### Bonus
- Manages multiple file descriptors “at the same time”:
  - You can run this program and alternate calls between fd 3, 4, 5, etc. and each keeps its own reading state 
- Still uses only **one static variable** (implemented as one static array indexed by fd). 

## Algorithm explanation & justification 

This implementation uses the standard (and intended) approach:

1. Maintain a static “stash” that persists between calls.
2. Append newly read bytes into the stash until:
3. newline \n exists in stash → split and return one line
4. Or read() returns 0 (EOF) → return the remaining stash as the last line (if non-empty).
5. Keep the leftover bytes (after the returned line) inside stash for the next call.

This design is justified because:

- It respects the requirement “read as little as possible”:

1. We only read more when we can’t yet produce a full line. 
2. It correctly returns a line including ```\n``` when present. 
3. The static stash is exactly the concept the project is meant to teach.

# Explanations of main parts of code

## Mandatory 

## 1. get_next_line.h

```c
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 67
# endif
```

- BUFFER_SIZE defaults to 67 here, but is overridable by the compiler flag -D BUFFER_SIZE=n

## 2. get_next_line.c

```c
char	*get_next_line(int fd)
{
	static char	*stash;
	char		*new_stash;	
	ssize_t		read_num;
	ssize_t		nl;

	if (fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE > SIZE_MAX - 1)
		return (NULL);
	while (1)
	{
		nl = find_len(stash, '\n');
		if (nl > 0 && stash[nl - 1] == '\n')
			return (newline_ret(&stash));
		new_stash = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!new_stash)
			return (NULL);
		read_num = read(fd, new_stash, BUFFER_SIZE);
		if (read_num < 0)
			return (read_error(&stash, new_stash));
		new_stash[read_num] = '\0';
		stash = gnl_strjoin(stash, new_stash);
		if (!stash)
			return (NULL);
		if (read_num == 0)
			return (fd_end_handler(&stash));
	}
}
```

### What each part is doing:

```static char *stash;```

- Persists between calls.
- Stores “unreturned leftover bytes” from previous reads.

Input validation:
- ```fd < 0``` rejects invalid fd.
- No fixed upper FD limit is needed for the mandatory single stash. When reading is needed, `read()` reports invalid descriptors; 1024 is not a universal OS limit.
- ```BUFFER_SIZE <= 0``` rejects nonsense buffer sizes.
- ```BUFFER_SIZE > SIZE_MAX - 1``` is a defensive overflow guard before BUFFER_SIZE + 1 allocations.

---

Main loop ```(while (1))```:

### Step A: detect whether stash already contains a full line

```nl = find_len(stash, '\n');```

This returns the length up to and including \n (or 0 if stash is NULL/empty).

```if (nl > 0 && stash[nl - 1] == '\n')```

Confirms that the counted prefix ends in newline.

If yes → ```newline_ret(&stash)``` splits stash into:

1. return line

2. leftover stash

### Step B: read more when we can’t return a line yet

1. Allocate ```new_stash``` of size ```BUFFER_SIZE + 1``` for a null-terminated chunk.

2. ```read(fd, new_stash, BUFFER_SIZE)``` fills it.

3. If ```read_num < 0``` → error:

	- ```read_error(&stash, new_stash)``` frees memory and returns NULL.

4. ```new_stash[read_num] = '\0';``` ensures it’s a C-string.

### Step C: append chunk to stash

```stash = gnl_strjoin(stash, new_stash);```

Returns a new combined buffer and frees both inputs.

- If join failed → return ```NULL```.

### Step D: EOF handling

If ```read_num == 0```, no more bytes:

- fd_end_handler(&stash) decides:

if ```stash``` non-empty → return it (final ```line```)

- else ```free``` stash and return ```NULL```

This loop structure matches the subject’s intended behavior: keep reading until you can return a line, without reading the entire file first.

## 3. get_next_line_utils.c (helpers)

### Helper Function 1 of 5: find_len

#### Purpose:

A small utility that behaves like:

- “count characters until ```look_for``` or ```\0```”
- includes ```look_for``` in the count if it’s found

#### Code:

```c
ssize_t	find_len(const char *s, int look_for)
{
	ssize_t	len;

	len = 0;
	if (!s)
		return (0);
	while (*s && *s != look_for)
	{
		len++;
		s++;
	}
	if (*s && *s == look_for)
		len++;
	return (len);
}
```

#### Key details:

- If ```s == NULL``` → ```returns 0``` (this is “implicit guard” for stash being ```NULL```).

- If ```look_for``` is found (e.g. ```\n```) → length **includes** it.

That’s why ```nl > 0 && stash[nl - 1]``` == '\n' works cleanly.

---

### Helper Function 2 of 5: newline_ret

#### Purpose:

Split stash into:

```gnl_buf``` = “line to return” (prefix ending at ```\n``` if present)

```temp_buf``` = “new stash” (remaining suffix after that prefix)

#### Code:

```c
char	*newline_ret(char **buf)
{
	char	*temp_buf;
	char	*gnl_buf;
	ssize_t	prefix;
	ssize_t	suffix;

	prefix = find_len(*buf, '\n');
	suffix = find_len(*buf, '\0') - prefix;
	temp_buf = malloc(sizeof(char) * (suffix + 1));
	if (!temp_buf)
		return (NULL);
	temp_buf[suffix] = '\0';
	while (suffix--)
		temp_buf[suffix] = (*buf)[prefix + suffix];
	gnl_buf = malloc(sizeof(char) * (prefix + 1));
	if (!gnl_buf)
	{
		free(temp_buf);
		return (NULL);
	}
	gnl_buf[prefix] = '\0';
	while (prefix--)
		gnl_buf[prefix] = (*buf)[prefix];
	free(*buf);
	*buf = temp_buf;
	return (gnl_buf);
}
```

#### Line-by-line logic:

1. ```prefix = find_len(*buf, '\n');```

	- If newline exists, prefix **includes** it.

	- If newline doesn’t exist, prefix **becomes full string length** (because it stops at ```\0``` and doesn’t add).

2. ```suffix = find_len(*buf, '\0') - prefix;```

**Remaining** chars **after** the prefix.

3. Allocate ```temp_buf``` sized ```suffix + 1```.

4. Copy the suffix segment from original ```buffer``` into ```temp_buf```.

5. Allocate ```gnl_buf``` sized ```prefix + 1```.

6. Copy the prefix segment from original ```buffer``` into ```gnl_buf```.

7. ```Free``` old stash ```(free(*buf))``` and **replace** it with the **new** leftover ```(*buf = temp_buf)```.

8. Return ```gnl_buf``` to caller; caller owns it and must ```free()``` it.

This is the core “split” mechanic that makes the ```static stash``` workable.

---

### Helper Function 3 of 5: read_error

#### Purpose:

Centralized cleanup for ```read()``` failure:

1. ```Free``` the newly allocated read buffer

2. ```Free``` ```stash``` and reset it to ```NULL```

3. Return ```NULL```

#### Code:

```c
char	*read_error(char **stash, char *new_stash)
{
	free(new_stash);
	free(*stash);
	*stash = NULL;
	return (NULL);
}
```

---

### Helper Function 4 of 5: gnl_strjoin

#### Purpose:

1. Append ```new_buf``` onto ````buf````, returning a newly allocated buffer:

2. Handles ```buf == NULL``` via ```find_len(NULL, '\0') == 0```

3. Always frees both inputs on success.

4. Also frees **both** on allocation failure (so caller doesn’t leak).

#### Code:

```c
char	*gnl_strjoin(char *buf, char *new_buf)
{
	char	*temp_buf;
	ssize_t	new_buf_len;
	ssize_t	buf_len;

	new_buf_len = find_len(new_buf, '\0');
	buf_len = find_len(buf, '\0');
	temp_buf = malloc(sizeof(char) * (new_buf_len + buf_len + 1));
	if (!temp_buf)
	{
		free(buf);
		free(new_buf);
		return (NULL);
	}
	temp_buf[new_buf_len + buf_len] = '\0';
	while (new_buf_len--)
		temp_buf[buf_len + new_buf_len] = new_buf[new_buf_len];
	while (buf_len--)
		temp_buf[buf_len] = buf[buf_len];
	free(buf);
	free(new_buf);
	return (temp_buf);
}
```

#### How it copies:

- It copies from the end backwards using ```while (len--)```.

	- That avoids needing an extra index variable.

1. First copies new_buf to the end region.

2. Then copies buf into the front region.

3. **Returns** the combined string.

---

### Helper Function 5 of 5: fd_end_handler

#### Purpose:

Handle EOF ```(read_num == 0)``` consistently:

- If ```stash``` contains something → return it as final ```line```

- Else clean up and return ```NULL```

#### Code:

```c
char	*fd_end_handler(char **stash)
{
	if (*stash && (*stash)[0] != '\0')
		return (newline_ret(stash));
	free(*stash);
	*stash = NULL;
	return (NULL);
}
```

#### Note:

- If ```stash``` is non-empty but has no newline, ```newline_ret()``` still returns the whole buffer as a line.

	- (because ```find_len(*buf, '\n')``` becomes full length).

- If stash is empty or ```NULL``` → it frees and returns ```NULL```.

# Bonus version documentation (multi-fd)

## File: get_next_line_bonus.c

### Function: ```get_next_line``` (bonus)

#### Code:

```c
char	*get_next_line(int fd)
{
	static char	*stash[1024];
	char		*new_stash;	
	ssize_t		read_num;
	ssize_t		nl;

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0 || BUFFER_SIZE > SIZE_MAX - 1)
		return (NULL);
	while (1)
	{
		nl = find_len(stash[fd], '\n');
		if (nl > 0 && stash[fd][nl - 1] == '\n')
			return (newline_ret(&stash[fd]));
		new_stash = malloc(sizeof(char) * (BUFFER_SIZE + 1));
		if (!new_stash)
			return (NULL);
		read_num = read(fd, new_stash, BUFFER_SIZE);
		if (read_num < 0)
			return (read_error(&stash[fd], new_stash));
		new_stash[read_num] = '\0';
		stash[fd] = gnl_strjoin(stash[fd], new_stash);
		if (!stash[fd])
			return (NULL);
		if (read_num == 0)
			return (fd_end_handler(&stash[fd]));
	}
}
```

## What changed vs mandatory:

Instead of ```static char *stash;```, the bonus uses:

```static char *stash[1024];```

Each **fd gets its own independent stash**:

```stash[3]```, ```stash[4]```, … each **persist** across calls

This satisfies the requirement to interleave reads across fds without mixing state. 

## File: get_next_line_utils_bonus.c

- Same helper logic as the mandatory utilities (split/join/find_len/error handling), reused for the bonus build.

# Instructions

1. Open ```terminal```, ```cd``` to desired storage directory, ```git clone``` my repository, ```cd``` into the get_next_line directory.

## Provided main.c (development tester)

#### Note: This main.c is **NOT** meant to be submitted in the final repo.

- It is included here as a convenient local tester and to document how I validated behavior.

- It is also not leak-perfect - Just good enough for a quick sanity check.

```c
#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h> 

static int gnl_atoi(char *v)
{
	int i = 0;
	int a = 0;
	while (v[i])
	{
		a *= 10;
		a += v[i] - '0';
		i ++;
	}
	if (!a)
		write(1, "invalid integer\n", 4);
	return (a);
}

int main(int c, char **v)
{
	int	i = 0;
	if (c < 3)
		return (1);
	int fd = open(v[1], O_RDONLY); //change to '0' for stdin
	if (fd < 0)
		return ((write(1, "u no the allow open\n", 8)), 1);
	int line = gnl_atoi(v[2]);
	char *the_line = NULL;
	while (line)
	{
		the_line = get_next_line(fd);
		if (the_line == NULL)
			return ((write(1, "finish le\n", 4)), (close(fd)), 1);
		while (the_line[i])
			i++;
		write(1, the_line, i);
		line--;
	}	
	free(the_line);
	close(fd);
}
```

## Terminal commands to compile and run program

1. Compile the needed .c file using:

	1. Mandatory segment: ```cc -Wall -Wextra -Werror -D BUFFER_SIZE=<INSERT_NUMBER> get_next_line.c get_next_line_utils.c get_next_line.h main.c```
	
	2. Bonus segment: ```cc -Wall -Wextra -Werror -D BUFFER_SIZE=<INSERT_NUMBER> get_next_line_bonus.c get_next_line_utils_bonus.c get_next_line_bonus.h main.c```

2. Run program with ```./a.out``` in terminal.

## Extra notes

- Project can compile with or without the -D BUFFER_SIZE flag (so BUFFER_SIZE has a default in the header).

## Post-submission update: file descriptor limits

### Why revisit this after submission?

While reviewing whether Pipex could reuse my original GNL, I noticed that both
versions rejected `fd > 1024`. My original assumption was that file descriptors
could only go up to 1024. Research showed that this mixed up an operating-system
resource limit, a `select()` implementation limit, and my own array capacity.
Passing online testers does not establish that every boundary was exercised.
We have not inspected those testers, so their exact coverage is unknown.

### What the research established

- File descriptors are nonnegative integer handles, not necessarily numbers
  below 1024. A process can have valid descriptors equal to or greater than
  1024 when its resource limits allow them.
- `RLIMIT_NOFILE` controls descriptor allocation. A limit of 1024 allows newly
  allocated descriptor numbers through **1023**, because numbering starts at
  zero. The soft limit can vary by environment and can be raised within the
  permitted hard limit. `ulimit -Sn` and `ulimit -Hn` show the shell's soft and
  hard limits; they are not constants built into C or GNL.
- Linux/glibc `select()` uses `FD_SETSIZE == 1024`, restricting the descriptor
  numbers that its sets can represent. That is a restriction of that interface,
  not a universal restriction on `read()`. GNL does not use `select()`.
- An array declared as `stash[1024]` has indexes **0 through 1023**, independently
  of OS limits. It stores pointers to buffered strings indexed by FD; it does
  not store or allocate the descriptors themselves.

References consulted:

- [POSIX getrlimit specification](https://pubs.opengroup.org/onlinepubs/7908799/xsh/getrlimit.html):
  explains `RLIMIT_NOFILE` and the maximum newly allocated descriptor value.
- [Linux getrlimit manual](https://man7.org/linux/man-pages/man2/getrlimit.2.html):
  explains soft/hard limits and failures when descriptor allocation exceeds them.
- [Linux select manual](https://man7.org/linux/man-pages/man2/select.2.html):
  explains the glibc 1024-descriptor-set limitation and alternatives.

### What changed in the code?

| Version | Previous guard | Updated guard | Reason |
|---|---|---|---|
| Mandatory, one `static char *stash` | `fd < 0 || fd > 1024` | `fd < 0` | FD is not an array index; valid high descriptors should not be rejected arbitrarily. |
| Bonus, `static char *stash[1024]` | `fd < 0 || fd > 1024` | `fd < 0 || fd >= 1024` | Reject index 1024 before accessing the fixed array. |

The `BUFFER_SIZE` checks remain. The bonus fix deliberately retains its existing
fixed capacity: it safely rejects high descriptors even when the OS considers
them valid. Supporting those descriptors in bonus would require a different or
larger storage design; merely removing its guard would cause out-of-bounds access.

At exactly `fd == 1024`, the old bonus guard allowed `stash[1024]` to be read
before any `read()` call could reject the descriptor. This is undefined behavior,
even if descriptor 1024 is not open. Tests of `-1` or a much larger number would
not catch it because those inputs were already rejected. A crash is not guaranteed;
a sanitizer can detect the invalid access more reliably than output-only tests.

### Validation approach

Compile mandatory and bonus separately, because they export the same function
names. Exercise negative descriptors, normal lines, empty input, final lines
without a newline, repeated EOF, and alternating descriptors for bonus. Test bonus
with exactly 1024 and above under AddressSanitizer/UndefinedBehaviorSanitizer.
For mandatory, use a test harness with `fcntl(F_DUPFD, ...)` to obtain actual open
descriptors at 1024 and above, subject to the process limit, then verify line output.
The harness may use system APIs outside the GNL subject's allowed functions;
those APIs are not added to the submitted implementation.

### Local results (2026-09-18)

The focused harness passed for mandatory and bonus with `-Wall -Wextra -Werror`
and UndefinedBehaviorSanitizer. It covered normal/empty input, a final line
without newline, repeated EOF, negative FD rejection, bonus alternating FDs,
bonus reading through FD 1023, bonus rejection at 1024/1025, and mandatory reading
through real open descriptors 1024/1025. Both changed C files passed Norminette.
The initial AddressSanitizer run aborted during macOS runtime initialization
with Apple Clang 16.0.0 on macOS 26.5. A separate minimal allocation program
reproduced the same failure, including with a clean environment, isolating it
from GNL. Retrying with Homebrew LLVM 23.1.1 resolved the startup failure: a
valid control passed, an intentional heap overflow was detected, and both GNL
harnesses passed with AddressSanitizer and UndefinedBehaviorSanitizer together.
This is consistent with the older Apple sanitizer/runtime compatibility issue
[documented by the c2pa-cpp project](https://github.com/contentauth/c2pa-cpp#sanitizer-test-builds-fail-on-macos).

The new compiler initially expected a missing macOS 26 SDK. Explicitly selecting
the installed SDK and a macOS 15 deployment target made the test builds work:

```sh
/opt/homebrew/opt/llvm/bin/clang \
    -isysroot "$(xcrun --show-sdk-path)" -mmacosx-version-min=15.0 \
    -Wall -Wextra -Werror -g -fsanitize=address,undefined \
    test_main.c get_next_line.c get_next_line_utils.c -o gnl_asan
./gnl_asan
```

Here `test_main.c` means the local test harness, not a submitted GNL source.
For bonus, compile its `_bonus.c` files instead. LLVM was installed alongside
the Apple compiler; no default compiler, shell PATH, or project Makefile was
changed. No reboot was needed. These focused checks do not constitute a complete
allocation-failure/leak audit.

### What this does not resolve for Pipex

Normal GNL is sufficient for reading heredoc stdin; multiple-FD support is not
needed for that use. However, the existing API returns `NULL` for both EOF and
errors, and stopping at a limiter can leave the static stash allocated. Some
allocation-failure paths also retain it. Error reporting and explicit early
cleanup still need a separate integration decision. This correction does not
replace Pipex's `read_line_bonus()` or claim to resolve those issues.

The original GNL project is the source of this update. Its two entry-point source
files and this README are copied into the main Ryker libft and the embedded
libraries in Pipex, FdF, and push_swap. Their helper implementations and APIs are
unchanged. AI assisted with the review, documentation, propagation, and validation.

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

### ChatGPT for coding:

#### Summary of A.I. usage

- Lots of A.I. used, but systematic and no vibe-coding.

- Explain till I understand, and (I try my best to) internalise.

- Asks and learns "more than required" with A.I.

- Help with README formatting and better layout and paraphrasing of concepts.

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

1. (Sometimes) Clarifying subject.pdf interpretation (I'm not too good at understanding extent required of questions yet, oops)

2. Of course, S.O.S. when I'm stuck/ no idea how to start, asking: "Do NOT give me any code (or solution when debugging), guide me through".

3. Difference between making function prototype with ```*stash``` and ```**stash```

---

## Contact Details

##### For further queries/ help:

I am Christopher Hui-Kang Nah and my 42 intra is ```hnah```.

I am contactable via:

1. Email: christopher_nah@yahoo.com.sg

2. LinkedIn: linkedin.com/in/crispynugget/

```
42BeyondTheCode
```

##### EOF :D
