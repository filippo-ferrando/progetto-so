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

## Attivatore
Attivatore decide quali atomi devono scindersi (criterio da decidere)
Attivatore entra in funzione ogni x tempo e scinde n atomi
Attivatore deve poter essere stoppato da Inibitore (o rallentato) se l'energia liberata dalla scissione è eccessiva (explode)

## Inibitore
Inibitore riceve da master un segnale; stoppa il processo attivatore
Inibitore deve poter essere stoppato e riattivato MENTRE il programma è in esecuzione
Inibitore è una sorta di handler di segnali

# Bozza descrizione variabili di esecuzione

### da dove le leggiamo
Le opzioni sono:
- Settare tutto quello che ci viene dato come variabile d'ambiente (tramite bash script)
- Leggere da un file preposto

## Variabili
- **ENERGY_DEMAND**
    Energia prelevata volta per volta dal processo master
- **N_ATOM_INIT**
    Numero di atomi da creare all'inizio della simulazione
- **N_ATOM_MAX**
    Numero atomico massimo che può avere ogni singolo atomo
- **MIN_N_ATOMICO**
    Numero atomico minimo che può avere ogni singolo atomo (più piccolo viene eliminato)
- **N_NUOVI_ATOMI**
    Numero di atomi che Alimentatore crea ogni n tempo
- **SIM_DURATION**
    Durata della simulazione
- **ENERGY_EXPLODE_THRESHOLD**
    Energia massima che può essere liberata prima del verificarsi di explode

## Casistiche di Terminazione

- **timeout**
    La simulazione termina in modo "naturale" allo scadere tel tempo massimo definito in SIM_DURATION
- **explode**
    La simulazione termina a causa della troppa energia liberata (energia liberata > ENERGY_EXPLODE_THRESHOLD)
- **blackout**
    La simulazione termina a causa della troppa energia consumata
- **meltdown**
    La simulazione termina a causa delle troppe fork compiute per scindere/creare gli atomi