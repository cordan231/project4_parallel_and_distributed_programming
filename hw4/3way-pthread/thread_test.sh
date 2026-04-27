#!/bin/bash
#SBATCH --job-name=hw4_perf
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=4G
#SBATCH --partition=killable.q
#SBATCH --nodelist=mole[001-120]
#SBATCH --time=01:00:00

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0 CUDA/11.7.0

# Create a CSV file with headers for the raw data
echo "Threads,Real_Time_Sec,User_Time,Sys_Time,Peak_Mem_KB,CPU_Percent" > raw_data.csv

# Loop through the required core counts up to 16
for t in 1 2 4 8 16; do
    echo "Running with $t threads..."
    # /usr/bin/time tracks performance. 
    # %e = real time, %U = user time, %S = sys time, %M = peak memory, %P = CPU usage
    /usr/bin/time -f "$t,%e,%U,%S,%M,%P" -a -o raw_data.csv ./pthreads_app $t ~eyv/cis520/wiki_dump.txt > /dev/null
done

echo "Done! Results saved to raw_data.csv"