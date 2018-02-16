#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define READ 0
#define WRITE 1

void main() {
    int pid, fds1[2], fds2[2], t;
    if (pipe(fds1) < 0 || pipe(fds2) < 0)  {
        perror("pipe");
        exit(-1);
    }
    if (!fork()) {
        // C1
        t = close(fds1[WRITE]); assert(t == 0);
        t = close(fds2[READ]);  assert(t == 0);
        t = close(fds2[WRITE]); assert(t == 0);
        if (!fork()) {
            // C2 (adopted by init)
            while(read(fds1[READ], &pid, sizeof(int))) {
                printf("C2 recieved: %d\n", pid);
            }
            t = close(fds1[READ]); assert(t == 0);
            exit(0);
        }
        t = close(fds1[READ]); assert(t == 0);
        exit(0);
    }
    wait(0); // Wait for C1
    system("ps -o pid,ppid,sess,comm");

    if (!fork()) {
        // C5 - Intermediary
        close(READ);        assert(t == 0);
        dup(fds2[READ]);    assert(t == 0);
        close(WRITE);       assert(t == 1);
        dup(fds1[WRITE]);   assert(t == 0);
        close(fds1[READ]);  assert(t == 0);
        close(fds1[WRITE]); assert(t == 0);
        close(fds2[READ]);  assert(t == 0);
        close(fds2[WRITE]); assert(t == 0);
        if (!fork()) {
            execlp("./double", "./double", NULL);
            perror("exec-double");
            exit(-1);
        }
        execlp("./double", "./double", NULL);
        perror("exec-double");
        exit(-1);
    }
    system("ps -o pid,ppid,sess,comm");
    t = close(fds1[READ]);  assert(t == 0);
    t = close(fds1[WRITE]); assert(t == 0);
    t = close(fds2[READ]);  assert(t == 0);
    if (!fork()) {
        // C3
        pid = fork();
        if (pid == 0) {
            // C4 (adopted by init)
            pid = getpid();
            write(fds2[WRITE], &pid, sizeof(int));
            close(fds2[WRITE]);
            exit(0);
        }
        write(fds2[WRITE], &pid, sizeof(int));
        close(fds2[WRITE]);
        sleep(2);
        exit(0);
    }
    close(fds2[WRITE]);
    wait(0); // Wait for C3
    wait(0); // Wait for Double
    system("ps -o pid,ppid,sess,comm");
}
