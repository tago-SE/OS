#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>

#define SOCK_PATH "/home/me/echo_socket"

int main(int argc, char* argv[]) {
    int s, s1;
    struct sockaddr_un local, remote;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    switch(fork()) {
        case 0: {
            sleep(1);
            printf("Trying to connect...\n");
            remote.sun_family = AF_UNIX;
            strcpy(remote.sun_path, SOCK_PATH);
            int len = strlen(remote.sun_path) + sizeof(remote.sun_family);
            if (connect(s, (struct sockaddr *)&remote, len) == -1) {
                perror("connect");
                exit(1);
            }
            printf("Connected.\n");
            break;
        }
        case -1: {
            perror("fork1");
            exit(1);
            break;
        }
        default: {
            local.sun_family = AF_UNIX;
            strcpy(local.sun_path, SOCK_PATH);
            unlink(local.sun_path);
            int len = strlen(local.sun_path) + sizeof(local.sun_family);
            if (bind(s, (struct sockaddr *)&local, len) == -1) {
                perror("server_bind");
                exit(1);
            }
            if (listen(s, 5) == -1) {
                perror("server_listen");
                exit(1);
            }
            for(;;) {
                int done, n;
                printf("Waiting for a connection...\n");
                int addrlen = sizeof(remote);
                if ((s = accept(s, (struct sockaddr *)&remote, &addrlen)) == -1) {
                  perror("server_accept");
                  exit(1);
                }
                printf("Connected.\n");
            }
        }
    }
    return 0;
}
