#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    pid_t pid;
    int num = atoi(argv[1]);
    int index_grand_child = atoi(argv[2]);

    // Check for argument errors
    if (num > 0 && (index_grand_child >= 0 || index_grand_child < num) && argc == 3) {

        int i, failed = 0;
        for (i = 0; i < num; i++) {
            pid = fork();
            if (pid == 0) {
                printf("Child: pid=%d, ppid=%d\n", getpid(), getppid());

                // One spawn creates a grand child
                if (i == index_grand_child) {
                    pid = fork();
                    if (pid == 0) {
                        printf("Grandchild: pid=%d, ppid=%d\n", getpid(), getppid());
                        sleep(5);
                        exit(0);
                    }
                    printf("Grandchild terminated=%d\n", wait(0));
                }

                sleep(5 + i);
                exit(0);
            }
            else if (pid < 0) {
                printf("Failed to fork (%d)\n", i);
                failed++;
            }
            else {
                printf(".\n");
            }
        }
        for (i = 0; i < num - failed; i++) {
            printf("Child terminated=%d\n", wait(0));
        }

        exit(0);
    }
    else {
        printf("Invalid argument.\n");
        exit(-1);
    }
}
