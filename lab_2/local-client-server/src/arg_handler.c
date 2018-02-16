#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_ENABLED 1

void main(int argc, char* argv[]) {

    int n, i, j, k, o;
    char* srcs = argv[1];
    char output[1000][100];
    for (i = 0, j = 0, o = 0; i < strlen(srcs); i++) {
        if (srcs[i] == ' ') {
            for (k = 0; j < i; j++, k++) {
                output[o][k] = srcs[j];
                #if DEBUG_ENABLED
                printf("copying to...%d: %c\n", j, srcs[j]);
                #endif
            }
            output[o][k] = '\0';
            #if DEBUG_ENABLED
            printf("%d\n", k);
            printf("%s\n", output[o]);
            #endif
            o++;
            j++;
        }
    }
    int changed = 0;
    for (k = 0; j < strlen(srcs); j++, k++) {
        output[o][k] = srcs[j];
        changed = 1;
    }
    if (changed)
        o++;
    #if DEBUG_ENABLED
    printf("results\n");
    for (i = 0; i < o; i++) {
        printf("%d: %s\n", i, output[i]);
    }
    printf("argc=%d\n", o);
    #endif

}
