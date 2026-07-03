/**
 * @file subshell_common.c
 * @brief Implementation of the shared table-driven sub-shell REPL engine.
 *        See subshell_common.h for the design rationale.
 */

#include "subshell_common.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ipc_stats.h"

void subshellParseSimple(char* input, char* arguments[], int* argumentsIndex) {
    *argumentsIndex = 0;

    char* token = strtok(input, " ");
    while (token != NULL && *argumentsIndex < SUBSHELL_MAX_ARGS - 1) {
        arguments[*argumentsIndex] = token;
        (*argumentsIndex)++;
        token = strtok(NULL, " ");
    }

    arguments[*argumentsIndex] = NULL;
}

void subshellCreateDir(const char* path, const char* label) {
    DIR* dir = opendir(path);

    if (dir) {
        closedir(dir);
        printf("The directory %s already exists.\n", path);
    } else if (errno == ENOENT) {
        if (mkdir(path, 0700) == -1) {
            printf("Making the new directory has failed.\n");
            exit(1);
        } else {
            printf("%s dir created in Commands dir.\n", label);
        }
    }
}

/**
 * @brief Finds the command whose name matches arguments[0], if any.
 */
static const SubshellCommand* findCommand(const SubshellConfig* config, char* arguments[]) {
    for (int i = 0; i < config->commandCount; i++) {
        if (strcmp(arguments[0], config->commands[i].name) == 0) {
            return &config->commands[i];
        }
    }
    return NULL;
}

/**
 * @brief Runs the matched command in the child, or reports a usage error
 * from the parent if the argument count doesn't match.
 */
static void dispatchCommand(const SubshellCommand* cmd, int pid, char* arguments[], int argumentsIndex) {
    if (argumentsIndex != cmd->requiredArgs) {
        if (pid != 0) printf("%s\n", cmd->usageError);
        return;
    }

    if (pid != 0) return; // Parent has nothing further to do for a matched command.

    char* execArgv[SUBSHELL_MAX_ARGS];
    execArgv[0] = (char*) cmd->name;
    for (int i = 1; i < argumentsIndex; i++) execArgv[i] = arguments[i];
    execArgv[argumentsIndex] = NULL;

    execv(cmd->execPath, execArgv);
    printf("Not Supported\n"); // execv() only returns on failure.
}

void runSubshellRepl(const SubshellConfig* config) {
    char userInput[SUBSHELL_INPUT_SIZE];
    char* arguments[SUBSHELL_MAX_ARGS];
    int argumentsIndex;
    SubshellParser parse = config->parser != NULL ? config->parser : subshellParseSimple;

    DIR* dirBeforeCreate = opendir(config->commandsDirPath);
    subshellCreateDir(config->commandsDirPath, config->commandsDirLabel);

    int historyFd = open(config->historyFilePath, O_RDWR | O_CREAT, 0644);
    if (historyFd < 0) {
        printf("Opening the file for the commands of \"%s\" has failed.\n", config->shellNameForError);
        exit(1);
    }
    lseek(historyFd, 0, SEEK_END); // Append, never overwrite prior history.

    if (!dirBeforeCreate) {
        printf("%s\n", config->historyCreatedMsg);
    } else {
        closedir(dirBeforeCreate);
    }

    sem_t* statsSem;
    ShellStats* stats = statsOpen(&statsSem);

    while (1) {
        printf("%s", config->promptLabel);
        fflush(stdout); // the prompt has no trailing '\n', so line-buffering
                        // alone won't flush it - force it out before fgets()
                        // blocks, or the prompt may sit invisible until later.

        if (fgets(userInput, SUBSHELL_INPUT_SIZE, stdin) == NULL) {
            printf("fgets in \"%s\" has failed to grab input.\n", config->shellNameForError);
            exit(1);
        }

        char* userInputForWriting = (char*) malloc(strlen(userInput) + 1);
        if (userInputForWriting == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        strcpy(userInputForWriting, userInput);

        userInput[strcspn(userInput, "\n")] = 0;
        parse(userInput, arguments, &argumentsIndex);

        if (argumentsIndex == 0) {
            printf("No input received.\n");
            free(userInputForWriting);
            continue;
        }

        statsIncrement(stats, statsSem);

        int isCls = strcmp(arguments[0], "Cls") == 0;
        const SubshellCommand* cmd = isCls ? NULL : findCommand(config, arguments);

        int pid = fork();
        if (pid < 0) {
            printf("Forking has failed for the %s.\n", config->shellNameForError);
            free(userInputForWriting);
            exit(1);
        }

        if (isCls) {
            if (pid != 0) {
                if (write(historyFd, userInputForWriting, strlen(userInputForWriting)) == -1) {
                    printf("Writing information to the history file has failed.\n");
                    free(userInputForWriting);
                    exit(1);
                }
                free(userInputForWriting);
                break;
            }
        } else {
            if (cmd != NULL) {
                dispatchCommand(cmd, pid, arguments, argumentsIndex);
            } else if (pid == 0) {
                printf("Not Supported\n");
            }

            if (pid != 0) {
                if (write(historyFd, userInputForWriting, strlen(userInputForWriting)) == -1) {
                    printf("Writing information to the history file has failed.\n");
                    free(userInputForWriting);
                    exit(1);
                }
            }
            free(userInputForWriting);
        }

        // _exit() (not exit()) skips flushing stdio buffers - this child
        // inherited a copy-on-write snapshot of the parent's still-unflushed
        // stdout buffer at fork() time, and a normal exit() would flush that
        // inherited copy too, duplicating every prompt/message printed so far.
        if (pid == 0) _exit(0);

        wait(NULL);
    }

    close(historyFd);
}
