/********************************/

// Includes:

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <stdlib.h>

/********************************/

// Defines:

#define PIPE_ERROR           ("pipe() failed.\n")
#define EXECVP_ERROR         ("execvp() failed.\n")

#define PIPE_FDS_AMOUNT      (2)

#define EXIT_ERROR_CODE      (1)
#define EXIT_OK_CODE         (0)

#define PRINTER_PROCESS_PATH ("./ex2_inp.o")
#define GAME_PROCESS_PATH    ("./ex2_upd.o")

#define MAX_PID_DIGITS       (10)

/********************************/

// Static Variables:

static int Pipe_Fds[PIPE_FDS_AMOUNT];

/********************************/

// Static Declarations:

static void createPipe(void);
static pid_t runProcess(char *args[]);

/********************************/

// Functions:

static void createPipe(void)
{
    if(pipe(Pipe_Fds) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, PIPE_ERROR, sizeof(PIPE_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }
}

static pid_t runProcess(char *args[])
{
    pid_t child_pid = fork();

    if(0 == child_pid)
    {
        // This is the printer's process.
        execvp(args[0], args);

        // No checking needed, exits with error code.
        write(STDERR_FILENO, EXECVP_ERROR, sizeof(EXECVP_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    return child_pid;
}

/********************************/

// Main:

int main(void)
{
    pid_t printer_pid = 0, game_pid = 0;
    char printer_pid_str[MAX_PID_DIGITS] = {0};

    char *printer_args[] = {PRINTER_PROCESS_PATH, NULL};
    char *game_args[] = {GAME_PROCESS_PATH, NULL, NULL};

    printf("parent pid: %d\n", getpid());

    // Create pipe.
    createPipe();

    // Run the printer process.
    printer_pid = runProcess(printer_args);
    printf("printer pid: %d\n", printer_pid);

    sprintf(printer_pid_str, "%d", printer_pid);
    game_args[1] = printer_pid_str;

    // Run the game process.
    game_pid = runProcess(game_args);
    printf("game pid: %d\n", game_pid);

    return EXIT_OK_CODE;
}