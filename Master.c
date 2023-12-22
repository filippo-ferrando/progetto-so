#include "lib/generalLib.h"
#include "lib/key.h"

int alarm_rt = 0;
pid_t pid_alimentatore,pid_attivatore;


// handler per SIGALRM
void handle_alarm(int signal){
    //Timeout SIGNAL
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

// handler per SIGUSR1
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

//handler per SIGUSR2
void handle_usr2(int signal){ 
    //Meltdown SIGNAL
    int i;

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 500000000;

    printf("MELTDOWN");

    kill(pid_attivatore,SIGUSR1);
    kill(pid_alimentatore,SIGUSR1);

    nanosleep(&remaining, &request);

    while(waitpid(-1, NULL, WNOHANG)>0);

    char* proc_names[] = {"Inibitore.out","Alimentatore.out","Atomo.out","Attivatore.out", NULL};
    if(fork() == 0){
        if(execve("/usr/bin/killall",proc_names,NULL) < 0){
            perror("execve timeout killall: ");
            exit(1);
        }
        exit(0);
    }

    while(waitpid(-1, NULL, WNOHANG)>0);

    nanosleep(&remaining, &request);

    char* argv[] = {"killer.sh",NULL};
    if(execve("./lib/killer.sh",argv,NULL) < 0){
        perror("execve timeout killer: ");
        exit(1);
    }
    exit(0);
}


int main(int argc, char* argv[]){
    //ENV VARIABLES
    char* ENERGY_DEMAND = env_get_ENERGY_DEMAND();
    char* N_ATOMI_INIT = env_get_N_ATOMI_INIT();
    char* N_ATOM_MAX = env_get_N_ATOM_MAX();
    char* MIN_N_ATOMICO = env_get_MIN_N_ATOMICO();
    char* N_NUOVI_ATOMI = env_get_N_NUOVI_ATOMI();
    char* SIM_DURATION = env_get_SIM_DURATON();
    char* ENERGY_EXPLODE_THRESHOLD = env_get_ENERGY_EXPLODE_THRESHOLD();
    char* STEP = env_get_STEP();
    char* INIBIT_CHECK = env_get_INIBIT_CHECK();
    char* ATT_STEP = env_get_ATT_STEP();
    char* SPLIT_ATOMS = env_get_SPLIT_ATOMS();
    char* MAX_PROCESS = env_get_MAX_PROCESS();

    //inibitore da subito?
    char inibit_start;

    //struct per nanosleep
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = 500000000;

    //file per gestione rimozione di risorse ipc
    FILE *ipcs_id = fopen("./ipcs_id_sem.txt","a");
    FILE *ipcs_id2 = fopen("./ipcs_id_mem.txt","a");

    //struct per shared memory
    struct stats *st;

    printf("Vuoi usare inibitore da subito?: (y/n)\n");
    scanf("%c", &inibit_start);

    //semafori partenza simulazione
    int sem_master_ready; 
    sem_master_ready = semget(KEY_SEM_ACT, 1, IPC_CREAT | 0777);
    fprintf(ipcs_id, "%d\n", sem_master_ready);
    if(semctl(sem_master_ready, 0, SETVAL, 0) < 0){
        perror("semctl master ready");
        exit(1);
    }

    int sem_proc_ready;
    sem_proc_ready = semget(KEY_PROC_READY, 1, IPC_CREAT | 0777);
    fprintf(ipcs_id, "%d\n", sem_proc_ready);
    if(semctl(sem_proc_ready, 0, SETVAL, 0) < 0){
        perror("semctl proc ready");
        exit(1);
    }

    //semaforo per sm atomi
    int sem_sm_atom;
    sem_sm_atom = semget(KEY_SEM_SM_ATOM, 1, IPC_CREAT | 0777);
    fprintf(ipcs_id, "%d\n", sem_sm_atom);
    if(semctl(sem_sm_atom, 0, SETVAL, 0) < 0){
        perror("semctl sm atom");
        exit(1);
    }
    releaseSem(sem_sm_atom, 0);

    //semaforo per attivatore
    int sem_att = semget(KEY_ATT, 1, IPC_CREAT|0777);
    fprintf(ipcs_id, "%d\n", sem_att);
    if(semctl(sem_att, 0, SETVAL, 0) < 0){
        perror("semctl attivatore");
        exit(1);
    }

    //semaforo shared memory
    int sem_sm_ready = semget(KEY_SEM_SM, 13, IPC_CREAT | 0777);
    fprintf(ipcs_id, "%d\n", sem_sm_ready);
    for(int i=0; i<=9; i++){
        semctl(sem_sm_ready, i, SETVAL, 0);
    }
    
    //shared memory
    int shmid = shmget(KEY_SHM, sizeof(st), IPC_CREAT | 0777);
    fprintf(ipcs_id2, "%d\n", shmid);
    st = shmat(shmid, NULL, 0);

    //sm only for atom
    struct atoms *st_atom;
    int shmid_atom = shmget(KEY_SHM_ATOM, sizeof(st_atom), IPC_CREAT | 0777);
    fprintf(ipcs_id2, "%d\n", shmid_atom);
    st_atom = shmat(shmid_atom, NULL, 0);
    st_atom->n = 0;

    //file per gestione rimozione di risorse ipc
    fclose(ipcs_id);
    fclose(ipcs_id2);

    int msgid_meltdown = msgget(KEY_INHIB,IPC_CREAT|0777); //msgid tiene id per comunicare con inibitore

    //file per gestione rimozione di risorse ipc
    FILE *ipcs_id_q = fopen("ipcs_id_q.txt", "a");
    fprintf(ipcs_id_q, "%d\n", msgid_meltdown);
    fclose(ipcs_id_q);

    st->activations_ls = 0;                 //sem 0
    st->activations_total = 0;              //sem 1
    st->split_ls = 0;                       //sem 2
    st->split_total = 0;                    //sem 3
    st->energy_created_ls = 0;              //sem 4
    st->energy_created_total = 0;           //sem 5
    st->energy_consumed_ls = 0;             //sem 6
    st->energy_consumed_total = 0;          //sem 7
    st->scrap = 0;                          //sem 8
    st->scrap_ls = 0;                       //sem 9
    st->scrap_inibitore = 0;                //sem 10
    st->energy_absorbed_inibitore = 0;      //sem 11
    st->current_atoms = atoi(N_ATOMI_INIT); //sem 12

    for(int i = 0; i < 13; i++){
        releaseSem(sem_sm_ready, i);
    }

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
    char* argv_attivatore[] = {"Attivatore",ATT_STEP,SPLIT_ATOMS,NULL};
    switch(fork()){
        case -1:
            perror("fork");
            exit(1);
        case 0:
            pid_attivatore = getpid(); //Aggiu
            if(execve("./Attivatore.out",argv_attivatore,NULL) < 0){
                perror("execve");
                exit(1);
            }
        default:
            break;
    }
    free(pid);

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
            pid_alimentatore=getpid(); //Aggiunto Ora
            switch(execve("./Alimentatore.out",argv_alimentatore,NULL)){
                case -1:
                    perror("execve");
                    exit(1);
            }
        default:
            break;
    }
    free(buf);

    //creazione processo inibitore; Argomenti: INIBIT_ATT
    if(inibit_start == 'y'){
        printf("Creo inibitore\n");
        char* argv_inibitore[] = {"Inibitore",INIBIT_CHECK,ENERGY_EXPLODE_THRESHOLD,MAX_PROCESS,NULL};
        if(fork() == 0){
            if(execve("./Inibitore.out",argv_inibitore,NULL) < 0){
                perror("execve");
                exit(1);
            }
        }
    }
    

    //ciclo creazione processi atomo
    
    char* rand_n = malloc(1);
    
    char* argv_atomo[] = {"./Atomo.out", rand_n, "0", MIN_N_ATOMICO,pid,NULL};
    
    int n_atom_rand;
    
    //crezione processo atomo
    for(int i=0; i<=atoi(N_ATOMI_INIT); i++){
        srand(getpid());
        n_atom_rand = rand() % atoi(N_ATOM_MAX) + 1;
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
    free(rand_n);

    while(1){ 
        if(inibit_start == 'y'){
            if(semctl(sem_proc_ready, 0, GETVAL) == atoi(N_ATOMI_INIT)+3){
                break;
            }
        }else{
            if(semctl(sem_proc_ready, 0, GETVAL) == atoi(N_ATOMI_INIT)+2){
                break;
            }
        }
        
    }


    printf("rilascio semaforo\n");
    if(semctl(sem_master_ready, 0, SETVAL, atoi(N_ATOMI_INIT)+3) < 0){
        perror("semctl");
        exit(1);
    }    

    alarm(atoi(SIM_DURATION));
    while(1){
        sleep(1);
        
        //semfori sm
        for(int i = 0; i < 13; i++){
            reserveSem(sem_sm_ready, i);
        }

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
        printf("scarti last sec: %d\n\n", st->scrap_ls);
        printf("scarti inibitore: %d\n", st->scrap_inibitore);
        printf("energia assorbita inibitore: %d\n\n", st->energy_absorbed_inibitore);
        printf("atomi attuali: %d\n", st->current_atoms);
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
        for(int i = 0; i < 13; i++){
            releaseSem(sem_sm_ready, i);
        }

        //se ci sono processi figli zombie, li elimino, altrimenti passo oltre -> WOHNANG settato
        while(waitpid(-1, NULL, WNOHANG)>0);
    }
    //terminazione
    exit(0);
}