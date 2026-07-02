/**
 * @file Highbit.c
 * @brief Standalone executable backing the Logic sub-shell's "Highbit"
 * command: counts the number of set bits (population count) in a decimal argument.
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Prints the population count (number of set bits) of argv[1].
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1] is the decimal number whose bits are counted.
 * @return The bit count itself (not the usual 0-for-success convention) -
 * harmless here since the caller never inspects the exit status, but worth
 * knowing before reusing this exit code for anything.
 */
int main(int argc, char* argv[]){
    (void) argc; // argv[1] presence is guaranteed by the sub-shell's own arg-count check
    int number = atoi(argv[1]);
    int count = 0;

    // Iterate through each bit of the number
    while (number) {
        count += number & 1; // Check if the current lsb is 1 or 0. 
        number >>= 1; // Consume the lsb.
    }
    printf("%d\n", count);

    return count;
}