#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


void showCwd() {
    char cwd[200];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("cwd");
    }
}

int main(int argc, char* argv[]) {

    char cmd[255];
    int i;
    for (i = 1; i < argc; i++) {
        strcat(cmd, argv[i]);
        strcat(cmd, " ");
    }
    printf("Command:%s \n", cmd);

    if (strcmp(argv[1], "cd") == 0) {
        if (chdir(argv[2]) != 0) {
            perror("cd");
            exit(-1);
        } else {
            printf("cwd is: "); showCwd(); printf("\n");
        }
        exit(1);
    }
    else if (strcmp(argv[1], "pwd") == 0) {
        printf("here we are...\n");
        showCwd();
        exit(1);
    }
    switch(fork()) {
        case 0:
            execlp("/bin/sh", "sh", "-c", cmd, NULL);
            perror("exec");
            exit(-1);
        case -1:
            perror("fork");
            exit(-1);
    }
    wait(0);
    return 0;
}
