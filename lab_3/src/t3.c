#include <pthread.h>
#include <stdio.h>

struct thread_arg {
    char ch;
    int count;
};

void* thread_func(void* arg) {
    int i;
    struct thread_arg* t_arg = (struct thread_arg*) arg;

    for (i = 0; i < t_arg->count; i++) {
        printf("%c\n", t_arg->ch);
    }
}

int main() {
    pthread_t t1_id;
    pthread_t t2_id;

    struct thread_arg t1_arg;
    struct thread_arg t2_arg;

    t1_arg.ch = 'o';
    t1_arg.count = 3;
    pthread_create(&t1_id, NULL, thread_func, &t1_arg);
    t2_arg.ch = 'x';
    t2_arg.count = 2;
    pthread_create(&t2_id, NULL, thread_func, &t2_arg);
    pthread_join(t2_id, NULL);
    pthread_join(t1_id, NULL);

    return 0;
}
