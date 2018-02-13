#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>

#define SOCK_PATH "/home/me/echo_socket"


reverse(char *str)
{
  char tmp; int i=0;
  for(;i<strlen(str)/2;i++)
  { tmp=str[i]; str[i] = str[strlen(str)-i-1];str[strlen(str)-i-1] = tmp; }
}

int main(int argc, char* argv[]) {
    int s, s2, t, len;
  struct sockaddr_un local, remote;
  char str[100];

  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("server_socket");
    exit(1);
  }

  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, SOCK_PATH);
  unlink(local.sun_path);
  len = strlen(local.sun_path) + sizeof(local.sun_family);
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
    t = sizeof(remote);
    if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
      perror("server_accept");
      exit(1);
    }

    printf("server_Connected.\n");

    done = 0;
    do {
      n = recv(s2, str, 100, 0);
      if (n <= 0) {
	if (n < 0) perror("server_recv");
	done = 1;
      }

      if (!done)
      {
        reverse(str);
	if (send(s2, str, n, 0) < 0) {
	  perror("server_send");
	  done = 1;
	}
      }
    } while (!done);

    close(s2);
  }
  return 0;
}
