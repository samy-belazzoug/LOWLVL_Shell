# LOWLVL Shell - Troubleshooting Guide

## Common Issues and Solutions

### Build Issues

#### Error: "gcc: command not found"
**Solution:** Use WSL (Windows Subsystem for Linux)
```bash
wsl bash build.sh
```
Or compile with clang if available:
```bash
sed -i 's/gcc/clang/' build.sh
bash build.sh
```

#### Error: "cannot find -lpthread"
**Solution:** Add pthread library to build script
```bash
# Modify Makefile or build.sh
LDFLAGS = -lpthread
```

#### Undefined reference to 'environ'
**Solution:** Add declaration at top of file:
```c
extern char **environ;
```

### Runtime Issues

#### Shell exits immediately after starting
**Possible causes:**
1. EOF from stdin - Add check in main loop
2. Compilation error - Verify binary was built successfully
3. File permissions - Check executable bit: `chmod +x bashr`

**Debug:**
```bash
./bashr <<< "echo test"    # Single command
echo "pwd" | ./bashr       # Piped input
strace -e trace=execve ./bashr <<< "ls"  # Trace system calls
```

#### "command not found" for valid commands
**Causes:**
1. Command not in PATH
2. Hash table not updated (if caching PATH)
3. Binary search failed

**Fix:**
```bash
# Check PATH
echo $PATH
/usr/bin/ls    # Use full path in bashr
which ls       # Find command location
```

#### Pipes not working ("ls | grep" shows nothing)
**Causes:**
1. Pipe not being detected - Check `contains_char()` logic
2. File descriptor not properly connected
3. Child process failure

**Debug:**
```bash
# Test with simple pipe
echo "test" | cat        # Should echo "test"

# Test in shell
ls | head -1             # Should show first file

# Check for errors
ls | nonexistent 2>&1    # Should show error
```

#### Redirections not creating files
**Causes:**
1. File opening failed (permissions)
2. Incorrect fd handling
3. Path not existing

**Debug:**
```bash
# Test redirection
echo "test" > /tmp/out.txt
ls -la /tmp/out.txt      # Check file was created
cat /tmp/out.txt         # Verify content
```

#### Background jobs not tracking
**Causes:**
1. SIGCHLD handler not installed
2. Job array full (MAX_JOBS limit)
3. Process completed before listing

**Fix:**
```bash
sleep 10 &      # Start background job
sleep 1         # Give it time to start
jobs            # List jobs immediately
```

### Memory/Stability Issues

#### Segmentation fault on certain commands
**Possible causes:**
1. Buffer overflow in tokenize/parse
2. NULL pointer dereference
3. Double free

**Debug with gdb:**
```bash
gdb ./bashr
(gdb) run
bashr> <problematic command>
(gdb) backtrace     # Show call stack
(gdb) print cmd     # Inspect variables
(gdb) quit
```

#### Memory leak (process grows with use)
**Solution:** Add valgrind check
```bash
valgrind --leak-check=full ./bashr <<< "ls"
```

**Common leaks:**
- Forgot to free tokens in `free_tokens()`
- Forgot to free command in main loop
- Forgot to free path string

### Signal Handling Issues

#### Ctrl+C not working
**Cause:** SIGINT handler not installed or not registered correctly

**Fix:**
```c
signal(SIGINT, sigint_handler);  // In main()

static void sigint_handler(int sig) {
    (void)sig;  // Suppress unused warning
    printf("\n");
    fflush(stdout);
}
```

#### Zombie processes accumulating
**Cause:** SIGCHLD handler not properly reaping children

**Fix:**
```c
signal(SIGCHLD, sigchld_handler);

static void sigchld_handler(int sig) {
    (void)sig;
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        // Reap all available children
    }
}
```

### PATH and Command Resolution

#### Command works in bash but not in bashr
**Cause:** PATH not set or different between shells

**Debug:**
```bash
# In bashr
env | grep PATH

# In bash
echo $PATH

# Check if paths differ
```

**Solution:**
```c
// In find_in_path, handle missing PATH gracefully
if (!path_env) {
    // Use default paths as fallback
    const char *defaults[] = {"/usr/bin", "/bin", "/usr/local/bin"};
}
```

#### find_in_path() too slow
**Cause:** PATH has many directories

**Solution:** Add simple caching
```c
typedef struct {
    char cmd[256];
    char path[1024];
    time_t cached_at;
} PathCache;

static PathCache cache[64];
static int cache_size = 0;

char* find_in_path_cached(const char *cmd) {
    // Check cache first
    for (int i = 0; i < cache_size; i++) {
        if (strcmp(cache[i].cmd, cmd) == 0) {
            return cache[i].path;
        }
    }
    // Fall back to search
    char *path = find_in_path(cmd);
    if (path) {
        // Add to cache
    }
    return path;
}
```

### File Descriptor Issues

#### "Too many open files" error
**Cause:** Not closing file descriptors properly

**Fix in execute_redirect():**
```c
// Always close FDs in child before execve
if (in_fd >= 0) close(in_fd);
if (out_fd >= 0) close(out_fd);

// Check for leaks
lsof -p <pid>    # List open files by PID
```

#### File permissions errors
**Cause:** File not writable, no permissions, or read-only filesystem

**Debug:**
```bash
# Check permissions
ls -la file.txt
# Fix permissions
chmod 644 file.txt
# Check disk space
df -h
```

### Performance Issues

#### Shell response slow
**Profiling:**
```bash
time echo "ls" | ./bashr    # Measure execution time
strace -c ./bashr <<< "ls"  # Count system calls
```

**Optimization:**
1. Cache PATH searches
2. Reduce malloc calls - use stack allocation for small objects
3. Batch close() calls
4. Avoid unnecessary string copies

#### High CPU usage
**Debug:**
```bash
# Top in one terminal
top -p <bashr_pid>

# In bashr
sleep 0.1    # Gives CPU time for profiling
```

**Common causes:**
- Busy loop in SIGCHLD handler
- Infinite recursion in parse
- Fork bomb protection

## Testing Checklist

- [ ] Basic commands work (ls, pwd, cat)
- [ ] Built-in commands work (cd, echo, exit)
- [ ] Pipes work (cmd1 | cmd2)
- [ ] Redirection works (>, >>, <)
- [ ] Background execution works (&)
- [ ] Ctrl+C returns to prompt
- [ ] No memory leaks (valgrind clean)
- [ ] No zombie processes (ps aux)
- [ ] Works with complex commands
- [ ] Handles errors gracefully
- [ ] Works across multiple invocations

## Getting Help

1. **Check error messages** - Usually point to the problem
2. **Use strace** - Trace system calls: `strace -e trace=process,fd ./bashr`
3. **Add debug output** - `fprintf(stderr, "DEBUG: ...")`
4. **Test in pieces** - Test tokenizer, parser, executor separately
5. **Compare with bash** - See how real shell handles edge cases

## Resources

- `man 2 execve` - execve() system call
- `man 2 pipe` - pipe() system call
- `man 2 fork` - fork() system call  
- `man 2 signal` - signal handling
- `man 3 strtok` - string tokenization
- `man 3 getenv` - environment variables
