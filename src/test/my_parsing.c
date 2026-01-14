#include <stdio.h> //printf
#include <stdlib.h> //malloc
#include <string.h> //strncpy

char **parsing(char *str);

int main() {
    char *str = "ls -a -l  -l";
    char **strtab = parsing(str);

    for (int i = 0; i < 10; i++) {
        printf("%s, ", strtab[i]);
    }

    return 0;
}

char **parsing(char *str) {
    char **argv = malloc(100 * sizeof(char*));  // 100 strings array
    int word_count = 0;
    int start = 0;
    
    for (int i = 0; str[i] != '\0'; i++) { //Loop through str

        if (str[i] == ' ' || str[i + 1] == '\0') { // If the current caracter is either a space or the end of the string ('\0').
            //Conditions to extract a word
            int end = (str[i] == ' ') ? i : i + 1; //End of the word or not
            /*
            If the caracter is a space, then we don't modify it (if we do i + 1, he will point to the space) 
            else we can go further (the word isn't finished). 
            */
            int len = end - start; //Lenght of the word
            
            argv[word_count] = malloc((len + 1) * sizeof(char)); //Allocate enough memory for the actual word (we do len + 1 for '\0')
            strncpy(argv[word_count], &str[start], len); //Copy the caracter of string from start to len to argv[word_count]
            argv[word_count][len] = '\0'; //Set the last caracter of the current string to '\0' (null).
            
            word_count++; //We can go further in the array
            start = i + 1; //We can go further in the string
        }
    }
    argv[word_count] = NULL;  // We must have a NULL terminated list for argv
    return argv; //We return argv.
}