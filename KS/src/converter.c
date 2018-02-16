#include <stdio.h>
#include <stdlib.h>

void main(int argc, char* argv[]) {

    int a = atoi(argv[1]);

    printf("int: %d\n", a);

    char* str = malloc(sizeof(char)*10);

    sprintf(str, "%d", a);

    printf("str: %s\n", str);
}
