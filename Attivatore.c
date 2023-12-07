#include "lib/projectLib.h"
#include "lib/key.h"

//handler per gestire segnale di pausa da inibitore

struct message_buff {
    long mtype;
    int mex;
};

int main(){
    int sem_start = semget(KEY_SEM_ACT, 1, IPC_CREAT | 0666);
    int sem_sm = semget(KEY_SEM_SM, 9, IPC_CREAT | 0666);
    int sem_att = semget(KEY_ATT, 1, IPC_CREAT | 0666);

    FILE *ipcs_id = fopen("ipcs_id_sem.txt", "a");
    fprintf(ipcs_id, "%d\n", sem_att);
    fclose(ipcs_id);

    int n_attivazioni = 10;

    int msgid = msgget(KEY_INHIB,IPC_CREAT|0600); //msgid tiene id per comunicare con inibitore
    struct message_buff messaggio; //Tiene il messaggio che riceve dall'inibitore
    messaggio.mtype = 1; //mtype a 1 per comunicare con l'inibitore

    FILE *ipcs_id = fopen("ipcs_id_q.txt", "a");
    fprintf(ipcs_id, "%d\n", msgid);
    fclose(ipcs_id);

    struct stats *st;
    int shmid = shmget(KEY_SHM, sizeof(st), 0666);
    st = shmat(shmid, NULL, 0);
    
    reserveSem(sem_start, 0);
    printf("SONO ATTIVATORE | HO PID %d\n", getpid());

    while(1){
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
    //riservo semaforo di sm per scrivere n attivazione
        if(reserveSem(sem_sm, 0) < 0){
            perror("reserveSem sm attivazioni attivatore: ");
            exit(1);
        }
        printf("attivatore in sezione critica sm\n");
        st->activations_ls+=n_attivazioni;
        if(releaseSem(sem_sm, 0) < 0){
            perror("releaseSem sm attivazioni attivatore: ");
            exit(1);
        }
        printf("attivatore rilasica sem att\n");
        if(semctl(sem_att, 0, SETVAL, n_attivazioni) < 0){
            perror("semctl attivatore: ");
            exit(1);
        }
        
    }
    //printf("SEM VAL attivatore: %d\n", semctl(semid, 0, GETVAL));
    
    exit(0);
}