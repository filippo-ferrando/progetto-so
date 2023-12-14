#include "lib/generalLib.h"
#include "lib/key.h"

void handle_SIGUSR1(int signal){
    printf("alimentatore: Mi spengo");
    exit(0);
}

int main(int argc, char* argv[]){
    int step = atoi(argv[1]);   //step in nanosecondi
    int n_nuovi_atomi = atoi(argv[2]);  //numero di atomi da creare a ogni step
    int n_atom_max = atoi(argv[3]); //numero atomico massimo che può avere un atomo creato
    char* pid_master = argv[4]; //pid del processo Master.out
    char* min_n_atomico = argv[5];  //numero atomico minimo che può avere un atomo creato

    int sem_sm = semget(KEY_SEM_SM, 13, 0777);

    //get semaforo sync-start
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);

    int sem_alimentatore_ready = semget(KEY_PROC_READY, 1, 0777);

    //numero atomico random
    int n_atom_rand = 0;
    
    //struct per nanosleep
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = step;
    //printf("\nstep: %d\n", step);
    
    struct sigaction sa; //Aggiunta ora
    bzero(&sa, sizeof(sa)); //Aggiunta ora
	sa.sa_handler = handle_SIGUSR1; //Aggiunta ora
    sigaction(SIGUSR1, &sa, NULL); //Aggiunta ora



    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0777);
    st = shmat(shmid, NULL, 0);

    //variabile per ciclo for
    int i = 0;

    //buffer per passare numero atomico ad Atomo.out
    char* buf = malloc(1);
    //argv per execve Atomo.out
    char* argv_atomo[] = {"Atomo.out", buf, "7",min_n_atomico,pid_master,NULL};

    if(releaseSem(sem_alimentatore_ready, 0) < 0){
        perror("releaseSem alimentatore ready: ");
        exit(1);
    }

    if(reserveSem(sem_start, 0) < 0){
        perror("reserveSem alimentatore master ready: ");
        exit(1);
    }

    while(1){
        //printf("\nALIMENTATORE %d | creo %d atomi\n", getpid(), n_nuovi_atomi);

        //sleep per step
        if(nanosleep(&remaining, &request) < 0){
            perror("nanosleep alimentatore: ");
            exit(1);
        }

        //se ci sono processi figli zombie, li elimino, altrimenti passo oltre -> WOHNANG settato
        while(waitpid(-1, NULL, WNOHANG)>0);
        
        //creo n_nuovi_atomi atomi
        for(i = 0; i < n_nuovi_atomi; i++ ) {
            srand(getpid());
            n_atom_rand = rand() % n_atom_max + 1;

            sprintf(buf, "%d", n_atom_rand);
            strcpy(argv_atomo[1],buf);

            if(reserveSem(sem_sm, 12) < 0){
                perror("reserveSem sm current_atoms alimentatore: ");
                exit(1);
            }
            st->current_atoms++;
            if(releaseSem(sem_sm, 12) < 0){
                perror("releaseSem sm current_atoms alimentatore: ");
                exit(1);
            }

            switch(fork()){
                case -1:
                    //meltdown a causa di fork alimentatore
                    perror("fork alimentatore: ");
                    kill(atoi(pid_master), SIGUSR2);
                    exit(1);
                case 0:
                    //Atomo.out creato da Alimentatore.out

                    if(execve("./Atomo.out", argv_atomo, NULL) < 0){
                        perror("execve alimentatore: ");
                        exit(1);
                    }
                    exit(0); // -> da vedere
            }
        }
        
    }
    exit(0);
}