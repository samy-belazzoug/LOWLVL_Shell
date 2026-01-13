# HOW CAN WE MAKE A SHELL ?!
This document will reverse engineering UNIX's shell for understanding purpose.

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

A command-line interface (CLI) is an interface paradigm mainly using text-based way to communicate with a computer's operating system **OR** software through text-based commands. It's the interacting section.
Every inputs/output you see are displayed by the CLI.

E.G : Docker, AWS CLI, PowerShell

### What is a Shell ?

The Shell is a program that acts as an intermediary between the user and the kernel. Basically, it interprets commands and communicates with the kernel to execute user's commands. It's the interpreting and executing section.
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
man env
man execve
man fork

Let's say you want to do **ls -a** or **dir /a**.

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
 
##### int execve(const char *pathname, char *const _Nullable argv[], char *const _Nullable envp[])

Let's decompose this function :
**What it does :** 
*"executes  the  program  referred  to  by  pathname.  This causes the program that is currently being run by the calling
process to be replaced with a new program, with newly initialized stack, heap, and (initialized and uninitialized) data segments."*

Bacically, it replaces and executes the current program being run by the program mentionned by pathname

**Parameters :**
const char *pathname : Program to execute (e.g : ls)
char *cons_Nullable argv[] : Arguments following the program (e.g : -a, -l, -o etc..)
char *const_Nullable envp[] : 

#### fork()

## Parsing : Transforming your command into an executable

From now on, it's gonna be shell's work.
So, the command you've typed in the CLI is basically a string. 

The thing is, a program doesn't executes in the way we've typed, so we need to adapt (parse) the input so we can make it as close as an executable.



