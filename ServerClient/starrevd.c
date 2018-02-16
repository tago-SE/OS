#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    pid_t pid, sid;

    pid = fork();

    if (pid < 0) {
        perror("Could not fork!");
    } else if (pid > 0) {
        exit(0);
    }

    printf("Hello im child under init\n");

    //writing pid to file
    int myPid = getpid();
    int fds = open("/home/me/revd.pid", O_CREAT | O_WRONLY); //if /var/run/revd.pid i need to be root, dont know sudo pass. :D
    char str[10];
    sprintf(str, "%d", myPid);
    write(fds, str, strlen(str));
    close(fds);

    printf("My pid %d, has been written to file \n", getpid());

    //now under init

    umask(0);
    sid = setsid();
    if (sid < 0) {
        perror("Could not setsid!\n");
        exit(EXIT_FAILURE);
    }

    printf("set umask and sid\n");

    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    printf("changed dir\n");

    freopen("dev/null", "r", stdin);
    freopen("dev/null", "w", stdout);
    freopen("dev/null", "w", stderr);

    execlp("/media/sf_shared/OS/OS/ServerClient/server", "/media/sf_shared/OS/OS/ServerClient/server", NULL);
    return 0;
}
