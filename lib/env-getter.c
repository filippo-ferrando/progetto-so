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