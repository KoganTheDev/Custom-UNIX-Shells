#ifndef JOBS_H
#define JOBS_H

/**
 * @file jobs.h
 * @brief Job-control subsystem: process-group/terminal-control setup,
 *        asynchronous SIGCHLD-driven reaping, and the "jobs"/"fg"/"bg"
 *        builtins. See docs on job control and process groups for the
 *        underlying mechanism (why Ctrl+C/Ctrl+Z target a job, not the shell).
 */

#include <sys/types.h>

#include "shell_config.h"

#define JOBS_MAX_JOBS 16                  ///< Max number of concurrently tracked jobs
#define JOBS_MAX_PROCS MAX_PIPE_STAGES    ///< Max processes in one job (one per pipeline stage)

typedef enum { JOB_RUNNING, JOB_STOPPED, JOB_DONE } JobState;

/** @brief One tracked pipeline: its process group and member pids. */
typedef struct {
    int jobId;                       ///< Visible job number, assigned lazily (see jobsMarkBackground)
    pid_t pgid;                      ///< Process group id shared by every stage in the pipeline
    pid_t pids[JOBS_MAX_PROCS];      ///< Individual pids, for matching in the SIGCHLD handler
    int numProcs;                    ///< Number of pipeline stages
    int alive;                       ///< Stages not yet reaped as exited/signaled
    JobState state;
    int reported;                    ///< Whether a Done/Stopped notification was already printed
    char cmdline[SIZE_OF_INPUT];     ///< Original command line, for "jobs"/notifications
} Job;

/** @brief This shell's own process group id, set once by jobsInit(). */
extern pid_t shellPgid;

/**
 * @brief Takes control of the terminal and installs signal handling for job
 * control: the shell ignores SIGINT/SIGTSTP/SIGTTOU/SIGTTIN (so it survives
 * Ctrl+C/Ctrl+Z and its own tcsetpgrp() calls) and reaps children
 * asynchronously via a SIGCHLD handler. Call once at shell startup.
 */
void jobsInit(void);

/**
 * @brief Registers a freshly-spawned pipeline as a new job.
 * @param pgid Process group id shared by every stage.
 * @param pids Array of the pipeline's stage pids.
 * @param numProcs Number of stages (elements in `pids`).
 * @param cmdline Original command line, copied into the job for display.
 * @return The new job, or NULL if the job table is full.
 */
Job* jobsCreate(pid_t pgid, const pid_t* pids, int numProcs, const char* cmdline);

/**
 * @brief Marks a job as backgrounded: assigns it a visible job number and
 * prints "[id] pgid", matching `cmd &` in a real shell.
 */
void jobsMarkBackground(Job* job);

/**
 * @brief Hands the terminal to `job`'s process group and blocks until it
 * either finishes or stops (Ctrl+Z), then reclaims the terminal. A job that
 * finishes normally while being watched here is silently dropped from the
 * table - the caller already saw it run, so there's nothing left to announce.
 */
void jobsWaitForeground(Job* job);

/**
 * @brief Prints "[id]+ Done/Stopped <cmdline>" for any background job whose
 * state changed since the last prompt, then compacts finished jobs out of
 * the table. Call once per prompt, before printing it.
 */
void jobsReportFinished(void);

/** @brief The "jobs" builtin: lists every job not yet fully done. */
void jobsBuiltinJobs(void);

/**
 * @brief The "fg [job_id]" builtin: resumes a stopped job (if needed) and
 * waits for it in the foreground. Defaults to the most recently created job.
 */
void jobsBuiltinFg(char* jobArg);

/**
 * @brief The "bg [job_id]" builtin: resumes a stopped job in the
 * background. Defaults to the most recently created job.
 */
void jobsBuiltinBg(char* jobArg);

#endif
