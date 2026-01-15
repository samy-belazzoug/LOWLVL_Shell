#include <stdio.h>
#include <stdlib.h>

void length(char *str);

char **parsing(char *str);

int main() {
    length("ls");
    length("   ls");                   
    length("ls   ");                 
    length("   ls   ");              
    length("   ls   -a   ");         
    length("               ");       

    /*Warning, in the case of :
    "              ls   "
    "ls               "
    There should be one word, however, it returns 3 for the first case
    and 2 for the second. */
    exit(EXIT_SUCCESS);
}

void length(char *str) {
    // Phase 1 : Count the words
    int start = 0, last_index = 0,  word_count = 0, space_counter = 0, length;
    
    while (str[start] == 32) start++; //Skip the first spaces, we want to begin our loop with a caracter.

    for (int i = start; str[i] != '\0';i++) { //Loops through the str until the last caracter
        // We must look for space
        if (str[i] == 32) { // The current caracter is a space ' '
            space_counter += 1;
            if (str[i + 1] == 32) { //If the next caracter is also a space (multiple space case)
                //Do nothing because there is multiple space
            }
            //The next caracter isn't a space
            else if (i > start) { //If i > start, it means there is a word. If str[start] == 32, it means there was multiples spaces before the caracter!
                for (int j = start; j < i; j++) {
                    printf("%c",str[j]);
                }
                word_count += 1;
                start = i + 1;
            }
            
        }
        last_index = i;
        //printf("%d ",i);
    }
    length = last_index + 1; //Length stopped at the last caracter, not the entire string!
    //printf("\nLast element %d\nSpace counter : %d\n",str[length + 1],space_counter);
    
    if (space_counter == length) { //Get a reliable result (idk why length is always +1 ahead of space_counter)
        word_count = 0;
    }

    if (length > start && str[start] != 32) { // Studying the very last word
        word_count += 1;
    }
    //Now we must check for the last caracter
    printf("\nSTRING : %s\nWORDS : %d\nLENGTH = %d\nSPACE COUNTER = %d\n\n",str,word_count,length,space_counter);
}

char **parsing(char *str) {
    // Phase 1 : count the words
    int length = 0;
    int actual = 0;
    int word_count = 0;
    while (str[length] != '\0') {
        if (str[length] == ' ') {
            if (length - actual == 0) { // If its still a SPACE
                actual = length;
            }
            else {
                actual = length;
                word_count += 1;
            }
        }
        length += 1;
    }

    char **argv;
    



    return argv;
}