#!/bin/bash

#SBATCH --job-name=hw4_mpi
#SBATCH --constraint=moles
#SBATCH --time=01:00:00
#SBATCH --output=results_%j.out
#SBATCH --partition=killable.q
#SBATCH --ntasks=4

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0 CUDA/11.7.0

# Run your application
# Use srun to ensure it utilizes the allocated cores and nodes
./3way-mpi 4 /homes/eyv/cis520/wiki_dump.txt
