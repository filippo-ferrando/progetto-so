#include "lib/projectLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, IPC_CREAT | 0666); //Semaforo per sincronizzare
    
    int sem_sm = semget(KEY_SEM_SM, 9, IPC_CREAT | 0666); //Semaforo per accesso a variabili da stampare
    int sem_att = semget(KEY_ATT, 1, IPC_CREAT | 0666); 
    //Semaforo per accesso a variabili da stampare
    
    int n_atomico = atoi(argv[1]);
    int energia_rilasciata = 7;
    int min_n_atomico = atoi(argv[3]);

    int bypass = atoi(argv[2]);

    int pid_master = atoi(argv[4]); 

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 500000000;
    //Salvo il PID del Master. La logica è la seguente; se la fork causa meltdown, mando al padre SIGUSR2


    //printf("invocato\n");
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), IPC_CREAT | 0666);
    st = shmat(shmid, NULL, 0);


    if(bypass == 0){
        printf("ATOMO CREATO DA MASTER\n");
        if(reserveSem(sem_start, 0) < 0){
            perror("reserveSem attivatore atomo: ");
            exit(1);
        }
    }else{
        printf("ATOMO CREATO DA ALIMENTATORE\n");
        printf("atomo passato con %d\n", atoi(argv[2]));
    }
    
    

    while(1){
        //################# TEST #################
        /*
        if(atoi(argv[2]) == -1){
            printf("atomo creato da alimentatore %d\n", getpid());
        }else{
            printf("atomo creato da master %d\n", getpid());
        }
        */
        //################# TEST #################
        
        if(reserveSem(sem_att, 0) < 0){
            perror("reserveSem attivatore atomo: ");
            exit(1);
        }
        //printf("qua\n");
        //calcolo numero atomico
        n_atomico = n_atomico / 2;
        
        waitpid(-1, NULL, WNOHANG);

        //controllo numero atomico e aumento scrap
        if(n_atomico <= min_n_atomico){
            if(reserveSem(sem_sm, 8) < 0){
                perror("reserveSem sm scrap atomo: ");
                exit(1);
            }
            st->scrap++;
            //printf("atomo %d scrap\n", getpid());
            if(releaseSem(sem_sm, 8) < 0){
                perror("releaseSem sm scrap atomo: ");
                exit(1);
            }
            exit(0);
        }
        //forko
        switch(fork()){
            case -1:
                //perror("fork atomo: ");
                //segnale di meltdown
                kill(pid_master, SIGUSR2);
                exit(1);
            case 0:
                //controllo se n_atomico < MIN_N_ATOMICO -> se si cancello atomo -> scrap++
                //printf("atomo figlio %d creato\n", getpid());

                //entro in sezione critica di sm per 
                if(reserveSem(sem_sm, 2) < 0){
                    perror("reserveSem sm atomo: ");
                    exit(1);
                }
                if(reserveSem(sem_sm, 4) < 0){
                    perror("reserveSem sm atomo: ");
                    exit(1);
                }
                //printf("atomo figlio %d in senzione critica\n", getpid());
                st->split_ls++;
                st->energy_created_ls += energia_rilasciata;
                //esco sezione critica di sm
                if(releaseSem(sem_sm, 2) < 0){
                    perror("releaseSem sm atomo: ");
                    exit(1);
                }
                if(releaseSem(sem_sm, 4) < 0){
                    perror("releaseSem sm atomo: ");
                    exit(1);
                }
        }
        nanosleep(&remaining, &request);
    }
    //printf("SEM VAL atomo: %d\n", semctl(semid, 0, GETVAL));

    //printf("SONO ATOMO | HO PID %d | HO N ATOMICO %d\n", getpid(), atoi(argv[1]));
    exit(0);
}