#include "environment.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EnvVar env_vars[MAX_VARS];
int env_count = 0;

char* get_env_var(const char* name) {
    for (int i = 0; i < env_count; i++) {
        if (strcmp(env_vars[i].name, name) == 0) {
            return env_vars[i].value;
        }
    }
    return NULL;
}

void set_env_var(const char* name, const char* value) {
    char* existing = get_env_var(name);
    if (existing) {
        strcpy(existing, value);
    } else {
        strcpy(env_vars[env_count].name, name);
        strcpy(env_vars[env_count].value, value);
        env_count++;
    }
}

void unset_env_var(const char* name) {
    for (int i = 0; i < env_count; i++) {
        if (strcmp(env_vars[i].name, name) == 0) {
            for (int j = i; j < env_count - 1; j++) {
                env_vars[j] = env_vars[j + 1];
            }
            env_count--;
            return;
        }
    }
}

void substitute_vars(char* command) {
    char buffer[MAX_INPUT];
    char* start = command;
    char *var_start, *var_end;

    buffer[0] = '\0';
    while ((var_start = strchr(start, '$')) != NULL) {
        strncat(buffer, start, var_start - start);
        var_end = var_start + 1;
        while (*var_end && (isalnum(*var_end) || *var_end == '_')) {
            var_end++;
        }
        char var_name[128] = {0};
        strncpy(var_name, var_start + 1, var_end - var_start - 1);
        char* value = get_env_var(var_name);
        if (value) {
            strcat(buffer, value);
        }
        start = var_end;
    }
    strcat(buffer, start);
    strcpy(command, buffer);
}
