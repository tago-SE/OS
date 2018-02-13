#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[]) {

    int i;
    char* arg_list = malloc(sizeof(char)*255);
    char path[255];
    strcpy(path, argv[1]);
    for (i = 2; i < argc; i++) {
        strcat(arg_list, argv[i]);
        strcat(arg_list, " ");
    }
    printf("Path:%s \nArgument:%s\n", path, arg_list);

    switch(fork()) {
        case 0:
            execlp(path, arg_list, NULL);
            perror("exec");
            exit(-1);
            break;

        case -1:
            perror("fork");
            exit(-1);
    }
    wait(0);
    return 0;
}
