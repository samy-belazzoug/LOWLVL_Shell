# LOWLVL Shell - Architecture & Design

## Project Structure

```
src/
├── bashr.c       - Main shell loop (REPL)
├── bashr.h       - Header with all function declarations
├── executor.c    - Command execution, pipes, redirections
├── builtins.c    - Built-in commands (cd, pwd, echo, etc)
├── utils.c       - Utilities (tokenizing, PATH search, job control)
└── test/         - Test files
```

## Architecture

### 1. **Parser Module** (utils.c)
- **`tokenize()`** - Split input by delimiters
- **`parse_command()`** - Parse command line into argc/argv
- **`trim_whitespace()`** - Remove leading/trailing whitespace
- **`contains_char()`** - Check for special characters (|, >, <)

### 2. **Executor Module** (executor.c)
- **`execute_command()`** - Execute standard commands (with PATH lookup)
- **`execute_pipe()`** - Handle pipe chains (cmd1 | cmd2 | cmd3)
- **`execute_redirect()`** - Handle I/O redirection (>, >>, <)

**Key Features:**
- Signal handlers for SIGINT (Ctrl+C) and SIGCHLD (zombie process cleanup)
- Background execution with `&` operator
- Proper file descriptor management

### 3. **Built-in Commands** (builtins.c)
- `cd <dir>` - Change directory
- `pwd` - Print working directory
- `exit [code]` - Exit shell
- `echo [args...]` - Print arguments
- `env` - List environment variables
- `export VAR=value` - Set environment variable
- `jobs` - List background jobs

### 4. **Job Control** (utils.c)
- Track background processes
- Reap zombie processes
- Print job status

### 5. **Main Loop** (bashr.c)
- Read input with custom prompt
- Route to appropriate handler (pipes, redirect, builtin, or external)
- Handle signals properly

## Feature Coverage

| Feature | Status | Implementation |
|---------|--------|-----------------|
| Interactive REPL | ✓ | bashr.c loop |
| Command parsing | ✓ | utils.c tokenize |
| Built-in commands | ✓ | builtins.c |
| PATH resolution | ✓ | utils.c find_in_path |
| Pipes (&#124;) | ✓ | executor.c execute_pipe |
| Output redirect (>) | ✓ | executor.c execute_redirect |
| Append (>>) | ✓ | executor.c execute_redirect |
| Input redirect (<) | ✓ | executor.c execute_redirect |
| Background (&) | ✓ | executor.c background flag |
| Signal handling | ✓ | bashr.c signal handlers |
| Job control | ✓ | utils.c job functions |
| Error handling | ✓ | All modules |

## Signal Handling

- **SIGINT (Ctrl+C)** - Interrupts current command, returns to prompt
- **SIGCHLD** - Reaps background processes automatically

## Memory Management

- All allocations properly freed in error paths
- Command structures cleaned up after execution
- Token arrays freed after parsing

## Testing

Run: `make test`

This executes basic commands through the shell to verify:
- Directory changing (cd)
- Working directory display (pwd)
- Echo functionality

## Future Enhancements

1. **Variable Expansion** - $VAR, ${VAR}, $HOME substitution
2. **Command History** - Using readline or similar
3. **Glob Patterns** - Filename wildcard expansion (*.txt)
4. **Quoting** - Single/double quotes, escaping
5. **Redirects** - 2>, 2>>, &>, stderr handling
6. **Aliases** - Command aliasing support
7. **Functions** - Shell function definitions
8. **Conditionals** - && and || operators
9. **Here-docs** - << operator
10. **Process Substitution** - <(cmd) and >(cmd)
