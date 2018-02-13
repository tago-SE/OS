#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

void main() {
    int fds[2];
    pipe(fds);
    if(!fork()) {
         close(1);
         dup(fds[1]);
         close(fds[0]);
         close(fds[1]);
         sleep(1);
         execlp("./out", "./out", "10", NULL);
    }
    if(!fork()) {
        close(0);
        dup(fds[0]);
        close(fds[0]);
        close(fds[1]);
        sleep(1);
        execlp("./in", "./in", NULL);
    }
    system("ps -o pid,ppid,pgid,sess,comm");
    close(fds[0]);
    close(fds[1]);
    wait(0);
    wait(0);
}
