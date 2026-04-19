#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the maximum batch size
#define BATCH_SIZE 5000

char **lines;
int *max_vals;
int current_batch_size;
int num_threads;

/*
 * Thread worker function that processes a subset of the current batch.
 * It iterates through its assigned lines and finds the maximum ASCII value in each.
 */
void *process_batch(void *arg) {
    // Cast the argument back to a thread ID
    long tid = (long)arg;
    
    // Calculate the chunk size and the start/end indices for this thread
    int chunk_size = current_batch_size / num_threads;
    int start = tid * chunk_size;
    int end = (tid == num_threads - 1) ? current_batch_size : start + chunk_size;

    // Iterate over the assigned subset of lines
    for (int i = start; i < end; i++) {
        // Initialize the maximum value for the current line
        int max_val = 0;
        char *curr = lines[i];
        
        // Loop through each character until the null terminator or newline is reached
        while (*curr != '\0' && *curr != '\n') {
            // Update the maximum value if the current character is greater
            if ((int)*curr > max_val) {
                max_val = (int)*curr;
            }
            // Move to the next character
            curr++;
        }
        // Store the computed maximum value in the global array
        max_vals[i] = max_val;
    }
    // Return NULL as the thread has completed its task
    return NULL;
}

/*
 * Main execution function that initializes data, reads the file in batches,
 * and manages the thread lifecycle to process the data concurrently.
 */
int main(int argc, char *argv[]) {
    // Verify that the correct number of arguments is provided
    if (argc < 3) {
        printf("Usage: %s <num_threads> <file_path>\n", argv[0]);
        return 1;
    }

    // Parse the number of threads using sscanf
    if (sscanf(argv[1], "%d", &num_threads) != 1) {
        num_threads = 1;
    }
    
    // Ensure at least one thread is used
    if (num_threads < 1) {
        num_threads = 1;
    }

    // Open the specified file for reading
    FILE *file = fopen(argv[2], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Allocate memory for the line pointers and maximum values arrays
    lines = malloc(BATCH_SIZE * sizeof(char *));
    max_vals = malloc(BATCH_SIZE * sizeof(int));

    // Initialize the line pointers to NULL
    for (int i = 0; i < BATCH_SIZE; i++) {
        lines[i] = NULL;
    }

    // Allocate memory for tracking line lengths used by getline
    size_t *lens = malloc(BATCH_SIZE * sizeof(size_t));
    for (int i = 0; i < BATCH_SIZE; i++) {
        lens[i] = 0;
    }

    // Allocate memory for the thread identifiers
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));

    // Initialize variables to track overall progress and loop status
    int global_line_num = 0;
    int keep_reading = 1;

    // Process the file continuously until the end is reached
    while (keep_reading) {
        // Reset the batch size counter for the new batch
        current_batch_size = 0;
        
        // Read lines sequentially up to the defined BATCH_SIZE
        for (int i = 0; i < BATCH_SIZE; i++) {
            // Read a single line and store the byte count
            ssize_t read = getline(&lines[i], &lens[i], file);
            
            // Check if the end of the file or an error occurred
            if (read == -1) {
                keep_reading = 0;
                break;
            }
            // Increment the counter for the current batch
            current_batch_size++;
        }

        // Exit the loop immediately if no lines were read
        if (current_batch_size == 0) {
            break;
        }

        // Spawn threads, assigning each a specific identifier to process the batch
        for (long i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, process_batch, (void *)i);
        }

        // Wait for all spawned threads to complete their execution
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // Print the line index and corresponding maximum ASCII value sequentially
        for (int i = 0; i < current_batch_size; i++) {
            printf("%d: %d\n", global_line_num++, max_vals[i]);
        }
    }

    // Free the dynamically allocated memory for each line
    for (int i = 0; i < BATCH_SIZE; i++) {
        free(lines[i]);
    }
    
    // Free the main arrays and thread structures
    free(lines);
    free(lens);
    free(max_vals);
    free(threads);
    
    // Close the file stream
    fclose(file);

    // Return standard success code
    return 0;
}