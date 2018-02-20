#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define DEBUG_ENABLED   0

#define MAX_PHIL        9
#define P_WAIT          0
#define P_EAT           1
#define P_THINK         2

struct Stick {
    bool used;
};

struct Philosopher {
    int pos;
    int left;
    int right;
    int num_phils;
    int status;
};

struct Stick stick[MAX_PHIL];
static pthread_mutex_t mutex[MAX_PHIL];

/* A semaphore counting the number of jobs in the queue.  */
sem_t job_count;
pthread_mutex_t job_mutex = PTHREAD_MUTEX_INITIALIZER;


void show_table(struct Stick *s, struct Philosopher *p, int round, int max) {
    int i;
    printf("round: %d:\t", round);
    for (i = 0; i < max; i++) {
        if (s[i].used == true)
            printf(" | ");
        else
            printf(" _ ");
        switch(p[i].status) {
            case P_EAT: printf("E"); break;
            case P_THINK: printf("T"); break;
            default: printf("W");
        }
    }
    if (s[0].used == true)
        printf(" | ");
    else
        printf(" _ ");
    printf("\n");
}

/** P H I L O S O P H E R S  A C T I O N S **/

/* Arbitrator solution */

/*
void pickup_left_right_arbitor(struct Philosopher* p) {
    int w;
    while (stick[p->left].used != false && ) {
}
*/

void pickup_left(struct Philosopher* p) {
    int w;
    while (stick[p->left].used != false) {
        w = rand() % 5 + 2;
        #if DEBUG_ENABLED
        printf("%d] thinking for %d (left) -- %d\n", p->pos, p->left, w);
        #endif
        p->status = P_THINK;
        sleep(w);
    }
    p->status = P_WAIT;
    pthread_mutex_lock(&mutex[p->left]);
    stick[p->left].used = true;
    #if DEBUG_ENABLED
    printf("%d] picked up %d (left)\n", p->pos, p->left);
    #endif
    pthread_mutex_unlock(&mutex[p->left]);
}

void pickup_right(struct Philosopher* p) {
    int w;
    while (stick[p->right].used != false) {
        p->status = P_THINK;
        w = rand() % 5 + 2;
        #if DEBUG_ENABLED
        printf("%d] thinking for %d (right) -- %d\n", p->pos, p->right, w);
        #endif
        sleep(w);
    }
    p->status = P_WAIT;
    pthread_mutex_lock(&mutex[p->right]);
    stick[p->right].used = true;
    #if DEBUG_ENABLED
    printf("%d] picked up %d (right)\n", p->pos, p->right);
    #endif
    pthread_mutex_unlock(&mutex[p->right]);
}

void consume(struct Philosopher* p) {
    int w = rand() % 3 + 1;
    p->status = P_EAT;
    #if DEBUG_ENABLED
    printf("%d] consuming -- %d\n", p->pos, w);
    #endif
    sleep(w);
}

void drop_right(struct Philosopher* p) {
    p->status = P_WAIT;
    pthread_mutex_lock(&mutex[p->right]);
    stick[p->right].used = false;
    pthread_mutex_unlock(&mutex[p->right]);
    sleep(1);
}

void drop_left(struct Philosopher* p) {
    p->status = P_WAIT;
    pthread_mutex_lock(&mutex[p->left]);
    stick[p->left].used = false;
    pthread_mutex_unlock(&mutex[p->left]);
    sleep(1);
}

/** Very likely dead lock **/

void* phil_func(void* arg) {
    int w;
    struct Philosopher* p = (struct Philosopher*) arg;
    p->status = P_WAIT;
    while (true) {
        pickup_left(p);
        pickup_right(p);
        consume(p);
        drop_right(p);
        drop_left(p);
    }
}

/** Guaranteed deadlock **/

void* phil_func_gd(void* arg) {
    int w;
    struct Philosopher* p = (struct Philosopher*) arg;
    p->status = P_WAIT;
    while (true) {
        pickup_left(p);
        sleep(1); // ^_^
        pickup_right(p);
        consume(p);
        drop_right(p);
        drop_left(p);
    }
}

/** Guaranteed not to lock using arbitration **/
void* phil_func_ndl(void* arg) {
    int w;
    struct Philosopher* p = (struct Philosopher*) arg;
    p->status = P_WAIT;
    while (true) {
        sem_wait(&job_count);
        //printf("%d\n", job_count);

        pickup_left(p);
        pickup_right(p);

        sem_post(&job_count);
        //printf("%d\n", job_count);

        consume(p);
        drop_right(p);
        drop_left(p);
    }
}


void main(int argc, char* argv[]) {


    srand(time(NULL));  // Seed random numbers

    int noPhils;        // number of philosophers
    bool lock;          // should dead-lock occour
    int i, round;

    noPhils = atoi(argv[1]); assert(noPhils > 0 && noPhils < MAX_PHIL);
    lock = atoi(argv[2]); assert(lock == 0 || lock == 1);     // if deadlock is going to occour
    printf("Number of philosophers: %d, deadlock: %d\n", noPhils, lock);

    for (i = 0; i < noPhils; i++) {
        stick[i].used = false;
        pthread_mutex_init(&mutex[i], NULL);
    }

    sem_init(&job_count, 0, noPhils/2);

    struct Philosopher phil[noPhils];
    pthread_t phil_t[noPhils];
    for (i = 0; i < noPhils; i++) {
        phil[i].pos = i;
        phil[i].left = i;
        phil[i].right = (i + 1) % noPhils;
        phil[i].num_phils = noPhils;
        printf("%d--[%d]--%d\n", phil[i].left, phil[i].pos, phil[i].right);
        if (lock == true) {
            pthread_create(&phil_t[i], NULL, phil_func_gd, &phil[i]);
        } else {
            pthread_create(&phil_t[i], NULL, phil_func_ndl, &phil[i]);
        }
    }
    sleep(1);
    for (round = 0; round < 48; round++) {
        show_table(stick, phil, round + 1, noPhils);
        sleep(1);
    }
    round = 0;

}
