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

    //get semaforo sm
    int sem_sm = semget(KEY_SEM_SM, 13, 0777);
    
    //get semaforo attivatore
    int sem_att = semget(KEY_ATT, 1, 0777);

    //get semaforo atom_start_ready
    int sem_atom_start_ready = semget(KEY_PROC_READY, 1, 0777);

    //get semaforo sm atom
    int sem_sm_atom = semget(KEY_SEM_SM_ATOM, 1, 0777);

    //attach to shared memory
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0777);
    st = shmat(shmid, NULL, 0);

    //sm only for atom
    struct atoms *st_atom;
    int shmid_atom = shmget(KEY_SHM_ATOM, sizeof(st_atom), 0777);
    st_atom = shmat(shmid_atom, NULL, 0);

    //attach to message queue

    int msgid = msgget(KEY_INHIB,0777); //msgid tiene id per comunicare con inibitore
    struct message_buf messaggio;

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
        //start impartito da Master
        if(reserveSem(sem_att, 0) < 0){    
            printf("sem_att: %d\n", sem_att);        
            perror("reserveSem attivatore atomo: ");
            exit(1);
        }

        if(st_atom->n <= 0){ //N.B: st_atom può avere anche valori negativi
            msgrcv(msgid, &messaggio, sizeof(messaggio.mex), 1, IPC_NOWAIT);    //se il messaggio è di tipo 1 -> inibitore ha mandato messaggio -> atomo deve morire
            if(errno != ENOMSG){
                if(messaggio.mex != 0){ 
                    if(reserveSem(sem_sm_atom, 0) < 0){
                        perror("reserveSem sm atom atomo: ");
                        exit(1);
                    }
                    st_atom->n = messaggio.mex;                    
                    if(releaseSem(sem_sm_atom, 0) < 0){
                        perror("releaseSem sm atom atomo: ");
                        exit(1);
                    }
                }
            }
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
        
        switch(fork()){
            case -1:
                //perror("fork atomo: ");
                //segnale di meltdown
                kill(pid_master, SIGUSR2);
                exit(1);
            case 0:
                //assegnazione numero atomico
                n_padre = n_atomico;
                n_atomico = rand_soglia;

                //AGGIORNO COUNTER ATOMI DOPO SCISSIONE
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

                //controllo se ultimo messaggio ricevuto è di tipo 3 -> se si -> energia viene prelevata
                st->energy_created_ls += energy_released(n_atomico, n_padre);
                //agggiorno counter di scissioni
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

                //controllo se counter atomi da uccidere > 0 -> se si -> uccido atomo
                if(st_atom->n > 0){
                    if(reserveSem(sem_sm, 9) < 0){
                        perror("reserveSem sm scrap_ls atomo: ");
                        exit(1);
                    }
                    if(reserveSem(sem_sm, 12) < 0){
                        perror("reserveSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    if(reserveSem(sem_sm, 10) < 0){
                        perror("reserveSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    
                    if(reserveSem(sem_sm_atom, 0) < 0){
                        perror("reserveSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    

                    st_atom->n--;
                    st->current_atoms--;
                    st->scrap_inibitore++;
                    st->scrap_ls++;

                    if(releaseSem(sem_sm, 9) < 0){
                        perror("releaseSem sm scrap_ls atomo: ");
                        exit(1);
                    }
                    if(releaseSem(sem_sm, 12) < 0){
                        perror("releaseSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    if(releaseSem(sem_sm, 10) < 0){
                        perror("releaseSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    
                    if(releaseSem(sem_sm_atom, 0) < 0){
                        perror("releaseSem sm atomi alimentatore: ");
                        exit(1);
                    }
                    
                    exit(0);                    
                }
        }
        //aspetto eventuali processi figli zombie
        while(waitpid(-1, NULL, 0)>0);
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
