#ifndef SUBSHELL_COMMON_H
#define SUBSHELL_COMMON_H

/**
 * @file subshell_common.h
 * @brief Shared table-driven REPL engine for the "fixed-command" sub-shells
 *        (Math, Logic, String). Each sub-shell only declares its own
 *        command table and a handful of labels/paths in a SubshellConfig -
 *        runSubshellRepl() owns the actual loop: prompt, read, parse,
 *        dispatch, fork/exec, history logging, and IPC command counting.
 *
 * This exists to eliminate the near-identical ~120-line main() that used to
 * be duplicated three times (once per sub-shell) with only the command
 * table and a few path strings differing between copies.
 */

#define SUBSHELL_INPUT_SIZE 256
#define SUBSHELL_MAX_ARGS 5

/** @brief One dispatchable builtin command in a sub-shell's command table. */
typedef struct {
    const char* name;       ///< Command name as typed by the user, e.g. "Sqrt"
    const char* execPath;   ///< Path to the standalone executable implementing it
    int requiredArgs;       ///< Required token count, including the command itself
    const char* usageError; ///< Printed when the token count doesn't match requiredArgs
} SubshellCommand;

/**
 * @brief Parses a line of input into a NULL-terminated argv-style array.
 * @param input Mutable input buffer; the parser is free to tokenize it in place.
 * @param arguments Output array of at least SUBSHELL_MAX_ARGS pointers into `input`.
 * @param argumentsIndex Output token count (excludes the NULL terminator slot).
 */
typedef void (*SubshellParser)(char* input, char* arguments[], int* argumentsIndex);

/** @brief Static configuration a sub-shell hands to the shared REPL loop. */
typedef struct {
    const char* promptLabel;       ///< e.g. "MathShell > "
    const char* shellNameForError; ///< e.g. "Math_shell" - used in fgets/fork error text
    const char* commandsDirPath;   ///< e.g. "./Commands/Math"
    const char* commandsDirLabel;  ///< e.g. "Math" - used in status messages
    const char* historyFilePath;   ///< e.g. "./Commands/Math/Math_Commands.txt"
    const char* historyCreatedMsg; ///< Printed the first time the history file is created
    const SubshellCommand* commands;
    int commandCount;
    SubshellParser parser;         ///< NULL selects the default whitespace tokenizer
} SubshellConfig;

/** @brief Default parser: tokenizes on spaces, capped at SUBSHELL_MAX_ARGS - 1 tokens. */
void subshellParseSimple(char* input, char* arguments[], int* argumentsIndex);

/**
 * @brief Creates `path` (mode 0700) if it doesn't already exist, printing a
 * status message either way. Exits the process on mkdir() failure.
 */
void subshellCreateDir(const char* path, const char* label);

/**
 * @brief Runs a sub-shell's full interactive loop until "Cls" is entered.
 * @details Every command entered is appended verbatim to the sub-shell's
 * history file (except "Cls" itself) and counted in the shared IPC stats
 * counter. Unrecognized commands print "Not Supported".
 */
void runSubshellRepl(const SubshellConfig* config);

#endif
