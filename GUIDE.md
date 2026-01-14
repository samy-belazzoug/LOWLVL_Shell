# LOWLVL Shell - Complete Guide

## Architecture

### Module Overview

```
bashr.c (Main)
  ├─ Parser (utils.c)
  │  ├─ tokenize() - split by delimiters
  │  ├─ parse_command() - create argv
  │  └─ find_in_path() - locate executables
  │
  ├─ Executor (executor.c)
  │  ├─ execute_command() - run single command
  │  ├─ execute_pipe() - chain commands
  │  └─ execute_redirect() - I/O redirection
  │
  └─ Built-ins (builtins.c)
     ├─ builtin_cd() - change directory
     ├─ builtin_pwd() - working directory
     ├─ builtin_echo() - print text
     ├─ builtin_exit() - quit shell
     ├─ builtin_env() - list variables
     ├─ builtin_export() - set variables
     └─ builtin_jobs() - list background jobs
```

### Execution Flow

```
Read Input
  ↓
Parse Command
  ├─ Detect '|' → execute_pipe()
  ├─ Detect '>' or '<' → execute_redirect()
  └─ Otherwise → execute_command()
  ↓
Fork Child Process
  ↓
Child: execve() to replace process
Parent: wait() or add_job()
  ↓
Back to Prompt
```

### Data Structures

```c
typedef struct {
    int argc;
    char *argv[MAX_ARGS];
} Command;

typedef struct {
    int id;
    pid_t pid;
    char cmd[MAX_CMD_LEN];
    int status;
} Job;
```

## Code Walkthrough

### Parser (utils.c)

**`tokenize(input, delim)`**
- Splits input by delimiter
- Returns NULL-terminated array
- Safe: limits tokens to MAX_ARGS

**`parse_command(input)`**
- Calls tokenize() with spaces
- Builds Command struct with argc/argv
- Trims whitespace automatically

**`find_in_path(cmd)`**
- Checks if cmd has '/'
- If yes: verify executable
- If no: search PATH directories
- Returns full path or NULL

### Executor (executor.c)

**`execute_command(cmd)`**
1. Check if built-in command
2. Look for '&' for background
3. Search PATH for executable
4. Fork child process
5. Child: execve() replaces process
6. Parent: waitpid() (unless background)

**`execute_pipe(input)`**
1. Tokenize by '|'
2. Create pipes between commands
3. Fork child for each command
4. Each child: dup2() stdin/stdout to pipes
5. execve() in each child
6. Parent: wait() for all children

**`execute_redirect(cmd)`**
1. Find '>', '>>', or '<'
2. Open files with appropriate flags
3. Fork child process
4. Child: dup2() stdin/stdout to files
5. Trim argv to remove redirect symbols
6. execve() child
7. Parent: wait() and close files

### Built-ins (builtins.c)

Each built-in:
- Takes `char **argv`
- Returns exit code (0=success)
- Called directly, no fork needed

Example: `builtin_cd(argv)`
```c
char *dir = argv[1] ? argv[1] : getenv("HOME");
if (chdir(dir) != 0) {
    perror("cd");
    return 1;
}
return 0;
```

## Adding Features

### Add a Built-in Command

1. **Declare** in bashr.h:
   ```c
   int builtin_mycommand(char **args);
   ```

2. **Implement** in builtins.c:
   ```c
   int builtin_mycommand(char **args) {
       // Your logic here
       return 0;
   }
   ```

3. **Register** in executor.c:
   ```c
   if (strcmp(cmd->argv[0], "mycommand") == 0)
       return builtin_mycommand(cmd->argv);
   ```

4. **Add to is_builtin()** array in builtins.c

### Add Variable Expansion ($VAR)

1. Create `expand_variables(input)` in utils.c
2. Find '$' followed by alphanumeric
3. Replace with `getenv()` value
4. Call in main loop before parsing

### Add Error Stream Redirection (2>)

1. Enhance `execute_redirect()` to detect "2>"
2. Open file with O_WRONLY | O_CREAT
3. Use `dup2(fd, STDERR_FILENO)` in child
4. Close and wait for child

### Add Logical Operators (&&, ||)

1. In main loop, detect && or ||
2. Parse left and right commands
3. Execute left: `int ret = execute_command(left)`
4. For &&: only execute right if ret == 0
5. For ||: only execute right if ret != 0

### Add Command History

1. Use `readline/history.h` library
2. Replace `read_line()` with `readline()`
3. Add to history with `add_history()`
4. Save/load from ~/.bashr_history

## API Reference

### Parser Functions
```c
char** tokenize(char *input, const char *delim);
Command* parse_command(char *input);
void free_command(Command *cmd);
void trim_whitespace(char *str);
int contains_char(const char *str, char c);
char* find_in_path(const char *cmd);
```

### Executor Functions
```c
int execute_command(Command *cmd);
int execute_pipe(char *input);
int execute_redirect(Command *cmd);
```

### Built-in Functions
```c
int is_builtin(const char *cmd);
int builtin_cd(char **args);
int builtin_pwd(char **args);
int builtin_exit(char **args);
int builtin_echo(char **args);
int builtin_env(char **args);
int builtin_export(char **args);
int builtin_jobs(void);
```

### Job Control
```c
void add_job(pid_t pid, const char *cmd);
void remove_job(pid_t pid);
void print_jobs(void);
void reap_zombies(void);
```

## Debugging Tips

### Print Debug Info
```c
fprintf(stderr, "DEBUG: %s\n", variable);
```

### Trace System Calls
```bash
strace -e trace=process,fd ./bashr <<< "ls"
```

### Check for Memory Leaks
```bash
valgrind --leak-check=full ./bashr <<< "pwd"
```

### Debug with GDB
```bash
gdb ./bashr
(gdb) break execute_command
(gdb) run
bashr> ls
(gdb) print cmd->argv[0]
```

## Common Patterns

### Safe String Handling
```c
char buf[256];
strncpy(buf, src, sizeof(buf) - 1);
buf[sizeof(buf) - 1] = '\0';
snprintf(buf, sizeof(buf), "format %s", var);
```

### Safe Memory Allocation
```c
char *ptr = malloc(size);
if (!ptr) return error;
// use ptr
free(ptr);
```

### Proper Signal Handling
```c
void handler(int sig) {
    (void)sig;  // Mark as used
    write(1, "\n", 1);  // Only async-signal-safe calls
}
signal(SIGINT, handler);
```

### Process Creation Pattern
```c
pid_t pid = fork();
if (pid == 0) {
    // Child process
    execve(path, argv, environ);
    perror("execve");
    exit(127);
} else if (pid > 0) {
    // Parent process
    waitpid(pid, &status, 0);
} else {
    perror("fork");
}
```

## Performance Tips

1. **Minimize fork() calls** - Expensive operation
2. **Cache PATH searches** - Don't search every time
3. **Use stack allocation** - Faster than heap
4. **Close file descriptors** - Before execve
5. **Reuse buffers** - Reduce malloc calls

## Security Notes

- ✅ Use strncpy, snprintf (not strcpy, sprintf)
- ✅ Use execve with argv (not system())
- ✅ Validate buffer limits
- ✅ Only async-signal-safe in handlers
- ✅ Close files after use

## Testing Strategy

1. **Unit test** each function
2. **Integration test** pipe + redirect together
3. **Stress test** rapid commands
4. **Valgrind** for memory safety
5. **Compare with bash** for edge cases

---

**Need help?** Check REFERENCE.md for diagrams and troubleshooting.
