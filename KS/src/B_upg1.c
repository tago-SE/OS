#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

void spawn(int delay) {
    switch(fork()) {
        case 0:
            switch(fork()) {
                case 0:
                    sleep(delay);
                    exit(0);
                case -1: perror("spawn_long 2"); exit(-1);
            }
            exit(0);
        case -1: perror("spawn_long 1"); exit(-1);
    }
    wait(0);
}


void main(int argc, char *argv[]) {
    int i, n, d;

    assert(argc == 3);
    n = atoi(argv[1]);
    d = atoi(argv[2]);
    assert(n > 0);
    assert(d > 0 && d <= n);

    for (i = 0; i < n; i++) {

        if (i == d)
            spawn(2);
        else
            spawn(4);
    }
    sleep(1);
    system("echo ============================== 1 second passed");
    system("ps -e -o pid,ppid,pgid,sess,comm | grep B_upg1");
    sleep(2);
    system("echo ============================== 3 seconds passed");
    system("ps -e -o pid,ppid,pgid,sess,comm | grep B_upg1");
    sleep(2);
    system("echo ============================== 5 seconds passed");
    system("ps -e -o pid,ppid,pgid,sess,comm | grep B_upg1");

}
