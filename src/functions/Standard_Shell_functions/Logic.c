/**
 * @file Logic.c
 * @brief Entry point for the Logic sub-shell. Declares its command table and
 *        hands off to the shared REPL engine (subshell_common) for the
 *        actual read/parse/dispatch/log loop.
 */

#include <stdio.h>

#include "../../common/subshell_common.h"

static const SubshellCommand kLogicCommands[] = {
    {"DectoBin", "./functions/Logic_Shell_functions/DectoBin", 2, "The DectoBin command requires exactly 1 arguments."},
    {"Highbit", "./functions/Logic_Shell_functions/Highbit", 2, "The Highbit command requires exactly 1 arguments."},
    {"DectoHex", "./functions/Logic_Shell_functions/DectoHex", 2, "The DectoHex command requires exactly 1 argument."},
    {"History", "./functions/Logic_Shell_functions/History", 1, "The History command requires no arguments."},
};

/**
 * @brief Main function for the Logic Shell program.
 * @param argc Number of command-line arguments. Must be 1 (no extra args).
 * @param argv Array of command-line argument strings (unused).
 * @return Exit status of the program.
 */
int main(int argc, char* argv[]) {
    (void) argv; // no positional args are supported, only the argc==1 check below
    if (argc != 1) {
        printf("To run the \"Logic_shell\" you need to give one argument which is \"Logic\".\n");
        return 1;
    }

    SubshellConfig config = {
        .promptLabel = "LogicShell > ",
        .shellNameForError = "Logic_shell",
        .commandsDirPath = "./Commands/Logic",
        .commandsDirLabel = "Logic",
        .historyFilePath = "./Commands/Logic/Logic_Commands.txt",
        .historyCreatedMsg = "Logic_Commands.txt created in Commands/Logic.",
        .commands = kLogicCommands,
        .commandCount = sizeof(kLogicCommands) / sizeof(kLogicCommands[0]),
        .parser = NULL, // default whitespace tokenizer
    };

    runSubshellRepl(&config);
    return 0;
}
