/**
 * @file Power.c
 * @brief Standalone executable backing the Math sub-shell's "Power"
 * command: computes base^exponent for two decimal arguments.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/**
 * @brief Prints argv[1]^argv[2], rounded to zero decimal places.
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1] is the base, argv[2] is the exponent.
 * @return 0 on success.
 */
int main(int argc, char* argv[]) {
    (void) argc; // argv[1]/argv[2] presence is guaranteed by the sub-shell's own arg-count check
    double base = atof(argv[1]);    // Convert the first argument to double (base)
    double power = atof(argv[2]);   // Convert the second argument to double (power)
    double result = pow(base, power);   // Calculate base^power

    printf("%.0lf\n", result);  // Print the result rounded to 0 decimal places
    return 0; // Represents a successful run.
}
