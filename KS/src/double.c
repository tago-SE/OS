#include <unistd.h>
#include <stdio.h>
main()
{
    int a;
   // while(read(0,&a,sizeof(int))) {
        a=2*a;
   //     write(1,&a,sizeof(int));
    //}
    read(0,&a,sizeof(int)); a=2*a;
    write(1,&a,sizeof(int));
}
