# Project 4: Parallel and Distributed Programming

## Compiling 3way-=pthread
To compile pthreads version on Beocat:
1. Navigate to pthread directory:
    cd 3way-pthread

2. Load required module:
    module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0 CUDA/11.7.0

3. Compile using command:
    gcc -pthread -O3 main.c -o pthreads_app


## Running and Scheduling
1. Submit job to scheduler:
    sbatch submit.sh

2. Monitor job status:
    squeue -u <your_eid>

3. View results:
    The output will be saved in a file named 'result_<job_id>.txt'

Example to view:
    head -n 20 result_7600430.txt\

## Usage
./pthreads_app <number_of_threads> <path_to_input_file>

Example:
./pthreads_app 4 ~eyv/cis520/wiki_dump.txt


To get precise execution durations you can run 

time ./pthreads_app 4 ~eyv/cis520/wiki_dump.txt