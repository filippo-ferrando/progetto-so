#!/bin/bash

# Set environment variables

echo Setting necessary environment variables...

export ENERGY_DEMAND=10
echo Setting ENERGY_DEMAND to $ENERGY_DEMAND

export N_ATOMI_INIT=50
echo Setting N_ATOMI_INIT to $N_ATOMI_INIT

export N_ATOM_MAX=200
echo Setting N_ATOM_MAX to $N_ATOM_MAX

export MIN_N_ATOMICO=1
echo Setting MIN_N_ATOMICO to $MIN_N_ATOMICO

export N_NUOVI_ATOMI=25
echo Setting N_NUOVI_ATOMI to $N_NUOVI_ATOMI

export SIM_DURATION=60
echo Setting SIM_DURATION to $SIM_DURATION

export ENERGY_EXPLODE_THRESHOLD=10000
echo Setting ENERGY_EXPLODE_THRESHOLD to $ENERGY_EXPLODE_THRESHOLD

export STEP=599999999
echo Setting STEP to $STEP

export ATT_STEP=150000000
echo Setting ATT_STEP to $ATT_STEP

export INIBIT_ATT=1
echo INIBITORE A $INIBIT_ATT

export INIBIT_CHECK=500000000
echo INIBIT_CHECK A $INIBIT_CHECK

exec $SHELL -i