#include <stdio.h> //printf, puts
#include <stdlib.h> //EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> //fork 
#include <sys/types.h> //pid_t instead of __pid_t
#include <sys/wait.h>

int main(void) {
    pid_t pid;
    pid = fork();
    switch (pid) {
        case -1: // Child creation process got an error 
            perror("fork");
            exit(EXIT_FAILURE); //Equivalent of exit(1)
        case 0: // Child is created. We are in child's process    
            printf("Currently child process with PID : %d\n",getpid());
            sleep(2);
            exit(EXIT_SUCCESS); //Equivalent of exit(0)
       default: // We are in parent's process
            puts("Actually parent process");
            int status;
            pid_t terminated_pid = wait(&status);
            
            if (terminated_pid == -1) { //
                perror("wait failed");
                return 1;
            }
            
            if (WIFEXITED(status)) { // Checks if the child terminated normally (e.g : with exit() or returning from main)
                /*
                status value from wait() is a bitwise structure that
                varies by Unix like system, WIFEXITED is a macro that can safely interprets it.
                This macro is important to avoid misunderstanding of status!*/
                printf("Child exited with status: %d\n", WEXITSTATUS(status));
            }
            printf("Parent process back with PID : %d\n",getpid());
    }
    puts("Now the program is gone.");
    puts("Parent exiting.");
    exit(EXIT_SUCCESS);
}