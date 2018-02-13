#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int countPrimes (int upTo) {
    int number, count, factor, factorFound;
    for(number=2, count=0;number<=upTo;number++) {
        for(factor=2,factorFound=0; factor*factor<=number;factor++)
            factorFound = (number%factor==0);
        if(!factorFound)count++;
    }
    return count;
}


int main() {
    int request[2], response[2], value, result;

    if (pipe(request) < 0 || pipe(response) < 0) {
        perror("Can't create pipes\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed to fork\n");
        return -1;
    }
    else if (pid == 0) {
        /* Disabled std_in, std_out */
        close(READ);
        close(WRITE);

        close(request[WRITE]);
        close(response[READ]);
        for (;;) {
            read(request[READ], &value, sizeof(value));
            result = countPrimes(value);
            write(response[WRITE], &result, sizeof(result));
            if (result == 0) exit(0);
        }
    }
    else {
        close(request[READ]);
        close(response[WRITE]);
        for (;;) {
            printf("Ange ett tal: ");
            scanf("%d", &value);
            if (sizeof(value) != write(request[WRITE], &value, sizeof(value))) {
                perror("Cannot write thru pipe.\n");
            }
            read(response[READ], &result, sizeof(result));
            printf("Result: %d.\n", result);
            if (result == 0) {
                break;
            }
        }
        wait();
        printf("Program terminated\n");
    }
    return 0;
}
