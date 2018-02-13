#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main() {
    int pid;
    while(read(0, &pid, sizeof(int))) {
        if (kill(pid,9)==0)
            write(1,&pid,sizeof(int));
        else {
            pid =-pid;
            write(1,&pid,sizeof(int));
        }
    }
    exit(0);
}
