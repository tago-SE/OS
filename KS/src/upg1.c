#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define READ 0
#define WRITE 1

void ps() {
    printf("-------------------------\n");
    system("ps -o pid,ppid,pgid,sess,comm");
}

void close_pipe(int fds[2]) {
    close(fds[READ]);
    close(fds[WRITE]);
}


void main() {

    int fds[2], fds2[2], id, x;

    if (pipe(fds) < 0 || pipe(fds2)) {
        perror("pipe");
        exit(-1);
    }

    printf("-------------------------\n");
    printf("A: %d\n", getpid());

    switch(fork()) {
        case 0:
            printf("B: %d\n", getpid());
            id = fork();
            if (id == 0) {
                printf("C: %d\n", getpid());
                close(fds[WRITE]);
                close(fds2[READ]);

                while (read(fds[READ], &x, sizeof(x)) > 0) {
                    if (x == 0) break; // Ending C
                    printf("C recieved %d\n", x);
                    write(fds2[WRITE], &x, sizeof(x));

                }
                close(fds2[WRITE]);
                close(fds[READ]);
                sleep(2);
                exit(0);
            }
            else if (id < 0) {
                perror("Fork C"); exit(-1);
            }
            int x = getpid();
            close(fds[READ]);
            write(fds[WRITE], &id, sizeof(id)); // C process id
            write(fds[WRITE], &x, sizeof(x)); // B process id
            close(fds[WRITE]);
            close_pipe(fds2);
            wait(0); // Wait for C
            sleep(2);
            exit(0);
        case -1: perror("Fork B"); exit(-1);
    }
    ps(); // A, B, C

    switch(fork()) {
        case 0:
            printf("D: %d\n", getpid());
            close(READ);
            dup(fds2[READ]);
            close_pipe(fds);
            close_pipe(fds2);
            sleep(1);
            execlp("./d", "./d", NULL);
            exit(0);
        case -1: perror("Fork D"); exit(-1);

    }
    ps(); // A, B, C, D

    // Terminate C
    x = 0;
    write(fds[WRITE], &x, sizeof(x));
    close_pipe(fds);
    close_pipe(fds2);

    wait(0); // Wait for D
    sleep(1);
    ps(); // A B
    wait(0); // Wait for B
    ps(); // A
}
