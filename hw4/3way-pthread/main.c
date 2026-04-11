#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define BATCH_SIZE 5000

// Global variables for threads to access
char **lines;   // Array of lines read from the file
int *max_vals;  // Array to store max ASCII values for each line
int current_batch_size;
int num_threads;

/**
 * Thread function to process a batch of lines and compute the max ASCII value for each line.
 * Each thread processes a portion of the batch based on its thread ID.
 */
void *process_batch(void *arg) {
    long tid = (long)arg;

    // Calculate the portion of the batch this thread will process
    int chunk_size = current_batch_size / num_threads;
    int start = tid * chunk_size;
    int end = (tid == num_threads - 1) ? current_batch_size : start + chunk_size;

    for (int i = start; i < end; i++) {
        int max_val = 0;
        char *curr = lines[i];

        // Iterate through the line to find the maximum ASCII value
        while (*curr != '\0' && *curr != '\n') {
            // Update max_val if the current character's ASCII value is greater
            if ((int)*curr > max_val) {
                max_val = (int)*curr;
            }
            curr++;
        }
        max_vals[i] = max_val;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // Check for correct number of command-line arguments
    if (argc < 3) {
        printf("Usage: %s <num_threads> <file_path>\n", argv[0]);
        return 1;
    }

    num_threads = atoi(argv[1]);
    if (num_threads < 1) num_threads = 1;

    // Open the file for reading
    FILE *file = fopen(argv[2], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Allocate memory for lines and max_vals arrays
    lines = malloc(BATCH_SIZE * sizeof(char *));
    max_vals = malloc(BATCH_SIZE * sizeof(int));

    for (int i = 0; i < BATCH_SIZE; i++) {
        lines[i] = NULL;
    }

    size_t *lens = malloc(BATCH_SIZE * sizeof(size_t));
    for (int i = 0; i < BATCH_SIZE; i++) lens[i] = 0;

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));

    int global_line_num = 0;
    int keep_reading = 1;

    // Main loop to read batches of lines, process them with threads, and print results
    while (keep_reading) {
        current_batch_size = 0;

        // Read a batch of lines from the file
        for (int i = 0; i < BATCH_SIZE; i++) {
            ssize_t read = getline(&lines[i], &lens[i], file);
            if (read == -1) {
                keep_reading = 0;   // End of file reached
                break;
            }
            current_batch_size++;
        }

        if (current_batch_size == 0) break;

        // Create threads to process the current batch of lines
        for (long i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, process_batch, (void *)i);
        }

        // Wait for all threads to finish processing
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // Print the max ASCII values for the current batch of lines
        for (int i = 0; i < current_batch_size; i++) {
            printf("%d: %d\n", global_line_num++, max_vals[i]);
        }
    }

    // Free allocated memory and close the file
    for (int i = 0; i < BATCH_SIZE; i++) {
        free(lines[i]);
    }
    free(lines);
    free(lens);
    free(max_vals);
    free(threads);
    fclose(file);

    return 0;
}