#!/bin/bash

# Set environment variables

echo Setting necessary environment variables...

export ENERGY_DEMAND=10
echo Setting ENERGY_DEMAND to $ENERGY_DEMAND

export N_ATOMI_INIT=10
echo Setting N_ATOMI_INIT to $N_ATOMI_INIT

export N_ATOM_MAX=118
echo Setting N_ATOM_MAX to $N_ATOM_MAX

export MIN_N_ATOMICO=1
echo Setting MIN_N_ATOMICO to $MIN_N_ATOMICO

export N_NUOVI_ATOMI=10
echo Setting N_NUOVI_ATOMI to $N_NUOVI_ATOMI

export SIM_DURATION=40
echo Setting SIM_DURATION to $SIM_DURATION

export ENERGY_EXPLODE_THRESHOLD=1000
echo Setting ENERGY_EXPLODE_THRESHOLD to $ENERGY_EXPLODE_THRESHOLD

export STEP=999999999
echo Setting STEP to $STEP

export INIBIT_ATT=1
echo INIBITORE A $INIBIT_ATT

export INIBIT_CHECK=500000000
echo INIBIT_CHECK A $INIBIT_CHECK

exec $SHELL -i