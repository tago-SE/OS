#include <stdio.h>
#include <assert.h>
#include <pthread.h>

void* func(void* arg) {
    int n = (int) arg;
    int x = pthread_self();
    printf("In funtion \nthread id = %d\n", pthread_self());
    pthread_exit(NULL);
    return NULL;
}

void main() {
    int t;
    pthread_t t1;
    t = pthread_create(&t1, NULL, func, (void**) 5); assert(t == 0);
    printf("In main \nthread id = %d\n", t1);
    pthread_join(t1, NULL);

}
