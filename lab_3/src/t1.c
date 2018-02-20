#include <stdio.h>
#include <assert.h>
#include <pthread.h>

void *mythread(void *arg) {
    int m = (int) arg;
    m = m*2;
    printf("%d\n", m);
    return (void*) (m);
}

int main() {
    pthread_t p1;int rc, m;
    rc = pthread_create(&p1, NULL, mythread, (void*) 100); assert(rc == 0);
    rc = pthread_join(p1, (void **) &m); assert(rc == 0);
    printf("Returned: %d\n", m);
    return 0;
}
