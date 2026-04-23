#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define BATCH_SIZE 5000


int max(const char *line) {
	int result = 0;

	while(*line) {
		result = (*line > result) ? *line : result;
		line += 1;
	}

	return result;
}

int main(int argc, char **argv) {
	MPI_Status status;
	int pid;
	int num_processes = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

	if(argc < 3) {
		fprintf(stderr, "Usage: %s <num_threads> <file_path>\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, 1);

		return 1;
	}

	num_processes = atoi(argv[1]);

	if(num_processes < 1) {
		num_processes = 1;
	}

	/* Read in file contents */
	if(pid == 0) {
		FILE *fp = NULL;
		bool keep_reading = true;
		// Open the file for reading
		fp = fopen(argv[2], "r");

		if(!fp) {
			perror("Error opening file");

			return 1;
		}

		// Process
		while(keep_reading) {
			char *lines[BATCH_SIZE] = { 0 };
			char maxes[BATCH_SIZE] = { 0 };
			size_t lens[BATCH_SIZE] = { 0 };
			size_t batch_size = 0;

			for (int i = 0; i < BATCH_SIZE; i++) {
				ssize_t read = getline(&lines[i], &lens[i], file);

				if(read == -1) {
					keep_reading = 0;   // End of file reached
					break;
				}

				current_batch_size++;
			}

			if(current_batch_size == 0) {
				break;
			}

			for(size_t i = 0; i < current_batch_size; i += 1) {
				free(lines[i]);
			}

		}


		fclose(fp);
		printf("hello from master\n");

		fclose(fp);
	} else {
		printf("hello from %d\n", pid);
	}

	MPI_Finalize();

	return 0;
}
