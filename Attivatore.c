#include "lib/generalLib.h"
#include "lib/key.h"

void handle_SIGUSR1(int signal){
    exit(0);
}

int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);
    int sem_sm = semget(KEY_SEM_SM, 13, 0777);
    int sem_attivatore_ready = semget(KEY_PROC_READY, 1, 0777);

    int sem_att = semget(KEY_ATT, 1, 0777);

    //file per gestione rimozione di risorse ipc
    FILE *ipcs_id_s = fopen("ipcs_id_sem.txt", "a");
    fprintf(ipcs_id_s, "%d\n", sem_att);
    fclose(ipcs_id_s);

    //struct per nanosleep
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = atoi(argv[1]);  

    //numero di attivazioni che l'attivatore deve fare
    int n_attivazioni = atoi(argv[2]);    

    struct sigaction sa; //Aggiunta ora
    bzero(&sa, sizeof(sa)); //Aggiunta ora
	sa.sa_handler = handle_SIGUSR1; //Aggiunta ora
    sigaction(SIGUSR1, &sa, NULL); //Aggiunta ora

    //attach to shared memory
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0777);
    st = shmat(shmid, NULL, 0);
    
    if(releaseSem(sem_attivatore_ready, 0) < 0){
        perror("releaseSem attivatore ready: ");
        exit(1);
    }


    //sincronizzo attivatore con resto dei processi
    if(reserveSem(sem_start, 0) < 0){
        perror("reserveSem start attivatore: ");
        exit(1);
    
    }
    //printf("SONO ATTIVATORE | HO PID %d\n", getpid());

    while(1){
        //riservo semaforo di sm per scrivere n attivazione
        if(reserveSem(sem_sm, 0) < 0){
            perror("reserveSem sm attivazioni attivatore: ");
            exit(1);
        }
        //printf("attivatore in sezione critica sm\n");
        st->activations_ls+=n_attivazioni;
        if(releaseSem(sem_sm, 0) < 0){
            perror("releaseSem sm attivazioni attivatore: ");
            exit(1);
        }
        //printf("attivatore rilasica sem att\n");
        if(semctl(sem_att, 0, SETVAL, n_attivazioni) < 0){
            perror("semctl attivatore: ");
            exit(1);
        }

        nanosleep(&remaining, &request);
        
    }    
    exit(0);
}