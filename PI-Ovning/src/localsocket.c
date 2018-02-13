#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>



int server(int client_socket) {
    for (;;) {
        int length;
        char* text;

        /*  First read the length of the text message from the socket.
            If read returns 0 the client closed the connection.
        */
        if (read(client_socket, &length, sizeof(length)) == 0) return 0;

        // Allocate text
        text = (char*) malloc(length);

        read(client_socket, text, length);
        printf("%s\n", text);
        free(text);
        // if client sent msg "quit" the program is done
        if (!strcmp(text, "quit"))
            return 1;
    }
}

int main(int argc, char* argv[]) {

    const char* const socket_name = argv[1];
    int socket_fd;
    struct sockaddr_un name;

    name.sun_family = AF_LOCAL;

    strcpy(name.sun_path, socket_name);
    bind(socket_fd, &name, SUN_LEN (&name));


    return 0;
}
