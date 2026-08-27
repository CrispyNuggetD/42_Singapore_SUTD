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

This initial repository structure and Makefile were generated with AI under
the author's direction, following the layout used by the author's FdF project.
The Pipex implementation will be developed and reviewed incrementally.
