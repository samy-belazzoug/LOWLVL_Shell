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

***AND NOTE THAT THIS IS WHAT WE HAVE TO DEVELOP***

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
https://www.delftstack.com/howto/c/wait-in-c/
https://linuxvox.com/blog/why-should-we-check-wifexited-after-wait-in-order-to-kill-child-processes-in-linux-system-call/
https://www.allinthedifference.com/difference-between-fgets-and-scanf/
https://serverfault.com/questions/163371/linux-command-line-character-limit
https://unix.stackexchange.com/questions/643777/is-there-any-limit-on-line-length-when-pasting-to-a-terminal-in-linuxhttps://stackoverflow.com/questions/69198577/linux-keyboard-terminal-kernel-input-buffer
https://www.geeksforgeeks.org/c/buffer-in-c-programming/
man env
man execve
man fork
man wait
man perror
man fgets()

#### Warning : Please focus on this part, its gonna be pretty technical.

Let's say you want to do **ls -a**.

There is basically 3 main steps for the shell to return what we want (in this case, the list of all the content in a directory) :

-Getting user input
-Parsing
-Execution

### Input / Output : Getting your command and return something

When you enter the command line interface (bash) it invites you to type a command.

What I mean by that is that the CLI doesn't perform any operation when it launches, it waits for your demand.

The CLI in reality is "just" a **while loop** with strings as **input/output**. It reads your input, send it to the shell and returns what the shell did with your input.

As for the input, we will use a buffer of **4096** caracters (char s[4096]). We are forced to have a **finite** size because of fgets() that we'll see just after..

***Linux kernel terminal input buffer*** (N_TTY_BUF_SIZE) is defined as **4096**.
So we're following that limit, its a safe and efficient method.

We can read inputs with fgets. However fgets asks for multilple stuff : 
-**const** __*char__ **_restrict_ _s** : The string
-**int __n** : How many bytes we want to read, In our case, the size of the buffer **4096**
-**FILE** *____restrict__ **__stream** : From which file, in our case, the standard input **stdio** (yes, standard input is a file like everything).

**NOTE:** Linux kernel terminal input buffer (N_TTY_BUF_SIZE) has a limit of **4096** caracters.
So we're following that limit, its a safe and efficient method.

From these informations, we should get a base :

```c
#include <stdio.h>

int main() {
    while (1) { //Infinite loop
        char s[4096]; //Buffer
        printf("Input : ");
        fgets(s,sizeof(s),stdin); //User types ls -a
        
        /*SHELL'S WORK
        parsing
        execution
        whatever*/
    }
    return 0;
}
```

So, as soon as you've sent the command, it's gonna be shell's work.

## Execution : The business

I know you're gonna tell me : Doesn't we have to parse before the execution? So why do you talk about the execution before the parsing, are you okay?

Well, let me tell you something.
Before talking about any shell's actions, we need to learn about exactly 2 very important functions : execve() and fork()

#### execve()
 
```c
#include <libsync.h>
int execve(const char *pathname, char *const _Nullable argv[], char *const _Nullable envp[])
```

Let's break-down this function :

**What it does :** 
*"executes  the  program  referred  to  by  pathname.  This causes the program that is currently being run by the calling
process to be replaced with a new program, with newly initialized stack, heap, and (initialized and uninitialized) data segments."*

Bacically, it replaces and executes the current program being run by the program mentionned by pathname

**Parameters :**
-__const char *pathname__ : Program to execute (e.g : ls)

-__char *cons_Nullable argv[]__ : Arguments following the program (e.g : -a, -l, -o etc..)

-__char *const_Nullable envp[]__ : String array of environment variables (execve will 
try to find pathname in all the environment variables in envp[])
*Generally speaking, you'll pass **__environ** as envp[] parameter as its simpler and faster. __environ contains every environment variables of the shell.*

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
Let's break-down this function :

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

#### Child processes

Question now, can the child process operate before its parent process end ?

That's a good question, because, its good (and cool) to have a child process, but if it can only executes after the parent... Its a bit limited isn't it.

The answer for this is : **yes**, we can do it with wait() function, but be **careful** :

```c
#include <stdio.h> //printf, puts
#include <stdlib.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> //fork 
#include <sys/types.h> //pid_t instead of __pid_t
#include <sys/wait.h>

int main(void) {
    pid_t pid;
    pid = fork();
    switch (pid) {
        case -1: // Child creation process got an error 
            perror("fork");
            exit(EXIT_FAILURE); //Equivalent of exit(1)
        case 0: // Child is created. We are in child's process    
            printf("Currently child process with PID : %d\n",getpid());
            sleep(2);
            exit(EXIT_SUCCESS); //Equivalent of exit(0)
       default: // We are in parent's process
            puts("Actually parent process");
            int status;
            pid_t terminated_pid = wait(&status);
            
            if (terminated_pid == -1) { //Ensuring that any issues with waiting for the child process are handled nicely.
                perror("wait failed");
                return 1;
            }
            
            if (WIFEXITED(status)) { // Checks if the child terminated normally (e.g : with exit() or returning from main)
                /*
                status value from wait() is a bitwise structure that
                varies by Unix like system, WIFEXITED is a macro that can safely interprets it.
                This macro is important to avoid misunderstanding of status!*/
                printf("Child exited with status: %d\n", WEXITSTATUS(status));
            }
            printf("Parent process back with PID : %d\n",getpid());
    }
    puts("Now the program is gone.");
    puts("Parent exiting.");
    exit(EXIT_SUCCESS);
}
```

When a child process **terminates**, it doesn't disappear, in fact, it enters in a **"zombie" state** where its process descriptor **remains in the kernel** until the parent acknowledge its death via 'wait()' or 'waitpid()'.
This must happen because the child **may** return **critical information** to the **parent**, such as why it has **terminated** and its **exit code**.
To do so, we must check **wait()** returning status. The thing is, status **may vary** by UNIX-like system, to safely interprets it, we must use the **WIFEXITED** macro.

*Yes, it's strict but mate, don't forget in C **you have control over almost anything**, so you must code in a pretty safely way. If you can't have control over errors and unexpected behaviour, in fact, you don't have control over anything, and thats pretty bad.*

### Linking the bits together

So, to conclude on this kinda technical part, we'll go with a simple analogy to tell you technically why execve() **AND** fork() have to work together (for this project).

Say you are a senior dev and you encounter a bug you don't want to work on. Fortunately, you have a junior dev that would be very happy to debug it.

execve() replaces the current program by the *pathname. So, if in our infinite loop we call execve, it will completely stop the loop and the entire program and start *pathname.

Here, you bring the junior dev to you, explains to him the problem and obviously you tell him to fix it.
The thing is, the junior now will litterally take your place, code in your pc and you will stop working, even when the junior fix the bug, your day is finished.

We can avoid that by **isolating** execve() call in a **copy** of the **current process**, which is made with fork()!

To not stop your day and let your student take over your PC, you will delegate him the exact code (with github or whatever) to another PC (**fork()**) and he will try to fix it. The thing is, you have to wait until he fixed the bug, you can't do anything (**wait()**). 

After fork has done its job, you have to know if child terminated without error or not and do something about it .

When the junior finished his work, he has to give you back, BUT he can not give you back also, so you must checks if he managed to fix it or not (**WIFEXITED()**) and do the right actions for it like tell him what was wrong, tell him something good because he manages it or not talking, tell boss to fire the junior because he did a stack overflow?

## Parsing : Transforming your command into an executable

From now on, it's gonna be shell's work.
So, the command you've typed in the CLI is basically a string. 

The thing is, a program doesn't executes in the way we've typed, so we need to adapt (parse) the input so we can make it as close as an executable.