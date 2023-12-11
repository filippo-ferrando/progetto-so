#include "lib/generalLib.h"
#include "lib/key.h"

//handler per gestire segnale di pausa da inibitore

//inibitore parla con attivatore attraverso una cosa di messaggi
struct message_buff {
    long mtype;
    int mex;
};

int main(int argc, char* argv[]){
    int sem_start = semget(KEY_SEM_ACT, 1, 0777);
    int sem_sm = semget(KEY_SEM_SM, 9, 0777);

    //inizializzo semaforo attivatore a 0
    int sem_att = semget(KEY_ATT, 1, IPC_CREAT|0777);
    semctl(sem_att, 0, SETVAL, 0);

    //file per gestione rimozione di risorse ipc
    FILE *ipcs_id_s = fopen("ipcs_id_sem.txt", "a");
    fprintf(ipcs_id_s, "%d\n", sem_att);
    fclose(ipcs_id_s);

    //struct per nanosleep
    struct timespec remaining, request;
    remaining.tv_sec = 0;
    remaining.tv_nsec = atoi(argv[1]);  //0,15s da definire metodo di decisione tempo di attivazione

    //numero di attivazioni che l'attivatore deve fare
    int n_attivazioni = 15;    //da definire metodo di decisione numero di attivazioni

    int msgid = msgget(KEY_INHIB,IPC_CREAT|0777); //msgid tiene id per comunicare con inibitore
    struct message_buff messaggio; //Tiene il messaggio che riceve dall'inibitore
    messaggio.mtype = 1; //mtype a 1 per comunicare con l'inibitore

    //file per gestione rimozione di risorse ipc
    FILE *ipcs_id_q = fopen("ipcs_id_q.txt", "a");
    fprintf(ipcs_id_q, "%d\n", msgid);
    fclose(ipcs_id_q);

    //attach to shared memory
    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0777);
    st = shmat(shmid, NULL, 0);
    
    //sincronizzo attivatore con resto dei processi
    if(reserveSem(sem_start, 0) < 0){
        perror("reserveSem start attivatore: ");
        exit(1);
    
    }
    printf("SONO ATTIVATORE | HO PID %d\n", getpid());

    while(1){
        /* da implementare inibitore

        msgrcv(msgid,&messaggio,sizeof(messaggio) - sizeof(long),1,0); //Dopo che leggo un messaggio, svuoto coda di messaggi.
        if(messaggio.mex == 0){  //Se leggo 1 dall'inibitore procedo, altrimenti sto aspetto di ricevere quel valore dalla receive
            printf("attivatore riceve 0\n");
            sleep(1);
            //svuota coda di messaggi
        }else if(messaggio.mex == 1){
            printf("attivatore riceve 1\n");
            sleep(5);
            //svuota coda di messaggi
        }

        */
        //riservo semaforo di sm per scrivere n attivazione
        if(reserveSem(sem_sm, 0) < 0){
            perror("reserveSem sm attivazioni attivatore: ");
            exit(1);
        }
        //printf("attivatore in sezione critica sm\n");
        st->activations_ls+=n_attivazioni;
        if(releaseSem(sem_sm, 0) < 0){
            perror("releaseSem sm attivazioni attivatore: ");
            exit(1);
        }
        //printf("attivatore rilasica sem att\n");
        if(semctl(sem_att, 0, SETVAL, n_attivazioni) < 0){
            perror("semctl attivatore: ");
            exit(1);
        }

        nanosleep(&remaining, &request);
        
    }
    //printf("SEM VAL attivatore: %d\n", semctl(semid, 0, GETVAL));
    
    exit(0);
}