#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64
#define MAX_JOBS 32

typedef struct {
    int argc;
    char *argv[MAX_ARGS];
} Command;

typedef struct {
    int id;
    pid_t pid;
    char cmd[MAX_CMD_LEN];
    int status;
} Job;

extern char **environ;
static Job jobs[MAX_JOBS];
static int job_count = 0;

/* ============ UTILITY FUNCTIONS ============ */

void trim_whitespace(char *str) {
    if (!str) return;
    int i = 0, j = strlen(str) - 1;
    while (i <= j && (str[i] == ' ' || str[i] == '\t')) i++;
    while (j >= i && (str[j] == ' ' || str[j] == '\t')) j--;
    int len = j - i + 1;
    for (int k = 0; k < len; k++) str[k] = str[i + k];
    str[len] = '\0';
}

int contains_char(const char *str, char c) {
    return str && strchr(str, c) != NULL;
}

char** tokenize(char *input, const char *delim) {
    if (!input || !delim) return NULL;
    char *copy = malloc(strlen(input) + 1);
    strcpy(copy, input);
    char **tokens = malloc(MAX_ARGS * sizeof(char*));
    int count = 0;
    char *token = strtok(copy, delim);
    while (token && count < MAX_ARGS - 1) {
        trim_whitespace(token);
        if (strlen(token) > 0) {
            tokens[count] = malloc(strlen(token) + 1);
            strcpy(tokens[count], token);
            count++;
        }
        token = strtok(NULL, delim);
    }
    tokens[count] = NULL;
    free(copy);
    return tokens;
}

void free_tokens(char **tokens) {
    if (!tokens) return;
    for (int i = 0; tokens[i]; i++) free(tokens[i]);
    free(tokens);
}

void free_command(Command *cmd) {
    if (!cmd) return;
    for (int i = 0; i < cmd->argc; i++) free(cmd->argv[i]);
    free(cmd);
}

char* find_in_path(const char *cmd) {
    if (!cmd) return NULL;
    if (strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) {
            static char result[1024];
            strcpy(result, cmd);
            return result;
        }
        return NULL;
    }
    char *path_env = getenv("PATH");
    if (!path_env) return NULL;
    char *path = malloc(strlen(path_env) + 1);
    strcpy(path, path_env);
    static char full_path[1024];
    char *save_ptr = path;
    while (*save_ptr) {
        char *colon = strchr(save_ptr, ':');
        size_t dir_len = colon ? (size_t)(colon - save_ptr) : strlen(save_ptr);
        strncpy(full_path, save_ptr, dir_len);
        full_path[dir_len] = '/';
        strcpy(full_path + dir_len + 1, cmd);
        if (access(full_path, X_OK) == 0) {
            free(path);
            return full_path;
        }
        if (!colon) break;
        save_ptr = colon + 1;
    }
    free(path);
    return NULL;
}

/* ============ JOB CONTROL ============ */

void add_job(pid_t pid, const char *cmd) {
    if (job_count >= MAX_JOBS) return;
    jobs[job_count].id = job_count + 1;
    jobs[job_count].pid = pid;
    strncpy(jobs[job_count].cmd, cmd, MAX_CMD_LEN - 1);
    jobs[job_count].cmd[MAX_CMD_LEN - 1] = '\0';
    jobs[job_count].status = 1;
    job_count++;
}

void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) jobs[j] = jobs[j + 1];
            job_count--;
            return;
        }
    }
}

void print_jobs(void) {
    for (int i = 0; i < job_count; i++) {
        printf("[%d]+ %d Running %s\n", jobs[i].id, jobs[i].pid, jobs[i].cmd);
    }
}

void reap_zombies(void) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        remove_job(pid);
    }
}

/* ============ BUILT-IN COMMANDS ============ */

int builtin_cd(char **args) {
    if (!args || !args[0]) {
        printf("cd: missing argument\n");
        return 1;
    }
    if (chdir(args[1] ? args[1] : getenv("HOME")) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

int builtin_pwd(char **args) {
    (void)args;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("pwd");
        return 1;
    }
    printf("%s\n", cwd);
    return 0;
}

int builtin_exit(char **args) {
    int code = 0;
    if (args && args[1]) {
        code = 0;
        for (int i = 0; args[1][i]; i++) {
            if (args[1][i] >= '0' && args[1][i] <= '9') {
                code = code * 10 + (args[1][i] - '0');
            }
        }
    }
    exit(code);
    return 0;
}

int builtin_echo(char **args) {
    if (!args) {
        printf("\n");
        return 0;
    }
    for (int i = 1; args[i]; i++) {
        printf("%s", args[i]);
        if (args[i + 1]) printf(" ");
    }
    printf("\n");
    return 0;
}

int builtin_env(char **args) {
    (void)args;
    for (int i = 0; environ[i]; i++) {
        printf("%s\n", environ[i]);
    }
    return 0;
}

int builtin_export(char **args) {
    if (!args || !args[1]) {
        printf("export: missing argument\n");
        return 1;
    }
    char *var = args[1];
    char *eq = strchr(var, '=');
    if (!eq) {
        printf("export: invalid format (use VAR=value)\n");
        return 1;
    }
    if (setenv(var, eq + 1, 1) != 0) {
        perror("export");
        return 1;
    }
    return 0;
}

int builtin_jobs(void) {
    print_jobs();
    return 0;
}

int is_builtin(const char *cmd) {
    const char *builtins[] = {
        "cd", "pwd", "exit", "echo", "env", "export", "jobs", NULL
    };
    for (int i = 0; builtins[i]; i++) {
        if (strcmp(cmd, builtins[i]) == 0) return 1;
    }
    return 0;
}

/* ============ EXECUTION ENGINE ============ */

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
            printf("syntax error: missing filename\n");
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
            printf("syntax error: missing filename\n");
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
            printf("command not found: %s\n", cmd->argv[0]);
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
    if (!input || !contains_char(input, '|')) return 0;
    char **cmds = tokenize(input, "|");
    int cmd_count = 0;
    while (cmds && cmds[cmd_count]) cmd_count++;
    int *pipes = malloc((cmd_count - 1) * 2 * sizeof(int));
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipes + i * 2) == -1) {
            perror("pipe");
            free(pipes);
            free_tokens(cmds);
            return 1;
        }
    }
    for (int i = 0; i < cmd_count; i++) {
        char **tokens = tokenize(cmds[i], " \t");
        Command *cmd = malloc(sizeof(Command));
        cmd->argc = 0;
        for (int j = 0; tokens && tokens[j]; j++) {
            cmd->argv[cmd->argc++] = tokens[j];
        }
        if (!cmd->argv[0]) {
            printf("syntax error\n");
            exit(1);
        }
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }
            if (i < cmd_count - 1) {
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }
            for (int j = 0; j < (cmd_count - 1) * 2; j++) {
                close(pipes[j]);
            }
            char *path = find_in_path(cmd->argv[0]);
            if (!path) {
                printf("command not found: %s\n", cmd->argv[0]);
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
        free(cmd);
    }
    for (int i = 0; i < (cmd_count - 1) * 2; i++) close(pipes[i]);
    free(pipes);
    for (int i = 0; i < cmd_count; i++) waitpid(-1, NULL, 0);
    free_tokens(cmds);
    return 0;
}

int execute_command(Command *cmd) {
    if (!cmd || cmd->argc == 0) return 1;
    if (strcmp(cmd->argv[0], "cd") == 0) return builtin_cd(cmd->argv);
    if (strcmp(cmd->argv[0], "pwd") == 0) return builtin_pwd(cmd->argv);
    if (strcmp(cmd->argv[0], "exit") == 0) return builtin_exit(cmd->argv);
    if (strcmp(cmd->argv[0], "echo") == 0) return builtin_echo(cmd->argv);
    if (strcmp(cmd->argv[0], "env") == 0) return builtin_env(cmd->argv);
    if (strcmp(cmd->argv[0], "export") == 0) return builtin_export(cmd->argv);
    if (strcmp(cmd->argv[0], "jobs") == 0) return builtin_jobs();
    if (cmd->argc > 1 && strcmp(cmd->argv[cmd->argc - 1], "&") == 0) {
        cmd->argv[cmd->argc - 1] = NULL;
        cmd->argc--;
        pid_t pid = fork();
        if (pid == 0) {
            char *path = find_in_path(cmd->argv[0]);
            if (!path) {
                printf("bashr: command not found: %s\n", cmd->argv[0]);
                exit(127);
            }
            execve(path, cmd->argv, environ);
            perror("execve");
            exit(127);
        } else if (pid > 0) {
            add_job(pid, cmd->argv[0]);
            printf("[1]+ %d\n", pid);
        } else {
            perror("fork");
            return 1;
        }
        return 0;
    }
    char *path = find_in_path(cmd->argv[0]);
    if (!path) {
        printf("bashr: command not found: %s\n", cmd->argv[0]);
        return 127;
    }
    pid_t pid = fork();
    if (pid == 0) {
        execve(path, cmd->argv, environ);
        perror("execve");
        exit(127);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    } else {
        perror("fork");
        return 1;
    }
}

Command* parse_command(char *input) {
    if (!input) return NULL;
    Command *cmd = malloc(sizeof(Command));
    char *copy = malloc(strlen(input) + 1);
    strcpy(copy, input);
    trim_whitespace(copy);
    char **tokens = tokenize(copy, " \t");
    int i = 0;
    while (tokens && tokens[i] && i < MAX_ARGS - 1) {
        cmd->argv[i] = tokens[i];
        i++;
    }
    cmd->argc = i;
    cmd->argv[i] = NULL;
    free(copy);
    free(tokens);
    return cmd;
}

/* ============ MAIN REPL LOOP ============ */

void sigint_handler(int sig) {
    (void)sig;
    printf("\n");
}

int main(void) {
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, SIG_IGN);
    
    printf("=== LOWLVL Shell ===\n");
    printf("Type 'exit' to quit. Try 'cd', 'pwd', 'ls', pipes (|), redirection (>, >>)\n\n");
    
    char line[4096];
    while (1) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) strcpy(cwd, "?");
        printf("bashr:%s$ ", cwd);
        
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        int len = strlen(line);
        if (line[len - 1] == '\n') line[len - 1] = '\0';
        
        if (strlen(line) == 0) {
            printf("\n");
            continue;
        }
        
        if (contains_char(line, '|')) {
            execute_pipe(line);
        } else if (contains_char(line, '>') || contains_char(line, '<')) {
            Command *cmd = parse_command(line);
            if (cmd) {
                execute_redirect(cmd);
                free_command(cmd);
            }
        } else {
            Command *cmd = parse_command(line);
            if (cmd && cmd->argc > 0) {
                execute_command(cmd);
                free_command(cmd);
            }
        }
        reap_zombies();
    }
    return 0;
}
