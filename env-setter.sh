#!/bin/bash

# Set environment variables

echo Setting necessary environment variables...

processsLimit=$(ulimit -u)

export ENERGY_DEMAND=500
echo Setting ENERGY_DEMAND to $ENERGY_DEMAND

export N_ATOMI_INIT=100
echo Setting N_ATOMI_INIT to $N_ATOMI_INIT

export N_ATOM_MAX=118
echo Setting N_ATOM_MAX to $N_ATOM_MAX

export MIN_N_ATOMICO=1
echo Setting MIN_N_ATOMICO to $MIN_N_ATOMICO

export N_NUOVI_ATOMI=50
echo Setting N_NUOVI_ATOMI to $N_NUOVI_ATOMI

export SIM_DURATION=120
echo Setting SIM_DURATION to $SIM_DURATION

export ENERGY_EXPLODE_THRESHOLD=500000
echo Setting ENERGY_EXPLODE_THRESHOLD to $ENERGY_EXPLODE_THRESHOLD

export STEP=500000000
echo Setting STEP to $STEP

export ATT_STEP=800000000
echo Setting ATT_STEP to $ATT_STEP

export INIBIT_CHECK=200000000
echo INIBIT_CHECK A $INIBIT_CHECK

export SPLIT_ATOMS=100
echo INIBIT_CHECK A $INIBIT_CHECK

export MAX_PROCESS=$processsLimit
echo Setting MAX_PROCESS to $MAX_PROCESS

exec $SHELL -i