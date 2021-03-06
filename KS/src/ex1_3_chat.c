#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <assert.h>
#include <stdbool.h>

#define READ 0
#define WRITE 1

void close_fds(int fds[2]) {
    close(fds[READ]);
    close(fds[WRITE]);
}

int main() {

    int fds[2], fds2[2], pid, test;

    if (pipe(fds) < 0) {
        perror("pipe fds");
        exit(-1);
    }
    /*
    if (pipe(fds2) < 0) {
        perror("pipe fds");
        exit(-1);
    }
    */
    printf("\n\nParent: %d\n\n", getpid());
    printf("    PID\t  PPID\n");

    switch (fork()) {
        case 0:
            printf("C1: %d %d\n", getpid(), getppid());
            sleep(1);
            switch(fork()) {
                case 0:
                    printf("C2: %d %d\n", getpid(), getppid());
                    while (read(fds[READ], &pid, sizeof(pid)) > 0) {
                        printf("C2 %d, recieved C4 pid: %d\n", getpid(), pid);
                        if (pid == 0) {
                            printf("closing C2\n");
                            break;
                        }
                    }
                   close_fds(fds);
                   exit(0);
                case -1: perror("fork 2");
            }
            close_fds(fds);
            exit(0);
        case -1: perror("fork 1"); exit(-1);
    }
    wait(0); // Waits for C1 to terminate
/*
    switch(fork()) {
        case 0:
            close(READ);
            dup(fds[READ]);
            close(WRITE);
            dup(fds[WRITE]);
            close_fds(fds);
            execlp("./double", "./double", NULL);
            perror("exec");
            exit(-1);
        case -1: perror("Fork 5"); exit(-1);
    }
*/
    sleep(1);
    printf("-----------------------\n");
    system("ps -o pid,ppid,pgid,sess,comm");
    printf("-----------------------\n");
    switch (fork()) {
        case 0:
            printf("C3: %d %d\n", getpid(), getppid());
            switch(fork()) {
                case 0:
                    printf("C4: %d %d\n", getpid(), getppid());
                    pid = -1;
                    while (pid != 0) {
                        printf("Enter number (0 to exit): ");
                        scanf("%d", &pid);
                        printf("\n");
                        test = write(fds[WRITE], &pid, sizeof(pid)); assert(test > 0);
                    }
                    close_fds(fds);
                    exit(0);
                case -1: perror("fork 4");
            }
            close_fds(fds);
            wait(0);
            exit(0);
        case -1: perror("fork 3"); exit(-1);
    }
    close_fds(fds);
    wait(0);
    printf("-----------------------\n");
    system("ps -o pid,ppid,pgid,sess,comm");
    printf("-----------------------\n");
    wait(0);
    wait(0);
    return 0;
}
