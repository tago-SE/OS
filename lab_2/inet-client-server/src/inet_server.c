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
#include <stdbool.h>

#define MSG_CLIENT_QUIT     "q client"
#define MSG_SERVER_QUIT     "q server"


#define EXEC_CMD2           "/home/me/server/exec_cmd2"
#define KILL_SERVER         "/home/me/server/ikill_server"
#define SERVER_PATH         "/home/me/server/"
#define SERVER_PID_PATH     "/home/me/server/echos.txt"


#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold


bool validate_server_pid(int pid) {
    char str[60];
    int f_pid;
    FILE *fptr = fopen(SERVER_PID_PATH, "r+");
    if(fptr == NULL) {
        perror("Error opening file");
        return false;
    }
    if( fgets (str, 60, fptr)!=NULL ) {
        f_pid = atoi(str);
    }
    fclose(fptr);
    return f_pid == pid;
}

int save_server() {
    FILE *fptr = fopen(SERVER_PID_PATH,"w");
    if(fptr == NULL) {
        perror("Error opening file");
        return -1;
    }
    fprintf(fptr, "%d", getpid());
    fclose(fptr);
}

int kill_server() {
    if(!fork()) {
        execlp(KILL_SERVER, "./ikill_server", NULL);
        perror("exec ikill_server");
        exit(EXIT_FAILURE);
    }
}


void sig_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    if (s == SIGINT) {
       // if (validate_server_pid(getpid())) {
            printf("\nServer terminated: %d\n", getpid());
            //killpg(pid, SIGINT);
        //}
        exit(0);
    }
    else if (s == SIGCHLD) {
        while(waitpid(-1, NULL, WNOHANG) > 0);
    }
}


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
    if ((chdir(SERVER_PATH)) <  0)
        exit(EXIT_FAILURE);

    /* Redirect standard files to /dev/null
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    */
}


void handle_client(int client_socket_fd) {
    int done = 0, len;
    char recv_msg[100], send_msg[100];
    printf("client handler started...\n");
    do {
        len = recv(client_socket_fd, recv_msg, 100, 0);
        printf("recv: %s\n", recv_msg);
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
                    execlp(EXEC_CMD2, "./exec_cmd2", recv_msg, NULL);
                    perror("exec_cmd2");
                    exit(EXIT_FAILURE);
                }
            }
        }
    } while(!done);
}

// ===================================================================================================

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
    struct sigaction sa;
    sa.sa_handler = sig_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction SIGCHLD");
        exit(1);
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        exit(1);
    }

    /* Deamonize this process */
    deamonize();

    /* Save server pid */
    printf("Served started on: %d\n", getpid());
    save_server();


    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
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
            close(sockfd); // child doesn't need the listener
            handle_client(new_fd);
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }
    close(sockfd);
    return 0;
}
