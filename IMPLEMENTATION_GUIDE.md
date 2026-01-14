# LOWLVL Shell - Implementation Guide

## How to Add New Features

### 1. Adding a New Built-in Command

**Example: Add `ls` as a built-in**

1. **Declare in `bashr.h`:**
```c
int builtin_ls(char **args);
```

2. **Implement in `builtins.c`:**
```c
int builtin_ls(char **args) {
    char *cmd = "ls";
    if (args && args[1]) {
        cmd = malloc(256);
        snprintf(cmd, 256, "ls %s", args[1]);
    }
    return system(cmd);
}
```

3. **Register in `is_builtin()` and `execute_command()` in `executor.c`:**
```c
if (strcmp(cmd->argv[0], "ls") == 0) return builtin_ls(cmd->argv);
```

### 2. Adding Variable Expansion ($VAR)

**In `utils.c`, enhance `tokenize()` or create `expand_variables()`:**

```c
char* expand_variables(char *str) {
    static char result[4096];
    char *var_start, *var_end;
    
    while ((var_start = strchr(str, '$')) != NULL) {
        var_end = var_start + 1;
        while (isalnum(*var_end) || *var_end == '_') {
            var_end++;
        }
        
        char var_name[256];
        strncpy(var_name, var_start + 1, var_end - var_start - 1);
        var_name[var_end - var_start - 1] = '\0';
        
        char *value = getenv(var_name);
        if (value) {
            // Replace $VAR with value
        }
    }
    return result;
}
```

Call before executing: `char *expanded = expand_variables(input);`

### 3. Adding Command History (readline)

**Install readline development headers:**
```bash
sudo apt-get install libreadline-dev
```

**Modify `bashr.c` main loop:**
```c
#include <readline/readline.h>
#include <readline/history.h>

while (running) {
    char *line = readline("bashr> ");
    if (line && *line) {
        add_history(line);
        // Process line...
    }
    free(line);
}
```

### 4. Adding Glob Pattern Expansion

**Create `glob_expand()` function:**

```c
#include <glob.h>

char** glob_expand(const char *pattern) {
    glob_t glob_result;
    
    if (glob(pattern, 0, NULL, &glob_result) != 0) {
        return NULL;
    }
    
    char **matches = malloc((glob_result.gl_pathc + 1) * sizeof(char*));
    for (size_t i = 0; i < glob_result.gl_pathc; i++) {
        matches[i] = strdup(glob_result.gl_pathv[i]);
    }
    matches[glob_result.gl_pathc] = NULL;
    
    globfree(&glob_result);
    return matches;
}
```

Use in parsing before execution.

### 5. Adding Logical Operators (&&, ||)

**In `bashr.c` main loop:**

```c
if (contains_char(input, '&') && strstr(input, "&&")) {
    // Handle AND operator
    char **cmds = tokenize(input, "&&");
    for (int i = 0; cmds[i]; i++) {
        Command *cmd = parse_command(cmds[i]);
        int result = execute_command(cmd);
        if (result != 0) break;  // Stop on first failure
    }
} else if (contains_char(input, '|') && strstr(input, "||")) {
    // Handle OR operator
    // ...
}
```

### 6. Adding Error Stream Redirection (2>)

**Enhance `execute_redirect()` in `executor.c`:**

```c
if (strcmp(cmd->argv[i], "2>") == 0) {
    redirect_err = i;
    break;
} else if (strcmp(cmd->argv[i], "2>>") == 0) {
    redirect_err = i;
    append_err = 1;
    break;
}

// In child process:
if (err_fd >= 0) {
    dup2(err_fd, STDERR_FILENO);
    close(err_fd);
}
```

### 7. Adding Aliases

**Add to data structures in `bashr.h`:**

```c
typedef struct {
    char name[256];
    char cmd[1024];
} Alias;

Alias aliases[MAX_ALIASES];
int alias_count = 0;
```

**Implement in `utils.c`:**

```c
void add_alias(const char *name, const char *cmd) {
    if (alias_count >= MAX_ALIASES) return;
    strncpy(aliases[alias_count].name, name, 255);
    strncpy(aliases[alias_count].cmd, cmd, 1023);
    alias_count++;
}

char* resolve_alias(const char *cmd) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, cmd) == 0) {
            return aliases[i].cmd;
        }
    }
    return NULL;
}
```

**Use in execution:**

```c
char *expanded = resolve_alias(cmd->argv[0]);
if (expanded) {
    // Use expanded command
}
```

### 8. Memory Leak Prevention

**Always free allocated memory:**

```c
// Good pattern:
Command *cmd = parse_command(input);
if (cmd) {
    int ret = execute_command(cmd);
    free_command(cmd);
}

// Use cleanup handlers for signals:
void cleanup(int sig) {
    // Free globals
    exit(0);
}

signal(SIGINT, cleanup);
signal(SIGTERM, cleanup);
```

### 9. Testing Your Changes

**Write unit tests:**

```c
void test_tokenize() {
    char **tokens = tokenize("ls -la | grep test", "|");
    assert(strcmp(tokens[0], "ls -la") == 0);
    assert(strcmp(tokens[1], " grep test") == 0);
    free_tokens(tokens);
    printf("✓ tokenize test passed\n");
}

void test_parse_command() {
    Command *cmd = parse_command("echo hello world");
    assert(cmd->argc == 3);
    assert(strcmp(cmd->argv[0], "echo") == 0);
    free_command(cmd);
    printf("✓ parse_command test passed\n");
}
```

Run tests frequently during development.

## Architecture Tips

1. **Keep functions small** - Each function should do one thing
2. **Use meaningful names** - `execute_with_pipe` > `exec_p`
3. **Handle errors properly** - Check return values, use `perror()`
4. **Avoid global state** - Minimize globals, encapsulate when possible
5. **Memory safety** - Always check allocations, free in error paths
6. **Signal safety** - Only call async-signal-safe functions in handlers

## Performance Considerations

- **Process creation is expensive** - Minimize forks, cache PATH lookups
- **String operations** - Use fixed buffers when limits are known
- **File descriptors** - Close unused FDs before execve
- **Memory allocation** - Pool allocations for commands/arguments

## Security Notes

- **Buffer overflows** - Use `strncpy`, `snprintf` instead of `strcpy`, `sprintf`
- **Command injection** - Don't use `system()`, always use `execve` with argv
- **Path traversal** - Validate file paths, don't trust user input blindly
- **Signal handlers** - Use only async-signal-safe functions
