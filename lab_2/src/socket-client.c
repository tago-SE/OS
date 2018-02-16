#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>



char* recvMsg(int socket_fd) {

}

void sendMsg(int socket_fd, char* msg) {
    int length = strlen(text) + 1;
    write(socket_fd, &length, sizeof(length));
    write(socket_fd, text, length);
}

int main(int argc, char* argv[]) {
    const char* const socket_name = argv[1];
    int socket_fd;
    struct sockaddr_un name;

    socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0); assert(socket_fd != -1);

    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, socket_name);

    if (connect(socket_fd, (struct sockaddr*) &name, SUN_LEN (&name)) == -1) {
        perror("connect");
        return -1;
    }

    printf("Connected to server...\n");

    while (1) {
        char * msg = malloc(sizeof(char)* 100);
        printf("Client msg: ");
        fgets(msg, 99, stdin);
        sendMsg(socket_fd, msg);
        free(msg);
    }



    write_text(socket_fd, "quit");
    close(socket_fd);


    return 0;
}
