#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
char table[50] = "";
int flag = 1;
pthread_mutex_t mut[9];
/*Example of illustration of five philosophers with
five chopsticks. Chopsticks are lying on the table*/
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
struct phil_parms
{
    int size;
    int pos;
    char *currentAction;
    int *chopsticks;
};

void *deadlock(struct phil_parms* parameters) {
    while(flag) {
        //printf("pos %d\n", parameters->pos);
        int pos = parameters->pos; //position of this phil
        int pos2 = pos +1;
        if (pos -1 == parameters->size) {
            pos2 = 0;
        }
        pthread_mutex_lock(&mut[pos]);
        parameters->chopsticks[pos] = "|";
        parameters->currentAction = "T";
        int sleepT = (random() % 5) + 2;
        printf("sleepT: %d\n", sleepT);
        sleep(sleepT);
        pthread_mutex_lock(&mut[pos2]);
        parameters->chopsticks[pos2] = "|";
        parameters->currentAction = "E";
        int sleepE = (random() % 3) + 1;
       // printf("sleeping: %d, im %d\n", sleepT, pos);
        sleep(sleepE);
        parameters->chopsticks[pos] = "_";
        pthread_mutex_unlock(&mut[pos]);
        parameters->chopsticks[pos2] = "_";
        pthread_mutex_unlock(&mut[pos2]);
        parameters->currentAction = "W";
        sleep(5);
    }
    pthread_exit(0);
}

void* philosophize (struct phil_parms* parameters) {
    while(flag) {
        //printf("pos %d\n", parameters->pos);
        int pos = parameters->pos; //position of this phil
        int pos2 = pos +1;
        printf("size %d\n", parameters->size);
        if (pos == parameters->size-1) {
            pos2 = 0;
        }

        if (pos < 0) {
            pthread_mutex_lock(&mut[pos2]);
            parameters->chopsticks[pos2] = "|";
            parameters->currentAction = "T";
            int sleepT = (random() % 5) + 2;
            printf("sleepT: %d\n", sleepT);
            sleep(sleepT);
            pthread_mutex_lock(&mut[pos]);
            parameters->chopsticks[pos] = "|";
            parameters->currentAction = "E";
            int sleepE = (random() % 3) + 1;
           // printf("sleeping: %d, im %d\n", sleepT, pos);
            sleep(sleepE);
            parameters->chopsticks[pos2] = "_";
            pthread_mutex_unlock(&mut[pos2]);
            parameters->chopsticks[pos] = "_";
            pthread_mutex_unlock(&mut[pos]);
            parameters->currentAction = "W";
        } else {
            pthread_mutex_lock(&mut[pos]);
            parameters->chopsticks[pos] = "|";
            parameters->currentAction = "T";
            int sleepT = (random() % 5) + 2;
            printf("sleepT: %d my pos: %d\n", sleepT, pos);
            sleep(sleepT);
            if (pthread_mutex_trylock(&mut[pos2]) == 0) {
                parameters->chopsticks[pos2] = "|";
                parameters->currentAction = "E";
                printf("im E, my pos %d, my sec pos %d\n", pos, pos2);
                int sleepE = (random() % 3) + 1;
               // printf("sleeping: %d, im %d\n", sleepT, pos);
                sleep(sleepE);
                parameters->chopsticks[pos] = "_";
                pthread_mutex_unlock(&mut[pos]);
                parameters->chopsticks[pos2] = "_";
                pthread_mutex_unlock(&mut[pos2]);
            } else {
                parameters->chopsticks[pos] = "_";
                pthread_mutex_unlock(&mut[pos]);
            }

            parameters->currentAction = "W";
            sleep(5);
        }
    }
    pthread_exit(0);
}
int main (int argc, char* argv[])
{
    pthread_t phils[9]; /*Some sort of array of phils are needed*/
    struct phil_parms control_phil[9];
    int i=0;
    int noPhils, lock;
    int round = 0;

    if (argc <= 2) {
        perror("Not enough args!\n"); return 1;
    }

    noPhils = atoi(argv[1]);
    lock = atoi(argv[2]);

    if(noPhils <= 0) {
        perror("not enough philosof..\n"); return 1;
    } else if(noPhils > 9) {
        perror("to many phils!\n"); return 1;
    }

    switch(lock) {
    case 0:
        printf("no deadlock\n"); break;
    case 1:
        printf("deadlock\n"); break;
    default:
        perror("not valid arg!\n"); return 1;
    }

    char* chopsticks[noPhils];

    for (i = 0; i< noPhils; i++) {
        pthread_mutex_init(&mut[i], NULL);
        chopsticks[i] = "_";

        control_phil[i].size = noPhils;
        control_phil[i].pos = i;
        control_phil[i].currentAction = "W";
        control_phil[i].chopsticks = chopsticks;
        if (lock) {
            pthread_create(&phils[i], NULL, &deadlock, &control_phil[i]);
        } else {
            pthread_create(&phils[i], NULL, &philosophize, &control_phil[i]);
        }
    }

    /*Overall design of the program
    1. Take in commandline arguments to set up how many phils are going to be
    simulated and if deadlock is going occur. Commandline arguments need to
    be checked and the program needs to exit if they are not in the correct format
    see Advanced Linux Programming for excellent advice on commandline arguments.
    2. Start simulation by starting the phil-threads and let the main program
    print out the contents of the string table declared above. No thread is going
    to communicate with the user but through the string table, it is the main
    program that prints out the contents of the string table. This means that
    we are separating the task of computation/simulation from the task of
    presentation of the results*/


    while(round<48)
    {
    strcpy(table, "");
    for (i = 0; i < noPhils; i++) {
        strcat(table, chopsticks[i]);
        strcat(table, control_phil[i].currentAction);
    }
    strcat(table, chopsticks[0]);

    printf("Round %2d: %s\n", round+1, table);
    sleep(1);
    round++;
    }
    flag = 0;
    printf("flag is 0\n");

    for (i = 0; i< noPhils; i++) {
        pthread_join(phils[i], NULL);
    }
    printf("end\n");

    /* The above loop runs in parallel to the threads/phils that affect the
    common resource table.
    IMPORTANT: The synchronization must not be through one mutex! We must have
    one mutex per chopstick, that means an array of mutexes is also needed!
    IMPORTANT: Remember that the program should also make deadlock possible
    through commandline arguments, there must be a way to force a deadlock to
    occur. Remember in this context that all thread-functions are to be based on
    one function, philosophize(), and that this function is the same for all
    threads.

    Of course it can behave differently for different philosopher-id's, but
    it must be one function which needs to be written to enable a forced deadlock
    so we can compare and understand why it normally avoids deadlock.
    3. When the loop has finished, all the threads are joined to the main program
    and then the main program exits.
    */
    return 0;
}
