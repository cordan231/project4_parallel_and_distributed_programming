#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Define the maximum batch size
#define BATCH_SIZE 5000

char **lines;
int *max_vals;
int current_batch_size;

/*
 * Main execution function
 */
int main(int argc, char *argv[]) {
    int num_threads;

    // Check arguments
    if (argc < 3) {
        printf("Usage: %s <num_threads> <file_path>\n", argv[0]);
        return 1;
    }

    // Parse thread count
    if (sscanf(argv[1], "%d", &num_threads) != 1) {
        num_threads = 1;
    }
    if (num_threads < 1) num_threads = 1;

    // Open file
    FILE *file = fopen(argv[2], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Allocate memory
    lines = malloc(BATCH_SIZE * sizeof(char *));
    max_vals = malloc(BATCH_SIZE * sizeof(int));
    size_t *lens = malloc(BATCH_SIZE * sizeof(size_t));

    for (int i = 0; i < BATCH_SIZE; i++) {
        lines[i] = NULL;
        lens[i] = 0;
    }

    int global_line_num = 0;
    int keep_reading = 1;

    // Start timing
    double start_time = omp_get_wtime();

    while (keep_reading) {
        current_batch_size = 0;

        // Read batch
        for (int i = 0; i < BATCH_SIZE; i++) {
            ssize_t read = getline(&lines[i], &lens[i], file);

            if (read == -1) {
                keep_reading = 0;
                break;
            }
            current_batch_size++;
        }

        if (current_batch_size == 0) break;

        // Parallel computation
        #pragma omp parallel for num_threads(num_threads) schedule(static)
        for (int i = 0; i < current_batch_size; i++) {
            int max_val = 0;
            char *curr = lines[i];

            while (*curr != '\0' && *curr != '\n') {
                if ((int)*curr > max_val) {
                    max_val = (int)*curr;
                }
                curr++;
            }

            max_vals[i] = max_val;
        }

        // Print results in order
        for (int i = 0; i < current_batch_size; i++) {
            printf("%d: %d\n", global_line_num++, max_vals[i]);
        }
    }

    // End timing
    double end_time = omp_get_wtime();
    printf("Total Time: %f seconds\n", end_time - start_time);

    // Cleanup
    for (int i = 0; i < BATCH_SIZE; i++) {
        free(lines[i]);
    }

    free(lines);
    free(lens);
    free(max_vals);

    fclose(file);

    return 0;
}