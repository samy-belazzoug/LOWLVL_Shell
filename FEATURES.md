# LOWLVL Shell - Features & Usage

## Building

```bash
make              # Compile the shell
make clean        # Remove build artifacts
make test         # Run basic tests
```

## Running

```bash
./bashr           # Start the interactive shell
```

You'll see:
```
=== LOWLVL Shell ===
Type 'exit' to quit. Try 'cd', 'pwd', 'ls', pipes (|), redirection (>, >>)

bashr:/home/user$ 
```

## Built-in Commands

### Navigation
```bash
cd /tmp              # Change directory
cd                   # Go to home directory
pwd                  # Show current directory
```

### Output
```bash
echo Hello World     # Print text
echo $HOME           # (Currently plain text, no expansion yet)
```

### System
```bash
env                  # List all environment variables
export PATH=/new    # Set environment variable
jobs                 # List background processes
exit                 # Quit shell
exit 42              # Exit with status code
```

## External Commands

Run any system command using PATH lookup:
```bash
ls -la              # List files
cat file.txt        # Display file
grep pattern file   # Search in file
mkdir newdir        # Create directory
```

## Pipes

Chain commands together:
```bash
ls | grep .txt      # List and filter by .txt files
cat file | wc -l    # Count lines in file
ps aux | grep bash  # Find bash processes
cat file | head -5  # Show first 5 lines
```

**How it works:**
- Creates pipes between commands
- Forks a child process for each command
- Connects stdout of cmd1 to stdin of cmd2, etc.
- Waits for all processes to complete

## Input/Output Redirection

### Output to File
```bash
echo "test" > file.txt     # Write to file (overwrite)
echo "more" >> file.txt    # Append to file
ls > listing.txt           # Redirect command output
```

### Input from File
```bash
cat < file.txt             # Read from file
wc -l < numbers.txt        # Count lines from file
```

### Combined
```bash
cat < input.txt > output.txt   # Copy file content
```

## Background Execution

Run commands without waiting:
```bash
sleep 100 &         # Run in background
long_task &         # Continue typing commands
jobs                # See background processes
```

**Note:** Background jobs are tracked in memory but not fully managed yet (no fg/bg).

## Signals

- **Ctrl+C** - Stop current command, return to prompt
- **Ctrl+D** - Exit shell (EOF)

## Examples

### System Information
```bash
bashr:/$ pwd
/home/user

bashr:/$ uname -a
Linux ubuntu 5.15.0 #1 SMP x86_64 GNU/Linux
```

### File Operations
```bash
bashr:/$ ls | grep src
src

bashr:/$ echo "# New file" > README.txt
bashr:/$ cat README.txt
# New file
```

### Multiple Pipes
```bash
bashr:/$ cat /etc/passwd | head -3 | cut -d: -f1
root
daemon
bin
```

### Directory Navigation
```bash
bashr:/home$ cd /tmp
bashr:/tmp$ pwd
/tmp
bashr:/tmp$ cd
bashr:/home/user$ pwd
/home/user
```

## Limitations (Current)

- No variable expansion ($VAR)
- No filename globbing (*.txt)
- No quotes handling (single/double)
- No command history (up/down arrows)
- No output errors redirection (2>)
- No logical operators (&&, ||)
- No command substitution ($(cmd))

## Error Handling

- Missing command: "bashr: command not found: xyz"
- Missing file: "No such file or directory"
- Permission denied: "Permission denied"
- Syntax errors: "syntax error: missing filename"

All errors print to stderr and continue shell operation.
