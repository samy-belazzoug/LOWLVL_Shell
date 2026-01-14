#include "bashr.h"

static volatile int running = 1;

static void sigint_handler(int sig) {
    (void)sig;
    printf("\n");
}

static void sigchld_handler(int sig) {
    (void)sig;
    reap_zombies();
}

static void print_prompt(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "?");
    }
    printf("bashr:%s$ ", cwd);
    fflush(stdout);
}

static char* read_line(void) {
    static char line[MAX_CMD_LEN];
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return NULL;
    }
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }
    return line;
}

int main(void) {
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
    
    printf("=== LOWLVL Shell ===\n");
    printf("Type 'exit' to quit. Try 'cd', 'pwd', 'ls', pipes (|), redirection (>, >>)\n\n");
    
    while (running) {
        print_prompt();
        
        char *input = read_line();
        if (!input) {
            printf("\n");
            break;
        }
        
        trim_whitespace(input);
        if (strlen(input) == 0) {
            continue;
        }
        
        if (contains_char(input, '|')) {
            execute_pipe(input);
        } else if (contains_char(input, '>') || contains_char(input, '<')) {
            Command *cmd = parse_command(input);
            if (cmd) {
                execute_redirect(cmd);
                free_command(cmd);
            }
        } else {
            Command *cmd = parse_command(input);
            if (cmd) {
                execute_command(cmd);
                free_command(cmd);
            }
        }
    }
    
    return 0;
}