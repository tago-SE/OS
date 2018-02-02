#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    pid_t pid;
    int num = atoi(argv[1]);

    if (num > 0) {
        int i, failed = 0;
        for (i = 0; i < num; i++) {
            pid = fork();
            if (pid == 0) {
                printf("Child: pid=%d, ppid=%d\n", getpid(), getppid());
                sleep(5 + i);
                exit(i);
            }
            else if (pid < 0) {
                printf("Failed to fork (%d)\n", i);
                failed++;
            }
            else {

            }
        }

        for (i = 0; i < num - failed; i++) {
            printf("child terminated=%d\n", wait(0));
            //system("ps -o pid,ppid,pgid,sid,comm");
        }
        exit(0);
    }
    else {
        printf("Invalid argument.\n");
        exit(-1);
    }
}
