#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr,"fork failed");
        exit (-1);
    }
    else if (pid == 0) {
        printf("Child with pid = %d\n", getpid());
        printf("I'm running ls:\n");
        if (execl("/bin/ls", "ls", NULL) == -1) {
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
