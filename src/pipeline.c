/**
 * @file pipeline.c
 * @brief Implementation of pipeline parsing and execution. See pipeline.h
 *        for the public API.
 */

#include "pipeline.h"

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "jobs.h"
#include "shell_config.h"

/**
 * @brief Parses one pipeline stage ("cmd arg1 arg2 < in > out") into an argv
 * array plus any redirection targets found on that stage.
 * @return 1 on success, 0 on a malformed redirection (missing filename) or
 * an empty stage.
 */
static int splitStage(char* stage, char* arguments[], char** inFile, char** outFile, int* appendMode) {
    int argIndex = 0;
    *inFile = NULL;
    *outFile = NULL;
    *appendMode = 0;

    char* token = strtok(stage, " ");
    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) return 0;
            *inFile = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) return 0;
            *outFile = token;
            *appendMode = 0;
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) return 0;
            *outFile = token;
            *appendMode = 1;
        } else if (argIndex < MAX_ARGS - 1) {
            arguments[argIndex++] = token;
        }
        token = strtok(NULL, " ");
    }
    arguments[argIndex] = NULL;
    return argIndex > 0;
}

/**
 * @brief Strips a trailing "&" (and any whitespace before it) from `line`
 * in place.
 * @return 1 if a trailing "&" was found and stripped, 0 otherwise.
 */
static int stripBackgroundMarker(char* line) {
    int len = strlen(line);
    while (len > 0 && line[len - 1] == ' ') len--;
    if (len > 0 && line[len - 1] == '&') {
        line[len - 1] = '\0';
        return 1;
    }
    return 0;
}

/**
 * @brief Runs one pipeline stage in a freshly-forked child: joins the
 * pipeline's process group, restores default signal handling, wires up its
 * stdin/stdout to the adjacent pipes (and to a redirected file at the
 * pipeline's ends, if requested), then exec()s the command.
 * @details Never returns - always exits via _exit() or successful exec().
 */
static void execPipelineStage(int stageIndex, int numStages, char* arguments[],
                               char* inFile, char* outFile, int appendMode,
                               int pipes[][2], pid_t pgid) {
    setpgid(0, pgid == 0 ? getpid() : pgid);
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);

    if (stageIndex > 0) dup2(pipes[stageIndex - 1][0], STDIN_FILENO);
    if (stageIndex < numStages - 1) dup2(pipes[stageIndex][1], STDOUT_FILENO);

    if (stageIndex == 0 && inFile != NULL) {
        int fd = open(inFile, O_RDONLY);
        if (fd == -1) { perror("open"); _exit(1); }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (stageIndex == numStages - 1 && outFile != NULL) {
        int flags = O_WRONLY | O_CREAT | (appendMode ? O_APPEND : O_TRUNC);
        int fd = open(outFile, flags, 0644);
        if (fd == -1) { perror("open"); _exit(1); }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    for (int p = 0; p < numStages - 1; p++) {
        close(pipes[p][0]);
        close(pipes[p][1]);
    }

    execvp(arguments[0], arguments);
    printf("Not Supported\n"); // execvp() only returns on failure.
    _exit(1);
}

void runPipeline(char* userInput) {
    char lineCopy[SIZE_OF_INPUT];
    strcpy(lineCopy, userInput);
    int background = stripBackgroundMarker(lineCopy);

    char* stages[MAX_PIPE_STAGES];
    int numStages = 0;
    char* stageTok = strtok(lineCopy, "|");
    while (stageTok != NULL && numStages < MAX_PIPE_STAGES) {
        stages[numStages++] = stageTok;
        stageTok = strtok(NULL, "|");
    }
    if (numStages == 0) return;

    int pipes[MAX_PIPE_STAGES - 1][2];
    for (int i = 0; i < numStages - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            printf("Creating a pipe has failed.\n");
            return;
        }
    }

    // Every stage joins the same process group (the first stage's pid
    // becomes the pgid), so job control signals and tcsetpgrp() affect the
    // whole pipeline as one unit, matching real shell behavior.
    pid_t pgid = 0;
    pid_t childPids[MAX_PIPE_STAGES];
    int spawned = 0;

    for (int i = 0; i < numStages; i++) {
        char* arguments[MAX_ARGS];
        char* inFile;
        char* outFile;
        int appendMode;

        if (!splitStage(stages[i], arguments, &inFile, &outFile, &appendMode)) {
            printf("Invalid redirection in command.\n");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            printf("Forking has failed for the Standard_Shell.\n");
            continue;
        }

        if (pid == 0) {
            execPipelineStage(i, numStages, arguments, inFile, outFile, appendMode, pipes, pgid);
        }

        setpgid(pid, pgid == 0 ? pid : pgid);
        if (pgid == 0) pgid = pid;
        childPids[spawned++] = pid;
    }

    for (int i = 0; i < numStages - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    if (spawned == 0) return;

    Job* job = jobsCreate(pgid, childPids, spawned, userInput);
    if (job == NULL) {
        // Job table is full: still let the pipeline run, just untracked.
        // It will be reaped by jobs.c's SIGCHLD handler, but won't show up
        // in "jobs" and can't be fg/bg'd.
        int status;
        waitpid(-pgid, &status, 0);
        return;
    }

    if (background) {
        jobsMarkBackground(job);
        return;
    }

    jobsWaitForeground(job);
}
