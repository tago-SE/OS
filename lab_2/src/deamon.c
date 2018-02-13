#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid;
    if ((pid = fork()) == 0) {
        printf("This is a child process: %d\n", getpid());
        system("ps -o pid,ppid,pgid,comm");
        if (pid = fork() == 0) {
            printf("This is a grad child process: %d\n", getpid());
            sleep(1);
            system("ps -o pid,ppid,pgid,comm");
            sleep(30);
            exit(0);

        } else if (pid < 0) {
            printf("error when trying to create grand child\n");
        }
        exit(0);
    }
    else if (pid < 0) {
        printf("error when trying to create child\n");
    }
}

