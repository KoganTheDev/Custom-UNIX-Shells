/**
 * @file Sqrt.c
 * @brief Standalone executable backing the Math sub-shell's "Sqrt"
 * command: computes the square root of a decimal argument.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/**
 * @brief Prints the square root of argv[1].
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1] is the number to take the square root of.
 * @return 0 on success; exits with status 1 (printing "Math Error!") for a
 * negative input.
 */
int main(int argc, char* argv[]) {
    (void) argc; // argv[1] presence is guaranteed by the sub-shell's own arg-count check
    double number = atof(argv[1]); // The number for which square root is to be calculated

    if (number < 0) {
        printf("Math Error!\n");
        exit(1);
    }

    printf("%.0lf\n", sqrt(number));
    return 0; // Represents a successful run
}
