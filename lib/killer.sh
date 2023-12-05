#!/bin/bash

# assure that all the process are killed

echo Killing all remaining process...

killall -9 Atomo.out 1>&2
killall -9 Alimentatore.out 1>&2
killall -9 Attivatore.out 1>&2
killall -9 Inibitore.out 1>&2
killall -9 Master.out 1>&2

echo Done.