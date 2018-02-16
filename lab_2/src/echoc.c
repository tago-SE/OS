
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include<signal.h>
#include<unistd.h>

#define SOCK_PATH "/home/me/server/echo_socket"
#define MAX_LEN 100000

#define DEBUG_ENABLED 0

int s, t, len;
struct sockaddr_un remote;
char str[MAX_LEN];


void sig_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nreceived SIGINT\n");
        if (send(s, "q echoc", 8, 0) == -1) {
            perror("send");
            exit(-1);
        }
        exit(0);
    }
}

int main(void)
{
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(-1);
    }

    printf("Trying to connect...\n");

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(-1);
    }

    printf("Connected.\n");

    while(printf("> "), fgets(str, MAX_LEN, stdin), !feof(stdin)) {

        t = strlen(str);
        str[t - 1] = '\0';

        #if DEBUG_ENABLED
            printf("str: '%s' %d %d\n", str, strlen(str), t);
        #endif

        if (send(s, str, t, 0) == -1) {
            perror("send");
            exit(-1);
        }
        t = recv(s, str, MAX_LEN, 0);
        #if DEBUG_ENABLED
            printf("recv_len: %d\n", t);
        #endif
        if (t > 0) {
            str[t] = '\0';
            printf("> %s\n", str);
        } else {
            if (t < 0)
                perror("recv");
            else printf("Server closed connection\n");
                exit(1);
        }
    }
    printf("program terminated\n");
    close(s);
    return 0;
}



