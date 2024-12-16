#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "executor.h"

extern char **environ;

char *abspath(const char *command) {
    if (strchr(command, '/') != NULL) {
        return strdup(command);
    }
    char *path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }
    char *path_copy = strdup(path);
    if (path_copy == NULL) {
        return NULL;
    }
    char *token = strtok(path_copy, ":");
    while (token != NULL) {
        size_t command_len = strlen(token) + strlen(command) + 2;
        char *command_path = malloc(command_len);
        if (command_path == NULL) {
            free(path_copy);
            return NULL;
        }
        snprintf(command_path, command_len, "%s/%s", token, command);
        if (access(command_path, X_OK) == 0) {
            free(path_copy);
            return command_path;
        }
        free(command_path);
        token = strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
}

void execute_external(char *args[], int bg, int input_fd, int output_fd) {
    pid_t pid = fork();
    if (pid == 0) {
        if (input_fd != 0) {
            dup2(input_fd, 0);
            close(input_fd);
        }
        if (output_fd != 1) {
            dup2(output_fd, 1);
            close(output_fd);
        }
        char *resolved_path = abspath(args[0]);
        if (resolved_path == NULL) {
            fprintf(stderr, "xsh: command not found: %s\n", args[0]);
            exit(1);
        }
        execve(resolved_path, args, environ);
        perror("xsh");
        free(resolved_path);
        exit(1);
    } else if (pid > 0) {
        if (!bg) {
            waitpid(pid, NULL, 0);
        } else {
            printf("[Background PID] %d\n", pid);
        }
    } else {
        perror("fork");
    }
}
