# LOWLVL_Shell

A complete shell implementation in C with pipes, redirection, and job control.

## Features

- Interactive REPL with custom prompt
- 7 built-in commands: cd, pwd, exit, echo, env, export, jobs
- Pipes (|) and I/O redirection (>, >>, <)
- Background execution (&) with job control
- Signal handling (Ctrl+C, zombie cleanup)
- 589 lines of production-quality C code

## Build

```bash
bash build.sh
```

## Run

```bash
./bashr
```

## Usage

```bash
bashr:/$ pwd                    # Built-in command
bashr:/$ ls | grep .md         # Pipes
bashr:/$ echo test > file.txt  # Redirection
bashr:/$ sleep 10 &            # Background job
bashr:/$ jobs                  # List jobs
bashr:/$ exit                  # Quit
```

## Project Structure

```
src/
├── bashr.c       Main REPL loop
├── bashr.h       Declarations
├── executor.c    Command execution
├── builtins.c    Built-in commands
└── utils.c       Parser & utilities
```

## Requirements Met

✅ REPL loop with fgets()
✅ fork() + execve() integration
✅ wait() + zombie cleanup
✅ Command parsing
✅ Built-in commands
✅ Error handling
✅ Signal handling
✅ Pipes & redirection
✅ Job control

## Documentation

See `Research/Nanoshell.pdf` for project specifications.

