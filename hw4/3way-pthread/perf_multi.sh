#!/bin/bash
#SBATCH --job-name=hw4_perf_multi
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=4G
#SBATCH --partition=killable.q
#SBATCH --nodelist=mole[001-120]
#SBATCH --time=01:00:00

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0 CUDA/11.7.0

echo "Generating smaller input datasets..."
head -n 100000 /homes/eyv/cis520/wiki_dump.txt > wiki_100k.txt
head -n 500000 /homes/eyv/cis520/wiki_dump.txt > wiki_500k.txt

echo "Run,File_Size,Threads,Real_Time_Sec,User_Time,Sys_Time,Peak_Mem_KB,CPU_Percent" > raw_multi_data.csv
FILES=("wiki_100k.txt" "wiki_500k.txt" "/homes/eyv/cis520/wiki_dump.txt")

echo "Starting 5-run performance evaluation..."

for f in "${FILES[@]}"; do
    for t in 1 2 4 8 16; do
        for run in {1..5}; do
            /usr/bin/time -f "$run,$f,$t,%e,%U,%S,%M,%P" -a -o raw_multi_data.csv ./pthreads_app $t $f > /dev/null
        done
    done
done

rm wiki_100k.txt wiki_500k.txt
echo "Testing complete. Results saved to raw_multi_data.csv"