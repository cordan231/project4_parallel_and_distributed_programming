#!/bin/bash
#SBATCH --job-name=hw4_openmp_perf
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --mem=4G
#SBATCH --partition=killable.q
#SBATCH --nodelist=mole[001-120]
#SBATCH --time=01:00:00
#SBATCH --output=perf_%j.txt

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a

# Compile once
make

# Output CSV header
echo "Threads,Nodes,Mem,Real_Time,User_Time,Sys_Time,Peak_Mem_KB,CPU_Percent" > openmp_results.csv

# -----------------------------
# Core scaling (OpenMP scaling test)
# -----------------------------
for t in 1 2 4 8 16; do
    echo "Running $t threads (1 node, 1G mem)..."

    export OMP_NUM_THREADS=$t

    /usr/bin/time -f "$t,1,1G,%e,%U,%S,%M,%P" \
    -a -o openmp_results.csv \
    ./openmp_app $t ~eyv/cis520/wiki_dump.txt > /dev/null
done

# -----------------------------
# Node scaling test (OpenMP limitation demonstration)
# -----------------------------
echo "Running node scaling test (8 threads, 2 nodes)..."

export OMP_NUM_THREADS=8

/usr/bin/time -f "8,2,1G,%e,%U,%S,%M,%P" \
-a -o openmp_results.csv \
./openmp_app 8 ~eyv/cis520/wiki_dump.txt > /dev/null

# -----------------------------
# Memory sensitivity tests
# -----------------------------
for mem in 512M 3G; do
    echo "Running 8 threads with $mem memory..."

    export OMP_NUM_THREADS=8

    /usr/bin/time -f "8,1,$mem,%e,%U,%S,%M,%P" \
    -a -o openmp_results.csv \
    ./openmp_app 8 ~eyv/cis520/wiki_dump.txt > /dev/null
done

echo "DONE - Results saved to openmp_results.csv"
