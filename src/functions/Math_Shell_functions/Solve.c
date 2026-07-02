/**
 * @file Solve.c
 * @brief Standalone executable backing the Math sub-shell's "Solve"
 * command: solves the quadratic equation ax^2 + bx + c = 0 for three
 * decimal coefficient arguments.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

/**
 * @brief Solves ax^2 + bx + c = 0 for argv[1..3] and prints the real root(s).
 * @details Prints "No Sol!" for a negative discriminant, the single root for
 * a zero discriminant, or forks a second process to print the two roots
 * concurrently when the discriminant is positive - their print order isn't
 * guaranteed, but both always appear.
 * @param argc Unused; argument count is validated by the calling sub-shell.
 * @param argv argv[1..3] are the coefficients a, b, c.
 * @return 0 on success.
 */
int main(int argc, char* argv[]) {
    (void) argc; // argv[1..3] presence is guaranteed by the sub-shell's own arg-count check
    int pid;
    double firstSolution, secondSolution;

    double a = atof(argv[1]);    // Coefficient of x^2
    double b = atof(argv[2]);    // Coefficient of x
    double c = atof(argv[3]);    // Constant term
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        printf("No Sol!\n");
        return 0;
    }
    else if (discriminant == 0) {
        firstSolution = (-b) / (2 * a);
        printf("%.0lf\n", firstSolution);
        return 0; // Represents a successful run
    }
    else {
        firstSolution = (-b + sqrt(discriminant)) / (2 * a);
        secondSolution = (-b - sqrt(discriminant)) / (2 * a);

        if ((pid = fork()) < 0) {
            printf("Creating a new process in \"Solve\" has failed.\n");
            exit(1);
        }

        if (pid != 0) {
            printf("%.0lf\n", firstSolution);
            return 0; // Represents a successful run
        }
        else {
            printf("%.0lf\n", secondSolution);
            return 0; // Represents a successful run
        }
    }
}
