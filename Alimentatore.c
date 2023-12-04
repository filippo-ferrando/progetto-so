#include "lib/projectLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    int semid = semget(KEY_SEM_ACT, 1, 0666);
    printf("invocato\n");
    reserveSem(semid);
    //printf("SEM VAL alimentatore: %d\n", semctl(semid, 0, GETVAL));
    printf("SONO ALIMENTATORE | HO PID %d | HO STEP %d \n", getpid(), atoi(argv[1]), atoi(argv[2]));
    exit(0);
}