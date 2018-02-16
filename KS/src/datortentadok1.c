#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

void main() {

    int pid, fds1[2], fds2[2];
    if (pipe(fds1) < 0 || pipe(fds2) < 0) {
        perror("pipe");
        exit(-1);
    }
    if (!fork()) {
        // C1
        close(fds1[WRITE]);
        close(fds1[READ]);
        close(fds2[WRITE]);
        if (!fork()) {
            // C2
            while(read(fds2[READ], &pid, sizeof(int))) {
                printf("C2 recieved: %d\n", pid);
            }
            close(fds2[READ]);
            sleep(5);
            exit(0);
        }
        exit(0);
    }
    wait(0);
    system("ps -o pid,ppid,comm");

    if (!fork()) {
        close(0);
        dup(fds1[READ]);
        close(1);
        dup(fds2[WRITE]);
        close(fds1[READ]);
        close(fds1[WRITE]);
        close(fds2[READ]);
        close(fds2[WRITE]);
        execlp("./double", "./double", NULL);
        perror("exec");
        exit(-1);
    }
    if (!fork()) {
        close(fds1[READ]);
        close(fds2[READ]);
        // C3
        pid = fork();
        if (!pid) {
            // C4
            pid = getpid();
            close(fds2[WRITE]);
            write(fds1[WRITE], &pid, sizeof(int));
            close(fds1[WRITE]);
            sleep(5);
            exit(0);
        }
        write(fds2[WRITE], &pid, sizeof(int));
        close(fds2[WRITE]);
        close(fds1[WRITE]);
        exit(0);
    }
    close(fds1[READ]);
    close(fds1[WRITE]);
    close(fds2[READ]);
    close(fds2[WRITE]);
    wait(0);
    wait(0);
    system("ps -o pid,ppid,comm");

}
