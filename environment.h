#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#define MAX_VARS 128
#define MAX_INPUT 1024

typedef struct {
    char name[128];
    char value[128];
} EnvVar;

char* get_env_var(const char* name);
void set_env_var(const char* name, const char* value);
void unset_env_var(const char* name);
void substitute_vars(char* command);

#endif
