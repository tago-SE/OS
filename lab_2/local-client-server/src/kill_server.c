#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define SERVER_ECHO_PATH    "/home/me/server/echos.txt"

int main() {
    char str[60];
    int pid;
    FILE *fptr = fopen(SERVER_ECHO_PATH, "r+");
    if(fptr == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    if( fgets (str, 60, fptr)!=NULL ) {
        pid = atoi(str);
        printf("kill server: %d\n", pid);
        killpg(pid, SIGINT);
    }
    fclose(fptr);
    return EXIT_SUCCESS;
}
