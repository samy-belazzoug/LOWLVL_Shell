#include <stdio.h> //printf, puts
#include <stdlib.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> //fork 
#include <sys/types.h> //pid_t instead of __pid_t
#include <sys/wait.h>

int main(void) {
    pid_t pid;
    pid = fork();
    puts("Actually parent process");
    switch (pid) {
        case -1: // Child creation process got an error 
            perror("fork");
            exit(EXIT_FAILURE);
        case 0: // Child is created. We are in child's process    
            printf("Currently child process with PID : %d\n",getpid());
            
            puts("Child exiting.");
            exit(EXIT_SUCCESS);
       default: // We are in parent's process
            int status;
            wait(&status);
            printf("Parent process back with PID : %d\n",getpid());
            puts("Parent exiting.");
            //exit(EXIT_SUCCESS);
    }
    puts("Now the program is gone.");
    exit(EXIT_SUCCESS);
}