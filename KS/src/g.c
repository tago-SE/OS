#include <unistd.h>
void main() {
    int pid=getpid();
    write(1,&pid,sizeof(int));
}
