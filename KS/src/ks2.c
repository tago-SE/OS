#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define DELAY   2
#define READ    0
#define WRITE   1

void main(int argc, char* argv[]) {
    int num_of_children = atoi(argv[1]), i, fds[2], num;
    printf("Creating %d children\n", num_of_children);
    if (pipe(fds) < 0) {
        perror("pipe");
        exit(-1);
    }
    for (i = 0; i < num_of_children; i++) {
        if (!fork()) {
            close(fds[WRITE]);
            read(fds[READ], &num, sizeof(int));
            printf("Child (%d) is sleeping for %d.\n", getpid(), num);
            sleep(num);
            close(fds[READ]);
            exit(0);
        }
    }
    system("ps -o pid,ppid,sess,comm | grep ks2");
    for (i = 0; i < num_of_children; i++) {
        printf("Enter sleep time: ");
        scanf("%d", &num);
        close(fds[READ]);
        write(fds[WRITE], &num, sizeof(int));
        wait(0);
        system("ps -o pid,ppid,comm | grep ks2");
    }
    close(fds[WRITE]);
    printf("Program completed\n");
}
