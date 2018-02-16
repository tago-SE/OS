#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fds = open("/home/me/revd.pid", O_RDONLY);
    char pid[10];
    read(fds, &pid, 10);
    close(fds);

    int convPid = atoi(pid);
    kill(convPid,SIGKILL);

    return 0;
}
