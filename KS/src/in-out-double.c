#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <assert.h>

// Programnamn: uppg5
main()
{
  int fds1[2], fds2[2], test, a;
  pipe(fds1); pipe(fds2);

  if(!fork())
  {
    char pidstr[20];
    if(!fork())
    {
      test = close(0); assert(test==0);
      test = dup(fds1[0]); assert(test==0);
      test = close(fds1[0]); assert(test==0);
      test = close(fds1[1]); assert(test==0);
      test = close(1); assert(test==0);
      test = dup(fds2[1]); assert(test==1);
      test = close(fds2[1]); assert(test==0);
      test = close(fds2[0]); assert(test==0);
      sleep(1);
      execlp("./double", "./double", NULL);
      fprintf(stderr,"execlp did not work\n");
      exit(1);
    }

    test = close(1); assert(test==0);
    test = dup(fds1[1]); assert(test==1);
    test = close(fds1[1]); assert(test==0);
    test = close(fds1[0]); assert(test==0);
    test = close(fds2[0]); assert(test==0);
    test = close(fds2[1]); assert(test==0);
    sleep(1);
    sprintf(pidstr,"%d",getpid());
    execlp("./out","./out",pidstr,NULL);
    fprintf(stderr,"execlp did not work\n");
    exit(1);
  }
  system("ps -o pid,ppid,pgid,sess,comm");

  read(fds2[0],&a,sizeof(int)); printf("pid * 2: %d.\n", a);

  wait(0);
}
