#include "lib/generalLib.h"
#include "lib/key.h"

int energy_released(int n1, int n2);

int main(int argc, char* argv[]){
    int n_atomico = atoi(argv[1]);
    int bypass = atoi(argv[2]);
    int min_n_atomico = atoi(argv[3]);
    int pid_master = atoi(argv[4]);
    int rand_soglia = 0;
    int n_padre = 0;

    //get semaforo sync-start
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);
    //printf("sem_start: %d\n", sem_start);

    //get semaforo sm
    int sem_sm = semget(KEY_SEM_SM, 13, 0777);
    //printf("sem_sm: %d\n", sem_sm);
    
    //get semaforo attivatore
    int sem_att = semget(KEY_ATT, 1, 0777);
    //printf("sem_att: %d\n", sem_att);

    int sem_atom_start_ready = semget(KEY_PROC_READY, 1, 0777);

    //attach to shared memory
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0777);
    st = shmat(shmid, NULL, 0);

    //attach to message queue

    int msgid = msgget(KEY_INHIB,0777); //msgid tiene id per comunicare con inibitore
    struct message_buf messaggio;
    
    //struct per nanosleep -> 0,5s
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 200000000;  //0,5s da definire metodo di decisione tempo di attivazione
    
    //se atomo viene creato da master deve aspettare il semaforo di start -> alimentatore setta bypass != 0 -> atomo bypassa semaforo
    if(bypass == 0){
        if(releaseSem(sem_atom_start_ready, 0) < 0){
            perror("releaseSem atom_start_ready atomo: ");
            exit(1);
        }
        if(reserveSem(sem_start, 0) == -1){
            printf("sem_start: %d\n", sem_start);
            perror("reserveSem start master atomo: ");
            exit(1);
        }
    }

    while(1){

        //se ci sono processi figli zombie, li elimino, altrimenti passo oltre -> WOHNANG settato
        while(waitpid(-1, NULL, WNOHANG)>0);

        if(reserveSem(sem_att, 0) < 0){    
            printf("sem_att: %d\n", sem_att);        
            perror("reserveSem attivatore atomo: ");
            exit(1);
        }

        //calcolo numero atomico
        srand(getpid());
        rand_soglia = rand() % n_atomico + 1;
        n_atomico -= rand_soglia;
        
        while(waitpid(-1, NULL, WNOHANG)>0);

        //controllo se n_atomico < MIN_N_ATOMICO -> se si cancello atomo -> scrap++
        
        if(n_atomico <= min_n_atomico){
            if(reserveSem(sem_sm, 9) < 0){
                perror("reserveSem sm scrap atomo: ");
                exit(1);
            }
            if(reserveSem(sem_sm, 12) < 0){
                perror("reserveSem sm atomi alimentatore: ");
                exit(1);
            }
            st->scrap_ls++;
            st->current_atoms--;
            //printf("atomo %d scrap\n", getpid());
            if(releaseSem(sem_sm, 9) < 0){
                perror("releaseSem sm scrap atomo: ");
                exit(1);
            }
            if(releaseSem(sem_sm, 12) < 0){
                perror("releaseSem sm atomi alimentatore: ");
                exit(1);
            }
            exit(0);
        }
        
        //forko
        /*
        *IF LAST MSG_TYPE == 3 -> n_atomico (figlio) = 1 -> scrap++
        */
        switch(fork()){
            case -1:
                //perror("fork atomo: ");
                //segnale di meltdown
                kill(pid_master, SIGUSR2);
                exit(1);
            case 0:
                //printf("atomo figlio %d creato\n", getpid());
                n_padre = n_atomico;
                n_atomico = rand_soglia;

                

                if(reserveSem(sem_sm, 12) < 0){
                perror("reserveSem sm atomi alimentatore: ");
                exit(1);
                }
                st->current_atoms++;
                if(releaseSem(sem_sm, 12) < 0){
                    perror("releaseSem sm atomi alimentatore: ");
                    exit(1);
                }

                //entro in sezione critica di sm per  split e energy_created_ls
                if(reserveSem(sem_sm, 2) < 0){
                    perror("reserveSem sm atomo: ");
                    exit(1);
                }
                if(reserveSem(sem_sm, 4) < 0){
                    perror("reserveSem sm atomo: ");
                    exit(1);
                }
                msgrcv(msgid, &messaggio, sizeof(messaggio.mex), 3, IPC_NOWAIT); //se il messaggio che POSSO ricevere è tipo 3 -> l'energia rilasciata si dimezza
                if(errno != ENOMSG){
                    //printf("atomo %d ricevuto messaggio\n", getpid());
                    if(reserveSem(sem_sm, 11) < 0){
                        perror("reserveSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    st->energy_absorbed_inibitore += energy_released(n_atomico, n_padre) / 2;
                    if(releaseSem(sem_sm, 11) < 0){
                        perror("releaseSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    //printf("atomo %d rilascia energia dimezzata\n", getpid());
                    st->energy_created_ls -= energy_released(n_atomico, n_padre) / 2;
                }else{
                    st->energy_created_ls += energy_released(n_atomico, n_padre);
                }

                st->split_ls++;
                
                //esco sezione critica di sm
                if(releaseSem(sem_sm, 2) < 0){
                    perror("releaseSem sm atomo: ");
                    exit(1);
                }
                if(releaseSem(sem_sm, 4) < 0){
                    perror("releaseSem sm atomo: ");
                    exit(1);
                }

                msgrcv(msgid, &messaggio, sizeof(messaggio.mex), 1, IPC_NOWAIT);    //se il messaggio è di tipo 1 -> inibitore ha mandato messaggio -> atomo deve morire

                if(errno != ENOMSG){
                    //printf("atomo %d ricevuto messaggio\n", getpid());
                    if(reserveSem(sem_sm, 9) < 0){
                        perror("reserveSem sm scrap_ls atomo: ");
                        exit(1);
                    }
                    if(reserveSem(sem_sm, 12) < 0){
                        perror("reserveSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    printf("atomo %d scrap\n", getpid());
                    st->current_atoms--;
                    st->scrap_ls++;
                    if(releaseSem(sem_sm, 9) < 0){
                        perror("releaseSem sm scrap_ls atomo: ");
                        exit(1);
                    }
                    exit(0);
                }
        }
        //nanosleep(&remaining, &request);
    }
    exit(0);
}

int energy_released(int n1, int n2){
    int max;
    if(n1 > n2){
        max = n1;
    }else{
        max = n2;
    }
    return ((n1 * n2) - max);
}