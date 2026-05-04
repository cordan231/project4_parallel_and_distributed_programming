#!/bin/bash
#SBATCH --job-name=hw4_mpi_perf
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --mem=4G
#SBATCH --partition=killable.q
#SBATCH --nodelist=mole[001-120]
#SBATCH --time=01:00:00
#SBATCH --output=perf_%j.txt

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a

# Compile once
make

# Output CSV header
echo "Threads,Nodes,Mem,Real_Time,User_Time,Sys_Time,Peak_Mem_KB,CPU_Percent" > mpi_results.csv

for t in 1 2 4 8 16; do
    echo "Running $t threads (1 node, 1G mem)..."

    /usr/bin/time -f "$t,1,1G,%e,%U,%S,%M,%P" \
    -a -o mpi_results.csv \
    ./3way-mpi 4 /homes/eyv/cis520/wiki_dump.txt > /dev/null
done

echo "Running node scaling test (8 threads, 2 nodes)..."

export OMP_NUM_THREADS=8

/usr/bin/time -f "8,2,1G,%e,%U,%S,%M,%P" \
-a -o mpi_results.csv \
./3way-mpi 4 /homes/eyv/cis520/wiki_dump.txt > /dev/null

for mem in 512M 3G; do
    echo "Running 8 threads with $mem memory..."

    /usr/bin/time -f "8,1,$mem,%e,%U,%S,%M,%P" \
    -a -o mpi_results.csv \
    ./3way-mpi 4 /homes/eyv/cis520/wiki_dump.txt > /dev/null
done

echo "DONE - Results saved to mpi_results.csv"
