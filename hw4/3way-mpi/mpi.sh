#!/bin/bash

mkdir -p logs/cores logs/mem logs/nodes

tasks_list=(2 8 32)
mem_list=(128M 1G 3G)
node_list=(2 8)

repeats=5

# -------------------------
# TASKS / CORES TEST
# -------------------------
for tasks in "${tasks_list[@]}"
do
  for run in $(seq 1 $repeats)
  do
    export SLURM_N_TASKS=$tasks
    sbatch \
      --nodes=1 \
      --ntasks=$tasks \
      --cpus-per-task=1 \
      --mem-per-cpu=512M \
      --output="logs/cores/out_tasks${tasks}_run${run}_%j.txt" \
      --error="logs/cores/err_tasks${tasks}_run${run}_%j.txt" \
      submit.sh
  done
done

# -------------------------
# MEMORY TEST
# -------------------------
for mem in "${mem_list[@]}"
do
  for run in $(seq 1 $repeats)
  do
    export SLURM_N_TASKS=$tasks

    sbatch \
      --nodes=1 \
      --ntasks=8 \
      --cpus-per-task=1 \
      --mem-per-cpu=$mem \
      --output="logs/mem/out_mem${mem}_run${run}_%j.txt" \
      --error="logs/mem/err_mem${mem}_run${run}_%j.txt" \
      submit.sh
  done
done

# -------------------------
# NODE TEST
# -------------------------
for nodes in "${node_list[@]}"
do
  for run in $(seq 1 $repeats)
  do
    export SLURM_N_TASKS=$tasks

    sbatch \
      --nodes=$nodes \
      --ntasks=$((nodes * 4)) \
      --cpus-per-task=1 \
      --mem-per-cpu=512M \
      --output="logs/nodes/out_nodes${nodes}_run${run}_%j.txt" \
      --error="logs/nodes/err_nodes${nodes}_run${run}_%j.txt" \
      submit.sh
  done
done
