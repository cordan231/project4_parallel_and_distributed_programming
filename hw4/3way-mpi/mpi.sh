#!/bin/bash

# Set the name of the source code file and the executable
SRC=main.c
OBJ=mpi_test

# Compile the source code
mpicc -o $OBJ $SRC

# number of processes to be spawned
NUM=4
# Run the executable file
mpirun -n $NUM ./$OBJ $1 $2

# Delete the executable file
rm $OBJ
