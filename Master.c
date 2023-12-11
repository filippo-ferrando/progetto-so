#include "lib/generalLib.h"
#include "lib/key.h"

int alarm_rt = 0;


void handle_alarm(int signal){ //Quando gestisco questo alarm, uccido Inibitore, Alimentatore, Atomo e Attivatore
    int i;

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 250000000;

    printf("Esco per Timeout\n");
    char* proc_names[] = {"Inibitore.out","Alimentatore.out","Atomo.out","Attivatore.out", NULL};
    if(fork() == 0){
        if(execve("/usr/bin/killall",proc_names,NULL) < 0){
            perror("execve timeout killall: ");
            exit(1);
        }
        exit(0);
    }

    nanosleep(&remaining, &request);

    if(fork() == 0){
        char* argv[] = {"killer.sh",NULL};
        if(execve("./lib/killer.sh",argv,NULL) < 0){
            perror("execve timeout killer: ");
            exit(1);
        }
        exit(0);
    }
    exit(0);
}

void handle_usr1(int signal){
    //Explode | Blackout SIGNAL
    int i;
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 250000000;

    char* proc_names[] = {"Inibitore.out","Alimentatore.out","Atomo.out","Attivatore.out", NULL};
    if(fork() == 0){
        if(execve("/usr/bin/killall",proc_names,NULL) < 0){
            perror("execve timeout killall: ");
            exit(1);
        }
        exit(0);
    }
    nanosleep(&remaining, &request);
    if(fork() == 0){
        char* argv[] = {"killer.sh",NULL};
        if(execve("./lib/killer.sh",argv,NULL) < 0){
            perror("execve timeout killer: ");
            exit(1);
        }
        exit(0);
    }
    exit(0);

}

void handle_usr2(int signal){ //segnale di meltdown
    int i;
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 250000000;
    printf("MELTDOWN");

    char* proc_names[] = {"Inibitore.out","Alimentatore.out","Atomo.out","Attivatore.out", NULL};
    if(fork() == 0){
        if(execve("/usr/bin/killall",proc_names,NULL) < 0){
            perror("execve timeout killall: ");
            exit(1);
        }
        exit(0);
    }
    nanosleep(&remaining, &request);
    if(fork() == 0){
        char* argv[] = {"killer.sh",NULL};
        if(execve("./lib/killer.sh",argv,NULL) < 0){
            perror("execve timeout killer: ");
            exit(1);
        }
        exit(0);
    }
    exit(0);
}

int main(int argc, char* argv[]){
    char* ENERGY_DEMAND = env_get_ENERGY_DEMAND();
    char* N_ATOMI_INIT = env_get_N_ATOMI_INIT();
    char* N_ATOM_MAX = env_get_N_ATOM_MAX();
    char* MIN_N_ATOMICO = env_get_MIN_N_ATOMICO();
    char* N_NUOVI_ATOMI = env_get_N_NUOVI_ATOMI();
    char* SIM_DURATION = env_get_SIM_DURATON();
    char* ENERGY_EXPLODE_THRESHOLD = env_get_ENERGY_EXPLODE_THRESHOLD();
    char* STEP = env_get_STEP();
    char* INIBIT_ATT = env_get_INIBIT_ATT();
    char* INIBIT_CHECK = env_get_INIBIT_CHECK();
    char* ATT_STEP = env_get_ATT_STEP();

    int i = 0;

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 500000000;

    FILE *ipcs_id = fopen("./ipcs_id_sem.txt","a");
    FILE *ipcs_id2 = fopen("./ipcs_id_mem.txt","a");

    struct stats *st;

    //semafori partenza simulazione
    int sem_master_ready; 
    sem_master_ready = semget(KEY_SEM_ACT, 1, IPC_CREAT | 0777);
    fprintf(ipcs_id, "%d\n", sem_master_ready);
    if(semctl(sem_master_ready, 0, SETVAL, 0) < 0){
        perror("semctl master ready");
        exit(1);
    }

    //semaforo shared memory
    int sem_sm_ready = semget(KEY_SEM_SM, 10, IPC_CREAT | 0777);
    fprintf(ipcs_id, "%d\n", sem_sm_ready);
    for(int i=0; i<=9; i++){
        semctl(sem_sm_ready, i, SETVAL, 0);
    }
    
    //shared memory
    int shmid = shmget(KEY_SHM, sizeof(st), IPC_CREAT | 0777);
    fprintf(ipcs_id2, "%d\n", shmid);
    st = shmat(shmid, NULL, 0);

    fclose(ipcs_id);
    fclose(ipcs_id2);

    st->activations_ls = 0;         //sem 0
    st->activations_total = 0;      //sem 1
    st->split_ls = 0;               //sem 2
    st->split_total = 0;            //sem 3
    st->energy_created_ls = 0;      //sem 4
    st->energy_created_total = 0;   //sem 5
    st->energy_consumed_ls = 0;     //sem 6
    st->energy_consumed_total = 0;  //sem 7
    st->scrap = 0;                  //sem 8
    st->scrap_ls = 0;               //sem 9

    releaseSem(sem_sm_ready, 0);
    releaseSem(sem_sm_ready, 1);
    releaseSem(sem_sm_ready, 2);
    releaseSem(sem_sm_ready, 3);
    releaseSem(sem_sm_ready, 4);
    releaseSem(sem_sm_ready, 5);
    releaseSem(sem_sm_ready, 6);
    releaseSem(sem_sm_ready, 7);
    releaseSem(sem_sm_ready, 8);
    releaseSem(sem_sm_ready, 9);

    //Creazione Handler
    struct sigaction sa;
    bzero(&sa, sizeof(sa));

	sa.sa_handler = handle_alarm;
    sigaction(SIGALRM, &sa, NULL);

    sa.sa_handler = handle_usr1;
    sigaction(SIGUSR1,&sa,NULL);

    sa.sa_handler = handle_usr2;
    sigaction(SIGUSR2,&sa,NULL);
    

    char* pid = malloc(5);
    sprintf(pid,"%d",getpid());

     //creazione processo attivatore; Argomenti: Nessuno
    printf("Creo attivatore\n");
    char* argv_attivatore[] = {"Attivatore",ATT_STEP,NULL};
    if(fork() == 0){
        if(execve("./Attivatore.out",argv_attivatore,NULL) < 0){
            perror("execve");
            exit(1);
        }
        
    }

    //creazione processo alimentatore; Argomenti: STEP
    printf("Creo alimentatore\n");
    char* buf = malloc(5);
    sprintf(buf,"%d",getpid());
    char* argv_alimentatore[] = { "Alimentatore", STEP, N_NUOVI_ATOMI, N_ATOM_MAX,buf,MIN_N_ATOMICO,NULL };
    switch(fork()){
        case -1:
            perror("fork");
            exit(1);
        case 0:
            switch(execve("./Alimentatore.out",argv_alimentatore,NULL)){
                case -1:
                    perror("execve");
                    exit(1);
            }
        default:
            break;
    }
    /*
    //creazione processo inibitore; Argomenti: INIBIT_ATT
    printf("Creo inibitore\n");
    char* argv_inibitore[] = {"Inibitore",INIBIT_ATT,INIBIT_CHECK,ENERGY_EXPLODE_THRESHOLD,NULL};
    if(fork() == 0){
        if(execve("./Inibitore.out",argv_inibitore,NULL) < 0){
            perror("execve");
            exit(1);
        }
        
    }
    */

    //ciclo creazione processi atomo
    
    char* rand_n = malloc(1);
    
    char* argv_atomo[] = {"./Atomo.out", rand_n, "0", MIN_N_ATOMICO,pid,NULL};
    
    int n_atom_rand;
    
    //crezione processo atomo
    for(i=0; i<atoi(N_ATOMI_INIT); i++){
        srand(getpid());
        n_atom_rand = rand() % atoi(N_ATOM_MAX) + 1;
        //printf("atomo %d n atomico %d\n", i, n_atom_rand);
        sprintf(rand_n, "%d", n_atom_rand);
        
        strcpy(argv_atomo[1],rand_n);
        if(fork() == 0){
            switch(execve("./Atomo.out", argv_atomo, NULL)){
                case -1:
                    perror("execve");
                    exit(1);
                    break;
            }
            break;
        }
        
        
    }


    printf("rilascio semaforo\n");
    if(semctl(sem_master_ready, 0, SETVAL, atoi(N_ATOMI_INIT)+3) < 0){
        perror("semctl");
        exit(1);
    }

    //nanosleep(&remaining, &request); //dò il tempo ad atomo di scindere e creare energia -> altrimenti master entra in blackout subito
    

    alarm(atoi(SIM_DURATION));
    while(1){
        sleep(1);
        //semfori sm
        reserveSem(sem_sm_ready, 0);
        reserveSem(sem_sm_ready, 1);
        reserveSem(sem_sm_ready, 2);
        reserveSem(sem_sm_ready, 3);
        reserveSem(sem_sm_ready, 4);
        reserveSem(sem_sm_ready, 5);
        reserveSem(sem_sm_ready, 6);
        reserveSem(sem_sm_ready, 7);
        reserveSem(sem_sm_ready, 8);
        reserveSem(sem_sm_ready, 9);

        printf("--------------------------------------------------\n");
        printf("attivazioni totali: %d\n", st->activations_total);
        printf("attivazioni last sec: %d\n\n", st->activations_ls);
        printf("scissioni totali: %d\n", st->split_total);
        printf("scissioni last sec: %d\n\n", st->split_ls);
        printf("energia totale: %d\n", st->energy_created_total);
        printf("energia creata last sec: %d\n", st->energy_created_ls);
        printf("energia totale consumata: %d\n", st->energy_consumed_total);
        printf("energia consumata last sec: %d\n\n", st->energy_consumed_ls);
        printf("scarti: %d\n", st->scrap);
        printf("scarti last sec: %d\n", st->scrap_ls);
        printf("--------------------------------------------------\n");

        st->energy_created_total = st->energy_created_total + st->energy_created_ls - atoi(ENERGY_DEMAND);
        st->activations_total += st->activations_ls;
        st->split_total += st->split_ls;
        st->energy_consumed_total += st->energy_consumed_ls;
        st->scrap += st->scrap_ls;
        st->energy_consumed_ls = atoi(ENERGY_DEMAND);
        st->activations_ls = 0;
        st->split_ls = 0;
        st->energy_created_ls = 0;
        st->scrap_ls = 0;

        if(st->energy_created_total > atoi(ENERGY_EXPLODE_THRESHOLD)){   //explode, ho creato più energia che io riesca a gestire
            printf("Esco per Explode\n");
            kill(getpid(),SIGUSR1);
        }

        if(st->energy_consumed_ls > st-> energy_created_total){ //blackout, la quantità di energia che voglio prelevare è minore di quella attuale
            printf("Esco per Blackout");
            kill(getpid(),SIGUSR1);
        }
        //release all semaphores
        releaseSem(sem_sm_ready, 0);
        releaseSem(sem_sm_ready, 1);
        releaseSem(sem_sm_ready, 2);
        releaseSem(sem_sm_ready, 3);
        releaseSem(sem_sm_ready, 4);
        releaseSem(sem_sm_ready, 5);
        releaseSem(sem_sm_ready, 6);
        releaseSem(sem_sm_ready, 7);
        releaseSem(sem_sm_ready, 8);
        releaseSem(sem_sm_ready, 9);

        //se ci sono processi figli zombie, li elimino, altrimenti passo oltre -> WOHNANG settato
        waitpid(-1, NULL, WNOHANG);
    }
    //terminazione
    exit(0);
}