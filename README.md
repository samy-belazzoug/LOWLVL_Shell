# LOWLVL_Shell

A complete shell implementation written in C with pipes, redirection, and job control.

## Quick Start

```bash
bash build.sh   # Build
./bashr         # Run
pwd             # Try: pwd
exit            # Quit
```

## Features

- ✅ Interactive REPL with prompt
- ✅ 7 built-in commands (cd, pwd, exit, echo, env, export, jobs)
- ✅ Pipes (|) and I/O redirection (>, >>, <)
- ✅ Background execution (&)
- ✅ Job control & signal handling
- ✅ 589 lines of clean C code

## Usage Examples

```bash
bashr:/$ pwd                   # Built-in
bashr:/$ ls | grep .md         # Pipes
bashr:/$ echo test > file.txt  # Redirection
bashr:/$ sleep 10 &            # Background
bashr:/$ jobs                  # List jobs
bashr:/$ exit                  # Quit
```

## Files

```
src/
├── bashr.c       - Main REPL loop
├── bashr.h       - Declarations
├── executor.c    - Execution engine
├── builtins.c    - Built-in commands
└── utils.c       - Parser & utilities
```

## Documentation

- **GUIDE.md** - Architecture, code walkthrough, how to extend
- **REFERENCE.md** - Visual diagrams, troubleshooting
- **PROJECT.md** - Summary, checklist, statistics

## Build

```bash
bash build.sh    # Build (Linux/WSL/Mac)
./bashr          # Run
```

## Learning

This teaches: fork, execve, pipes, signals, memory safety, modular design.

---

**[SCHOOL PROJECT]** - Educational shell implementation.
