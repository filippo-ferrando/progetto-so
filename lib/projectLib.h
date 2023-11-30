#include "header.h"

void env_get(){
    printf("Env variable getter\n");

    int ENERGY_DEMAND = getenv("ENERGY_DEMAND");
    int N_ATOMI_INIT = getenv("N_ATOMI_INIT");
    int N_ATOM_MAX = getenv("N_ATOM_MAX");
    int MIN_N_ATOMICO = getenv("MIN_N_ATOMICO");
    int N_NUOVI_ATOMI = getenv("N_NUOVI_ATOMI");
    int SIM_DURATION = getenv("SIM_DURATION");
    int ENERGY_EXPLODE_THRESHOLD = getenv("ENERGY_EXPLODE_THRESHOLD");

}

int releaseSem(int semid){
    struct sembuf sops;

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    if(semop(semid, &sops, 1) < 0){
        perror("semop: ");
        exit(1);
    }

    printf("changed sem val to -> semctl: %d\n", semctl(semid, 0, GETVAL, 0));

    return 0;
}

int reserveSem(int semid){
    struct sembuf sops;

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    if(semop(semid, &sops, 1) < 0){
        perror("semop: ");
        exit(1);
    }

    printf("changed sem val to -> semctl: %d\n", semctl(semid, 0, GETVAL, 0));

    return 0;
}