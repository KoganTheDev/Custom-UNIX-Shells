/**
 * @file ipc_stats.c
 * @brief Implementation of the cross-process shared command counter. See
 *        ipc_stats.h for the rationale and usage contract.
 */

#include "ipc_stats.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define STATS_SHM_NAME "/custom_unix_shells_stats"
#define STATS_SEM_NAME "/custom_unix_shells_stats_sem"

ShellStats* statsOpen(sem_t** semOut) {
    int fd = shm_open(STATS_SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("shm_open");
        *semOut = SEM_FAILED;
        return NULL;
    }

    if (ftruncate(fd, sizeof(ShellStats)) == -1) {
        perror("ftruncate");
        close(fd);
        *semOut = SEM_FAILED;
        return NULL;
    }

    ShellStats* stats = mmap(NULL, sizeof(ShellStats), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd); // The mapping stays valid after closing the descriptor.

    if (stats == MAP_FAILED) {
        perror("mmap");
        *semOut = SEM_FAILED;
        return NULL;
    }

    sem_t* sem = sem_open(STATS_SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        munmap(stats, sizeof(ShellStats));
        *semOut = SEM_FAILED;
        return NULL;
    }

    *semOut = sem;
    return stats;
}

void statsResetSession(ShellStats* stats, sem_t* sem) {
    if (stats == NULL || sem == SEM_FAILED) return;
    sem_wait(sem);
    stats->totalCommands = 0;
    sem_post(sem);
}

void statsIncrement(ShellStats* stats, sem_t* sem) {
    if (stats == NULL || sem == SEM_FAILED) return;
    sem_wait(sem);
    stats->totalCommands++;
    sem_post(sem);
}

unsigned long statsRead(ShellStats* stats, sem_t* sem) {
    if (stats == NULL || sem == SEM_FAILED) return 0;
    sem_wait(sem);
    unsigned long value = stats->totalCommands;
    sem_post(sem);
    return value;
}

void statsCleanup(ShellStats* stats, sem_t* sem) {
    if (stats != NULL) munmap(stats, sizeof(ShellStats));
    if (sem != SEM_FAILED) sem_close(sem);
    shm_unlink(STATS_SHM_NAME);
    sem_unlink(STATS_SEM_NAME);
}
