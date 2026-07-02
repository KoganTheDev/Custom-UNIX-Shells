#ifndef SHELL_CONFIG_H
#define SHELL_CONFIG_H

/**
 * @file shell_config.h
 * @brief Compile-time limits shared across Standard_shell's modules (main
 *        dispatch, job control, pipeline execution) so they can't drift out
 *        of sync with each other.
 */

#define SIZE_OF_INPUT 256   ///< Max length of one line of user input
#define MAX_ARGS 8           ///< Max tokens per command (including argv[0]), plus a NULL slot
#define MAX_PIPE_STAGES 4    ///< Max number of "|"-separated commands in one pipeline

#endif
