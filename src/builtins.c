#include "bashr.h"

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
        // Manual string to int conversion
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
    extern char **environ;
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
    if (!cmd) return 0;
    
    static const char *builtins[] = {
        "cd", "pwd", "exit", "echo", "env", "export", "jobs", NULL
    };
    
    for (int i = 0; builtins[i]; i++) {
        if (strcmp(cmd, builtins[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
