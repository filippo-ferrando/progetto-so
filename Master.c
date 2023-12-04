#include "lib/projectLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    char* ENERGY_DEMAND = env_get_ENERGY_DEMAND();
    char* N_ATOMI_INIT = env_get_N_ATOMI_INIT();
    char* N_ATOM_MAX = env_get_N_ATOM_MAX();
    char* MIN_N_ATOMICO = env_get_MIN_N_ATOMICO();
    char* N_NUOVI_ATOMI = env_get_N_NUOVI_ATOMI();
    char* SIM_DURATION = env_get_SIM_DURATON();
    char* ENERGY_EXPLODE_THRESHOLD = env_get_ENERGY_EXPLODE_THRESHOLD();
    char* STEP = env_get_STEP();

    struct stats *st;

    //semafori partenza simulazione
    int sem_master_ready;

    sem_master_ready = semget(KEY_SEM_ACT, 1, IPC_CREAT | 0666);
    semctl(sem_master_ready, 0, SETVAL, 0);

    //semaforo shared memory
    int sem_sm_ready = semget(KEY_SEM_SM, 9, IPC_CREAT | 0666);
    for(int i=0; i<9; i++){
        semctl(sem_sm_ready, i, SETVAL, 0);
    }
    
    //shared memory
    int shmid = shmget(KEY_SHM, sizeof(st), IPC_CREAT | 0666);
    st = shmat(shmid, NULL, 0);

    st->activations_ls = 0;         //sem 0
    st->activations_total = 0;      //sem 1
    st->split_ls = 0;               //sem 2
    st->split_total = 0;            //sem 3
    st->energy_created_ls = 0;      //sem 4
    st->energy_created_total = 0;   //sem 5
    st->energy_consumed_ls = 0;     //sem 6
    st->energy_consumed_total = 0;  //sem 7
    st->scrap = 0;                  //sem 8

    /*
    struct sigaction sa;
    */
    /*
    bzero(&sa, sizeof(sa));
	sa.sa_handler = handle_signal;
    sigaction(SIGALRM, &sa, NULL);
    */
    
    //semafori gestione shared memory

    //alarm per durata simulazione

    //ciclo infinito di stampa con sleep(1)


    

    //ciclo creazione processi atomo
    //char* argv_atomo[3];
    char* buf = malloc(2);
    char* argv_atomo[3] = {"./Atomo.out", buf, NULL};

    int n_atom_rand;
    
    for(int i=0; i<atoi(N_ATOMI_INIT); i++){
        n_atom_rand = rand() % atoi(N_ATOM_MAX) + 1;
        //printf("atomo %d n atomico %d\n", i, n_atom_rand);
        sprintf(buf, "%d", n_atom_rand);
        
        strcpy(argv_atomo[1],buf);
        if(fork() == 0){
            if(execve("./Atomo.out", argv_atomo, NULL) < 0){
                perror("execve");
                exit(1);
            }
            break;
        }
        
        
    }

    //creazione processo alimentatore; Argomenti: STEP
    printf("Creo alimentatore\n");
    char* argv_alimentatore[] = { "Alimentatore", STEP, N_NUOVI_ATOMI, N_ATOM_MAX, NULL };
    switch(fork()){
        case -1:
            perror("fork");
            exit(1);
        case 0:
            if(execve("./Alimentatore.out",argv_alimentatore,NULL) < 0){
                perror("execve");
                exit(1);
            }
            break;
        default:
            break;
    }

    
    //creazione processo attivatore; Argomenti: Nessuno
    printf("Creo attivatore\n");
    char* argv_attivatore[] = {"Attivatore",NULL};
    if(fork() == 0){
        if(execve("./Attivatore.out",argv_attivatore,NULL) < 0){
            perror("execve");
            exit(1);
        }
        
    }

    //creazione processo inibitore; Argomenti: Nessuno
    printf("Creo inibitore\n");
    char* argv_inibitore[] = {"Inibitore",NULL};
    if(fork() == 0){
        if(execve("./Inibitore.out",argv_inibitore,NULL) < 0){
            perror("execve");
            exit(1);
        }
        
    }

    sleep(1);

    printf("rilascio semaforo\n");
    if(semctl(sem_master_ready, 0, SETVAL, atoi(N_ATOMI_INIT)+2) < 0){
        perror("semctl");
        exit(1);
    }

    sleep(100);

    if(semctl(sem_master_ready, 0, IPC_RMID, 0) < 0){
        perror("semctl");
        exit(1);
    }

    //crezione processo inibitore
    //execve();

    //ciclo di stampa per status
    //while();

    //terminazione
    exit(0);


}