#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define DELAY 10

void spawn_even() {
    if (!fork()) {
        printf("even %d\n", getpid());
        sleep(DELAY);
        exit(0);
    }
}

void spawn_odd() {
    if (!fork()) {
        printf("odd %d\n", getpid());
        if (!fork()) {
            sleep(DELAY);
            exit(0);
        }
        sleep(DELAY);
        wait(0);
        exit(0);
    }
}

void main(int argc, char* argv[]) {

    int num_of_children = atoi(argv[1]), i;
    printf("Creating children %d\n", num_of_children);
    if (!fork()) {
        for (i = 0; i < num_of_children; i++) {
            if (i % 2 == 0) {
                spawn_even();
            }
            else {
                spawn_odd();
            }
        }
        exit(0);
    }
    wait(0);
    system("ps -o pid,ppid,sess,comm | grep ks1");
}
