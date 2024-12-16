#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "redirect.h"
#include "split.h"
#include "executor.h"

#define MAX_ARGS 100
#define MAX_INPUT 1024

void handle_pipes_and_redirection(char *command) {
    char *input_file = NULL;
    char *output_file = NULL;
    int bg = 0;

    if (command[strlen(command) - 1] == '&') {
        bg = 1;
        command[strlen(command) - 1] = '\0';
    }

    char *input_redirect = strchr(command, '<');
    if (input_redirect) {
        *input_redirect = '\0';
        input_file = strtok(input_redirect + 1, " ");
    }

    char *output_redirect = strchr(command, '>');
    if (output_redirect) {
        *output_redirect = '\0';
        output_file = strtok(output_redirect + 1, " ");
    }

    char *commands[MAX_ARGS];
    int num_commands = 0;
    char *pipe_token = strtok(command, "|");
    while (pipe_token) {
        commands[num_commands++] = pipe_token;
        pipe_token = strtok(NULL, "|");
    }

    int input_fd = 0;
    int output_fd = 1; 
    for (int i = 0; i < num_commands; i++) {
        char *args[MAX_ARGS];
        char cmd_copy[MAX_INPUT];
        strcpy(cmd_copy, commands[i]);

        split(cmd_copy, args);

        if (i == 0 && input_file) {
            input_fd = open(input_file, O_RDONLY);
            if (input_fd < 0) {
                perror("xsh: Cannot open input file");
                return;
            }
        }

        if (i == num_commands - 1 && output_file) {
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0) {
                perror("xsh: Cannot open output file");
                return;
            }
        }

        if (i == num_commands - 1) {
            execute_external(args, bg, input_fd, output_fd);
        } else {
            int pipefd[2];
            pipe(pipefd);
            execute_external(args, bg, input_fd, pipefd[1]);
            close(pipefd[1]);
            input_fd = pipefd[0];
        }
    }

    if (input_fd != 0) {
        close(input_fd);
    }

    if (output_fd != 1) {
        close(output_fd);
    }
}
