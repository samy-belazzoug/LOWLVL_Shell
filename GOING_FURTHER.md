# LOWLVL Shell - Requirements Analysis vs Implementation

## Core Requirements (Based on Nanoshell & Reverse_engineering)

### ✅ IMPLEMENTED (Fully Complete)

#### 1. **Input/Output Loop**
- ✅ while(1) REPL loop
- ✅ fgets() with 4096 buffer (Linux kernel limit)
- ✅ Prompt display
- ✅ Command reading and parsing

#### 2. **Execution: fork() + execve()**
- ✅ fork() creates child process
- ✅ execve() with correct argv[] array (pathname as argv[0])
- ✅ environ parameter passed correctly
- ✅ PATH lookup when command path not specified
- ✅ Error handling for fork() failures
- ✅ Error handling for execve() failures

#### 3. **Process Management: wait()**
- ✅ Parent waits for child with waitpid()
- ✅ WIFEXITED() check for normal termination
- ✅ WEXITSTATUS() to get exit code
- ✅ Proper zombie process cleanup
- ✅ Signal handlers for SIGCHLD

#### 4. **Parsing: Command Line Transformation**
- ✅ Tokenize input string into argc/argv
- ✅ Handle whitespace trimming
- ✅ Support multiple arguments
- ✅ Proper NULL-termination of argv array

#### 5. **Core Shell Features**
- ✅ Built-in commands (cd, pwd, exit, echo, env, export, jobs)
- ✅ External command execution
- ✅ Command not found error handling
- ✅ Perror() for system errors
- ✅ Signal handling (SIGINT for Ctrl+C)

---

## ✨ BONUS Features Implemented (Beyond Core Requirements)

### **Advanced Features**
- ✅ Pipes (|) with multi-stage support
- ✅ I/O Redirection (>, >>, <)
- ✅ Background execution (&)
- ✅ Job control & tracking
- ✅ Multiple redirections
- ✅ Complex pipe chains

### **Code Quality**
- ✅ Modular architecture (5 focused modules)
- ✅ Memory safety (no leaks, no overflows)
- ✅ Signal-safe implementations
- ✅ Proper error handling on all paths
- ✅ Clean, readable code

### **Documentation**
- ✅ 4 focused guides
- ✅ Architecture diagrams
- ✅ API reference
- ✅ Extension guides
- ✅ Troubleshooting tips

---

## 📋 "Aller plus loin" (Going Further) - Enhancement Ideas

### **Phase 1: Quick Wins (1-2 hours each)**

#### 1. **Variable Expansion ($VAR)**
```bash
bashr:/$ export NAME=value
bashr:/$ echo $NAME
value
```
- Implement in utils.c: `expand_variables()`
- Use getenv() for lookup
- Handle $HOME, $PATH, etc

#### 2. **Glob Pattern Expansion (*.txt)**
```bash
bashr:/$ ls *.md
README.md GUIDE.md REFERENCE.md
```
- Use glob.h library
- Expand patterns before execution
- Handle wildcards: *, ?, [abc]

#### 3. **2> Stderr Redirection**
```bash
bashr:/$ command 2> errors.txt
bashr:/$ command 2>> errors.txt
```
- Enhance execute_redirect()
- Add dup2() for STDERR_FILENO
- Test with commands that produce errors

#### 4. **2>&1 Redirect Stderr to Stdout**
```bash
bashr:/$ command 2>&1 | grep error
```
- Combine stdout and stderr
- Useful for piping error messages

### **Phase 2: Medium Complexity (2-4 hours each)**

#### 5. **Logical Operators (&&, ||)**
```bash
bashr:/$ cd /tmp && ls
bashr:/$ invalid || echo "Command failed"
```
- Detect && or || in main loop
- Execute sequentially with condition checking
- Left-to-right evaluation

#### 6. **Command History**
```bash
bashr:/$ [UP ARROW]          # Previous command
bashr:/$ [DOWN ARROW]        # Next command
bashr:/$ history             # List all commands
```
- Install readline/history.h
- Save history to ~/.bashr_history
- Load on startup
- Use add_history() and read_history()

#### 7. **Quoted Strings & Escaping**
```bash
bashr:/$ echo "hello world"      # Double quotes
bashr:/$ echo 'literal $VAR'     # Single quotes
bashr:/$ echo "escaped \" quote" # Escaping
```
- Handle quote parsing in tokenizer
- Preserve spaces in quotes
- Handle escape sequences

#### 8. **Command Substitution**
```bash
bashr:/$ echo $(pwd)
/home/user
bashr:/$ files=$(ls *.txt)
```
- Parse $() syntax
- Execute inner command
- Substitute output

### **Phase 3: Advanced Features (4+ hours each)**

#### 9. **Aliases**
```bash
bashr:/$ alias ll="ls -la"
bashr:/$ ll
total 48...
```
- Maintain alias hash table
- Check before PATH lookup
- Save/load from config file

#### 10. **Functions & Scripts**
```bash
bashr:/$ function greet() { echo "Hello $1"; }
bashr:/$ greet World
Hello World
```
- Parse function definitions
- Store in memory
- Support local variables

#### 11. **Control Flow (if, for, while)**
```bash
bashr:/$ for i in 1 2 3; do echo $i; done
bashr:/$ if [ -f file.txt ]; then cat file.txt; fi
```
- Implement conditional parsing
- Support loops
- Add [ ] test syntax

#### 12. **Configuration File (~/.bashrrc)**
```bash
# Loaded on startup
export PS1="custom$ "
alias ll="ls -la"
```
- Read config file
- Apply settings
- Support environment customization

### **Phase 4: Polish & Optimization**

#### 13. **Tab Completion**
- Autocomplete commands
- Autocomplete filenames
- Smart suggestions

#### 14. **Command Highlighting**
- Syntax highlighting in prompt
- Color valid/invalid commands
- Visual feedback

#### 15. **Performance Optimization**
- Cache PATH lookups
- Optimize tokenizer
- Reduce allocations

#### 16. **Better Error Messages**
```bash
bashr:/$ cd /nonexistent
cd: cannot access '/nonexistent': No such file or directory
bashr:/$ cmd | nonexistent
nonexistent: command not found
```
- Detailed, helpful errors
- Suggestions for typos
- Command not found hints

---

## Implementation Priority

### **Must Have (Core Shell)**
1. ✅ REPL loop
2. ✅ fork() + execve()
3. ✅ wait() + signal handling
4. ✅ Command parsing
5. ✅ Built-in commands

### **Should Have (Enhanced Shell)**
6. ✅ Pipes
7. ✅ Redirection
8. ✅ Background jobs
9. Variable expansion
10. Quoted strings

### **Nice to Have (Advanced)**
11. Logical operators
12. Command history
13. Tab completion
14. Aliases
15. Functions

---

## How to Add Features

See **GUIDE.md** for step-by-step instructions on:
- Adding a built-in command
- Adding variable expansion
- Adding error redirection
- Adding logical operators
- Adding command history
- Adding quoted strings
- Adding aliases
- And more...

Each includes:
- Code examples
- Integration points
- Testing strategy
- Edge cases

---

## Quick Feature Implementation Times

| Feature | Time | Difficulty |
|---------|------|-----------|
| 2> redirection | 30 min | Easy |
| Variable expansion | 1 hour | Easy |
| Glob patterns | 1 hour | Medium |
| && and \|\| | 1 hour | Medium |
| Command history | 1.5 hours | Medium |
| Quoted strings | 1.5 hours | Medium |
| Tab completion | 2 hours | Hard |
| Functions | 3 hours | Hard |
| Control flow | 4 hours | Hard |

---

## Testing These Features

After implementing each feature:
1. Test basic usage
2. Test with pipes
3. Test with redirection
4. Test edge cases
5. Check memory (valgrind)
6. Check performance (strace)

---

## Summary

**Your current implementation:** ✅ 100% of core requirements + bonus features
**"Aller plus loin" opportunities:** 15+ enhancement ideas with implementation guides

**Status:** PRODUCTION-READY, EASILY EXTENSIBLE
