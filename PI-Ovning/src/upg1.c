#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define READ 0
#define WRITE 1

int main() {
    char p1[] = "/bin/ls", p2[]="/usr/bin/wc";
    int fd[2], r;

    if (pipe(fd) != 0) {
        perror("Failed to craete a pipe\n");
        exit(-1);
    }
    /* Child - Producer
        Can read from stdin, but instead writes to f    */
    if (fork() == 0) {
        printf("Inside the child process 1: %d\n", getpid());
        r = close(WRITE);           // 1: closed 1 no longer points to stdout on OS FT
        r = dup(fd[WRITE]);         // 1: now points to fd-write filedescriptor on OS FT
        r = close(fd[READ]);        // 3/4: removing pipe references from FDT
        r = close(fd[WRITE]);
        execlp(p1, p1, NULL);
        perror("Failed to execlp 1\n");
        exit(0);
    }
    /* Child - Consumer
        Can write from stdin, but now recieves
    */
    if (fork() == 0) {
        printf("Inside the child process 2: %d\n", getpid());
        r = close(READ);            // 0: closed 0, no longer points to stdin on OS FT
        r = dup(fd[READ]);          // 0: now points to fd-read filedescriptor on OS-FT
        r = close(fd[WRITE]);       // 3/4: removing pipe references from FDT
        r = close(fd[READ]);
        execlp(p2, p2, NULL);
        perror("Failed to execlp 1\n");
    }
    return 0;
}
