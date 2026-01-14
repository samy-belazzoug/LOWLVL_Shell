# LOWLVL_Shell

A lightweight, feature-rich shell implementation written in C from scratch.

## Quick Start

```bash
make              # Build the shell
./bashr           # Run it
```

## Features ✨

- **Interactive REPL** - Full read-eval-print loop with custom prompt
- **Built-in Commands** - cd, pwd, exit, echo, env, export, jobs
- **Pipes** - Chain commands with `|`
- **Redirection** - Input `<`, output `>`, append `>>`
- **Background Execution** - Run commands with `&`
- **Job Control** - Track background processes
- **Signal Handling** - Ctrl+C and zombie process cleanup
- **PATH Resolution** - Automatically find commands in $PATH
- **Error Handling** - Proper error messages and recovery

## Documentation

- **[FEATURES.md](FEATURES.md)** - Usage guide with examples
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Code design and structure
- **[RESEARCH/](Research/)** - Low-level Unix concepts reference

## Project Structure

```
src/
├── bashr.c       - Main shell loop
├── bashr.h       - Declarations & data structures
├── executor.c    - Command execution, pipes, redirects
├── builtins.c    - Built-in commands
└── utils.c       - Parser, tokenizer, utilities
```

## Build

```bash
make           # Compile
make clean     # Remove artifacts
make test      # Run tests
make help      # Show targets
```

## Usage Examples

```bash
bashr:/$ ls | grep .c          # Pipes
bashr:/$ echo "test" > out.txt # Redirection
bashr:/$ cd /tmp               # Built-ins
bashr:/$ sleep 100 &           # Background jobs
bashr:/$ jobs                  # List jobs
bashr:/$ exit                  # Quit
```

## Learning Resources

This project demonstrates:
- Process creation (`fork`, `execve`)
- Inter-process communication (pipes)
- File descriptor manipulation (`dup2`)
- Signal handling (`signal`)
- Dynamic memory management
- Systems programming in C

## Limitations & Future Work

Current limitations:
- No variable expansion (`$VAR`)
- No filename globbing (`*.txt`)
- No command history
- No error stream redirection (`2>`)

See [ARCHITECTURE.md](ARCHITECTURE.md) for enhancement ideas.

---

**[SCHOOL PROJECT]** - Educational shell implementation for learning low-level Unix concepts.
