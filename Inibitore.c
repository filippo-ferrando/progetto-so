#include "lib/generalLib.h"
#include "lib/key.h"

//I SIGUSR fanno riferimento allo stato dell'inibitore. SIGUSR1: Inibitore acceso, da 0 a Attivatore per continuare in modo normale;
//SIGUSR2: Inibitore spento, da 1 ad Attivatore per rallentare l'attivazione di N secondi;

int msgid; 
struct message_buf messaggio;
int stato_inib; //Se inib vale 0, è spento, invia messaggio 0 ad attivatore, che deve procedere normalmente;
                //Se inib vale 1, è acceso e invia messaggio 1 ad attivatore, che deve fermarsi


void handle_SIGUSR1(int signal){ //Se ricevo SIGUSR1, spengo l'inibitore
    //printf("Ho ricevuto SIGUSR1\n");
    stato_inib = 0;
}

void handle_SIGUSR2(int signal){ //Se ricevo SIGUSR2, accendo l'inibitore
    //printf("Ho ricevuto SIGUSR2\n");
    stato_inib = 1;
}

/*
* ATTACH SM PER:
*   - ENERGIA ASSORBITA DA INIBITORE
* METODO PER CREARE LOG DI INIBITORE
*/


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

    int actual_energy = 0;

    int gravita_EXP = 0; //Aggiunto
    int energia_attuale = 0; //Aggiunto
    int Energy_Threshold = atoi(env_get_ENERGY_EXPLODE_THRESHOLD()); //Aggiunto

    int sem_sm = semget(KEY_SEM_SM, 13, 0777);

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = atoi(argv[1]);

    //Creo coda di messaggi per parlare con Atomo
    int msgid = msgget(KEY_INHIB,0777); //msgid tiene id per comunicare con inibitore
    struct message_buf mex;
    mex.mex = 0;

    if(releaseSem(sem_inibitore_ready, 0) < 0){
        perror("releaseSem inibitore: ");
        exit(1);
    }
    

    if(reserveSem(sem_start, 0) < 0){ //Sincronizzo l'inibitore con il resto dei processi tramite semaforo.
        perror("reserveSem start inibitore: ");
        exit(1);
    }
    //sleep(1);

    /*
    * INIBITORE ACCESO O SPENTO (varibile ambente passata da master o SIGUSR1/2)
    * DEVE GESTIRE SIA MELTDOWN CHE EXPLODE
    * SE INIBITORE ACCESO:
    *   - CONTROLLA LA REAZIONE TRANSORMANDO RAND ATOMI IN SCORIE (?)
    *   - CONTROLLA IL RILASCIO DI ENERGIA ASSORBENDONE UNA PARTE -> L'ENERGIA VIENE ASSORBITA DIRETTAMENTE DALLA SCISSIONE (?)
    * SE INIBITORE SPENTO:
    *   - NON FACCIO NIENTE
    */

    srand(getpid()); //Spostato

    while(1){ 
        if(stato_inib == 1){
            //CODICE INIBITORE
            //Faccio controlli sullo stato ogni mezzo secondo
            //usleep(atoi(argv[1]));
            nanosleep(&remaining, &request);

            energia_attuale = st->energy_created_total;
            if(energia_attuale + (Energy_Threshold/10) > Energy_Threshold) {
                printf("\nRISCHIO EXP : 4\n");
                gravita_EXP = 4;
            }else if(energia_attuale + (Energy_Threshold/8) > Energy_Threshold){
                printf("\nRISCHIO EXP : 3\n");
                gravita_EXP = 3; 
            }else if(energia_attuale + (Energy_Threshold/6) > Energy_Threshold){
                printf("\nRISCHIO EXP : 2\n");
                gravita_EXP = 2;
                }
            else if(energia_attuale + (Energy_Threshold/4) > Energy_Threshold){
                printf("\nRISCHIO EXP : 1\n");
                gravita_EXP = 1;
                }
            else{
                gravita_EXP = 0;
		    }

/*Rimuovo MOMENTANEAMENTE le operazioni sul MELTDOWN
            mex.mex = 0;
            mex.mtype = 1;

            for(int i = 0; i < rand() % 100 +1; i++){
                if(msgsnd(msgid, &mex, sizeof(mex.mex), 0)){
                    perror("msg send inibitore scrap: ");
                }
            }
*/
            mex.mex = 0;
            mex.mtype = 3;

            for(int i = 0; i < gravita_EXP * (rand() % 200 +1); i++){
                if(msgsnd(msgid, &mex, sizeof(mex.mex), 0)){
                    perror("msg send inibitore energia: ");
                }
            }
        }
    }
}
//Inizialmente dorme per tot secondi
//Inibitore controlla statistiche ogni mezzo secondo
//Se inibitore è attivo, controlla se le statistiche siano rispettate
//Se si raggiunge meltdown, ferma l'attivatore
