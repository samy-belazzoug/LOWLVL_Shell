#ifndef PARSING_H
#define PARSING_H

#include <stdio.h>
#include <stdlib.h>

int length(char *str);

int word_counter(char *str); // Count how many words there is in a string

char **parsing(char *str); //Return a list of str's words (specifically formatted to work for execve)

#endif