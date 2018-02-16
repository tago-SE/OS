#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid;
    pid = fork();

    if (pid < 0) {
        fprintf(stderr,"fork failed");
        exit (1);
    }
    else if (pid == 0) {
        printf("I'm a child process %d, of parent %d.\n", getpid(), getppid());
        exit(0);
    }
    else {
        sleep(1);
        system("ps -o pid,ppid,comm");
        sleep(15);
        printf("I'm a parent process %d and my childs process is: %d\n", getpid(), pid);
    }
    return 0;
}
