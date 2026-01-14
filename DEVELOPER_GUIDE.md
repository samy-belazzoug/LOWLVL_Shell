# LOWLVL Shell - Complete Developer Guide

## Table of Contents

1. [Quick Reference](#quick-reference)
2. [Architecture Overview](#architecture-overview)
3. [Code Organization](#code-organization)
4. [API Reference](#api-reference)
5. [Data Structures](#data-structures)
6. [Control Flow](#control-flow)
7. [Adding Features](#adding-features)
8. [Debugging Tips](#debugging-tips)

## Quick Reference

### Build
```bash
./build.sh              # Compile
./build.sh clean        # Clean artifacts
./build.sh test         # Run tests
```

### File Map
| File | Purpose |
|------|---------|
| `bashr.c` | Main REPL loop, signal handlers |
| `bashr.h` | All declarations, data structures |
| `executor.c` | Command execution, pipes, redirects |
| `builtins.c` | Built-in commands (cd, pwd, echo, etc) |
| `utils.c` | Parser, tokenizer, job control |

## Architecture Overview

```
┌─────────────────────────────────────────────┐
│            Main REPL Loop (bashr.c)         │
│  - Read input (fgets)                       │
│  - Route to handler based on special chars  │
│  - Signal handling (SIGINT, SIGCHLD)        │
└──────────┬──────────────────────────────────┘
           │
      ┌────┴─────────────────────────┐
      │                              │
      v                              v
  Parser (utils.c)          Executor (executor.c)
  - tokenize()              - execute_command()
  - parse_command()         - execute_pipe()
  - trim_whitespace()       - execute_redirect()
      │                              │
      └────────────┬─────────────────┘
                   │
            ┌──────v──────┐
            │  Built-ins  │
            │ (builtins.c)│
            └─────────────┘
```

## Code Organization

### Initialization Flow

```
main()
  ├─ signal(SIGINT, sigint_handler)
  ├─ signal(SIGCHLD, sigchld_handler)
  └─ Read-loop
      ├─ print_prompt()
      ├─ read_line()
      ├─ Route input
      │  ├─ If contains '|' → execute_pipe()
      │  ├─ If contains '>', '<' → execute_redirect()
      │  └─ Else → execute_command()
      └─ free_command()
```

### Execution Flow for External Command

```
execute_command(cmd)
  ├─ Check if builtin → call builtin
  ├─ Check for '&' flag
  ├─ find_in_path() → locate binary
  ├─ fork()
  │  └─ Child: execve() → replace process
  └─ Parent: 
      ├─ If background: add_job()
      └─ Else: waitpid()
```

### Pipe Execution Flow

```
execute_pipe(input)
  ├─ tokenize(input, "|")
  ├─ Create pipes[] for inter-process communication
  ├─ For each command:
  │  ├─ fork()
  │  └─ Child:
  │      ├─ dup2() stdin if not first
  │      ├─ dup2() stdout if not last
  │      ├─ execve()
  └─ Parent: close all pipes, wait for all children
```

## API Reference

### Parser (utils.c)

```c
// Tokenize string by delimiters
char** tokenize(char *input, const char *delim);

// Parse command line into argc/argv
Command* parse_command(char *input);

// Free allocated tokens
void free_tokens(char **tokens);

// Free command structure
void free_command(Command *cmd);

// Remove leading/trailing whitespace
void trim_whitespace(char *str);

// Check if string contains character
int contains_char(const char *str, char c);
```

### Executor (executor.c)

```c
// Execute a single command
int execute_command(Command *cmd);

// Execute piped commands
int execute_pipe(char *input);

// Execute with I/O redirection
int execute_redirect(Command *cmd);
```

### Built-ins (builtins.c)

```c
// Check if command is built-in
int is_builtin(const char *cmd);

// Change directory
int builtin_cd(char **args);

// Print working directory
int builtin_pwd(char **args);

// Exit shell
int builtin_exit(char **args);

// Echo arguments
int builtin_echo(char **args);

// List environment
int builtin_env(char **args);

// Set environment variable
int builtin_export(char **args);

// List background jobs
int builtin_jobs(void);
```

### Job Control (utils.c)

```c
// Add background job
void add_job(pid_t pid, const char *cmd);

// Remove completed job
void remove_job(pid_t pid);

// Print all jobs
void print_jobs(void);

// Reap zombie processes
void reap_zombies(void);
```

### Utilities (utils.c)

```c
// Find command in PATH
char* find_in_path(const char *cmd);
```

## Data Structures

### Command
```c
typedef struct {
    int argc;                    // Argument count
    char *argv[MAX_ARGS];        // Arguments (NULL-terminated)
} Command;
```

### Job
```c
typedef struct {
    int id;                      // Job ID
    pid_t pid;                   // Process ID
    char cmd[MAX_CMD_LEN];       // Command name
    int status;                  // Job status
} Job;
```

## Control Flow

### Command Execution Decision Tree

```
Input: "ls -la | grep test"
  │
  ├─ Contains '|'? YES
  │   └─ execute_pipe()
  │
  ├─ Contains '>' or '<'? NO
  │
  └─ Is builtin? NO
      └─ execute_command()
```

### Pipe Mechanics

```
For "cmd1 | cmd2 | cmd3":

cmd1  ┌─────┐      cmd2  ┌─────┐      cmd3
      │PIPE1│           │PIPE2│
stdin ├─────┤ stdout    ├─────┤ stdout
      │     │ (to cmd2) │     │ (to cmd3)
      └─────┘           └─────┘
```

Implemented with:
1. `pipe()` - Create pipe file descriptors
2. `fork()` - Create process for each command
3. `dup2()` - Redirect stdin/stdout through pipes

## Adding Features

### Feature: Add "history" built-in

**Step 1: Define in bashr.h**
```c
int builtin_history(char **args);
```

**Step 2: Implement in builtins.c**
```c
int builtin_history(char **args) {
    (void)args;
    // Print command history from file
    char history_file[256];
    snprintf(history_file, sizeof(history_file), 
             "%s/.bashr_history", getenv("HOME"));
    
    FILE *f = fopen(history_file, "r");
    if (f) {
        char line[1024];
        int num = 1;
        while (fgets(line, sizeof(line), f)) {
            printf("%d  %s", num++, line);
        }
        fclose(f);
    }
    return 0;
}
```

**Step 3: Register in executor.c**
```c
if (strcmp(cmd->argv[0], "history") == 0) 
    return builtin_history(cmd->argv);
```

**Step 4: Add to is_builtin() check**
```c
"history"  // Add to builtins array
```

**Step 5: Test**
```bash
echo "history" | ./bashr
```

## Debugging Tips

### Print Debug Information

```c
// Add to code
fprintf(stderr, "DEBUG: cmd='%s', argc=%d\n", 
        cmd->argv[0], cmd->argc);

// Compile with debug symbols
gcc -g -O0 ...
```

### Trace System Calls

```bash
# See all execve calls
strace -e trace=execve ./bashr <<< "ls"

# See all process creation
strace -e trace=process ./bashr <<< "ls"

# Follow child processes
strace -f ./bashr <<< "ls | wc -l"
```

### Memory Debugging

```bash
# Check for leaks
valgrind --leak-check=full ./bashr <<< "ls"

# Check for invalid access
valgrind --track-origins=yes ./bashr <<< "ls"
```

### GDB Debugging

```bash
gdb ./bashr
(gdb) break execute_command
(gdb) run
bashr> ls
(gdb) next
(gdb) print cmd->argv[0]
(gdb) quit
```

### Performance Profiling

```bash
# Count system calls
strace -c ./bashr <<< "ls"

# Time execution
time ./bashr <<< "ls"

# Profile CPU
perf record ./bashr <<< "ls"
perf report
```

## Common Patterns

### Safe String Operations

```c
// GOOD - Length limited
char buf[256];
strncpy(buf, src, sizeof(buf) - 1);
buf[sizeof(buf) - 1] = '\0';

// GOOD - Formatted
snprintf(buf, sizeof(buf), "cmd %s", arg);

// BAD - Unbounded
strcpy(buf, src);           // Stack overflow!
sprintf(buf, "cmd %s", arg);  // Buffer overrun!
```

### Safe Memory Allocation

```c
// GOOD - Check allocation
char *ptr = malloc(size);
if (!ptr) {
    perror("malloc");
    return -1;
}

// GOOD - Free in all paths
if (error) {
    free(ptr);
    return -1;
}
free(ptr);

// BAD - No error check
char *ptr = malloc(size);  // What if NULL?
strcpy(ptr, "...");        // Crash!
```

### Proper Signal Handling

```c
// GOOD - Only call async-signal-safe functions
void handler(int sig) {
    (void)sig;
    write(STDOUT_FILENO, "\n", 1);  // Only safe calls
}

// BAD - Unsafe in signal handler
void handler(int sig) {
    printf("Interrupt\n");           // NOT safe!
    malloc(size);                    // NOT safe!
}
```

## Performance Tips

1. **Minimize fork() calls** - Create processes only when needed
2. **Cache PATH lookups** - Don't search PATH for every command
3. **Use stack allocation** - Faster than heap for fixed sizes
4. **Batch system calls** - Group close() operations
5. **String optimization** - Avoid unnecessary copies

## Further Reading

- `man 2 execve` - Execute file
- `man 2 fork` - Create process
- `man 2 pipe` - Interprocess communication
- `man 2 dup2` - Duplicate file descriptors
- `man 2 wait` - Wait for child process
- `man 2 signal` - Send signal to process
- C POSIX standard - Complete specification

---

**Last Updated:** 2024
**Version:** 1.0
