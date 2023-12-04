#include "lib/projectLib.h"
#include "lib/key.h"

int main(){
    int semid = semget(KEY_SEM_ACT, 1, 0666);

    reserveSem(semid);
    //printf("SEM VAL attivatore: %d\n", semctl(semid, 0, GETVAL));
    printf("SONO ATTIVATORE | HO PID %d\n", getpid());
    exit(0);
}