#!/bin/bash
#SBATCH --job-name=hw4_openmp
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=2G
#SBATCH --partition=killable.q
#SBATCH --nodelist=mole[001-120]
#SBATCH --time=00:20:00
#SBATCH --output=run_%j.txt

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a

export OMP_NUM_THREADS=8

make

./openmp_app 8 ~eyv/cis520/wiki_dump.txt
