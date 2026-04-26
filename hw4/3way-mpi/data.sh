#!/bin/bash

echo "jobid,type,param,tasks,nodes,mem,time_sec,cpu_percent" > results.csv

for out in logs/*/out_*.txt
do
    jobid=$(basename "$out" .txt | awk -F'_' '{print $NF}')
    err="${out/out_/err_}"

    type=$(echo "$out" | cut -d'/' -f2)
    param=$(basename "$out" | sed 's/out_\(.*\)\.txt/\1/')

    tasks=$(awk '/Tasks:/ {print $2}' "$out")
    nodes=$(awk '/Nodes:/ {print $2}' "$out")
    mem=$(awk '/Mem per CPU:/ {print $4}' "$out")

    time=$(awk -F': ' '/Elapsed \(wall clock\)/ {print $2}' "$err")
    cpu=$(awk '/Percent of CPU/ {gsub("%","",$7); print $7}' "$err")

    echo "$jobid,$type,$param,$tasks,$nodes,$mem,$time,$cpu" >> results.csv
done
