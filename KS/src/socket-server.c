#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <assert.h>

int server(int client_socket) {
    printf("Handing a client...\n");
    while(1) {
        int length;
        char* text;

        if (read(client_socket, &length, sizeof(length)) == 0)
            return 0;

        text = (char*) malloc(length);

        read(client_socket, text, length);
        printf("%s\n", text);
        free(text);
        if (!strcmp(text, "quit"))
            return -1;

    }
}

int main(int argc, char* argv[]) {
    const char* socket_name = argv[1];
    printf("socket_name: %s\n", socket_name);

    int socket_fd;
    struct sockaddr_un name;
    int client_sent_quit_message;

    if ((socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(-1);
    }

    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, socket_name);
    if (bind(socket_fd, (struct sockaddr*) &name, SUN_LEN(&name)) != 0) {
        perror("bind");
        return -1;
    }
    if (listen(socket_fd, 5) != 0) {
        perror("listen");
        return -1;
    }

    do {
        struct sockaddr_un client_name;

        //socklen_t client_name_len = sizeof(sock_len);
        int sock_len = sizeof(client_name); // ersätter boken

        int client_socket_fd = accept(socket_fd, (struct sockaddr*) &client_name, &client_name_len);
        assert(client_socket_fd >= 0);

        client_sent_quit_message = server(client_socket_fd);

        close(client_socket_fd);

    } while (!client_sent_quit_message);

    close(socket_fd);
    unlink(socket_name);

    return 0;


}
