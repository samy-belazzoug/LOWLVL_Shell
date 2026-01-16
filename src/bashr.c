#include "header/bashr.h"
#include "header/my_parsing.h"

int main() {   
    
    while (1) {
        
        // ACT 1, GET THE INPUTS
        
        char cwd[FILENAME_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {

        } 
        else {
            printf("pwd not worked, well, thats weird.\n");
        }

        char cmd[N_TTY_BUF_SIZE]; //Reminder : 4096 size buffer
        printf("%s > ",cwd); 
        fgets(cmd,sizeof(cmd),stdin); //Always leave /n at the end of the word, we need to remove it!
        
        // ACT 2, CLEAN THE INPUTS
        size_t len = strlen(cmd);
            if (len > 0 && cmd[len - 1] == '\n') {
            cmd[len - 1] = '\0';
        }

        // ACT 3, PARSE THE INPUTS
        char **argv = parsing(cmd);

        for (int i = 0; argv[i] != NULL;i++) {
            printf("%s\n",argv[i]);
        }

        // ACT 4, BUILTINS
        
        // exit
        if (strcmp(argv[0],"exit") == 0) { 
            puts("You exited the program");
            exit(EXIT_SUCCESS);
        }
    
        // env
        else if (strcmp(argv[0],"env") == 0) { 
            for (int i = 0; __environ[i] != NULL; i++) {
                printf("%s\n",__environ[i]);
            }
        }

        //pwd
        else if (strcmp(argv[0],"pwd") == 0) {
            printf("%s\n",cwd);
        }

        // cd
        else if (strcmp(argv[0],"cd") == 0) { 
            char *path = argv[1];
            // cd ~ AND cd
            if (argv[1] == NULL || strcmp(argv[1],"~") == 0) {
                path = getenv("HOME");
            }
            // cd ..
            else if (strcmp(argv[1], "..") == 0) {
                path = "..";
            }

            // cd pathxyz
            else {
                path = argv[1];
            }

            // We try to change directory
            if (chdir(path) != 0) {
                printf("Couldn't find path : %s\n",path);
            }
        }

        
        // ACT 5, EXECUTABLES
    }
        
    exit(EXIT_SUCCESS);   
}
        
        

     