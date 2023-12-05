#include "lib/projectLib.h"
#include "lib/key.h"

int main(){
    int sem_start = semget(KEY_SEM_ACT, 1, 0666);
    int sem_sm = semget(KEY_SEM_SM, 9, 0666);

    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0666);
    st = shmat(shmid, NULL, 0);

    reserveSem(sem_start, 0);
    //printf("SEM VAL attivatore: %d\n", semctl(semid, 0, GETVAL));
    printf("SONO ATTIVATORE | HO PID %d\n", getpid());
    exit(0);
}