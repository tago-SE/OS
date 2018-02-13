#include <unistd.h>
main()
{
  int a;
  read(0,&a,sizeof(int)); a=2*a;
  write(1,&a,sizeof(int));
}
