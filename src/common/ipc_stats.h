#ifndef IPC_STATS_H
#define IPC_STATS_H

/**
 * @file ipc_stats.h
 * @brief Cross-process command counter shared by every shell (Standard,
 *        Math, Logic, String) via POSIX shared memory, guarded by a named
 *        POSIX semaphore so concurrent increments from sibling shell
 *        processes can't race each other.
 */

#include <semaphore.h>

/** @brief Layout of the shared memory segment - currently just a counter. */
typedef struct {
    unsigned long totalCommands;
} ShellStats;

/**
 * @brief Opens (creating if needed) the shared memory segment and semaphore
 * used to track the total number of commands executed across every shell
 * process in the current session.
 *
 * @param semOut Output parameter set to the opened semaphore (or SEM_FAILED
 *               on failure).
 * @return Pointer to the mapped shared stats struct, or NULL on failure.
 */
ShellStats* statsOpen(sem_t** semOut);

/**
 * @brief Resets the shared counter to zero for a new top-level session.
 * @details Only the top-level Standard_shell calls this, once, at startup -
 * sub-shells only increment the existing counter.
 */
void statsResetSession(ShellStats* stats, sem_t* sem);

/** @brief Atomically increments the shared command counter. */
void statsIncrement(ShellStats* stats, sem_t* sem);

/** @brief Reads the shared command counter under the semaphore lock. */
unsigned long statsRead(ShellStats* stats, sem_t* sem);

/**
 * @brief Tears down the shared memory segment and semaphore at session end.
 * @details Called once by the "exit" builtin so /dev/shm doesn't accumulate
 * stale segments across unrelated shell sessions.
 */
void statsCleanup(ShellStats* stats, sem_t* sem);

#endif
