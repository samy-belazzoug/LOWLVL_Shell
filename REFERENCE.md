# LOWLVL Shell - Reference & Troubleshooting

## Visual Diagrams

### Command Execution Flow
```
Input: "ls -la | grep test > output.txt"
  ↓
Is '|' present? YES → execute_pipe()
  ↓
Is '>' or '<' present? YES → execute_redirect()
  ↓
Is '&' present? YES → execute in background
  ↓
Fork and execve
  ↓
Back to prompt
```

### Pipe Architecture
```
ls | grep .md | wc -l

[ls]        outputs to pipe1
  ↓         
[grep]      reads from pipe1, outputs to pipe2
  ↓
[wc]        reads from pipe2
  ↓
Terminal
```

### Signal Flow
```
User presses Ctrl+C
  ↓
OS sends SIGINT
  ↓
sigint_handler() called
  ↓
Print newline
  ↓
Return to main loop
```

### Process Tree
```
bashr (shell)
  ├─ ls (fork + execve)
  ├─ grep (fork + execve)
  ├─ cat (fork + execve)
  └─ ... (background jobs)
```

## Common Issues & Solutions

### Build Issues

**Error: gcc not found**
- Windows: Use WSL
- Linux/Mac: Install build-essential

**Error: undefined reference to 'environ'**
- Add: `extern char **environ;` at top of file

**Warnings during compilation**
- Mark unused params: `(void)param;`

### Runtime Issues

**Command not found**
```
bashr:/$ nonexistent
bashr: command not found: nonexistent
```
→ Check if command is in PATH: `which ls`

**Pipes produce no output**
```bash
ls | grep nonexistent
# Shows nothing? That's correct - nothing matches
```
→ Test with: `ls | head -3`

**Redirection creates no file**
```bash
echo test > file.txt
# File not created?
```
→ Check directory permissions: `ls -la`

**Ctrl+C doesn't work**
→ Ensure signal handler is installed: check bashr.c

**Background jobs accumulate**
→ SIGCHLD handler should reap them: check bashr.c

### Memory & Stability

**Segmentation fault**
- Run with valgrind: `valgrind ./bashr <<< "pwd"`
- Use gdb: `gdb ./bashr` then `run`

**Memory leaks**
```bash
valgrind --leak-check=full ./bashr <<< "ls"
```
→ Check for missing free() calls

**Too many open files**
→ Not closing file descriptors after use

## Testing Checklist

```
☐ Basic commands work (pwd, ls, cat)
☐ cd changes directory
☐ Pipes work (cmd1 | cmd2)
☐ Redirection works (>, >>, <)
☐ Background execution works (&)
☐ Ctrl+C returns to prompt
☐ Multiple commands in sequence
☐ Complex pipes (3+ stages)
☐ No memory leaks (valgrind)
☐ No zombie processes
```

## Quick Commands

### Build & Test
```bash
bash build.sh              # Compile
./bashr                    # Run shell
echo "pwd" | ./bashr       # Test single command
bash test_shell.sh         # Run test suite
```

### Debugging
```bash
strace -e trace=process ./bashr <<< "ls"    # System calls
valgrind ./bashr <<< "pwd"                   # Memory check
gdb ./bashr                                   # Interactive debug
```

### Find Issues
```bash
# List open files
lsof -p $(pgrep bashr)

# Check processes
ps aux | grep bashr

# Trace system calls
strace -e trace=execve ./bashr <<< "ls"
```

## Performance Tips

- **First call is slowest** due to binary loading
- **Pipes are efficient** for chaining commands
- **Large pipelines** (10+) still fast on modern systems
- **Background jobs** add minimal overhead

## File Descriptor Reference

```
STDIN_FILENO   = 0   # Standard input
STDOUT_FILENO  = 1   # Standard output
STDERR_FILENO  = 2   # Standard error
```

### dup2() Examples
```c
dup2(input_fd, STDIN_FILENO);   // Redirect input
dup2(output_fd, STDOUT_FILENO); // Redirect output
dup2(error_fd, STDERR_FILENO);  // Redirect errors
```

## Process Functions Reference

```c
fork()              // Create child process
execve(path, argv)  // Replace process with program
waitpid(pid)        // Wait for child to complete
getpid()            // Get current process ID
getenv(var)         // Get environment variable
setenv(var, val)    // Set environment variable
```

## Error Messages Explained

```
bashr: command not found: xyz
→ Command not in PATH

No such file or directory
→ File doesn't exist or wrong permissions

Permission denied
→ File not executable or no read access

Broken pipe
→ Reading end closed while writing

Too many open files
→ Not closing file descriptors
```

## System Call Tracing

```bash
# Trace all execve() calls
strace -e trace=execve ./bashr <<< "ls"

# Trace all file operations
strace -e trace=open,close,read,write ./bashr

# Follow child processes
strace -f ./bashr <<< "ls | grep md"

# Count system calls
strace -c ./bashr <<< "pwd"
```

## Environment Variables

```bash
# Check PATH
echo $PATH

# Check HOME
echo $HOME

# List all variables
env

# Set in bashr
bashr:/$ export MYVAR=value
```

## Further Reading

```
man 2 fork          # Create process
man 2 execve        # Execute program
man 2 pipe          # Create pipe
man 2 dup2          # Duplicate file descriptor
man 2 wait          # Wait for process
man 2 signal        # Signal handling
man 3 getenv        # Get environment variable
```

---

**See GUIDE.md for architecture and how to add features.**
**See README.md for quick start.**
