/**
 * @file DectoHex.c
 * @brief Standalone executable backing the Logic sub-shell's "DectoHex"
 * command: converts a decimal argument to its hexadecimal string representation.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Prints the hexadecimal representation of argv[1].
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1] is the decimal number to convert.
 * @return 0 on success.
 */
int main(int argc, char* argv[]){
    (void) argc; // argv[1] presence is guaranteed by the sub-shell's own arg-count check
    int number = atoi(argv[1]);

    printf("%X\n", (unsigned int) number); // %X expects unsigned int.

    return 0; // When the function succeeds.
}