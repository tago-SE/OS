#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("failed to work.\n");
    }
    /* Child */
    else if (pid == 0) {
        execlp("/bin/ls", "ls", "-l", NULL);
    }
    /* Parent */
    else {

    }
    return 0;
}
