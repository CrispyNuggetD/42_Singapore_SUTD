*This project has been created as part of the 42 curriculum by hnah.*

# pipex

## Description

Pipex recreates the shell pipeline below using Unix file descriptors and
processes:

```sh
< infile cmd1 | cmd2 > outfile
```

The mandatory program is invoked as:

```sh
./pipex infile "cmd1" "cmd2" outfile
```

## Build

```sh
make
```

## Implementation

The parent opens the input and output files, creates one pipe, and forks two
children. The first child redirects the input file to standard input and the
pipe's write end to standard output. The second redirects the pipe's read end
to standard input and the output file to standard output. Each child then
resolves its command through `PATH` and calls `execve`.

This initial repository structure and Makefile were generated with AI under
the author's direction, following the layout used by the author's FdF project.
AI also assisted with the first mandatory implementation and edge-case tests;
the author will review and understand each function before submission.
