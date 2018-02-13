#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main() {
    int run = 10, fd[2]; pipe(fd); srand(time(0)); char ch, x = 'x', o = 'o';

    if (!fork()) {
        close(fd[0]);
        while(run) {
            sleep(1+rand()%6); write(fd[1], &x, 1);
        }
        printf("x ended\n");
        close(fd[1]);
        exit(0);
    }
    if (!fork()) {
        close(fd[0]);
        while(run) {
            sleep(1+rand()%3); write(fd[1], &o, 1);
        }
        printf("o ended\n");
        close(fd[1]);
        exit(0);
    }
    close(fd[1]);
    while(run--) {
        read(fd[0], &ch, 1);
        printf("%d %c\n", run, ch);
        sleep(1);
    }
    close(fd[0]);
    run = wait(0); printf("%d ended\n", run);
    run = wait(0); printf("%d ended\n", run);
}
