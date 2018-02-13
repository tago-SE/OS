#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define READ 0
#define WRITE 1

int main() {

    int fds[2], status, test;
    printf("%s | %s \n", "/bin/ls", "/usr/bin/wc");

    if (pipe(fds) < 0) {
        perror("Failed to create a pipe.\n");
        exit(-1);
    }

    switch(fork()) {
        case 0: { /* Producer */
            test = close(WRITE);            assert(test == 0);
            test = dup(fds[WRITE]);         assert(test == WRITE);
            test = close(fds[READ]);        assert(test == 0);
            test = close(fds[WRITE]);       assert(test == 0);
            execlp("/bin/ls", "ls", (char*) 0);
            printf("hi?");
            perror("Cannot excel 1\n");
            exit(-1);
            break;
        }
        case -1: {
            perror("Failed to fork 1\n");
            break;
        }
    }
    switch (fork()) {
        case 0: {
            test = close(READ);                     assert(test == 0);
            test = dup(fds[READ]);                  assert(test == READ);
            test = close(fds[READ]);                assert(test == 0);
            test = close(fds[WRITE]);               assert(test == 0);
            execlp("/usr/bin/wc", "wc",(char*) 0);
            break;
        }
        case -1: {
            perror("Failed to fork 2\n");
            break;
        }
    }

    test = close(fds[READ]);                assert(test == 0);
    test = close(fds[WRITE]);               assert(test == 0);
    wait(&status);
    wait(&status);


    return 0;
}
