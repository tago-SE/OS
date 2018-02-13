#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1



void sys() {
    system("echo ============================== ");
    system("ps -o pid,ppid,pgid,sess,comm");
}

void main() {
    pid_t pid;
    int fds1[2], fds2[2];
    if (pipe(fds1) < 0 || pipe(fds2) < 0) {
        perror("pipe"); exit(-1);
    }

    switch(fork()) {
        printf("b: %d\n", getpid());
        case 0:
            pid = fork();
            if (pid == 0) {
                printf("c: %d\n", getpid());
                close(fds1[READ]);
                close(fds1[WRITE]);
                close(fds2[WRITE]);
                close(fds2[READ]);
                for (;;) {
                    sleep(1);
                    printf("C still running... %d\n", getpid());
                }
            }
            else if (pid < 0) {
                perror("fork c");
                exit(-1);
            }
            sleep(1);
            close(fds1[READ]);
            write(fds1[WRITE], &pid, sizeof(pid));
            close(fds1[WRITE]);
            close(fds2[WRITE]);
            close(fds2[READ]);
            wait(0); // wait for c
            sleep(8);
            exit(0);
        case -1: perror("fork b"); exit(-1);
    }

    sys();

    switch(fork()) {
        case 0:
            printf("d: %d\n", getpid());
            close(READ);
            dup(fds1[READ]);
            close(fds1[READ]);
            close(fds1[WRITE]);
            close(WRITE);
            dup(fds2[WRITE]);
            close(fds2[WRITE]);
            close(fds2[READ]);
            sleep(2)
            execlp("./killer", "./killer", NULL);
            perror("killer");
            exit(-1);
        case -1: perror("fork k"); exit(-1);
    }
    sys();
    sleep(3);
    sys();
    pid = 0;
    read(fds2[READ], &pid, sizeof(pid));
    printf("Process %d was killed\n", pid);
    close(fds1[READ]);
    close(fds1[WRITE]);
    close(fds2[WRITE]);
    close(fds2[READ]);
    wait(0); // wait for k
    sys();
    wait(0); // wait for b
}

/*
void close_pipe(int fds[2]) {
    close(fds[READ]);
    close(fds[WRITE]);
}

void main() {
    pid_t pid;

    int fds[2], fds2[2];
    if (pipe(fds) < 0 || pipe(fds2) < 0) {
        perror("pipe"); exit(-1);
    }
    // B process
    switch(fork()) {
        case 0:
            printf("B %d\n", getpid());
            // C process
            pid = fork();
            if (pid == 0) {
                printf("C %d\n", getpid());
                close_pipe(fds);
                close_pipe(fds2);
                sleep(3);
                exit(0);
            }
            else if (pid < 0) {
                perror("fork c");  exit(-1);
            }
            close_pipe(fds2);
            close(fds[READ]);
            write(fds[WRITE], &pid, sizeof(pid));
            close(fds[READ]);
            wait(0);
            sleep(15);
            exit(0);

        case -1: perror("fork b"); exit(-1);
    }
    system("echo ============================== ");
    system("ps -o pid,ppid,pgid,sess,comm");

    // K Process

    pid_t killer = fork();
    if (killer == 0) {
        printf("K %d\n", getpid());
        sleep(1);
        close(READ);
        dup(fds[READ]);
        close_pipe(fds);
        close(WRITE);
        dup(fds2[WRITE]);
        close_pipe(fds2);
        execl("./killer", "./killer", NULL);
        perror("killer");
        exit(-1);
    }
    else if (killer < 0) {
        perror("fork k"); exit(-1);
    }
    system("echo ============================== ");
    system("ps -o pid,ppid,pgid,sess,comm");
    close(fds2[WRITE]);
    read(fds2[READ], &pid, sizeof(pid));
    close(fds2[READ]);
    system("echo ============================== ");
    system("ps -o pid,ppid,pgid,sess,comm");
    printf("Child killed: %d\n", pid);
    //write(fds[WRITE], &killer, sizeof(killer));
    close_pipe(fds);
    wait(0); // Waiting for killer
    system("echo ============================== ");
    system("ps -o pid,ppid,pgid,sess,comm");
    wait(0); // Waiting for B
}
*/
