#include "split.h"

#include <string.h>

void split(const char* command, char** argv) {
    char* command_copy = strdup(command);
    char* token = strtok(command_copy, " ");
    int i = 0;
    while (token != NULL) {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;
}
