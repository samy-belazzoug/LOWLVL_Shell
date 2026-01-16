#ifndef BASHR_H
#define BASH_H

#define N_TTY_BUF_SIZE 4096

#include <stdio.h> //debbugging
#include <stdlib.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> //fork
#include <sys/types.h> //pid_t instead of __pid_t
#include <sys/wait.h> //You can read it
#include <string.h> //strcmp

int length(char *str); //returns the length of a string

int word_counter(char *str); // Count how many words there is in a string

char **parsing(char *str); //Return a list of str's words (specifically formatted to work for execve)

#endif