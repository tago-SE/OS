#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[]) {

    int i;
    char* command = malloc(sizeof(char)*255);

    for (i = 1; i < argc; i++) {
        strcat(command, argv[i]);
        strcat(command, " ");
    }
    printf("Command:%s \n", command);

    /* Not working!
    if (strcmp(argv[1], "cd") == 0) {
        printf("Dir: %s\n", argv[2]);
        if (chdir(argv[2]) != 0) {
            perror("cd");
            exit(-1);
        }
        exit(1);
    }
    */

    switch(fork()) {
        case 0:
            execlp("/bin/sh", "sh", "-c", command, NULL);
            perror("exec");
            exit(-1);
        case -1:
            perror("fork");
            exit(-1);
    }
    wait(0);
    return 0;
}
