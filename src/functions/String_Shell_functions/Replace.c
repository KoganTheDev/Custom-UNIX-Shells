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
 * @brief Inserts argv[2] into argv[1] at the 1-based position argv[3] and
 * prints the result.
 * @param argc Number of command-line arguments; must be 4.
 * @param argv argv[1] is the sentence, argv[2] the word to insert, argv[3]
 * the 1-based insertion position.
 * @return 0 on success; 1 on a usage or allocation error.
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
 * @brief Overwrites `sentence` in place, inserting `word` starting at the
 * 1-based `location`. Caller must ensure `sentence`'s buffer is large enough
 * (see main()) since this can extend past the original string's length.
 * @param sentence Buffer to modify in place.
 * @param word The word to insert.
 * @param location 1-based index into `sentence` where `word` starts.
 * Exits with status 1 if out of range ([1, strlen(sentence)]).
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
