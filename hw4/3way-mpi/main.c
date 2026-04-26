#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <mpi.h>

#define MAX_BATCH_SIZE 5000
#define TAG_CHILD_KILL 0xFF


char max(const char *line, size_t len) {
	char result = 0;

	for(size_t i = 0; i < len; i += 1) {
		result = (line[i] > result && line[i] != '\n') ? line[i] : result;
	}

	return result;
}

int main(int argc, char **argv) {
	int pid;
	int num_processes = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

	printf("%d\n", num_processes);

	/* Read in file contents */
	if(pid == 0) {
		MPI_Status status;
		bool keep_reading = true;
		FILE *fp = fopen("/homes/eyv/cis520/wiki_dump.txt", "r");

		if(!fp) {
			perror("Error opening file");
			MPI_Abort(MPI_COMM_WORLD, 1);

			return 1;
		}

		// Receiving
		char maxes[MAX_BATCH_SIZE] = { 0 };
		// Sending
		size_t lens[MAX_BATCH_SIZE] = { 0 };
		char *lines[MAX_BATCH_SIZE] = { 0 };
		// Current line in the entire file
		size_t line = 0;

		// Process
		while(keep_reading) {
			size_t process_batch_size = 0;
			size_t batch_size = 0;

			for(int i = 0; i < MAX_BATCH_SIZE; i++) {
				ssize_t read = getline(&lines[i], &lens[i], fp);

				// EOF
				if(read == -1) {
					keep_reading = 0;
					break;
				}

				batch_size++;
			}

			if(batch_size == 0) {
				break;
			}

			process_batch_size = batch_size / (num_processes - 1);

			/* Divide work to children */
			for(int p = 1; p < num_processes; p += 1) {
				size_t start = (p - 1) * process_batch_size;
				size_t count = process_batch_size;

				MPI_Send(&count, 1, MPI_UNSIGNED_LONG_LONG, p, 0, MPI_COMM_WORLD);
				MPI_Send(&lens[start], count, MPI_UNSIGNED_LONG_LONG, p, 0, MPI_COMM_WORLD);

				// Send lines to children
				for(size_t li = 0; li < count; li += 1) {
					MPI_Send(lines[start + li], lens[start + li], MPI_CHAR, p, 0, MPI_COMM_WORLD);
				}

			}

			printf("Process batch_size: %zu\n", process_batch_size * (num_processes - 1));

			/* Master work, should be a lot smaller than the others so do now */
			{
				size_t master_start = process_batch_size * (num_processes - 1);
				size_t count = batch_size - master_start;

				for(size_t li = 0; li < count; li += 1) {
					maxes[li + master_start] = max(lines[li + master_start], lens[li + master_start]);
				}
			}

			/* Non blocking wait for result */
			int processes_running = num_processes - 1;

			while(processes_running != 0) {
				char temp_maxes[MAX_BATCH_SIZE] = { 0 };

				MPI_Recv(&temp_maxes, process_batch_size, MPI_CHAR,
					 MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				processes_running -= 1;
				memcpy(&maxes[(status.MPI_SOURCE - 1) * process_batch_size], temp_maxes, process_batch_size);
			}

			for(size_t i = 0; i < batch_size; i += 1) {
				printf("%zu: %d\n", line + 1, maxes[i]);
				free(lines[i]);
				lines[i] = NULL;
				line += 1;
			}
		}

		for(int p = 1; p < num_processes; p++) {
			size_t stop = 0;
			MPI_Send(&stop, 1, MPI_UNSIGNED_LONG_LONG, p, TAG_CHILD_KILL, MPI_COMM_WORLD);
		}

		fclose(fp);
	} else {
		while(true) {
			// Receiving
			size_t process_batch_size = 0;
			size_t *lens = NULL;
			MPI_Status child_status;
			// Sending
			char *maxes = NULL;

			// Receive process batch size
			MPI_Recv(&process_batch_size, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &child_status);

			if(child_status.MPI_TAG == TAG_CHILD_KILL) {
				break;
			}

			// Allocate memory for maxesupfront
			maxes = malloc(process_batch_size * sizeof(*maxes));

			if(maxes == NULL) {
				// Error
			}

			// Receive length
			lens = malloc(process_batch_size * sizeof(*lens));

			if(lens == NULL) {
				// Error
			}

			MPI_Recv(lens, process_batch_size, MPI_UNSIGNED_LONG_LONG, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &child_status);

			for(size_t i = 0; i < process_batch_size; i += 1) {
				char *line = malloc((lens[i] + 1) * sizeof(*line));

				if(line == NULL) {
					// Error
					break;
				}

				MPI_Recv(line, lens[i], MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &child_status);
				maxes[i] = max(line, lens[i]);
				free(line);
			}

			MPI_Send(maxes, process_batch_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			free(lens);
			free(maxes);
		}
	}

	MPI_Finalize();

	return 0;
}
