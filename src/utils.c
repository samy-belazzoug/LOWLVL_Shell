#include "bashr.h"

static Job jobs[MAX_JOBS];
static int job_count = 0;

void trim_whitespace(char *str) {
    if (!str) return;
    int i = 0, j = strlen(str) - 1;
    while (i <= j && (str[i] == ' ' || str[i] == '\t')) i++;
    while (j >= i && (str[j] == ' ' || str[j] == '\t')) j--;
    
    // Copy trimmed portion using strcpy logic
    int len = j - i + 1;
    for (int k = 0; k < len; k++) {
        str[k] = str[i + k];
    }
    str[len] = '\0';
}

int contains_char(const char *str, char c) {
    if (!str) return 0;
    return strchr(str, c) != NULL;
}

char** tokenize(char *input, const char *delim) {
    if (!input || !delim) return NULL;
    
    // Allocate and copy input
    char *copy = malloc(strlen(input) + 1);
    strcpy(copy, input);
    
    char **tokens = malloc(MAX_ARGS * sizeof(char*));
    int count = 0;
    
    char *token = strtok(copy, delim);
    while (token && count < MAX_ARGS - 1) {
        trim_whitespace(token);
        if (strlen(token) > 0) {
            // Allocate and copy token
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
    for (int i = 0; tokens[i]; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

Command* parse_command(char *input) {
    if (!input) return NULL;
    
    Command *cmd = malloc(sizeof(Command));
    
    // Copy input instead of strdup
    char *copy = malloc(strlen(input) + 1);
    strcpy(copy, input);
    trim_whitespace(copy);
    
    char **tokens = tokenize(copy, " \t");
    int i = 0;
    while (tokens && tokens[i] && i < MAX_ARGS - 1) {
        cmd->argv[i] = tokens[i];
        i++;
    }
    cmd->argv[i] = NULL;
    cmd->argc = i;
    
    free(copy);
    free(tokens);
    return cmd;
}

void free_command(Command *cmd) {
    if (!cmd) return;
    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
    free(cmd);
}

char* find_in_path(const char *cmd) {
    if (!cmd) return NULL;
    
    // If path contains '/', check if executable
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
    
    // Copy PATH to modify
    char *path = malloc(strlen(path_env) + 1);
    strcpy(path, path_env);
    
    static char full_path[1024];
    char *save_ptr = path;
    
    // Manual tokenization using strchr
    while (*save_ptr) {
        // Find end of directory (: or \0)
        char *colon = strchr(save_ptr, ':');
        size_t dir_len = colon ? (size_t)(colon - save_ptr) : strlen(save_ptr);
        
        // Build full path
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

void add_job(pid_t pid, const char *cmd) {
    if (job_count >= MAX_JOBS) return;
    jobs[job_count].id = job_count + 1;
    jobs[job_count].pid = pid;
    jobs[job_count].status = 1;
    strncpy(jobs[job_count].cmd, cmd, MAX_CMD_LEN - 1);
    job_count++;
}

void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
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
