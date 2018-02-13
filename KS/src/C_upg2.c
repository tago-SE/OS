#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

void sys() {
    system("echo --------------------");
    system("ps -o pid,ppid,pgid,sess,comm");
}

void main(int argc, char* argv[]) {
    assert(argc == 2);

    int fds[2], i, n, pid, sum;
    n = atoi(argv[1]);
    assert(n > 0);
    printf("n = %d\n", n);
    if (pipe(fds) < 0) {
        perror("pipe"); exit(-1);
    }
    switch(fork()) {
        case 0:
            printf("b: %d\n", getpid());
            sleep(1);
            sum = 0;
            for (i = 0; i < n; i++) {
                read(fds[READ], &pid, sizeof(pid));
                printf("recieved: %d\n", pid);
                sum += pid;
            }
            close(fds[READ]);
            close(fds[WRITE]);
            printf("\nsum of pids: %d\n", sum);
            sleep(3);
            exit(0);
        case -1: perror("fork b"); exit(-1);

    }
    sys();
    for (i = 0; i < n; i++) {
        sleep(1);
        switch(fork()) {
            case 0:
                close(WRITE);
                dup(fds[WRITE]);
                close(fds[WRITE]);
                close(fds[READ]);
                execlp("./g", "./g", NULL);
                error("exec");
                exit(-1);
        }
        wait(0);
        sys();
    }
    wait(0);
    sys();
}
