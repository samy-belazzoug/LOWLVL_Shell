#include "bashr.h"
#include <errno.h>

extern char **environ;

int execute_redirect(Command *cmd) {
    if (!cmd || cmd->argc == 0) return 1;
    
    int in_fd = -1, out_fd = -1, append = 0;
    int redirect_in = -1, redirect_out = -1;
    
    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], ">") == 0) {
            redirect_out = i;
            append = 0;
            break;
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            redirect_out = i;
            append = 1;
            break;
        } else if (strcmp(cmd->argv[i], "<") == 0) {
            redirect_in = i;
            break;
        }
    }
    
    if (redirect_in != -1) {
        if (redirect_in + 1 >= cmd->argc) {
            fprintf(stderr, "syntax error: missing filename\n");
            return 1;
        }
        in_fd = open(cmd->argv[redirect_in + 1], O_RDONLY);
        if (in_fd < 0) {
            perror("open");
            return 1;
        }
    }
    
    if (redirect_out != -1) {
        if (redirect_out + 1 >= cmd->argc) {
            fprintf(stderr, "syntax error: missing filename\n");
            if (in_fd >= 0) close(in_fd);
            return 1;
        }
        int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
        out_fd = open(cmd->argv[redirect_out + 1], flags, 0644);
        if (out_fd < 0) {
            perror("open");
            if (in_fd >= 0) close(in_fd);
            return 1;
        }
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        if (in_fd >= 0) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_fd >= 0) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        
        int argc = cmd->argc;
        if (redirect_in != -1) argc = redirect_in;
        if (redirect_out != -1 && (redirect_in == -1 || redirect_out < redirect_in)) {
            argc = redirect_out;
        }
        
        cmd->argv[argc] = NULL;
        char *path = find_in_path(cmd->argv[0]);
        if (!path) {
            fprintf(stderr, "command not found: %s\n", cmd->argv[0]);
            exit(127);
        }
        execve(path, cmd->argv, environ);
        perror("execve");
        exit(127);
    } else if (pid > 0) {
        if (in_fd >= 0) close(in_fd);
        if (out_fd >= 0) close(out_fd);
        
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    } else {
        perror("fork");
        if (in_fd >= 0) close(in_fd);
        if (out_fd >= 0) close(out_fd);
        return 1;
    }
}

int execute_pipe(char *input) {
    if (!input || !contains_char(input, '|')) {
        return 0;
    }
    
    char **cmds = tokenize(input, "|");
    if (!cmds || !cmds[0]) {
        free_tokens(cmds);
        return 1;
    }
    
    int cmd_count = 0;
    while (cmds[cmd_count]) cmd_count++;
    
    int *pipes = malloc((cmd_count - 1) * 2 * sizeof(int));
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipes + i * 2) < 0) {
            perror("pipe");
            free(pipes);
            free_tokens(cmds);
            return 1;
        }
    }
    
    for (int i = 0; i < cmd_count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < cmd_count - 1) {
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < cmd_count - 1; j++) {
                close(pipes[j * 2]);
                close(pipes[j * 2 + 1]);
            }
            
            Command *cmd = parse_command(cmds[i]);
            if (!cmd || cmd->argc == 0) {
                fprintf(stderr, "syntax error\n");
                exit(1);
            }
            
            char *path = find_in_path(cmd->argv[0]);
            if (!path) {
                fprintf(stderr, "command not found: %s\n", cmd->argv[0]);
                exit(127);
            }
            execve(path, cmd->argv, environ);
            perror("execve");
            exit(127);
        } else if (pid < 0) {
            perror("fork");
            free(pipes);
            free_tokens(cmds);
            return 1;
        }
    }
    
    for (int i = 0; i < cmd_count - 1; i++) {
        close(pipes[i * 2]);
        close(pipes[i * 2 + 1]);
    }
    
    int status;
    for (int i = 0; i < cmd_count; i++) {
        wait(&status);
    }
    
    free(pipes);
    free_tokens(cmds);
    return 0;
}

int execute_command(Command *cmd) {
    if (!cmd || cmd->argc == 0) return 1;
    
    if (is_builtin(cmd->argv[0])) {
        if (strcmp(cmd->argv[0], "cd") == 0) return builtin_cd(cmd->argv);
        if (strcmp(cmd->argv[0], "pwd") == 0) return builtin_pwd(cmd->argv);
        if (strcmp(cmd->argv[0], "exit") == 0) return builtin_exit(cmd->argv);
        if (strcmp(cmd->argv[0], "echo") == 0) return builtin_echo(cmd->argv);
        if (strcmp(cmd->argv[0], "env") == 0) return builtin_env(cmd->argv);
        if (strcmp(cmd->argv[0], "export") == 0) return builtin_export(cmd->argv);
        if (strcmp(cmd->argv[0], "jobs") == 0) return builtin_jobs();
    }
    
    int background = 0;
    if (cmd->argc > 1 && strcmp(cmd->argv[cmd->argc - 1], "&") == 0) {
        background = 1;
        cmd->argv[--cmd->argc] = NULL;
    }
    
    char *path = find_in_path(cmd->argv[0]);
    if (!path) {
        fprintf(stderr, "bashr: command not found: %s\n", cmd->argv[0]);
        return 127;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGCHLD, SIG_DFL);
        execve(path, cmd->argv, environ);
        perror("execve");
        exit(127);
    } else if (pid > 0) {
        free(path);
        if (background) {
            add_job(pid, cmd->argv[0]);
            printf("[1]+ %d\n", pid);
            return 0;
        } else {
            int status;
            waitpid(pid, &status, 0);
            return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
        }
    } else {
        perror("fork");
        free(path);
        return 1;
    }
}
