/**
 * @file Find.c
 * @brief Standalone executable backing the String sub-shell's "Find"
 * command: searches a file for a substring, printing "WIN" if found.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h> // Include errno for error handling

#define MAX_LINE_LENGTH 256

void searchWordInFile(const char* word, const char* fileName);

/**
 * @brief Searches argv[1] for argv[2] and prints "WIN" or "Try Again".
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1] is the file to search, argv[2] is the word to find.
 * @return 0 on success.
 */
int main(int argc, char* argv[]) {
    (void) argc; // argv[1]/argv[2] presence is guaranteed by the sub-shell's own arg-count check
    char* fileName = argv[1];
    char* word = argv[2];

    searchWordInFile(word, fileName);

    return 0;
}

/**
 * @brief Prints "WIN" if `word` occurs in `fileName`, otherwise "Try Again".
 * @details Reads the file in fixed 255-byte chunks and searches each chunk
 * independently; a match that straddles a chunk boundary will be missed.
 * Exits with status 1 (via perror()) on an open/read failure.
 * @param word The substring to search for.
 * @param fileName Path to the file to search.
 */
void searchWordInFile(const char* word, const char* fileName) {
    int fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    ssize_t bytes_read;
    int found = 0;

    while ((bytes_read = read(fd, line, MAX_LINE_LENGTH - 1)) > 0) {
        // Null-terminate the line
        line[bytes_read] = '\0';

        // Search for the word in the line
        if (strstr(line, word) != NULL) {
            printf("WIN\n");
            found = 1;
            break; // Exit the loop if the word is found
        }
    }

    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd); // Close file descriptor before returning
        exit(1);
    }

    close(fd); // Close file descriptor at the end of usage

    // If the word is not found, indicate it
    if (!found) {
        printf("Try Again\n");
    }
}
