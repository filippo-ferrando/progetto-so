#include "lib/projectLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    int semid = semget(KEY_SEM_ACT, 1, 0666);
    struct sembuf sops;
    //printf("invocato\n");

    reserveSem(semid);
    //printf("SEM VAL atomo: %d\n", semctl(semid, 0, GETVAL));

    printf("SONO ATOMO | HO PID %d | HO N ATOMICO %d\n", getpid(), atoi(argv[1]));
    exit(0);
}