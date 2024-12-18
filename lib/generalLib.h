#include "header.h"

char* env_get_ENERGY_DEMAND(){
    char * ENERGY_DEMAND = getenv("ENERGY_DEMAND");
    return ENERGY_DEMAND;
}
char* env_get_N_ATOMI_INIT(){
    char* N_ATOMI_INIT = getenv("N_ATOMI_INIT");
    return N_ATOMI_INIT;
}

char* env_get_INIBIT_CHECK(){
    char* INIBIT_CHECK = getenv("INIBIT_CHECK");
    return INIBIT_CHECK;
}

char* env_get_N_ATOM_MAX(){
    char* N_ATOM_MAX = getenv("N_ATOM_MAX");
    return N_ATOM_MAX;
}

char* env_get_MIN_N_ATOMICO(){
    char* MIN_N_ATOMICO = getenv("MIN_N_ATOMICO");
    return MIN_N_ATOMICO;
}

char* env_get_N_NUOVI_ATOMI(){
    char* N_NUOVI_ATOMI = getenv("N_NUOVI_ATOMI");
    return N_NUOVI_ATOMI;
}

char* env_get_SIM_DURATON(){
    char* SIM_DURATION = getenv("SIM_DURATION");
    return SIM_DURATION;
}

char* env_get_ENERGY_EXPLODE_THRESHOLD(){
    char* ENERGY_EXPLODE_THRESHOLD = getenv("ENERGY_EXPLODE_THRESHOLD");
    return ENERGY_EXPLODE_THRESHOLD;
}

char* env_get_ATT_STEP(){
    char* ATT_STEP = getenv("ATT_STEP");
    return ATT_STEP;
}

char* env_get_STEP(){
    char* STEP = getenv("STEP");
    return STEP;
}

char* env_get_SPLIT_ATOMS(){
    char* SPLIT_ATOMS = getenv("SPLIT_ATOMS");
    return SPLIT_ATOMS;
}

char* env_get_MAX_PROCESS(){
    char* MAX_PROCESS = getenv("MAX_PROCESS");
    return MAX_PROCESS;
}

int releaseSem(int semid, int n_sem){
    struct sembuf sops;

    sops.sem_num = n_sem;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    if(semop(semid, &sops, 1) < 0){
        perror("semop release: ");
        return -1;
    }

    //printf("Changed sem val to -> semctl: %d\n", semctl(semid, 0, GETVAL, 0));

    return 0;
}

int reserveSem(int semid, int n_sem){
    struct sembuf sops;

    sops.sem_num = n_sem;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    if(semop(semid, &sops, 1) < 0){
        perror("semop reserve: ");
        return -1;
    }

    //printf("changed sem val to -> semctl: %d\n", semctl(semid, 0, GETVAL, 0));

    return 0;
}

struct message_buf{
    long mtype;
    int mex;
};

typedef struct stats {
    int activations_ls;
    int activations_total;
    int split_ls;
    int split_total;
    int energy_created_ls;
    int energy_created_total;
    int energy_consumed_ls;
    int energy_consumed_total;
    int scrap;
    int scrap_ls;
    int scrap_inibitore;
    int energy_absorbed_inibitore;
    int current_atoms;
}stats;

typedef struct atoms {
    int n;
}atoms;
