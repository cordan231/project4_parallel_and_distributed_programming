#!/bin/bash
#SBATCH --job-name=hw4_pthread          # Job name
#SBATCH --nodes=1                       # Run on a single node
#SBATCH --ntasks=1                      # Run a single task
#SBATCH --cpus-per-task=4               # Number of CPU cores per task
#SBATCH --mem=1G                        # Total memory limit
#SBATCH --partition=killable.q          # Partition to submit to   
#SBATCH --nodelist=mole[001-120]        # Specify the node(s) to run on
#SBATCH --time=00:15:00                 # Time limit hrs:min:sec
#SBATCH --output=result_%j.txt          # Standard output and error log

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0 CUDA/11.7.0

./pthreads_app 4 ~eyv/cis520/wiki_dump.txt