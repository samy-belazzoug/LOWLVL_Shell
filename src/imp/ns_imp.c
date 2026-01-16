#include "../header/nanoshell.h"

int length(char *str) { //Returns the length of a string
    int length = 0;
    for (int i = 0; str[i] != '\0'; i++) { //Note that we will stop to to last index, not the end of the string
        length += 1;
    }
    //printf("length = %d\n",length);
    return length + 1;
}

int word_counter(char *str) { // Count how many words there is in a string
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
                word_count += 1;
                start = i + 1;
            }
        }
        last_index = i;
    }
    length = last_index + 1; //Length stopped at the last caracter, not the entire string!
    //printf("LENGTH = %d\n",length);
    if (space_counter == length) { //Get a reliable result
        word_count = 0;
    }

    if (length > start && str[start] != 32) { // Studying the very last word
        word_count += 1;
    }
    //Now we must check for the last caracter
    //printf("\nSTRING : %s\nWORDS : %d\nLENGTH = %d\nSPACE COUNTER = %d\n\n",str,word_count,length,space_counter);
    return word_count;
}

char **parsing(char *str) {
    int words = word_counter(str);
    char **argv = malloc((words + 1) * sizeof(char *)); // +1 pour NULL
    
    int start = 0, argv_iterat = 0, len = length(str);

    while (str[start] == 32) start++; // Skip les premiers espaces

    for (int i = start; str[i] != '\0'; i++) {
        if (str[i] == 32 && i > start) { // On a trouvé un espace après un mot
            if (str[i + 1] != 32 && str[i + 1] != '\0') { // Le prochain n'est pas un espace
                int word_len = i - start;
                argv[argv_iterat] = malloc((word_len + 1) * sizeof(char));
                
                // Copie byte par byte
                for (int l = 0; l < word_len; l++) {
                    argv[argv_iterat][l] = str[start + l];
                }
                argv[argv_iterat][word_len] = '\0';
                
                argv_iterat++;
                start = i + 1;
            }
        }
    }
    
    // Dernier mot (si la chaîne ne se termine pas par un espace)
    if (len > start && str[start] != 32) {
        int word_len = len - start;
        argv[argv_iterat] = malloc((word_len + 1) * sizeof(char));
        
        for (int l = 0; l < word_len; l++) {
            argv[argv_iterat][l] = str[start + l];
        }
        argv[argv_iterat][word_len] = '\0';
        argv_iterat++;
    }
    
    argv[argv_iterat] = NULL; // Termine avec NULL pour execve
    
    // Debug
    for (int m = 0; m <= argv_iterat; m++) {
        //printf("argv[%d] = %s\n", m, argv[m]);
    }
    
    return argv;
}