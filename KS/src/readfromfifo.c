#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void spawn(int n) {
    int i;
    switch(fork()) {
        case 0:
            for (i = 0; i < n; i++) {
                switch(fork()) {
                    case 0:
                        execlp("./child", "./child", NULL);
                        perror("execlp");
                        exit(-1);
                    case -1: perror("grandchild"); break;
                }
            }
            exit(0);
        case -1:
            perror("child");
            exit(-1);
    }
    wait(0);
}

void read_from_fifo() {
    printf("reading from fifo\n");
    char str[10], *res;
    int x;
    FILE *fd = fopen("/home/me/pipe", "r");
    while(1) {
        res=fgets(str,10,fd);
        if(res!=NULL) {
            x = atoi(str); printf("x: %d.\n", x);
            if (x > 0)
                spawn(x);
            else {
                printf("stopping deamon\n");
                return;
            }
            system("ps -e -o pid,ppid,comm | grep readfromfifo");
            system("ps -e -o pid,ppid,comm | grep child");
        }
        sleep(1);
    }
}

main() {
    switch(fork()) {
        case 0:
            switch(fork()) {
                case 0:
                    read_from_fifo();
                    exit(0);
                case -1: perror(" grand child"); exit(-1);
            }
            exit(0);
        case -1: perror("child"); exit(-1);
    }
    wait(0);
}

