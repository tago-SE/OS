#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr,"fork failed");
        exit (-1);
    }
    else if (pid == 0) {

        char* argv[] = {"null", "how", "are", "we?"};

        printf("Child with pid = %d\n", getpid());
        if (execv("./command", argv) == -1) {
            printf("Failed to exec command\n");
            exit(-1);
        }
    }
    else {
        wait (0);
        printf("Parent, my child's pid = %d\n", pid);
        printf("My pid = %d.\n\n", getpid());
    }
    return 0;
}


