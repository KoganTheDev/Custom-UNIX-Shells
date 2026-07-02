/**
 * @file String.c
 * @brief Entry point for the String sub-shell. Declares its command table
 *        and a quote-aware parser (needed for "Replace" to accept a
 *        space-containing sentence as one argument), then hands off to the
 *        shared REPL engine (subshell_common) for the read/dispatch/log loop.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../common/subshell_common.h"

static const SubshellCommand kStringCommands[] = {
    {"Replace", "./functions/String_Shell_functions/Replace", 4, "The Replace command requires exactly 3 arguments."},
    {"Find", "./functions/String_Shell_functions/Find", 3, "The Find command requires exactly 2 arguments."},
    {"PrintFile", "./functions/String_Shell_functions/PrintFile", 2, "The PrintFile command requires exactly 1 argument."},
    {"History", "./functions/String_Shell_functions/History", 1, "The History command requires no arguments."},
};

/**
 * @brief Tokenizes on spaces like subshellParseSimple(), but treats a
 * "double-quoted phrase" as a single argument, so `Replace "hello world" hi 1`
 * passes "hello world" to Replace as one token instead of two.
 */
static void stringParseWithQuotes(char* input, char* arguments[], int* argumentsIndex) {
    *argumentsIndex = 0;

    char* token = strtok(input, " ");
    if (token == NULL) return;

    arguments[(*argumentsIndex)++] = token;

    while ((token = strtok(NULL, " ")) != NULL) {
        if (token[0] == '"') {
            char* quoteStart = token + 1;
            char* quoteEnd = strchr(quoteStart, '"');

            if (quoteEnd != NULL) {
                // Closing quote found in the same token - a single-word phrase.
                *quoteEnd = '\0';
                arguments[(*argumentsIndex)++] = quoteStart;
            } else {
                // Phrase spans multiple space-separated tokens - accumulate
                // them into a fresh buffer until the closing quote appears.
                char* phrase = (char*) malloc(SUBSHELL_INPUT_SIZE);
                strcpy(phrase, quoteStart);

                while ((token = strtok(NULL, " ")) != NULL) {
                    quoteEnd = strchr(token, '"');
                    if (quoteEnd != NULL) {
                        strncat(phrase, " ", SUBSHELL_INPUT_SIZE - strlen(phrase) - 1);
                        strncat(phrase, token, quoteEnd - token);
                        break;
                    }
                    strncat(phrase, " ", SUBSHELL_INPUT_SIZE - strlen(phrase) - 1);
                    strncat(phrase, token, SUBSHELL_INPUT_SIZE - strlen(phrase) - 1);
                }
                arguments[(*argumentsIndex)++] = phrase;
            }
        } else {
            arguments[(*argumentsIndex)++] = token;
        }

        if (*argumentsIndex >= SUBSHELL_MAX_ARGS - 1) break;
    }

    arguments[*argumentsIndex] = NULL;
}

/**
 * @brief Main function for the String Shell program.
 * @param argc Number of command-line arguments. Must be 1 (no extra args).
 * @param argv Array of command-line argument strings (unused).
 * @return Exit status of the program.
 */
int main(int argc, char* argv[]) {
    (void) argv; // no positional args are supported, only the argc==1 check below
    if (argc != 1) {
        printf("To run the \"String_shell\" you need to give one argument which is \"String\".\n");
        return 1;
    }

    SubshellConfig config = {
        .promptLabel = "StringShell > ",
        .shellNameForError = "String_shell",
        .commandsDirPath = "./Commands/String",
        .commandsDirLabel = "String",
        .historyFilePath = "./Commands/String/String_Commands.txt",
        .historyCreatedMsg = "String_Commands.txt created in Commands/String.",
        .commands = kStringCommands,
        .commandCount = sizeof(kStringCommands) / sizeof(kStringCommands[0]),
        .parser = stringParseWithQuotes,
    };

    runSubshellRepl(&config);
    return 0;
}
