#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
// Programnamn: in
main(int argc, char *argv[])
{
  int a;
  read(0,&a,sizeof(int));
  printf("%d\n",a);
}
