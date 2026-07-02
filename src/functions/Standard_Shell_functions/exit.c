/**
 * @file exit.c
 * @brief Standalone executable backing the Standard shell's "exit" command:
 * prints a goodbye message and tears down every sub-shell's Commands/ subdirectory.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void emptyDirectory(void);

/**
 * @brief Forks a child to print the goodbye message while the parent tears
 * down every sub-shell's Commands/ subdirectory and the top-level Commands/
 * directory itself.
 * @return 0 on success.
 */
int main(void){
    int pid;

    // Fork a child process
    if ((pid = fork()) < 0){
        printf("Forking in the function \"exit\" has failed.");
        exit(1);
    }
    if (pid == 0){
        // Child process executes "Goodbye" function
        execlp("./functions/Standard_Shell_functions/Goodbye", "Goodbye", NULL);
    }
    else{
        // Parent process
        emptyDirectory(); // Clean up command directories
        rmdir("./Commands"); // Remove main Commands directory
        printf("Commands dir removed.\n");
        wait(NULL); // Wait for child process to finish
    }
    return 0;
}

/**
 * @brief Removes each sub-shell's history file and its Commands/ subdirectory:
 * Math_Commands.txt, Logic_Commands.txt, String_Commands.txt, and the
 * Commands/Math, Commands/Logic, Commands/String directories.
 */
void emptyDirectory(void){
    char* filename = "./Commands/Math/Math_Commands.txt";
    char* filename2 = "./Commands/Logic/Logic_Commands.txt";
    char* filename3 = "./Commands/String/String_Commands.txt";

    char* mathDir = "./Commands/Math";
    char* stringDir = "./Commands/String";
    char* logicDir = "./Commands/Logic";

    // Remove specific files
    remove(filename);
    remove(filename2);
    remove(filename3);

    // Remove specific directories
    rmdir(mathDir);
    rmdir(stringDir);
    rmdir(logicDir);
}