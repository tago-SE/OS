/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MSG_CLIENT_QUIT     "q client"
#define MSG_SERVER_QUIT     "q server"


#define EXEC_CMD2           "/home/me/server/exec_cmd2"
#define KILL_SERVER         "/home/me/server/igcckill_server"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
// ===================================================================================================

int kill_server() {
    if(!fork()) {
        /* Change current working directory
        if ((chdir(SERVER_PATH)) <  0) {
            perror("chdir kill_server");
            exit(EXIT_FAILURE);
        }
        */
        execlp(KILL_SERVER, "./kill_server", NULL);
        perror("exec kill_server");
        exit(EXIT_FAILURE);
    }
    wait(0);
}

void handle_client(int client_socket_fd) {
    int done = 0, len;
    char recv_msg[100], send_msg[100];
    printf("client handler!\n");
    do {
        len = recv(client_socket_fd, recv_msg, 100, 0);
        printf("client_handler: %s\n", recv_msg);
        if (len <= 0) {
            if (len < 0)
                perror("recv");
            done = 1;
        }
        else {
            if (strcmp(recv_msg, MSG_CLIENT_QUIT) == 0) {
                done = 1;
            }
            else if (strcmp(recv_msg, MSG_SERVER_QUIT) == 0) {
                done = 1;
                kill_server();

            } else {
                if (!fork()) {
                    close(1);
                    dup(client_socket_fd);
                    close(2);
                    dup(client_socket_fd);
                      /* Change current working directory *
                    if ((chdir(SERVER_PATH)) <  0) {
                        perror("chdir handle_client");
                        exit(EXIT_FAILURE);
                    }
                    */
                    execlp(EXEC_CMD2, "./exec_cmd2", recv_msg, NULL);
                    perror("exec_cmd2");
                    exit(EXIT_FAILURE);
                }
                wait(0);
            }
        }
    } while(!done);
}

// ===================================================================================================

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            printf("before\n");
            close(sockfd); // child doesn't need the listener
            printf("after\n");
            handle_client(new_fd);
            //if (send(new_fd, "Hello, world!", 13, 0) == -1)
            //    perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }
    close(sockfd);
    return 0;
}
