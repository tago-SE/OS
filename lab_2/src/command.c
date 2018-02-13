#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

int main(int argc, char* argv[]){
    int i, num;

    printf("argc= %d\n", argc);
    printf("Arguments:\n");
    for (i = 1; i < argc; i++) {
        num = atoi(argv[i]);
        if ((num == 0 && isDigit(argv[i][0])) || num != 0) {
        printf("number: %d\n", num);
        }
    else {
      printf("word: %s\n", argv[i]);
    }
    }
    return 0;
}
