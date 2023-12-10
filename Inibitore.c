#include "lib/generalLib.h"
#include "lib/key.h"

struct message_buf{
    long mtype;
    int mex;
};

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


int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);
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

    int sem_sm = semget(KEY_SEM_SM, 9, 0777);

    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = atoi(argv[2]);

    //Creo coda di messaggi per parlare con Attivatore
    msgid = msgget(KEY_INHIB,0777);
    //Imposto tipo di messaggio per la coda di messaggi
    messaggio.mtype = 1; 

    if(reserveSem(sem_start, 0) < 0){ //Sincronizzo l'inibitore con il resto dei processi tramite semaforo.
        perror("reserveSem start inibitore: ");
        exit(1);
    }

    sleep(1);

    while(1){ 
        if(reserveSem(sem_sm, 5) < 0){
            perror("reserveSem sm inibitore: ");
            exit(1);
        }
        actual_energy = st->energy_created_total;
        if(releaseSem(sem_sm, 5) < 0){
            perror("releaseSem sm inibitore: ");
            exit(1);
        }

        //Ad ogni ciclo controllo lo stato dell'inibitore. Se è spento(0), mando un messaggio all'Attivatore di continuare; 
        //Se è acceso(1), controllo i valori per decidere se tenere a bada l'attivatore o meno

        if(stato_inib == 0){ //Se inibitore è spento, mando messaggio 0 all'attivatore
            messaggio.mex = 0;
            msgsnd(msgid,&messaggio,sizeof(messaggio) - sizeof(long),0);
        }else if(stato_inib == 1){ //Se inibitore è acceso, controllo se devo mandare messaggio 1 o 0 all'attivatore
            if(actual_energy + 150 > atoi(argv[3])){ //Se sbrocco, mando il messaggio di stop.
                messaggio.mex = 1;
                msgsnd(msgid,&messaggio,sizeof(messaggio) - sizeof(long),0);
            }else{
                messaggio.mex = 0;
                msgsnd(msgid,&messaggio,sizeof(messaggio) - sizeof(long),0);
            }
        }

        if(actual_energy + 150 > atoi(argv[3])){ //Se sbrocco, mando il messaggio di stop.
            kill(getpid(),SIGUSR2);
        }else{ //Altrimenti no.
            kill(getpid(),SIGUSR1);
        }
        nanosleep(&remaining, &request);

    }
//Inizialmente dorme per tot secondi
//Inibitore controlla statistiche ogni mezzo secondo
//Se inibitore è attivo, controlla se le statistiche siano rispettate
//Se si raggiunge meltdown, ferma l'attivatore

    exit(0); 
}