# HOW CAN WE MAKE A SHELL ?!

*fork() + execve() is all you need*

This document will **reverse engineering** UNIX's shell in a very simple way for understanding purpose.

## What are the differences between a Shell, Terminal and CLI ?
https://www.tempmail.us.com/en/terminal/understanding-shell-terminal-and-cli-key-differences-explained
https://www.geeksforgeeks.org/operating-systems/difference-between-terminal-console-shell-and-command-line/
https://www.coursera.org/articles/command-line-interface
https://en.wikipedia.org/wiki/List_of_terminal_emulators

I think its important to know the differences between a Shell, Terminal and CLI to exactly know what we have to develop, the limits and the most important features. Personally, I'll discover with you.


### What is a Terminal ?

A terminal is a text input/output environment window emulating a console in a graphical user interface. To make it simple, the terminal is the environment where the user can access to consoles. It's the emulating section.

E.G : GNOME Terminal, Termux, Alacritty, Windows Terminal.

### What is a CLI ?

A command-line interface (CLI) is an interface paradigm mainly using text-based way to communicate with a computer's operating system (kernel) **OR** software through text-based commands. It's the interacting section.
Every inputs/output you see are displayed by the CLI.

E.G : Docker, AWS CLI, PowerShell

### What is a Shell ?

The Shell is a **program** that acts as an intermediary between the user and the kernel. Basically, it **interprets** commands and **communicates** with the kernel to execute user's commands. It's the interpreting and executing section.
Please note that we do not see the shell at all in the CLI!

E.G : bash, zsh, fish, this project!

## BASICALLY

When you open a *TERMINAL*, it **emulates** *COMMAND LINE INTERFACES*, where you can **interact** with the computer (kernel/software etc..) with **text-based commands**. The *SHELL*, IN THE BACKGROUND, **interprets, executes** the commands you've typed in the CLI. 

Without shell, you are just writing text, and without a terminal, you can't write text.

**Terminal = Emulation, CLI = Interface, Shell = Execution**

## What happens when you execute a prompt in the Bash ?
https://hackr.io/blog/how-linux-commands-work
https://helloroot.medium.com/how-linux-commands-work-what-happens-when-you-run-a-command-in-linux-26253b693ac9
https://docstore.mik.ua/orelly/unix/upt/ch08_01.htm
https://www.man7.org/linux/man-pages/man2/execve.2.html
https://www.codestudy.net/blog/how-do-i-parse-command-line-arguments-in-bash/
https://stackoverflow.com/questions/71333156/pathname-vs-arguments-for-execve-parameters
https://shunchild.com/article/will-valies-in-parent-process-be-altered-by-chile-process
https://www.geeksforgeeks.org/c/fork-system-call/
https://www.geeksforgeeks.org/c/wait-system-call-c/
man env
man execve
man fork

Let's say you want to do **ls -a**.

There is basically 4 main steps for the shell to return what we want (in this case, the list of all the content in a directory) :

### Input / Output : Getting your command and return something

When you enter the command line interface (bash) it invites you to type a command.

What I mean by that is that the CLI doesn't perform any operation when it launches, it waits for your demand.

The CLI in reality is "just" a while loop with strings input/output. It reads your input, send it to the shell and returns what the shell did with your input.

So, as soon as you've sent the command, it's gonna be shell's work.

## Execution : The business

So, you're gonna tell me : Doesn't we have to parse before the execution? So why do you talk about the execution before the parsing, are you okay?

Well, let me cook.
Before talking about shell's actions, we need to know exactly 2 very important functions : execve() and fork()

#### execve()
 
```c
#include <libsync.h>
int execve(const char *pathname, char *const _Nullable argv[], char *const _Nullable envp[])
```

Let's decompose this function :
**What it does :** 
*"executes  the  program  referred  to  by  pathname.  This causes the program that is currently being run by the calling
process to be replaced with a new program, with newly initialized stack, heap, and (initialized and uninitialized) data segments."*

Bacically, it replaces and executes the current program being run by the program mentionned by pathname

**Parameters :**
const char *pathname : Program to execute (e.g : ls)
char *cons_Nullable argv[] : Arguments following the program (e.g : -a, -l, -o etc..)
char *const_Nullable envp[] : String array of environment variables (execve will try to find pathname in all the environment variables in envp[])
*Generally speaking, you'll pass __environ as envp[] parameter as its simpler and faster. __environ contains every environment variables of the shell.*

Example of use of execve : 
```c
#include <libsync.h>

int main() {   
    // ls -a
    char *args[] = {"/bin/ls","-a",NULL};
    execve(args[0],args,__environ);
    return 0;
}
```
If you noticed it, yes, we passed args as the second argument, meaning we also pass args[0] which we already had passed for *pathname. This is a  system convention, in fact, args[] have to include the program name as first element.

#### fork()

```c
#include <unistd.h>
pid_t fork(void)
```
Let's decompose this function :

**What it does :**
*"creates a new process by duplicating the calling process.  The new process is referred to as the child process. The calling process is referred to as the parent process."*

Bacically, it creates a **child process** of the **current program**. The child process inherits the **COPY** of the parent's memory space, including variables.

**HOWEVER, BE CAREFUL !!** The child parent have only a **COPY** of its parent, it **cannot modify the content of its parent** (the inheritance just copies, it doesn't create a shared memory space!).

**Parameters :** 
void : no parameters needed.

Example of use of fork : 

```c
#include <stdio.h> //printf, puts
#include <stdlib.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> //fork 
#include <sys/types.h> //pid_t instead of __pid_t

int main(void) {
    pid_t pid;

    pid = fork();
    switch (pid) {
        case -1: // Child creation process got an error 
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: // Child is created. We are in child's process    
            printf("Currently child process with PID : %d\n",getpid());
            puts("Child exiting.");
            exit(EXIT_SUCCESS);
       default: // We are in parent's process
            printf("Currently parent process with PID : %d\n",getpid());
            puts("Parent exiting.");
    }
    puts("Now the program is gone.");
    exit(EXIT_SUCCESS);
}
```
*This is the code (slightly modified for easier understanding) in the official documentation of man fork.*

So, fork **returns* a **pid_t** value (which is a kind of int), we need to stock this value in a variable (pid_t pid) so its easier for us to code.

If fork() **returns -1** -> the child process creation has **failed**, you do not have your child process.

if fork() **returns 0** -> The child is **created**, we can operate on it.

if you launch this code, you'll notice two things :
-child's program is actually in **case 0**.
-child process will operate **after parent** program has finished.

Good question now, can the child process operate before its parent process end ?

And, yes, we can do it with wait() function

## Parsing : Transforming your command into an executable

From now on, it's gonna be shell's work.
So, the command you've typed in the CLI is basically a string. 

The thing is, a program doesn't executes in the way we've typed, so we need to adapt (parse) the input so we can make it as close as an executable.



