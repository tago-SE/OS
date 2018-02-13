#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

void closeFDS(int fds1[2], int fds2[2]) {
    close(fds1[READ]);
    close(fds1[WRITE]);
    close(fds2[READ]);
    close(fds2[WRITE]);
}

int main() {
    pid_t pid;
    int num_proc = 0, i, ret;
    int fds1[2], fds2[2];

    if (pipe(fds1) != 0 || pipe(fds2) != 0) {
        perror("Failed to create pipe\n");
        exit(-1);
    }

    pid = fork();      /* Process 1: Producer*/
    if (pid == 0) {
        close(WRITE);
        dup(fds1[WRITE]);
        closeFDS(fds1, fds2);
        execlp("/bin/sh","/bin/sh", "-c", "ls -l /bin/??", NULL);
        perror("Cannot execlp 1.\n");
        exit(-1);
    } else if (pid < 0) {
        perror("failed to create process 1\n");
        exit(-1);
    }
    num_proc++;

    pid = fork();     /* Process 2: Intermediary */
    if (pid == 0) {
        close(READ);
        dup(fds1[READ]);
        close(WRITE);
        dup(fds2[WRITE]);
        closeFDS(fds1, fds2);
        execlp("/bin/grep", "grep", "rwxr-xr-x", NULL);
        perror("Cannot execlp 2.\n");
        exit(-1);
    } else if (pid < 0) {
        perror("failed to create process 2\n");
        exit(-1);
    }
    num_proc++;

    pid = fork();       /* Process 3: Consumer */
    if (pid == 0) {
        close(READ);
        dup(fds2[READ]);
        closeFDS(fds1, fds2);
        execlp("/usr/bin/sort", "sort", NULL);
        perror("Cannot execlp 3.\n");
        exit(-1);
    } else if (pid < 0) {
        perror("failed to create process 3\n");
        exit(-1);
    }
    num_proc++;

    closeFDS(fds1, fds2);
    for (i = 0; i < num_proc; i++) {
        pid = wait(0);
        printf("P: Child terminated %d\n", pid);
    }
    return 0;
}





