#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "environment.h"

void execute_cd(char* path) {
    if (path == NULL) {
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
    }
    if (chdir(path) != 0) {
        perror("cd");
    }
}

void execute_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

void execute_echo(char* args[]) {
    for (int i = 1; args[i] != NULL; i++) {
        char* arg = args[i];
        if (arg[0] == '$') {  // Check if argument is an environment variable
            char* value = get_env_var(arg + 1);  // Skip the '$' character
            if (value) {
                printf("%s ", value);
            }
        } else {
            printf("%s ", arg);
        }
    }
    printf("\n");
}

int execute_builtin(char* args[]) {
    if (strcmp(args[0], "cd") == 0) {
        execute_cd(args[1]);
        return 1;
    } else if (strcmp(args[0], "pwd") == 0) {
        execute_pwd();
        return 1;
    } else if (strcmp(args[0], "set") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            set_env_var(args[1], args[2]);
        } else {
            fprintf(stderr, "set: usage: set VAR VALUE\n");
        }
        return 1;
    } else if (strcmp(args[0], "unset") == 0) {
        if (args[1] != NULL) {
            unset_env_var(args[1]);
        } else {
            fprintf(stderr, "unset: usage: unset VAR\n");
        }
        return 1;
    } else if (strcmp(args[0], "echo") == 0) {
        execute_echo(args);
        return 1;
    } else if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
        exit(0);
    }
    return 0;
}
