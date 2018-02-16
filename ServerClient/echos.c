/*
** echos.c -- the echo server for echoc.c; demonstrates unix sockets
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SOCK_PATH "/tmp/echo_socket"

reverse(char *str)
{
    char tmp; int i;
    for(i=0;i<10;i++){sleep(1); printf("%d ", i); fflush(stdout);}
    for(i=0;i<strlen(str)/2;i++)
    {tmp=str[i];str[i]=str[strlen(str)-i-1];str[strlen(str)-i-1]=tmp;}
}

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}


int main(void)
{
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

	int s, s2, len;
	unsigned t;
	struct sockaddr_un local, remote;
	char str[100];

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
	    perror("Could not bind!");
		exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}

	for(;;) {
		int done, n;
		printf("Waiting for a connection...\n");
		t = sizeof(remote);
		if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
			perror("accept");
			exit(1);
		}

		switch(fork()) {
            case 0:
                printf("Connected.\n");

                close(s);
                done = 0;
                do {
                    n = recv(s2, str, 100, 0);
                    if (n <= 0) {
                        if (n < 0) perror("recv");
                        done = 1;
                    }

                    if (!done)
                        printf("Recieved: %s\n", str);
                        reverse(str);
                        printf("Reversed: %s\n", str);

                        if (send(s2, str, n, 0) < 0) {
                            perror("send");
                            done = 1;
                        }
                } while (!done);

                close(s2);
                exit(0);
        }
        close(s2);
	}

	return 0;
}


