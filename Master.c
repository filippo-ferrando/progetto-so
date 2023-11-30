#include "lib/header.h"

int main(int argc, char* argv[]){
    
    
    //ciclo creazione processi atomo
    for(int i=0; i<NUM_ATOMI; i++){
        pid_t pid = fork();
        if(pid == 0){
            //processo atomo
            execve();
        }
    }

    //creazione processo attivatore
    execve();

    //creazione processo alimentatore

    execve();

    //crezione processo inibitore
    execve();

    //ciclo di stampa per status

    while();

    //terminazione
    exit(0);


}