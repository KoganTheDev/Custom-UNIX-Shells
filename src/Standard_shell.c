/**
 * @file Standard_shell.c
 * @brief Entry point and top-level command dispatch for the Standard Shell.
 *        Owns the read-eval-print loop and the small set of builtins
 *        (exit/Math/Logic/String/jobs/fg/bg/Stats); everything else is
 *        delegated to jobs.c (job control) and pipeline.c (pipes/redirection).
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common/ipc_stats.h"
#include "jobs.h"
#include "pipeline.h"
#include "shell_config.h"

static void createNewDir(void);
static void parseUserInput(char* userInput, char* arguments[], int* argumentsIndex);

static ShellStats* stats = NULL;
static sem_t* statsSem = NULL;

/**
 * @brief Forks and execs one of the fixed sub-shell/exit helper binaries,
 * waiting for it to finish before returning to the prompt.
 * @param path Path to the executable.
 * @param name Program name to pass as argv[0] (matches the binary's own name).
 */
static void execBuiltinHelper(const char* path, const char* name) {
    int pid = fork();
    if (pid < 0) {
        printf("Forking has failed for the Standard_Shell.\n");
        exit(1);
    }
    if (pid == 0) {
        execlp(path, name, NULL);
        printf("Not Supported\n"); // execlp() only returns on failure.
        _exit(1);
    }
    waitpid(pid, NULL, 0);
}

/**
 * @brief Main function for the Standard Shell program.
 *
 * @details
 * - Ensures correct usage with one argument.
 * - Creates the necessary directory if not already present.
 * - Installs signal handlers and takes control of the terminal for job control.
 * - Processes user commands, forks child processes to execute specific commands, and handles errors.
 * - Supports built-in commands like exit, Math, Logic, String, jobs, fg, bg, Stats.
 * - Executes pipelines of regular shell commands with pipe/redirection/background support.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit status of the program.
 */
int main(int argc, char* argv[]) {
    (void) argv; // no positional args are supported, only the argc==1 check below
    if (argc != 1) {
        printf("To run the \"Math_shell\" you need to give one argument which is \"Math\".\n");
        exit(1);
    }

    char userInput[SIZE_OF_INPUT];
    char* arguments[MAX_ARGS];
    int argumentsIndex;

    createNewDir();
    jobsInit();

    stats = statsOpen(&statsSem);
    statsResetSession(stats, statsSem); // fresh counter for this top-level session

    while (1) {
        jobsReportFinished();
        printf("StandardShell > ");
        fflush(stdout); // the prompt has no trailing '\n', so line-buffering
                        // alone won't flush it - force it out before fgets()
                        // blocks, or the prompt may sit invisible until later.

        if (fgets(userInput, SIZE_OF_INPUT, stdin) == NULL) {
            printf("fgets in \"Standard_shell\" has failed to grab input.\n");
            exit(1);
        }
        userInput[strcspn(userInput, "\n")] = 0;

        // Keep an unmodified copy for the builtin dispatch below, since
        // parseUserInput/strtok destructively tokenizes the buffer.
        char inputCopy[SIZE_OF_INPUT];
        strcpy(inputCopy, userInput);
        parseUserInput(inputCopy, arguments, &argumentsIndex);

        if (argumentsIndex == 0) {
            printf("No input received.\n");
            continue;
        }

        if (strcmp(arguments[0], "exit") == 0) {
            execBuiltinHelper("./functions/Standard_Shell_functions/exit", "exit");
            statsCleanup(stats, statsSem);
            break;
        } else if (strcmp(arguments[0], "Math") == 0) {
            statsIncrement(stats, statsSem);
            execBuiltinHelper("./functions/Standard_Shell_functions/Math", "Math");
        } else if (strcmp(arguments[0], "Logic") == 0) {
            statsIncrement(stats, statsSem);
            execBuiltinHelper("./functions/Standard_Shell_functions/Logic", "Logic");
        } else if (strcmp(arguments[0], "String") == 0) {
            statsIncrement(stats, statsSem);
            execBuiltinHelper("./functions/Standard_Shell_functions/String", "String");
        } else if (strcmp(arguments[0], "jobs") == 0) {
            jobsBuiltinJobs();
        } else if (strcmp(arguments[0], "fg") == 0) {
            jobsBuiltinFg(arguments[1]);
        } else if (strcmp(arguments[0], "bg") == 0) {
            jobsBuiltinBg(arguments[1]);
        } else if (strcmp(arguments[0], "Stats") == 0) {
            printf("Total commands executed across all shells this session: %lu\n", statsRead(stats, statsSem));
        } else {
            // Try executing a pipeline of regular shell command(s), with
            // optional redirection and background execution.
            statsIncrement(stats, statsSem);
            runPipeline(userInput);
        }
    }
    return 0;
}

/**
 * @brief Creates ./Commands (mode 0700) if it doesn't already exist,
 * printing a status message either way. Exits the process on failure.
 */
static void createNewDir(void) {
    DIR* dir = opendir("./Commands");

    if (dir) {
        closedir(dir);
        printf("The directory ./Commands already exists.\n");
    } else if (errno == ENOENT) {
        if (mkdir("./Commands", 0700) == -1) {
            printf("Making the new directory has failed.\n");
            exit(1);
        } else {
            printf("Commands dir created.\n");
        }
    }
}

/**
 * @brief Tokenizes user input into an argv-style array, splitting on spaces
 * and capping at MAX_ARGS - 1 tokens (leaving room for the NULL terminator).
 */
static void parseUserInput(char* userInput, char* arguments[], int* argumentsIndex) {
    *argumentsIndex = 0;

    char* token = strtok(userInput, " ");
    while (token != NULL && *argumentsIndex < MAX_ARGS - 1) {
        arguments[*argumentsIndex] = token;
        (*argumentsIndex)++;
        token = strtok(NULL, " ");
    }

    arguments[*argumentsIndex] = NULL;
}
