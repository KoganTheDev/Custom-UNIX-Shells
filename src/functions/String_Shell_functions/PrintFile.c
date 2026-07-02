/**
 * @file PrintFile.c
 * @brief Standalone executable backing the String sub-shell's "PrintFile"
 * command: prints a file's contents to standard output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void printFileContent(const char* file_name);

/**
 * @brief Prints the contents of argv[1] to standard output.
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1] is the file to print.
 * @return 0 on success.
 */
int main(int argc, char* argv[]) {
    (void) argc; // argv[1] presence is guaranteed by the sub-shell's own arg-count check
    char* file_name = argv[1];

    // Print the content of the specified file
    printFileContent(file_name);
    printf("\n");

    return 0; // For a successful run.
}

/**
 * @brief Streams `file_name`'s contents to standard output in 1023-byte chunks.
 * @details Exits with status 1 (via perror()) on an open/read/close failure.
 * @param file_name Path to the file to print.
 */
void printFileContent(const char* file_name) {
    char buffer[BUFFER_SIZE];

    // Open the file for reading
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    ssize_t bytes_read;

    // Read and print the file content in chunks
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer after reading
        printf("%s", buffer);
    }

    // Check for read error
    if (bytes_read == -1) {
        perror("Error reading file");
        exit(1);
    }

    // Close the file descriptor
    if (close(fd) == -1) {
        perror("Error closing file");
        exit(1);
    }
}