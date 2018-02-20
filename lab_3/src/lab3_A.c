#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PHIL            9

char table[20] = "";


/*
_W_W|E|W_W_ This is when the third guy is eating.
_W|E|T_T_W_ This is when the second guy is eating.
 third and fourth are thinking. (Philosophizing.)
_W|E|T|E|W_ This is when the second and fourth guys are eating.
|E|W_W_W_W| This is when the first guy is eating.

 To note: The first "|" symbolizes one chopstick to
 the left of the first philosopher. But the last "|"
 is also symbolizing the same chopstick since they
 are sitting around a round table.

*/

struct phil_parms {
    int pos;
    int noPhils;
};



struct s_philosopher {
    int pos;
    int left;
    int right;
    int numPhils;
}



void* philosopher(void* arg) {
    struct  phil_parms* phil = (struct phil_parms*) arg;

    printf("Pos: %d, %d\n", phil->pos, phil->noPhils);


    pthread_exit(NULL);
}

void main(int argc, char* argv[]) {


    pthread_t phils[MAX_PHIL];
    struct phil_parms control_phil[MAX_PHIL];

    //struct chop_sticks sticks[MAX_PHIL];


    int i=0;
    int noPhils, lock;
    int round = 0;

    // number of philosophers
    noPhils = atoi(argv[1]); assert(noPhils > 0 && noPhils < MAX_PHIL);
    // if deadlock is going to occour
    lock = atoi(argv[2]); assert(lock == 0 || lock == 1);

    printf("Number of philosophers: %d, deadlock: %d\n", noPhils, lock);

    int left = 0;
    for (i = 0; i < noPhils; i++) {
        //control_phil[i].left_stick = i;
        //control_phil[i].right_stick = (i + 1) % noPhils;
        control_phil[i].noPhils = noPhils;
        control_phil[i].pos = i;
        pthread_create(&phils[i], NULL, philosopher, &control_phil[i]);
        strcat(table, "_W");
    }
    strcat(table, "_");

    while(round<48) {
        printf("Round %2d: %s\n", round+1, table);
        sleep(1);
        round++;
    }


}
*/
