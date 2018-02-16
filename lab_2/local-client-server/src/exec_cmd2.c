
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define DEBUG_ENABLED 0

void showCwd() {
    char cwd[200];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("cwd");
    }
}

int main(int argc, char* argv[]) {
    int i, j, k, o;
    char* srcs = argv[1];
    char output[1000][100];

    #if DEBUG_ENABLED
    printf("srcs = '%s'\n", srcs);
    #endif
    for (i = 0, j = 0, o = 0; i < strlen(srcs); i++) {
        if (srcs[i] == ' ') {
            for (k = 0; j < i; j++, k++) {
                output[o][k] = srcs[j];
                #if DEBUG_ENABLED
                printf("copying to...%d: %c\n", j, srcs[j]);
                #endif
            }
            output[o][k] = '\0';
            #if DEBUG_ENABLED
            printf("%d\n", k);
            printf("%s\n", output[o]);
            #endif
            o++;
            j++;
        }
    }
    int changed = 0;
    for (k = 0; j < strlen(srcs); j++, k++) {
        output[o][k] = srcs[j];
        changed = 1;
    }
    if (changed)
        o++;
    #if DEBUG_ENABLED
    printf("results\n");
    for (i = 0; i < o; i++) {
        printf("%d: %s\n", i, output[i]);
    }
    printf("argc=%d\n", o);
    #endif

    argc = o;

    char cmd[255];
    cmd[0] = '\0';

    for (i = 1; i < argc; i++) {
        strcat(cmd, output[i]);
        strcat(cmd, " ");
    }

    #if DEBUG_ENABLED
    printf("Command:%s \n", cmd);
    #endif

    if (strcmp(argv[1], "cd") == 0) {
        if (chdir(argv[2]) != 0) {
            perror("cd");
            exit(EXIT_FAILURE);
        }
        printf("cd: "); showCwd();
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(argv[1], "pwd") == 0) {
        printf("pwd: "); showCwd();
        exit(EXIT_SUCCESS);
    }
    switch(fork()) {
        case 0:
            execlp("/bin/sh", "sh", "-c", srcs, NULL);
            perror("exec");
            exit(EXIT_FAILURE);
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
