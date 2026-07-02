/**
 * @file Math.c
 * @brief Entry point for the Math sub-shell. Declares its command table and
 *        hands off to the shared REPL engine (subshell_common) for the
 *        actual read/parse/dispatch/log loop.
 */

#include <stdio.h>

#include "../../common/subshell_common.h"

static const SubshellCommand kMathCommands[] = {
    {"Solve", "./functions/Math_Shell_functions/Solve", 4, "The Solve command requires exactly 3 arguments."},
    {"Power", "./functions/Math_Shell_functions/Power", 3, "The Power command requires exactly 2 arguments."},
    {"Sqrt", "./functions/Math_Shell_functions/Sqrt", 2, "The Sqrt command requires exactly 1 argument."},
    {"History", "./functions/Math_Shell_functions/History", 1, "The History command requires no arguments."},
};

/**
 * @brief Main function for the Math Shell program.
 * @param argc Number of command-line arguments. Must be 1 (no extra args).
 * @param argv Array of command-line argument strings (unused).
 * @return Exit status of the program.
 */
int main(int argc, char* argv[]) {
    (void) argv; // no positional args are supported, only the argc==1 check below
    if (argc != 1) {
        printf("To run the \"Math_shell\" you need to give one argument which is \"Math\".\n");
        return 1;
    }

    SubshellConfig config = {
        .promptLabel = "MathShell > ",
        .shellNameForError = "Math_shell",
        .commandsDirPath = "./Commands/Math",
        .commandsDirLabel = "Math",
        .historyFilePath = "./Commands/Math/Math_Commands.txt",
        .historyCreatedMsg = "Math_Commands.txt created in Commands/Math.",
        .commands = kMathCommands,
        .commandCount = sizeof(kMathCommands) / sizeof(kMathCommands[0]),
        .parser = NULL, // default whitespace tokenizer
    };

    runSubshellRepl(&config);
    return 0;
}
