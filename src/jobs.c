/**
 * @file jobs.c
 * @brief Implementation of the job-control subsystem. See jobs.h for the
 *        public API and design rationale.
 */

#include "jobs.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static Job jobsTable[JOBS_MAX_JOBS];
static int jobCount = 0;
static int nextJobId = 1;

pid_t shellPgid;

/**
 * @brief Lazily assigns a visible job number the first time a job actually
 * needs one (going to the background, or stopping) - purely synchronous
 * foreground commands never consume a number, matching bash's numbering.
 * Only touches integers, so it's safe to call from sigchldHandler() too.
 */
static void assignJobId(Job* job) {
    if (job->jobId <= 0) job->jobId = nextJobId++;
}

/** @brief Finds a job by its visible job id, or NULL if none matches. */
static Job* findJobById(int jobId) {
    for (int i = 0; i < jobCount; i++) {
        if (jobsTable[i].jobId == jobId) return &jobsTable[i];
    }
    return NULL;
}

/** @brief The job created most recently, or NULL if the table is empty. */
static Job* mostRecentJob(void) {
    return jobCount > 0 ? &jobsTable[jobCount - 1] : NULL;
}

/**
 * @brief Resolves an "[job_id]" builtin argument to a job: a specific id if
 * given, otherwise the most recently created job.
 */
static Job* resolveJobArg(char* jobArg) {
    return jobArg != NULL ? findJobById(atoi(jobArg)) : mostRecentJob();
}

/**
 * @brief Drops a job from the table immediately, with no notification.
 *
 * @details
 * Used when a job finishes while being waited on in the foreground (whether
 * it started there or was brought there via "fg") - the caller already
 * watched it run to completion, so there's nothing left to announce.
 * Contrast with a background job finishing on its own: that path leaves
 * the job DONE in the table for jobsReportFinished() to announce later.
 */
static void removeJob(Job* job) {
    int idx = (int) (job - jobsTable);
    for (int i = idx; i < jobCount - 1; i++) {
        jobsTable[i] = jobsTable[i + 1];
    }
    jobCount--;
}

/**
 * @brief Reaps any child (foreground or background) whose status changed.
 *
 * @details
 * Runs asynchronously whenever any child of this shell exits, stops, or
 * continues. Only touches the job table (no printf - not async-signal-safe,
 * see docs on async-signal-safety); jobsReportFinished() prints the
 * resulting notifications later, from the main loop.
 */
static void sigchldHandler(int sig) {
    (void) sig;
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        for (int i = 0; i < jobCount; i++) {
            Job* job = &jobsTable[i];
            if (job->state == JOB_DONE) continue;

            for (int p = 0; p < job->numProcs; p++) {
                if (job->pids[p] != pid) continue;

                if (WIFSTOPPED(status)) {
                    job->state = JOB_STOPPED;
                    job->reported = 0;
                    assignJobId(job);
                } else if (WIFCONTINUED(status)) {
                    job->state = JOB_RUNNING;
                } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    job->alive--;
                    if (job->alive <= 0) {
                        job->state = JOB_DONE;
                        job->reported = 0;
                    }
                }
            }
        }
    }
}

void jobsInit(void) {
    shellPgid = getpid();
    setpgid(shellPgid, shellPgid);
    tcsetpgrp(STDIN_FILENO, shellPgid);

    // Ignored so Ctrl+C/Ctrl+Z stop the foreground job (which keeps the
    // default disposition) instead of the shell itself. SIGTTOU/SIGTTIN are
    // ignored so tcsetpgrp() below doesn't stop the shell when it isn't the
    // foreground process group at the moment of the call.
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    struct sigaction sa;
    sa.sa_handler = sigchldHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);
}

Job* jobsCreate(pid_t pgid, const pid_t* pids, int numProcs, const char* cmdline) {
    if (jobCount >= JOBS_MAX_JOBS) return NULL;

    Job* job = &jobsTable[jobCount++];
    job->jobId = -1; // assigned lazily - see assignJobId()
    job->pgid = pgid;
    job->numProcs = numProcs;
    job->alive = numProcs;
    job->state = JOB_RUNNING;
    job->reported = 0;
    strncpy(job->cmdline, cmdline, SIZE_OF_INPUT - 1);
    job->cmdline[SIZE_OF_INPUT - 1] = '\0';
    for (int i = 0; i < numProcs; i++) job->pids[i] = pids[i];

    return job;
}

void jobsMarkBackground(Job* job) {
    assignJobId(job);
    printf("[%d] %d\n", job->jobId, job->pgid);
}

void jobsWaitForeground(Job* job) {
    tcsetpgrp(STDIN_FILENO, job->pgid);

    int status;
    while (job->state == JOB_RUNNING &&
           waitpid(-job->pgid, &status, WUNTRACED) > 0) {
        if (WIFSTOPPED(status)) {
            job->state = JOB_STOPPED;
            assignJobId(job);
        } else {
            job->alive--;
            if (job->alive <= 0) job->state = JOB_DONE;
        }
    }

    tcsetpgrp(STDIN_FILENO, shellPgid);

    if (job->state == JOB_DONE) removeJob(job);
}

void jobsReportFinished(void) {
    for (int i = 0; i < jobCount; i++) {
        Job* job = &jobsTable[i];
        if (job->reported) continue;

        if (job->state == JOB_DONE) {
            printf("[%d]+ Done\t%s\n", job->jobId, job->cmdline);
            job->reported = 1;
        } else if (job->state == JOB_STOPPED) {
            printf("[%d]+ Stopped\t%s\n", job->jobId, job->cmdline);
            job->reported = 1;
        }
    }

    // Compact away jobs that are done and have already been reported.
    int writeIndex = 0;
    for (int i = 0; i < jobCount; i++) {
        if (jobsTable[i].state == JOB_DONE && jobsTable[i].reported) continue;
        jobsTable[writeIndex++] = jobsTable[i];
    }
    jobCount = writeIndex;
}

void jobsBuiltinJobs(void) {
    for (int i = 0; i < jobCount; i++) {
        Job* job = &jobsTable[i];
        const char* label = job->state == JOB_STOPPED ? "Stopped" : "Running";
        printf("[%d]+ %s\t%s\n", job->jobId, label, job->cmdline);
    }
}

void jobsBuiltinFg(char* jobArg) {
    Job* job = resolveJobArg(jobArg);
    if (job == NULL) {
        printf("fg: no such job.\n");
        return;
    }

    printf("%s\n", job->cmdline);

    if (job->state == JOB_STOPPED) {
        killpg(job->pgid, SIGCONT);
        job->state = JOB_RUNNING;
    }

    jobsWaitForeground(job);
}

void jobsBuiltinBg(char* jobArg) {
    Job* job = resolveJobArg(jobArg);
    if (job == NULL) {
        printf("bg: no such job.\n");
        return;
    }

    if (job->state != JOB_STOPPED) {
        printf("bg: job %d is not stopped.\n", job->jobId);
        return;
    }

    killpg(job->pgid, SIGCONT);
    job->state = JOB_RUNNING;
    printf("[%d]+ %s &\n", job->jobId, job->cmdline);
}
