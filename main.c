#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environment.h"
#include "executor.h"
#include "redirect.h"
#include "split.h"

#define MAX_INPUT 1024

void handle_set_command(char* args[]) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "xsh: Usage: set <variable> <value>\n");
        return;
    }
    set_env_var(args[1], args[2]);
    printf("Set variable '%s' to '%s'.\n", args[1], args[2]);
}

void handle_unset_command(char* args[]) {
    if (args[1] == NULL) {
        fprintf(stderr, "xsh: Usage: unset <variable>\n");
        return;
    }
    unset_env_var(args[1]);
    printf("Unset variable '%s'.\n", args[1]);
}

int main() {
    char input[MAX_INPUT];

    while (1) {
        printf("xsh# ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';  // Remove newline character
        if (strlen(input) == 0) {
            continue;  // Skip empty input
        }

        // Substitute environment variables
        substitute_vars(input);

        char* args[MAX_INPUT];
        split(input, args);

        // Handle "exit" command
        if (strcmp(args[0], "exit") == 0) {
            printf("Exiting xsh...\n");
            break;
        }

        // Handle "set" command
        if (strcmp(args[0], "set") == 0) {
            handle_set_command(args);
            continue;
        }

        // Handle "unset" command
        if (strcmp(args[0], "unset") == 0) {
            handle_unset_command(args);
            continue;
        }

        // Handle other commands
        handle_pipes_and_redirection(input);
    }

    return 0;
}
