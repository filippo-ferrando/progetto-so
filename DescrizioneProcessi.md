# Bozza descrizione di ogni processo (e interazione con gli altri processi)

## Master
Master inizializzia (execve) gli altri processi che andranno in running nella prima fase di esecuzione
Master tiene traccia delle "statistiche" della simulazione
Master sarà preposto a "segnalare" all'handler (inibitore) il mometo in cui stoppare alimetatore (troppi atomi running) o stoppare la scissione (troppa energia rilasciata) 
Master ogni secondo stamperà le statistiche del programma in esecuzione
Master ogni x tempo preleva n energia dal sistema
Master si occuperà di terminare i processi arrivati alla fine del tempo assegnato per l'esecuzione

## Atomo
Il processo atomo è creato esclusivamente dall'alimentatore
Ogni processo atomo contiene il numero atomico: fissato che generato dall'alimentatore - calcolato se generato da una scissione

## Alimentatore
Alimentatore ad inizio simulazione crea n-atomi definiti dalla variabile in env o da file
Alimentatore ogni x tempo genera n atomi
Alimentatore deve poter essere stoppato da Inibitore (o rallentato) se il numero di atomi running è eccessivo (meltdown)

## Attivatore
Attivatore decide quali atomi devono scindersi (criterio da decidere)
Attivatore entra in funzione ogni x tempo e scinde n atomi
Attivatore deve poter essere stoppato da Inibitore (o rallentato) se l'energia liberata dalla scissione è eccessiva (explode)

## Inibitore
Inibitore riceve da master un segnale; in base al segnale ricevuto decide quale dei processi è da rallentare (Attivatore o Alimentatore)
Inibitore deve poter essere stoppato e riattivato MENTRE il programma è in esecuzione
Inibitore è una sorta di handler di segnali

