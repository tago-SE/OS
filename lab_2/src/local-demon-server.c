#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/un.h>

#define READ    0
#define WRITE   1

static void deamonize() {
    pid_t pid, sid;
    if (getpid() == 1) return; // already a daemon

    pid = fork();
    if (pid > 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Inside child process */
    umask(0); // Change the filemode mask

    /* Create a new session id for the process */
    sid = setsid();
    if (sid < 0)
        exit(EXIT_FAILURE);

    /* Change current working directory */
    if ((chdir("/home/me"))<  0)
        exit(EXIT_FAILURE);

    /* Redirect standard files to /dev/null

    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    */
}

void exec_command(char* command) {
    switch(fork()) {
        case 0:
           // close(WRITE);

            printf("exec command %s...", command);
            execlp("/bin/sh", "sh", "-c", command, NULL);
            perror("exec");
            exit(EXIT_FAILURE);
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
    }
    wait(0);
}

int clientHandler(int client_socket_fd) {
    printf("Handing a client...\n");
    while(1) {
        int length;
        char* text;

        if (read(client_socket, &length, sizeof(length)) == 0)
            return 0;

        text = (char*) malloc(length);


        read(client_socket, text, length);

        if (strcmp(text, "quit") == 0) {
            printf("client sent quit msg\n");
        }

        // Quit client
        if (!strcmp(text, "quit"))
            exit(EXIT_FAILURE);

        exec_command(text);

        write(socket_fd, "hello", sizeo(char)*10);

        free(text);
    }
    close(client_socket_fd);
}

void main(int argc, char* argv[]) {

    /* Deamonize this process */
    deamonize();
    printf("pid: %d\n", getpid());

    /* Saving pid into file (in working directory) for later use */
    FILE *fptr = fopen("local-demon-server.txt","w");
    fprintf(fptr, "%d", getpid());
    fclose(fptr);

    /* Server related code */

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
        exit(EXIT_FAILURE);
    }

   // printf("socket_name%s\n", name.sun_path);

    if (listen(socket_fd, 5) != 0) {
        perror("listen");
        unlink(socket_name);
        exit(EXIT_FAILURE);
    }

    do {
        struct sockaddr_un client_name;
        //socklen_t client_name_len = sizeof(sock_len);
        int sock_len = sizeof(client_name); // ersätter boken

        int client_socket_fd = accept(socket_fd, (struct sockaddr*) &client_name, &sock_len);
        if (!fork()) {
            if (client_socket_fd < 0) {
                perror("accept");
                unlink(socket_name);
                exit(EXIT_FAILURE);
            }
            clientHandler(client_socket_fd);
            exit(0);
        }

        //client_sent_quit_message = clientHandler(client_socket_fd);

        close(client_socket_fd);

    } while (1);//(!client_sent_quit_message);

    close(socket_fd);
    unlink(socket_name);
}
