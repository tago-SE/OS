#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdbool.h>

#define DEBUG_ENABLED       0
#define SOCK_PATH           "/home/me/server/echo_socket"
#define SERVER_PATH         "/home/me/server/"
#define SERVER_ECHO_PATH    "/home/me/server/echos.txt"
#define MSG_CLIENT_QUIT     "q client"
#define MSG_SERVER_QUIT     "q server"

bool validate_server_pid(int pid) {
    char str[60];
    int f_pid;
    FILE *fptr = fopen(SERVER_ECHO_PATH, "r+");
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
    FILE *fptr = fopen(SERVER_ECHO_PATH,"w");
    if(fptr == NULL) {
        perror("Error opening file");
        return -1;
    }
    fprintf(fptr, "%d", getpid());
    fclose(fptr);
}

int kill_server() {
    if(!fork()) {
        /* Change current working directory */
        if ((chdir(SERVER_PATH)) <  0) {
            perror("chdir kill_server");
            exit(EXIT_FAILURE);
        }
        execlp("./kill_server", "./kill_server", NULL);
        perror("exec kill_server");
        exit(EXIT_FAILURE);
    }
    wait(0);
}

/*
    Signal Handler
*/

void sig_handler(int sig) {
    if (sig == SIGINT) {
        if (validate_server_pid(getpid())) {
            printf("Server terminated: %d\n", getpid());
        }
        exit(0);
    }
    else if (sig == SIGCHLD) {
        while(waitpid(-1, NULL, WNOHANG) > 0);  // not certain if this works correctly
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
    //client_directory();
    int done = 0, len;
    char recv_msg[100], send_msg[100];

    /* Redirect stdout & stderr to client_socket */

    do {
        len = recv(client_socket_fd, recv_msg, 100, 0);
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
                      /* Change current working directory */
                    if ((chdir(SERVER_PATH)) <  0) {
                        perror("chdir handle_client");
                        exit(EXIT_FAILURE);
                    }
                    execlp("./exec_cmd2", "./exec_cmd2", recv_msg, NULL);
                    perror("exec_cmd2");
                    exit(EXIT_FAILURE);
                }
                wait(0);
            }
        }
    } while(!done);
    close(client_socket_fd);
    exit(0);
}

int main(void)
{
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    if(signal(SIGCHLD, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGCHLD\n");

    /* Deamonize this process */
    deamonize();

    printf("Served started on: %d\n", getpid());
    save_server();

    int s, s2, t, len;
    struct sockaddr_un local, remote;
    char recv_msg[100];
    char send_msg[100];

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(-1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
        exit(-1);
    }

    if (listen(s, 5) == -1) {
        perror("listen");
        exit(-1);
    }

    for(;;) {
        int done, n;
        /* Listen for connections */
        printf("Waiting for a connection...\n");
        t = sizeof(remote);
        if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
            perror("accept");
            exit(-1);
        }
        printf("Connected\n", s2);


        if (!fork()) {
            handle_client(s2);
            exit(0);
        }
        close(s2);
    }
    return EXIT_SUCCESS;
}



