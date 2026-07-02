#ifndef PIPELINE_H
#define PIPELINE_H

/**
 * @file pipeline.h
 * @brief Parses and executes one command line as a pipeline: "a | b | c",
 *        optional "< in" on the first stage, "> out"/">> out" on the last
 *        stage, and a trailing "&" for background execution. See docs on
 *        shell pipelines and I/O redirection for the underlying mechanism
 *        (pipe()+dup2() rewiring stdin/stdout before exec()).
 */

/**
 * @brief Executes `userInput` as a pipeline of one or more external
 * commands, registers it as a job (see jobs.h), and either waits for it in
 * the foreground or reports it as backgrounded.
 */
void runPipeline(char* userInput);

#endif
