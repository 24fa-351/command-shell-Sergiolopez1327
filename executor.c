#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void execute_external(char* args[], int bg, int input_fd, int output_fd) {
    pid_t pid = fork();
    if (pid == 0) {  
        if (input_fd != 0) {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        if (output_fd != 1) {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        execvp(args[0], args);
        perror("xsh");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  
        if (!bg) {
            waitpid(pid, NULL, 0);
        } else {
            printf("[Background PID] %d\n", pid);
        }
    } else {
        perror("xsh: Fork failed");
    }
}
