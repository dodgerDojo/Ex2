// TODO:
// 1. how to wait for printer to initialize?

/********************************/

// Includes:

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <errno.h>

/********************************/

// Defines:

#define PIPE_ERROR           ("pipe() failed.\n")
#define EXECVP_ERROR         ("execvp() failed.\n")
#define CLOSE_ERROR          ("close() failed.\n")
#define FORK_ERROR           ("fork() failed.\n")
#define DUP2_ERROR           ("dup2() failed.\n")
#define KILL_ERROR           ("kill() failed.\n")

#define PIPE_FDS_AMOUNT      (2)
#define PIPE_READ            (0)
#define PIPE_WRITE           (1)

#define EXIT_ERROR_CODE      (1)
#define EXIT_OK_CODE         (0)

#define PRINTER_PROCESS_PATH ("./ex2_inp.o")
#define GAME_PROCESS_PATH    ("./ex2_upd.o")

#define MAX_PID_DIGITS       (10)

/********************************/

// Static Variables:

static int Game_To_Printer_Pipe_Fds[PIPE_FDS_AMOUNT];

/********************************/

// Static Declarations:

static void createPipe(void);
static void closePipe(int pipe[]);

static pid_t runPrinterProcess(char *args[]);
static pid_t runGameProcess(char *args[]);

/********************************/

// Functions:

static void createPipe(void)
{
    if(pipe(Game_To_Printer_Pipe_Fds) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, PIPE_ERROR, sizeof(PIPE_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }
}

static void closePipe(int pipe[])
{
    if((close(pipe[PIPE_WRITE]) < 0) || (close(pipe[PIPE_READ]) < 0))
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, CLOSE_ERROR, sizeof(CLOSE_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }
}

static pid_t runPrinterProcess(char *args[])
{
    pid_t child_pid = fork();

    // Assure fork() succeeded.
    if(child_pid < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, FORK_ERROR, sizeof(FORK_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    // This is the printer's process.
    if(0 == child_pid)
    {
        // Close write side of pipe.
        if(close(Game_To_Printer_Pipe_Fds[PIPE_WRITE]) < 0)
        {
            // No checking needed, exits with error code.
            write(STDERR_FILENO, CLOSE_ERROR, sizeof(CLOSE_ERROR) - 1);
            exit(EXIT_ERROR_CODE);
        }

        // Redirect I/O - STDIN
        if(dup2(Game_To_Printer_Pipe_Fds[PIPE_READ], STDIN_FILENO) < 0)
        {
            // No checking needed, exits with error code.
            write(STDERR_FILENO, DUP2_ERROR, sizeof(DUP2_ERROR) - 1);
            exit(EXIT_ERROR_CODE);
        }

        // Run the printer's process.
        execvp(args[0], args);

        // got here - execvp() failed! No checking needed, exits with error code.
        write(STDERR_FILENO, EXECVP_ERROR, sizeof(EXECVP_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    return child_pid;
}

static pid_t runGameProcess(char *args[])
{
    pid_t child_pid = fork();

    // Assure fork() succeeded.
    if(child_pid < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, FORK_ERROR, sizeof(FORK_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    // This is the game's process.
    if(0 == child_pid)
    {
        printf("Game:: Close printer read side.\n");
        fflush(stdout);

        // Close read side of the game-printer pipe.
        if(close(Game_To_Printer_Pipe_Fds[PIPE_READ]) < 0)
        {
            // No checking needed, exits with error code.
            write(STDERR_FILENO, CLOSE_ERROR, sizeof(CLOSE_ERROR) - 1);
            exit(EXIT_ERROR_CODE);
        }

        // Redirect I/O: STDOUT
        if(dup2(Game_To_Printer_Pipe_Fds[PIPE_WRITE], STDOUT_FILENO) < 0)
        {
            // No checking needed, exits with error code.
            write(STDERR_FILENO, DUP2_ERROR, sizeof(DUP2_ERROR) - 1);
            exit(EXIT_ERROR_CODE);
        }

        // Run the game's process.
        execvp(args[0], args);

        // got here - execvp() failed! No checking needed, exits with error code.
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
    printer_pid = runPrinterProcess(printer_args);
    printf("printer pid: %d\n", printer_pid);

    // Wait for the printer to initialize.
    sleep(1);

    // Convert the printer's pid into a string.
    sprintf(printer_pid_str, "%d", printer_pid);
    game_args[1] = printer_pid_str;

    // Run the game process.
    game_pid = runGameProcess(game_args);
    printf("game pid: %d\n", game_pid);

    // Parent closes pipe [doesn't use it].
    closePipe(Game_To_Printer_Pipe_Fds);

    sleep(300);

    if(kill(printer_pid, SIGINT) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, KILL_ERROR, sizeof(KILL_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    if(kill(game_pid, SIGINT) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, KILL_ERROR, sizeof(KILL_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    printf("Game Ended.\n");
    return EXIT_OK_CODE;
}
