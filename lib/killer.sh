#!/bin/bash

# assure that all the process are killed

echo $'Killing all remaining process...'

killall -9 Atomo.out > /dev/null 2>&1
killall -9 Alimentatore.out > /dev/null 2>&1
killall -9 Attivatore.out > /dev/null 2>&1
killall -9 Inibitore.out > /dev/null 2>&1
killall -9 Master.out > /dev/null 2>&1

echo $'Done.\n'
exit 0