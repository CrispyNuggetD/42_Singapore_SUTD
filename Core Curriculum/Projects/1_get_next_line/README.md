*This project has been created as part of the 42 curriculum by hnah.*

# Description

## get_next_line

**get_next_line** is a C function that reads from a **file descriptor (fd)** and returns **one line per call**.

- Repeated calls should return successive lines from the same fd, **until EOF**, then return `NULL` after the last line in that fd is returned. 
- The returned line **includes the `\n`** if one was present in the input (except possibly the last line if the file doesn’t end with `\n`). 
- Must work for both **reading from files** and **standard input**.

This repository contains:
- Mandatory implementation: `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h` 
- Bonus implementation (multi-fd): `get_next_line_bonus.c`, `get_next_line_utils_bonus.c`, `get_next_line_bonus.h` 

## Quick feature list

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

## Explanations of main parts of code

### 1. get_next_line.c

```c
char	*get_next_line(int fd)
{
	static char	*stash;
	char		*new_stash;	
	ssize_t		read_num;
	ssize_t		nl;

	if (fd < 0 || fd > 1024 || BUFFER_SIZE <= 0 || BUFFER_SIZE > SIZE_MAX - 1)
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
- ```fd > 1024``` matches a common fd limit assumption used in many GNL solutions.
- ```BUFFER_SIZE <= 0``` rejects nonsense buffer sizes.
- ```BUFFER_SIZE > SIZE_MAX - 1``` is a defensive overflow guard before BUFFER_SIZE + 1 allocations.

---

Main loop ```(while (1))```:

Step A: detect whether stash already contains a full line

nl = find_len(stash, '\n');

This returns the length up to and including \n (or 0 if stash is NULL/empty).

if (nl > 0 && stash[nl - 1] == '\n')

Confirms that the counted prefix ends in newline.

If yes → newline_ret(&stash) splits stash into:

return line

leftover stash

Step B: read more when we can’t return a line yet

Allocate new_stash of size BUFFER_SIZE + 1 for a null-terminated chunk.

read(fd, new_stash, BUFFER_SIZE) fills it.

If read_num < 0 → error:

read_error(&stash, new_stash) frees memory and returns NULL.

new_stash[read_num] = '\0'; ensures it’s a C-string.

Step C: append chunk to stash

stash = gnl_strjoin(stash, new_stash);

Returns a new combined buffer and frees both inputs.

If join failed → return NULL.

Step D: EOF handling

If read_num == 0, no more bytes:

fd_end_handler(&stash) decides:

if stash non-empty → return it (final line)

else free stash and return NULL

This loop structure matches the subject’s intended behavior: keep reading until you can return a line, without reading the entire file first.

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

I, (42 intra) hnah (Christopher Hui-Kang Nah) am contactable via:

1. Email: christopher_nah@yahoo.com.sg

2. LinkedIn: linkedin.com/in/crispynugget/

```
42BeyondTheCode
```

##### EOF :D
