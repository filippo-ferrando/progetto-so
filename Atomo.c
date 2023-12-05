#include "lib/projectLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, 0666); //Semaforo per sincronizzare
    int sem_sm = semget(KEY_SEM_SM, 9, 0666); //Semaforo per accesso a variabili da stampare
    int sem_att = semget(KEY_ATT, 1, 0666); //Semaforo per accesso a variabili da stampare
    int n_atomico = atoi(argv[1]);

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
        //printf("atomo %d\n", getpid());
        //sleep(1);
        //assegna numero atomico
        //controlla scrap
        //aggiorna stats

        if(reserveSem(sem_sm, 0) < 0){
            perror("reserveSem attivazione atomo: ");
            exit(1);
        }
        switch(fork()){
            case -1:
                perror("fork atomo: ");
                //segnale di meltdown
                exit(1);
            case 0:
                //calcolo numero atomico
                //entro sezione critica di semaforo attivazione
                //entro in sezione critica di sm
                //scrivo energia rilasciata
                //esco sezione critica di sm
                //controllo se n_atomico < MIN_N_ATOMICO -> se si cancello atomo -> scrap++
                //forko
                //esco sezione critica di attivazione
            default:
                //calcolo n_atomico padre
        }

        exit(0);
    }
    //printf("SEM VAL atomo: %d\n", semctl(semid, 0, GETVAL));

    //printf("SONO ATOMO | HO PID %d | HO N ATOMICO %d\n", getpid(), atoi(argv[1]));
    exit(0);
}