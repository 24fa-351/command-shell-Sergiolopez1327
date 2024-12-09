#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "executor.h"
#include "redirect.h"
#include "split.h"

#define MAX_INPUT 1024

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

        // Convert input to lowercase to handle case-insensitivity for "exit"
        char lower_input[MAX_INPUT];
        strcpy(lower_input, input);
        for (int i = 0; lower_input[i]; i++) {
            lower_input[i] = tolower(lower_input[i]);
        }

        // Handle "exit" command
        if (strcmp(lower_input, "exit") == 0) {
            printf("Exiting xsh...\n");
            break;
        }

        // Handle other commands
        handle_pipes_and_redirection(input);
    }

    return 0;
}
