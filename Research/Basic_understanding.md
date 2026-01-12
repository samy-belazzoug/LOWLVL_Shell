# HOW CAN WE MADE A SHELL ?!
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

When you open a TERMINAL, it emulates COMMAND LINE INTERFACES, where you can interact with the computer with text-based commands. The SHELL, IN THE BACKGROUND, interprets, executes the commands you've typed in the CLI. 

Without shell, you are just writing text, and without a terminal, you can't write text.

**Terminal = Emulation, CLI = Interface, Shell = Execution**

## What happens when you execute a prompt in ?
