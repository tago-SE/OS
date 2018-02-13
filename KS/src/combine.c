#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

close_fds(int fds[2]) {
    close(fds[0]);
    close(fds[1]);
}

void main() {
    char buffer[10];
    pid_t pid;
    int fds[2], fds2[2];
    pipe(fds);
    pipe(fds2);

    printf("----------------\n");
    switch(fork()) {
        case 0:
            switch(fork()) {
                    case 0:
                        sprintf(buffer, "%d", getpid());
                        sleep(1);
                        close(1);
                        dup(fds[1]);
                        close_fds(fds);
                        close_fds(fds2);
                        sleep(1);


                        execlp("./out", "./out", buffer, NULL);
                        exit(-1);
                    case -1:
                        perror("fork 2");
                        exit(-1);
            }
            close_fds(fds);
            close_fds(fds2);
            sleep(1);
            exit(0);
        case -1:
            perror("fork 1");
            exit(-1);
    }
    switch(fork()) {
        case 0:
            close(0);
            dup(fds[0]);
            close_fds(fds);

            close(1);
            dup(fds2[1]);
            close_fds(fds2);

            sleep(1);
            execlp("./double", "./double", NULL);
            exit(-1);
        case -1:
            perror("fork 5");
            exit(-1);
    }
    switch (fork()) {
        case 0:
            switch(fork()) {
                case 0:
                    close(0);
                    dup(fds2[0]);
                    close_fds(fds);
                    close_fds(fds2);
                    sleep(1);
                    execlp("./in", "./in", NULL);
                    exit(-1);
                case -1:
                    perror("fork 4");
                    exit(-1);
            }
            close_fds(fds);
            close_fds(fds2);
            sleep(1);
            exit(0);
        case -1:
            perror("fork 3");
            exit(-1);
    }
    system("ps -o pid,ppid,pgid,sess,comm");
    pid = wait(0);
    pid = wait(0);
    pid = wait(0);
    pid = wait(0);
    pid = wait(0);
    system("ps -o pid,ppid,pgid,sess,comm");
    close_fds(fds);
    close_fds(fds2);
}
