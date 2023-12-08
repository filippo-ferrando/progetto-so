#!/bin/bash

# assure that all the process are killed

echo $'Killing all remaining process...'

killall -9 Atomo.out > /dev/null 2>&1
killall -9 Alimentatore.out > /dev/null 2>&1
killall -9 Attivatore.out > /dev/null 2>&1
killall -9 Inibitore.out > /dev/null 2>&1
killall -9 Master.out > /dev/null 2>&1

# close ipc resources
filename="ipcs_id_sem.txt"
filename1="ipcs_id_mem.txt"
filename2="ipcs_id_q.txt"

for sem in $(cat $filename)
do
    ipcrm -s $sem > /dev/null 2>&1
done

for mem in $(cat $filename1)
do
    ipcrm -m $mem > /dev/null 2>&1
done

for q in $(cat $filename2)
do
    ipcrm -q $q > /dev/null 2>&1
done

#rm ipcs_id_mem.txt ipcs_id_sem.txt ipcs_id_q.txt

echo $'Done.\n'
exit 0