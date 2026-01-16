#include "header/nanoshell.h"

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
        else { 
            // WE SET PATHNAME BY ADDING /BIN/
            char pathname[strlen(argv[0]) + 6];
            strcpy(pathname, "/bin/");
            strcat(pathname, argv[0]);
            printf("PATHNAME : %s\n", pathname);

            // WE FORK SO EXECVE DOESN'T AFFECT THE LOOOP
            pid_t pid;
            pid = fork();
            
            switch (pid) {
                case -1: // Child creation process got an error 
            puts("fork failed!!");
            exit(EXIT_FAILURE); //Equivalent of exit(1)
            case 0: // Child is created. We are in child's process    
                printf("Currently child process with PID : %d\n",getpid());
                execve(pathname,argv,__environ);
                exit(EXIT_SUCCESS); //Equivalent of exit(0)
            default: // We are in parent's process
                puts("Actually parent process");
                int status;
                pid_t terminated_pid = wait(&status);
            
                if (terminated_pid == -1) { //
                    puts("wait failed");
                    return 1;
                
                if (WIFEXITED(status)) { // Checks if the child terminated normally (e.g : with exit() or returning from main)
                    /*
                    status value from wait() is a bitwise structure that
                    varies by Unix like system, WIFEXITED is a macro that can safely interprets it.
                    This macro is important to avoid misunderstanding of status!*/
                    printf("Child exited with status: %d\n", WEXITSTATUS(status));
                }
                printf("Parent process back with PID : %d\n",getpid());
                }
            }
        }
    }
        
    exit(EXIT_SUCCESS);   
}