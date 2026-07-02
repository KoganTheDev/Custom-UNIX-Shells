/**
 * @file DectoBin.c
 * @brief Standalone executable backing the Logic sub-shell's "DectoBin"
 * command: converts a decimal argument to its binary string representation.
 */

#include <stdio.h>
#include <stdlib.h>

#define DIGITS_IN_INT 32

/**
 * @brief Prints the binary representation of argv[1], with no leading zeros.
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1] is the decimal number to convert.
 * @return 0 on success.
 */
int main(int argc, char* argv[]) {
    (void) argc; // argv[1] presence is guaranteed by the sub-shell's own arg-count check
    int number = atoi(argv[1]);
    unsigned int numberCopy = (unsigned int)number;
    int dectoBinString[DIGITS_IN_INT] = {0};
    int noMoreLeadingZeros = 0;
    int i;

    // Convert to an integer that represents number in its binary format
    for (i = DIGITS_IN_INT - 1; i >= 0; i--) {
        dectoBinString[i] = numberCopy & 1;
        numberCopy >>= 1;
    }

    // Print the binary representation, removing leading zeros
    for (i = 0; i < DIGITS_IN_INT; i++) {
        if (number == 0) {
            printf("0");
            break;
        } else if (noMoreLeadingZeros) {
            printf("%d", dectoBinString[i]);
        } else if (dectoBinString[i] == 1) {
            printf("%d", dectoBinString[i]);
            noMoreLeadingZeros = 1; 
        }
    }
    printf("\n");

    return 0;
}
