#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
// Programnamn: out
main(int argc, char *argv[])
{
  int a = atoi(argv[1]);
  write(1,&a,sizeof(int));
}
