#ifndef BASHR_H
#define BASHR_H

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

/* Parser functions */
Command* parse_command(char *input);
void free_command(Command *cmd);
char** tokenize(char *input, const char *delim);
void free_tokens(char **tokens);

/* Executor functions */
int execute_command(Command *cmd);
int execute_pipe(char *input);
int execute_redirect(Command *cmd);

/* Built-in commands */
int builtin_cd(char **args);
int builtin_pwd(char **args);
int builtin_exit(char **args);
int builtin_echo(char **args);
int builtin_env(char **args);
int builtin_export(char **args);
int builtin_jobs(void);
int is_builtin(const char *cmd);

/* Job control */
void add_job(pid_t pid, const char *cmd);
void remove_job(pid_t pid);
void print_jobs(void);
void reap_zombies(void);

/* Utility functions */
char* find_in_path(const char *cmd);
void trim_whitespace(char *str);
int contains_char(const char *str, char c);

#endif
