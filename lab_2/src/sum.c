#include <stdio.h>
#include <stdbool.h>

bool isDigit(char c) {
  return c >= '0' && c <= '9';
}

int main(int argc, char* argv[]){
    int i, num, sum;
    for (i = 1, sum = 0; i < argc; i++) {
        num = atoi(argv[i]);
        if ((num == 0 && isDigit(argv[i][0])) || num != 0) {
            sum += num;
        }
        else {
          // ignore
        }
    }
    printf("sum = %d\n", sum);
    return 0;
}
