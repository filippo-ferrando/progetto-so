#include "lib/projectLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, 0666); //Semaforo per sincronizzare
    int sem_sm = semget(KEY_SEM_SM, 9, 0666); //Semaforo per accesso a variabili da stampare

    //printf("invocato\n");
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0666);
    st = shmat(shmid, NULL, 0);

    if(atoi(argv[2]) != -1){
        if(reserveSem(sem_start, 0) < 0){
            perror("reserveSem");
            exit(1);
        }
    }

    while(1){
        //sblocca semaforo
        //forka
        printf("atomo %d\n", getpid());
        sleep(1);
        //assegna numero atomico
        //controlla scrap
        //aggiorna stats
        exit(0);
    }
    //printf("SEM VAL atomo: %d\n", semctl(semid, 0, GETVAL));

    //printf("SONO ATOMO | HO PID %d | HO N ATOMICO %d\n", getpid(), atoi(argv[1]));
    exit(0);
}