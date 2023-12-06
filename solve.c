#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORD_LEN 50
#define MAX_BUFFER_SIZE (64 * 1024 * 1024)

// Define the structure for a puzzle piece
typedef struct {
    char data[MAX_WORD_LEN];
    int row;
    int col;
} PuzzlePiece;

// Define the structure for the buffer used for communication
typedef struct {
    PuzzlePiece pieces[MAX_BUFFER_SIZE / sizeof(PuzzlePiece)];
    int count;
    int rows;
    int cols;
    char puzzle[MAX_WORD_LEN][MAX_WORD_LEN];
    pthread_mutex_t mutex;
    pthread_cond_t produce_cond;
    pthread_cond_t consume_cond;
} Buffer;

// Function prototypes
void *producer(void *arg);
void *consumer(void *arg);
void searchInDirection(Buffer *buffer, int startRow, int startCol, int rowStep, int colStep);

// Global variables
Buffer sharedBuffer;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <buffer_cells>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *inputFile = argv[1];
    int bufferCells = atoi(argv[2]);

    if (bufferCells != 1 && bufferCells != 4 && bufferCells != 16 && bufferCells != 64) {
        fprintf(stderr, "Invalid buffer cell count. It should be 1, 4, 16, or 64.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(inputFile, "r");

    if (!file) {
        fprintf(stderr, "Error opening file %s.\n", inputFile);
        exit(EXIT_FAILURE);
    }

    // Read puzzle dimensions
    int rows, cols;
    fscanf(file, "%d %d", &rows, &cols);

    // Read puzzle data
    // char puzzle[MAX_WORD_LEN][MAX_WORD_LEN];
    // for (int i = 0; i < rows; i++) {
    //     for (int j = 0; j < cols; j++) {
    //         fscanf(file, " %c", &puzzle[i][j]);
    //     }
    // }
    char puzzle[MAX_WORD_LEN][MAX_WORD_LEN];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%c", &puzzle[i][j]);
        }
        fscanf(file, "\n"); // Read the newline character after each row
    }

    fclose(file);

    // Initialize buffer
    sharedBuffer.count = 0;
    sharedBuffer.rows = rows;
    sharedBuffer.cols = cols;
    memcpy(sharedBuffer.puzzle, puzzle, sizeof(char) * MAX_WORD_LEN * MAX_WORD_LEN);
    pthread_mutex_init(&sharedBuffer.mutex, NULL);
    pthread_cond_init(&sharedBuffer.produce_cond, NULL);
    pthread_cond_init(&sharedBuffer.consume_cond, NULL);

    // Create producer and consumer threads
    pthread_t producerThread, consumerThreads[bufferCells];

    if (pthread_create(&producerThread, NULL, producer, inputFile) != 0) {
        fprintf(stderr, "Error creating producer thread.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < bufferCells; i++) {
        if (pthread_create(&consumerThreads[i], NULL, consumer, NULL) != 0) {
            fprintf(stderr, "Error creating consumer thread %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Join threads
    pthread_join(producerThread, NULL);
    for (int i = 0; i < bufferCells; i++) {
        pthread_join(consumerThreads[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&sharedBuffer.mutex);
    pthread_cond_destroy(&sharedBuffer.produce_cond);
    pthread_cond_destroy(&sharedBuffer.consume_cond);

    return 0;
}

void *producer(void *arg) {
    char *inputFile = (char *)arg;

    // ... (unchanged code)

    // Produce pieces and add them to the buffer
    for (int i = 0; i < sharedBuffer.rows; i++) {
        for (int j = 0; j < sharedBuffer.cols; j++) {
            PuzzlePiece piece;
            strcpy(piece.data, &sharedBuffer.puzzle[i][j]);
            piece.row = i;
            piece.col = j;

            pthread_mutex_lock(&sharedBuffer.mutex);

            while (sharedBuffer.count == MAX_BUFFER_SIZE / sizeof(PuzzlePiece)) {
                pthread_cond_wait(&sharedBuffer.produce_cond, &sharedBuffer.mutex);
            }

            sharedBuffer.pieces[sharedBuffer.count++] = piece;

            pthread_cond_signal(&sharedBuffer.consume_cond);
            pthread_mutex_unlock(&sharedBuffer.mutex);
        }
    }

    pthread_exit(NULL);
}

void *consumer(void *arg) {
    // ... (unchanged code)

    while (1) {
        pthread_mutex_lock(&sharedBuffer.mutex);

        while (sharedBuffer.count == 0) {
            pthread_cond_wait(&sharedBuffer.consume_cond, &sharedBuffer.mutex);
        }

        PuzzlePiece piece = sharedBuffer.pieces[--sharedBuffer.count];

        pthread_cond_signal(&sharedBuffer.produce_cond);
        pthread_mutex_unlock(&sharedBuffer.mutex);

        // Process the puzzle piece
        searchInDirection(&sharedBuffer, piece.row, piece.col, 1, 0);   // Search horizontally
        searchInDirection(&sharedBuffer, piece.row, piece.col, 0, 1);   // Search vertically
        searchInDirection(&sharedBuffer, piece.row, piece.col, 1, 1);   // Search diagonally (slope +1)
        searchInDirection(&sharedBuffer, piece.row, piece.col, 1, -1);  // Search diagonally (slope -1)
    }
}

void searchInDirection(Buffer *buffer, int startRow, int startCol, int rowStep, int colStep) {
    int currentRow = startRow;
    int currentCol = startCol;
    int wordLen = 0;
    char word[MAX_WORD_LEN];

    // Initialize the word with the starting character
    word[wordLen++] = buffer->puzzle[currentRow][currentCol];

    // Move in the specified direction and append characters to the word
    while (currentRow >= 0 && currentRow < buffer->rows && currentCol >= 0 && currentCol < buffer->cols) {
        currentRow += rowStep;
        currentCol += colStep;

        if (currentRow >= 0 && currentRow < buffer->rows && currentCol >= 0 && currentCol < buffer->cols) {
            word[wordLen++] = buffer->puzzle[currentRow][currentCol];
        }
    }

    // Null-terminate the word
    word[wordLen] = '\0';

    // Print the word (or perform additional processing)
    printf("Found word: %s\n", word);
}