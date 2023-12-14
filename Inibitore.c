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

    int max_c_process = atoi(argv[3]);
    int curr_process;
    int actual_energy = 0;
    int gravita_MTD = 0, gravita_EXP = 0;
    int min_MTD = 0, min_EXP = 0;
    int max_MTD = 0, max_EXP = 0;

    int energia_attuale = 0; //Aggiunto
    int Energy_Threshold = atoi(argv[2]); // argv[2]

    int sem_sm = semget(KEY_SEM_SM, 13, 0777);

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = atoi(argv[1]);

    //Creo coda di messaggi per parlare con Atomo
    int msgid = msgget(KEY_INHIB,0777); //msgid tiene id per comunicare con inibitore
    struct message_buf mex;
    mex.mex = 0;

    if(releaseSem(sem_inibitore_ready, 0) < 0){
        perror("releaseSem inibitore ready: ");
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

    
        srand(getpid());

    while(1){
        if(stato_inib == 1){
            //printf("ciclo\n");
            //CODICE INIBITORE
            //Faccio controlli sullo stato ogni mezzo secondo
            //usleep(atoi(argv[1]));
            /*
            if(reserveSem(sem_sm, 5) < 0){
                perror("reserveSem inibitore energy total: ");
                exit(1);
            }
            */
            energia_attuale = st->energy_created_total;
            /*
            if(releaseSem(sem_sm, 5) < 0){
                perror("reserveSem inibitore energy total: ");
                exit(1);
            }
            */
            //printf("Energia attuale: %d\n", energia_attuale);
            if(energia_attuale + (Energy_Threshold/30) > Energy_Threshold){
                //printf("\nRISCHIO MTD : 10\n");
                gravita_EXP = 30;
                min_EXP = 500;
                max_EXP = 700;
            }else if(energia_attuale + (Energy_Threshold/15) > Energy_Threshold) {
                //printf("\nRISCHIO EXP : 4\n");
                gravita_EXP = 25;
                min_EXP = 400;
                max_EXP = 500;
            }else if(energia_attuale + (Energy_Threshold/12) > Energy_Threshold){
                //printf("\nRISCHIO EXP : 3\n");
                gravita_EXP = 15;
                min_EXP = 150;
                max_EXP = 300;
            }else if(energia_attuale + (Energy_Threshold/10) > Energy_Threshold){
                //printf("\nRISCHIO EXP : 2\n");
                gravita_EXP = 9;
                min_EXP = 500;
                max_EXP = 700;
            }else if(energia_attuale + (Energy_Threshold/8) > Energy_Threshold){
                //printf("\nRISCHIO EXP : 1\n");
                gravita_EXP = 7;
                min_EXP = 100;
                max_EXP = 300;
            }else if(energia_attuale + (Energy_Threshold/6) > Energy_Threshold){
                gravita_EXP = 6;
                min_EXP = 100;
                max_EXP = 300;
		    }else if(energia_attuale + (Energy_Threshold/4) > Energy_Threshold){
                gravita_EXP = 4;
                min_EXP = 100;
                max_EXP = 300;
            }else if(energia_attuale + (Energy_Threshold/2) > Energy_Threshold){
                gravita_EXP = 2;
                min_EXP = 50;
                max_EXP = 200;
            }else if(energia_attuale + Energy_Threshold > Energy_Threshold){
                gravita_EXP = 1;
                min_EXP = 25;
                max_EXP = 200;
            }else{
                gravita_EXP = 0;
                min_EXP = 1;
                max_EXP = 300;
            }

            mex.mex = 0;
            mex.mtype = 3;

            for(int i = 0; i < gravita_EXP * ((rand() % max_EXP - min_EXP + 1) + min_EXP); i++){
                if(msgsnd(msgid, &mex, sizeof(mex.mex), 0)){
                    perror("msg send inibitore energia: ");
                    printf("errore");
                }
            }

            curr_process = st->current_atoms;
    
            if(curr_process > max_c_process/2){
                //printf("\nRischio MTD: 10");
                gravita_MTD = 67;
                min_MTD = 600;
                max_MTD = 700;
            }else if(curr_process > max_c_process/4){
                //printf("\nRischio MTD: 10");
                gravita_MTD = 53;
                min_MTD = 500;
                max_MTD = 600;
            }else if(curr_process > max_c_process/5){
                gravita_MTD = 30;
                min_MTD = 400;
                max_MTD = 500;
            }else if(curr_process > max_c_process/6){
                //printf("\nRischio MTD: 7\n");
                gravita_MTD = 18;
                min_MTD = 350;
                max_MTD = 500;
            }else if(curr_process > max_c_process/7){
                //printf("\nRischio MTD: 6");
                gravita_MTD = 16;
                min_MTD = 300;
                max_MTD = 500;
            }else if(curr_process > max_c_process/8){
                //printf("\nRischio MTD: 4");
                gravita_MTD = 14;
                min_MTD = 300;
                max_MTD = 500;
            }else if(curr_process > max_c_process/10){
                //printf("\nRischio MTD: 0");
                gravita_MTD = 12;
                min_MTD = 250;
                max_MTD = 500;
            }else if(curr_process > max_c_process/15){
                //printf("\nRischio MTD: 0");
                gravita_MTD = 10;
                min_MTD = 150;
                max_MTD = 500;
            }else if(curr_process > max_c_process/20){
                //printf("\nRischio MTD: 0");
                gravita_MTD = 8;
                min_MTD = 100;
                max_MTD = 500;
            }else if(curr_process > max_c_process/30){
                //printf("\nRischio MTD: 0");
                gravita_MTD = 6;
                min_MTD = 100;
                max_MTD = 500;
            }else{
                gravita_MTD = 0;
                min_MTD = 1;
                max_MTD = 500;
            }

            mex.mex = 0;
            mex.mtype = 1;

            for(int i = 0; i < gravita_MTD * ((rand() % max_MTD - min_MTD + 1) + min_MTD); i++){
                if(msgsnd(msgid, &mex, sizeof(mex.mex), 0)){
                    perror("msg send inibitore scrap: ");
                }
                //printf("\nmessaggio mandato\n");
            }


            
        }

        if(nanosleep(&remaining, &request) < 0){
            perror("nanosleep inibitore: ");
            //exit(1);
        }
    }
}
//Inizialmente dorme per tot secondi
//Inibitore controlla statistiche ogni mezzo secondo
//Se inibitore è attivo, controlla se le statistiche siano rispettate
//Se si raggiunge meltdown, ferma l'attivatore
