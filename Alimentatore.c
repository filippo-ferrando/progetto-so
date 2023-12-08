#include "lib/projectLib.h"
#include "lib/key.h"

int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);
    int n_nuovi_atomi = atoi(argv[2]);
    int step = atoi(argv[1]);
    int n_atom_rand = 0;
    int n_atom_max = atoi(argv[3]);
    
    char* pid_master = argv[4]; //Salvo pid del master per inviargli SIGUSR2 in caso di MELTDOWN
    char* min_n_atomico = argv[5];
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    printf("\nstep: %d\n", step);
    remaining.tv_nsec = step;

    int i = 0;

    char* buf = malloc(1);
    char* argv_atomo[] = {"Atomo.out", buf, "7",min_n_atomico,pid_master,NULL};

    if(reserveSem(sem_start, 0) < 0){
        perror("reserveSem alimentatore atomo: ");
        exit(1);
    }

    while(1){
        printf("\nALIMENTATORE %d | creo %d atomi\n", getpid(), n_nuovi_atomi);

        if(nanosleep(&remaining, &request) < 0){
            perror("nanosleep alimentatore: ");
            exit(1);
        }

        waitpid(-1, NULL, WNOHANG);
        
        for(i = 0; i < n_nuovi_atomi; i++ ) {
            srand(getpid());
            n_atom_rand = rand() % n_atom_max + 1;
            sprintf(buf, "%d", n_atom_rand);
            strcpy(argv_atomo[1],buf);

            switch(fork()){
                case -1:
                    perror("fork alimentatore: ");
                    kill(atoi(pid_master), SIGUSR2);
                    exit(1);
                case 0:
                    //printf("Alimentatore crea atomo\n");
                    if(execve("./Atomo.out", argv_atomo, NULL) < 0){ //Problema: Stampa la print sopra, ma non fa l'execve sotto.
                        perror("execve alimentatore: ");
                        exit(1);
                    }
                    exit(0);
            }
        }
        
    }

    //printf("SEM VAL alimentatore: %d\n", semctl(semid, 0, GETVAL));
    //printf("SONO ALIMENTATORE | HO PID %d | HO STEP %d \n", getpid(), atoi(argv[1]), atoi(argv[2]));
    exit(0);
}