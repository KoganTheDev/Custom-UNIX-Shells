/**
 * @file Replace.c
 * @brief Standalone executable backing the String sub-shell's "Replace"
 * command: inserts a word into a sentence at a given 1-based position.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void replaceWordInSentence(char* sentence, char* word, int location);

/**
 * Summary:
 * Main function entry point.
 *
 * Details:
 * Validates command-line arguments, calls replaceWordInSentence() to modify the sentence,
 * and prints the modified sentence to standard output.
 *
 * Arguments:
 * @param argc - (int): Number of command-line arguments.
 * @param argv - (char*[]): Array of command-line argument strings.
 *
 * Returns:
 * @return - (int): Exit status of the program. Returns 0 for successful execution.
 */
int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <sentence> <word> <location>\n", argv[0]);
        return 1;
    }

    char* word = argv[2];
    int location = atoi(argv[3]);

    // Allocate a buffer large enough to hold the sentence even if the
    // replacement word extends past the original sentence's end, since
    // modifying argv[1] in-place would overflow its buffer.
    int sentenceLength = strlen(argv[1]);
    int wordLength = strlen(word);
    int neededSize = sentenceLength;
    if (location - 1 + wordLength > neededSize) {
        neededSize = location - 1 + wordLength;
    }

    char* sentence = (char*) malloc(neededSize + 1);
    if (sentence == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }
    strcpy(sentence, argv[1]);

    replaceWordInSentence(sentence, word, location);

    printf("%s\n", sentence); // Print the modified sentence
    free(sentence);
    return 0;
}

/**
 * Summary:
 * Replaces a word in a sentence at a specified location.
 *
 * Details:
 * Modifies the input sentence by replacing a substring starting at the specified location with the given word.
 * Ensures the location is within the bounds of the sentence length and performs the replacement in-place.
 * If the location is invalid, prints an error message and exits.
 *
 * Arguments:
 * @param sentence - (char*): The sentence in which the word will be replaced.
 * @param word - (char*): The word to replace in the sentence.
 * @param location - (int): The 1-based index indicating the position in the sentence to start replacing.
 *
 * Returns:
 * None.
 */
void replaceWordInSentence(char* sentence, char* word, int location) {
    int sentenceLength = strlen(sentence);
    int wordLength = strlen(word);

    // Ensure the location is valid
    if (location < 1 || location > sentenceLength) {
        printf("Invalid location.\n");
        exit(1);
    }

    // Calculate the starting index of the word to replace
    char* startReplace = sentence + location - 1; // Adjust for 1-based index

    // Calculate the ending index of the word to replace
    char* endReplace = startReplace + wordLength;

    // Calculate the remaining part of the sentence after the replaced word.
    // If the replacement word extends past the original sentence's end,
    // there is no remaining text to preserve.
    char* sentenceEnd = sentence + sentenceLength;
    char* remaining = (endReplace < sentenceEnd) ? endReplace : sentenceEnd;

    // Perform the replacement in-place
    strncpy(startReplace, word, wordLength);
    strcpy(startReplace + wordLength, remaining);
}
