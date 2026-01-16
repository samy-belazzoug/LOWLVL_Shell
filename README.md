# LOWLVL Shell

A complete shell implementation in C (single file, 460 lines).

## Features

- Interactive REPL with prompt
- 7 built-in commands: cd, pwd, exit, echo, env, export, jobs
- Pipes (|), I/O redirection (>, >>, <), background execution (&)
- Job control, signal handling, PATH resolution

## Build

```bash
make              # or: gcc -Wall -Wextra -std=c99 -o bashr bashr.c
make clean        # remove artifacts
make rebuild      # clean + build
```

## Run

```bash
./bashr
bashr:~$ pwd
/path/to/shell
bashr:~$ ls | grep bashr
bashr bashr.c
bashr:~$ exit
```

## Files

- **bashr.c** - Single source file (460 lines, all-in-one)
- **Makefile** - Build system
- **README.md** - This file
- **.gitignore** - Git ignore rules

## Compliance

Uses only allowed functions:
- I/O: printf, fgets
- Memory: malloc, free
- Process: fork, execve, waitpid, exit
- Environment: getenv, perror
- Directory: getcwd, chdir, access
- String: strlen, strcmp, strcpy, strncpy, strchr, strtok

## Built-in Commands

| Command | Function |
|---------|----------|
| cd [dir] | Change directory |
| pwd | Print working directory |
| echo [text] | Print text |
| env | List environment variables |
| export VAR=val | Set environment variable |
| jobs | List background jobs |
| exit [code] | Exit shell |

## Examples

```bash
# Pipes
bashr:~$ ls -la | grep .c

# Redirection  
bashr:~$ echo "hello" > file.txt
bashr:~$ cat < file.txt

# Background jobs
bashr:~$ sleep 100 &
bashr:~$ jobs
```

