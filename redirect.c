#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "executor.h"
#include "split.h"

#define MAX_ARGS 100
#define MAX_INPUT 1024

void handle_pipes_and_redirection(char* command) {
    char* input_file = NULL;
    char* output_file = NULL;
    int bg = 0;

    if (command[strlen(command) - 1] == '&') {
        bg = 1;
        command[strlen(command) - 1] = '\0';
    }

    // Input redirection
    char* input_redirect = strchr(command, '<');
    if (input_redirect) {
        *input_redirect = '\0';
        input_file = strtok(input_redirect + 1, " \t");
        if (!input_file) {
            fprintf(stderr, "xsh: Missing input file for redirection\n");
            return;
        }
    }

    // Output redirection
    char* output_redirect = strchr(command, '>');
    if (output_redirect) {
        *output_redirect = '\0';
        output_file = strtok(output_redirect + 1, " \t");
        if (!output_file) {
            fprintf(stderr, "xsh: Missing output file for redirection\n");
            return;
        }
    }

    // Split commands by pipes
    char* commands[MAX_ARGS];
    int num_commands = 0;
    char* pipe_token = strtok(command, "|");
    while (pipe_token) {
        commands[num_commands++] = pipe_token;
        pipe_token = strtok(NULL, "|");
    }

    int input_fd = 0;
    if (input_file) {
        input_fd = open(input_file, O_RDONLY);
        if (input_fd < 0) {
            fprintf(stderr, "xsh: Cannot open input file '%s'\n", input_file);
            perror("xsh");
            return;
        }
    }

    for (int i = 0; i < num_commands; i++) {
        char* args[MAX_ARGS];
        split(commands[i], args);

        int pipefd[2];
        if (i < num_commands - 1) {
            if (pipe(pipefd) < 0) {
                perror("xsh: Pipe creation failed");
                return;
            }
        }

        int output_fd = 1;
        if (i == num_commands - 1 && output_file) {
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0) {
                fprintf(stderr, "xsh: Cannot open output file '%s'\n",
                        output_file);
                perror("xsh");
                return;
            }
        }

        // Debugging the arguments for grep
        printf("Debug: Executing command: '%s'\n", args[0]);

        execute_external(args, bg, input_fd,
                         (i < num_commands - 1) ? pipefd[1] : output_fd);

        if (i < num_commands - 1) {
            close(pipefd[1]);
            input_fd = pipefd[0];
        }

        if (output_fd != 1) {
            close(output_fd);
        }
    }

    if (input_fd > 0) {
        close(input_fd);
    }
}
