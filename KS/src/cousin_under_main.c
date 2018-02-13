#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

main() {
    int fds[2];
    pid_t pid;'
    if (pipe(fds) < 0)
        exit(-1);

    pid=fork();
    if(pid==0) {
        pid = fork();
        if (pid==0) {
            int a;
            read(fds[0], &a, sizeof(int));
            printf("Child1: PID: %d. My friend: %d.\n", getpid(), a);
            exit(0);
        }
        exit(0); // Put the child under init.
    }
    wait(0); // Wait for the child to be put under init.
    sleep(1);
    system("ps -o pid,ppid,pgid,sess,comm");
    system("echo");
    sleep(10);

}
