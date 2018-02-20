#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int fds1[2], fds2[2];

int countPrimes (int upTo) {
    int number, count, factor, factorFound;
    for(number=2, count=0;number<=upTo;number++) {
        for(factor=2,factorFound=0; factor*factor<=number;factor++)
            factorFound = (number%factor==0);
        if(!factorFound)count++;
    }
    return count;
}

void* func_input(void* arg) {
    int num, result;
    for (;;) {
        read(fds1[READ], &num, sizeof(int));
        result = countPrimes(num);
        write(fds2[WRITE], &result, sizeof(int));
        if (result == 0) break;
    }
    pthread_exit(NULL);
}


void* func_output(void* arg) {
    int num, result;
    for (;;) {
        printf("Ange ett tal: ");
        scanf("%d", &num);
        if (write(fds1[WRITE], &num, sizeof(int)) < 0) {
            perror("cannot write thru pipe\n");
        }
        read(fds2[READ], &result, sizeof(int));
        printf("result: %d\n", result);
        if (result == 0) break;
    }
    pthread_exit(NULL);
}


void main() {
    int t;
    if (pipe(fds1) < 0 || pipe(fds2) < 0 ) {
        perror("pipe"); exit(-1);
    }
    pthread_t t1;
    pthread_t t2;
    t = pthread_create(&t1, NULL, func_input, 0); assert(t == 0);
    t = pthread_create(&t1, NULL, func_output, 0); assert(t == 0);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    close(fds1[READ]);
    close(fds1[WRITE]);
    close(fds2[READ]);
    close(fds2[WRITE]);
    printf("program terminated\n");

}
