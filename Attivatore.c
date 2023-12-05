#include "lib/projectLib.h"
#include "lib/key.h"

//handler per gestire segnale di pausa da inibitore

int main(){
    int sem_start = semget(KEY_SEM_ACT, 1, 0666);
    int sem_sm = semget(KEY_SEM_SM, 9, 0666);
    int sem_att = semget(KEY_ATT, 1, 0666);

    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0666);
    st = shmat(shmid, NULL, 0);

    reserveSem(sem_start, 0);

    while(1){
        //riservo semaforo di sm per scrivere n attivazione
        //incremento di n semaforo att
        //aspetto
        //ripeto
    }
    //printf("SEM VAL attivatore: %d\n", semctl(semid, 0, GETVAL));
    printf("SONO ATTIVATORE | HO PID %d\n", getpid());
    exit(0);
}