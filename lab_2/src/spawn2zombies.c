#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    pid_t pid;

    int num = atoi(argv[1]);
    int index_grand_child = atoi(argv[2]);
    int num_zombies = atoi(argv[3]);

    int waiting[num];
    int waitingIndex = 0;

    if (num > 0 && (index_grand_child >= 0 || index_grand_child < num) && argc == 4 && num_zombies >= 0 && num_zombies <= (num - 1)) {

        int i, z, failed = 0;

        for (i = 0, z = 0; i < num; i++) {
            // Create Zombies
            if (z < num_zombies && i != index_grand_child) {
                z++;
                pid = fork();
                if (pid == 0) {
                    printf("Zombie-child: pid=%d, ppid=%d\n", getpid(), getppid());
                    exit(0);
                }
                else if (pid < 0) {
                    printf("Error - failed to create zombie-child.\n");
                    exit(-1);
                }
            }
            // Create child
            else {
                pid = fork();
                if (pid == 0) {
                    printf("Child: pid=%d, ppid=%d\n", getpid(), getppid());
                    if (i == index_grand_child) {
                        pid = fork();
                        if (pid == 0) {
                            printf("Grandchild: pid=%d, ppid=%d\n", getpid(), getppid());
                            sleep(5);
                            exit(0);
                        }
                        else if (pid < 0) {
                            printf("Error - failed to create grand-child.\n");
                            exit(-1);
                        }
                        printf("Grandchild terminated=%d\n", wait(0));
                    }
                    sleep(20);
                    exit(0);
                } else if (pid < 0) {
                    printf("Error - failed to create child.\n");
                }
                else {
                    waiting[waitingIndex++] = pid;
                }
            }
        }
        system("ps -o pid,ppid,pgid,sid,comm");
        int status;
        for (i = 0; i < waitingIndex; i++) {
            printf("Child terminated=%d\n", waitpid(waiting[i], &status, WCONTINUED));
        }
        exit(0);
    }
    else {
        printf("Invalid argument.\n");
        exit(-1);
    }
}
