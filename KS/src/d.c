#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main() {
    int p;
    while(read(0, &p, sizeof(int)))
        printf("p = %d.\n", p);
}
