#include "lib/generalLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    int n_atomico = atoi(argv[1]);
    int bypass = atoi(argv[2]);
    int min_n_atomico = atoi(argv[3]);
    int pid_master = atoi(argv[4]);

    //get semaforo sync-start
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);
    //printf("sem_start: %d\n", sem_start);

    //get semaforo sm
    int sem_sm = semget(KEY_SEM_SM, 9, 0777);
    //printf("sem_sm: %d\n", sem_sm);
    
    //get semaforo attivatore
    int sem_att = semget(KEY_ATT, 1, 0777);
    //printf("sem_att: %d\n", sem_att);

    //attach to shared memory
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0777);
    st = shmat(shmid, NULL, 0);
    
    int energia_rilasciata = 7; // energia rilasciata sarà calcolata con una funzione fornita dai professori
    
    //struct per nanosleep -> 0,5s
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 500000000;  //0,5s da definire metodo di decisione tempo di attivazione
    
    //se atomo viene creato da master deve aspettare il semaforo di start -> alimentatore setta bypass != 0 -> atomo bypassa semaforo
    if(bypass == 0){
        if(reserveSem(sem_start, 0) == -1){
            printf("sem_start: %d\n", sem_start);
            perror("reserveSem start master atomo: ");
            exit(1);
        }
    }

    while(1){   
        if(reserveSem(sem_att, 0) < 0){            
            perror("reserveSem attivatore atomo: ");
            exit(1);
        }

        //calcolo numero atomico
        n_atomico = n_atomico / 2; // funzione da scegliere
        
        //se ci sono processi figli zombie, li elimino, altrimenti passo oltre -> WOHNANG settato
        waitpid(-1, NULL, WNOHANG);

        //controllo se n_atomico < MIN_N_ATOMICO -> se si cancello atomo -> scrap++
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
                //printf("atomo figlio %d creato\n", getpid());

                //entro in sezione critica di sm per  split e energy_created_ls
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
    exit(0);
}