#include "lib/generalLib.h"
#include "lib/key.h"

//I SIGUSR fanno riferimento allo stato dell'inibitore. SIGUSR1: Inibitore acceso, da 0 a Attivatore per continuare in modo normale;
//SIGUSR2: Inibitore spento, da 1 ad Attivatore per rallentare l'attivazione di N secondi;


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

struct message_buffer{
    long mtype;
    int mex;
};


int main(int argc, char* argv[]){

    
    int msgid; 
    struct message_buf messaggio;


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
    int energia_ls = 0;
    int Energy_Threshold = atoi(argv[2]); // argv[2]

    int sem_sm = semget(KEY_SEM_SM, 13, 0777);

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = atoi(argv[1]);

    //Creo coda di messaggi per parlare con Atomo
    int msgid_meltdown = msgget(KEY_INHIB_MELTDOWN,0777); //msgid tiene id per comunicare con inibitore

    //struttura msgctl
    struct msqid_ds buf; //Nicola: Questo credo sia inutile perché non lo vedo usato da nessuna parte qui.
    //struct messaggio
    struct message_buffer mex;
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
            //printf("\nciclo\n");
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
            energia_ls = st->energy_created_ls;

            if(energia_attuale * 2 > Energy_Threshold){
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

            curr_process = st->current_atoms;
            //Possibile cambiare le soglie, ma l'avanzata degli atomi si rallenta di brutto a 3500 circa.
            if(curr_process > max_c_process/2){ //Mangio fra 2000 e 3000 atomi
                gravita_MTD = (rand() % (3000-2000))+2000;
            }else if(curr_process > max_c_process/4){ //Mangio fra 1500 e 2300 atomi
                gravita_MTD = (rand() % (2300-1500))+1500;
            }else if(curr_process > max_c_process/5){//Mangio fra i 1200 e i 1800 atomi
                //gravita_MTD = 1500;
                gravita_MTD = (rand() % (1800-1200))+1200;
            }else if(curr_process > max_c_process/6){ // Mangio fra 1100 e 1600 atomi
                gravita_MTD = (rand() % (1600-1100))+1100;
            }else if(curr_process > max_c_process/7){ //Mangio fra 1000 e 1400 atomi
                gravita_MTD = (rand() % (1400-1000))+1000;
            }else if(curr_process > max_c_process/8){// Mangio fra 900 e 1200 atomi
                gravita_MTD = (rand() % (1200-900))+900;
            }else if(curr_process > max_c_process/10){ //Mangio fra i 600 e i 1000 atomi
                gravita_MTD = (rand() % (1000-600))+600;
            }else if(curr_process > max_c_process/15){ //Mangio fra 40 e 120 atomi
                gravita_MTD = (rand() % (120-40))+40;
            }else if(curr_process > max_c_process/20){ //Mangio fra i 20 e gli 80 atomi
                gravita_MTD = (rand() % (80-20))+20;
            }else if(curr_process > max_c_process/30){ //Mangio fra 0 e 50 atomi
                gravita_MTD = rand() % 50+1;
            }else{ //Non mangio atomi
                gravita_MTD = 0;
            }
            if(gravita_MTD != 0){ //INVIO MESSAGGI SOLO SE LA QUANTITÀ DI ATOMI DA UCCIDERE È MAGGIORE DI ZERO
		    mex.mex = gravita_MTD;
		    printf("\nUccido %d Atomi\n", mex.mex);
		    msgsnd(msgid_meltdown, &mex, sizeof(mex.mex), 0);
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
