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

#define DEBUG_ENABLED       0
#define SOCK_PATH           "/home/me/server/echo_socket"
#define SERVER_PATH         "/home/me/server/"
#define SERVER_ECHO_PATH    "/home/me/server/echos.txt"
#define MSG_CLIENT_QUIT     "q echoc"
#define MSG_SERVER_QUIT     "q echos"


void sig_handler(int sig) {
    if (sig == SIGINT) {
        #if DEBUG_ENABLED
            printf("\n SIGINT \n");
        #endif
        // Check to see if it's the parent... then execute kill
        exit(0);
    }
    else if (sig == SIGCHLD) {
        printf("SGCHILD...");
        while(waitpid(-1, NULL, WNOHANG) > 0);
        printf("DONE....\n");
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
    if ((chdir("/home/me/server")) <  0)
        exit(EXIT_FAILURE);

    /* Redirect standard files to /dev/null
    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    */

}

int exec_norm_cmd(int client_socket_fd, char* command)  {
    int p = fork();
    if (p == 0) {
        close(1);
        dup(client_socket_fd);
        close(2);
        dup(client_socket_fd);
        close(client_socket_fd);
        printf("exec command %s...", command);
        execlp("/bin/sh", "sh", "-c", command, NULL);
        perror("exec");
        return EXIT_FAILURE;
    }
    else if (p < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    int status;
    if (waitpid(p, &status, 0) == -1 ) {
        perror("waitpid");
        return EXIT_FAILURE;
    }
    if (WIFEXITED(status) ) {
        const int es = WEXITSTATUS(status);
        if (status == 0) {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}


int exec_command(int client_socket_fd, char* cmd) {
    if (cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' ') {
        int i;
        for (i = 3; i <= strlen(cmd) + 1; i++) {
            cmd[i - 3] = cmd[i];
        }
        printf("changing dir: %s\n", cmd);
        if (chdir(cmd) <  0) {
            printf("Error?\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return exec_norm_cmd(client_socket_fd, cmd);
}

int save_deamon() {
    FILE *fptr = fopen(SERVER_ECHO_PATH,"w");
    if(fptr == NULL) {
        perror("Error opening file");
        return -1;
    }
    fprintf(fptr, "%d", getpid());
    fclose(fptr);
}



int kill_server() {
    char str[60];
    int pid;
    FILE *fptr = fopen(SERVER_ECHO_PATH,"r+");
    if(fptr == NULL) {
        perror("Error opening file");
        return -1;
    }
    if( fgets (str, 60, fptr)!=NULL ) {
        pid = atoi(str);
        printf("killing: %d\n", pid);
        killpg(pid, SIGINT);
    }
    fclose(fptr);
}

void client_directory() {
    char* dir = malloc(sizeof(char)*100);
    char str_id[10];
    struct stat st = {0};

    dir[0] = '\0';
    strcpy(dir, SERVER_PATH);
    sprintf(str_id, "%d", getpid());
    strcat(dir, str_id);
    if (stat(dir, &st) == -1) {
        printf("dir craeted: %s\n", dir);
        if (mkdir(dir, 0700) < 0)
            perror("mkdir");
    }
    if (chdir(dir) <  0)
        perror("chdir");
    free(dir);
    free(str_id);
}

void handle_client(int client_socket_fd) {
    client_directory();
    int done = 0, len;
    char recv_msg[100], send_msg[100];

    do {
        len = recv(client_socket_fd, recv_msg, 100, 0);
        if (len <= 0) {
            if (len < 0)
                perror("recv");
            done = 1;
        }
        else {
            printf("> %s' %d\n", recv_msg, strlen(recv_msg));

            if (strcmp(recv_msg, MSG_CLIENT_QUIT) == 0) {
                done = 1;
            }
            else if (strcmp(recv_msg, MSG_SERVER_QUIT) == 0) {
                done = 1;
                kill_server();
            }
            else {
                /* Execute command */
                if (exec_command(client_socket_fd, recv_msg) == EXIT_FAILURE) {
                    #if DEBUG_ENABLED
                        printf("Command failed to execute\n");
                    #endif
                    send_msg[0] = '\0';
                    strcat(send_msg, "[Exec failure]: ");
                    strcat(send_msg, recv_msg);
                    len = strlen(send_msg);
                    #if DEBUG_ENABLED
                        printf("Send: '%s' %d %d\n", send_msg, len, strlen(send_msg));
                    #endif
                    if (send(client_socket_fd, send_msg, len, 0) < 0) {
                        perror("send");
                        done = 1;
                    }
                }
            }
        }
    } while(!done);
    printf("client_handler terminated %d\n", getpid());
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
    save_deamon();

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
    return 0;
}



