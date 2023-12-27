#include "lib/generalLib.h"
#include "lib/key.h"

//I SIGUSR fanno riferimento allo stato dell'inibitore. SIGUSR1: Inibitore acceso, da 0 a Attivatore per continuare in modo normale;
//SIGUSR2: Inibitore spento, da 1 ad Attivatore per rallentare l'attivazione di N secondi;

int msgid; 
struct message_buf messaggio;
int stato_inib; //Se inib vale 0, è spento, invia messaggio 0 ad attivatore, che deve procedere normalmente;
                //Se inib vale 1, è acceso e invia messaggio 1 ad attivatore, che deve fermarsi


void handle_SIGUSR1(int signal){ //Se ricevo SIGUSR1, spengo l'inibitore
    printf("\nDISATTIVO INIBITORE\n");
    stato_inib = 0;
}

void handle_SIGUSR2(int signal){ //Se ricevo SIGUSR2, accendo l'inibitore
    printf("\nATTIVO ATTIVATORE\n");
    stato_inib = 1;
}


int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);
    int sem_inibitore_ready = semget(KEY_PROC_READY, 1, 0777);

    stato_inib = 1;

    //Creo Sigaction
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
	sa.sa_handler = handle_SIGUSR1;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler = handle_SIGUSR2;
    sigaction(SIGUSR2,&sa, NULL);

    //Creo Shared Memory
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0777);
    st = shmat(shmid, NULL, 0);

    int max_c_process = atoi(argv[3]); 
    int curr_process;
    int actual_energy = 0;
    int gravita_MTD = 0, gravita_EXP = 0;

    int energia_attuale = 0;
    int energia_ls = 0;
    int Energy_Threshold = atoi(argv[2]);

    int sem_sm = semget(KEY_SEM_SM, 13, 0777);

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = atoi(argv[1]);

    //Creo coda di messaggi per parlare con Atomo
    int msgid_meltdown = msgget(KEY_INHIB_MELTDOWN,0777); //msgid tiene id per comunicare con inibitore

    //struttura msgctl
    struct msqid_ds buf; //Cancella, inutile

    //struct messaggio
    struct message_buf mex;
    mex.mex = 0;
    mex.mtype = 1;
    
    if(releaseSem(sem_inibitore_ready, 0) < 0){
        perror("releaseSem inibitore ready: ");
        exit(1);
    }
    

    if(reserveSem(sem_start, 0) < 0){ //Sincronizzo l'inibitore con il resto dei processi tramite semaforo.
        perror("reserveSem start inibitore: ");
        exit(1);
    }
    //sleep(1);
    
    srand(getpid());

    while(1){
        if(stato_inib == 1){

            //GESTIONE EXPLODE

            energia_attuale = st->energy_created_total;
            energia_ls = st->energy_created_ls;

            if(energia_attuale > Energy_Threshold / 2){
                if(reserveSem(sem_sm, 5) < 0){
                    perror("reserveSem inibitore energy total: ");
                    exit(1);
                }
                if(reserveSem(sem_sm, 11) < 0){
                    perror("reserveSem inibitore energy taked: ");
                    exit(1);
                }
                st->energy_absorbed_inibitore += st->energy_created_total/2;
                st->energy_created_total -= st->energy_created_total/2;
                if(releaseSem(sem_sm, 5) < 0){
                    perror("releaseSem inibitore energy total: ");
                    exit(1);
                }
                if(releaseSem(sem_sm, 11) < 0){
                    perror("releaseSem inibitore energy taked: ");
                    exit(1);
                }
            }

            if(energia_attuale > Energy_Threshold / 4){
                if(reserveSem(sem_sm, 5) < 0){
                    perror("reserveSem inibitore energy total: ");
                    exit(1);
                }
                if(reserveSem(sem_sm, 11) < 0){
                    perror("reserveSem inibitore energy taked: ");
                    exit(1);
                }
                st->energy_absorbed_inibitore += st->energy_created_total/4;
                st->energy_created_total -= st->energy_created_total/4;
                if(releaseSem(sem_sm, 5) < 0){
                    perror("releaseSem inibitore energy total: ");
                    exit(1);
                }
                if(releaseSem(sem_sm, 11) < 0){
                    perror("releaseSem inibitore energy taked: ");
                    exit(1);
                }
            }else if(energia_attuale > Energy_Threshold / 8){
                if(reserveSem(sem_sm, 5) < 0){
                    perror("reserveSem inibitore energy total: ");
                    exit(1);
                }
                if(reserveSem(sem_sm, 11) < 0){
                    perror("reserveSem inibitore energy taked: ");
                    exit(1);
                }
                st->energy_absorbed_inibitore += st->energy_created_total/4;
                st->energy_created_total -= st->energy_created_total/4;
                if(releaseSem(sem_sm, 5) < 0){
                    perror("releaseSem inibitore energy total: ");
                    exit(1);
                }
                if(releaseSem(sem_sm, 11) < 0){
                    perror("releaseSem inibitore energy taked: ");
                    exit(1);
                }
            }else if(energia_attuale > Energy_Threshold / 12){
                if(reserveSem(sem_sm, 5) < 0){
                    perror("reserveSem inibitore energy total: ");
                    exit(1);
                }
                if(reserveSem(sem_sm, 11) < 0){
                    perror("reserveSem inibitore energy taked: ");
                    exit(1);
                }
                st->energy_absorbed_inibitore += st->energy_created_total/8;
                st->energy_created_total -= st->energy_created_total/8;
                if(releaseSem(sem_sm, 5) < 0){
                    perror("releaseSem inibitore energy total: ");
                    exit(1);
                }
                if(releaseSem(sem_sm, 11) < 0){
                    perror("releaseSem inibitore energy taked: ");
                    exit(1);
                }
            }

            if(energia_ls > Energy_Threshold / 3){
                if(reserveSem(sem_sm, 5) < 0){
                    perror("reserveSem inibitore energy total: ");
                    exit(1);
                }
                if(reserveSem(sem_sm, 11) < 0){
                    perror("reserveSem inibitore energy taked: ");
                    exit(1);
                }
                st->energy_created_ls -= energia_attuale;
                st->energy_absorbed_inibitore += energia_attuale;
                if(releaseSem(sem_sm, 5) < 0){
                    perror("releaseSem inibitore energy total: ");
                    exit(1);
                }
                if(releaseSem(sem_sm, 11) < 0){
                    perror("releaseSem inibitore energy taked: ");
                    exit(1);
                }

            }
            
            //GESTIONE MELTDOWN


            curr_process = st->current_atoms;
    
            if(curr_process > max_c_process/2){ //Mangio fra 2000 e 3000 atomi
                gravita_MTD = (rand() % (3000-2000))+2000;
            }else if(curr_process > max_c_process/4){ //Mangio fra 1500 2300 atomi
                gravita_MTD = (rand() % (2300-1500))+1500;
            }else if(curr_process > max_c_process/5){//Mangio fra 1200 e 1800 atomi
                gravita_MTD = (rand() % (1800-1200))+1200;
            }else if(curr_process > max_c_process/6){ //Mangio fra 1100 e 1600 atomi
                gravita_MTD = (rand() % (1600-1100))+1100;
            }else if(curr_process > max_c_process/7){ //Mangio fra 1000 e 1400 atomi
                gravita_MTD = (rand() % (1400-1000))+1000;
            }else if(curr_process > max_c_process/8){ //Mangio fra 900 e 1200 atomi
                gravita_MTD = (rand() % (1200-900))+900;
            }else if(curr_process > max_c_process/10){ //Mangio fra 600 e 1000 atomi
                gravita_MTD = (rand() % (1000-600))+600;
            }else if(curr_process > max_c_process/15){ //Mangio fra 40 e 120 atomi
                gravita_MTD = (rand() % (120-40))+40;
            }else if(curr_process > max_c_process/20){ //Mangio fra 20 e 80 atomi
                gravita_MTD = (rand() % (80-20))+20;
            }else if(curr_process > max_c_process/30){ //Mangio fra 1 e 50 atomi
                gravita_MTD = rand() % 50+1; 
            }else{
                gravita_MTD = 0; //Non mangio atomi
            }
            
            if(gravita_MTD!= 0){ //Non mando messaggi all'Atomo, se devo inviare 0 elementi
                mex.mex = gravita_MTD;
                msgsnd(msgid_meltdown, &mex, sizeof(mex.mex), 0);
            }
        }

        if(nanosleep(&remaining, &request) < 0){
            perror("nanosleep inibitore: ");
            //exit(1);
        }
    }
}