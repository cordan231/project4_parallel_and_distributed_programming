#!/bin/bash

#SBATCH --job-name=scorecard_mpi
#SBATCH --output=logs/out_%j.txt
#SBATCH --error=logs/err_%j.txt
#SBATCH --time=00:30:00
#SBATCH --partition=batch.q
#SBATCH --constraint=moles

echo "=============================="
echo "Job ID: $SLURM_JOB_ID"
echo "Nodes: $SLURM_JOB_NUM_NODES"
echo "Tasks: $SLURM_NTASKS"
echo "CPUs per Task: $SLURM_CPUS_PER_TASK"
echo "Mem per CPU: $SLURM_MEM_PER_CPU"
echo "=============================="

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0

# Run with timing
/usr/bin/time -v srun ./3way-mpi
