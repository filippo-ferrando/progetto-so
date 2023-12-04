#include "lib/projectLib.h"
#include "lib/key.h"

int main(){
    int sem_start = semget(KEY_SEM_ACT, 1, 0666);

    reserveSem(sem_start, 0);
    exit(0);
}