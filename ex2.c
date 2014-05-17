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
#define NOF_INPUTS_ERROR     ("Wrong number of inputs.\n")

#define PIPE_FDS_AMOUNT      (2)
#define PIPE_READ            (0)
#define PIPE_WRITE           (1)

#define EXIT_ERROR_CODE      (1)
#define EXIT_OK_CODE         (0)

#define PRINTER_PROCESS_PATH ("./ex2_inp.out")
#define GAME_PROCESS_PATH    ("./ex2_upd.out")

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
    if(close(pipe[PIPE_WRITE]) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, CLOSE_ERROR, sizeof(CLOSE_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    if(close(pipe[PIPE_READ]) < 0)
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

        // Close read side of pipe.
        if(close(Game_To_Printer_Pipe_Fds[PIPE_READ]) < 0)
        {
            // No checking needed, exits with error code.
            write(STDERR_FILENO, CLOSE_ERROR, sizeof(CLOSE_ERROR) - 1);
            exit(EXIT_ERROR_CODE);
        }

        sleep(2);

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

        // Close write side of pipe.
        if(close(Game_To_Printer_Pipe_Fds[PIPE_WRITE]) < 0)
        {
            // No checking needed, exits with error code.
            write(STDERR_FILENO, CLOSE_ERROR, sizeof(CLOSE_ERROR) - 1);
            exit(EXIT_ERROR_CODE);
        }

        sleep(2);

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

int main(int argc, char *argv[])
{
    const unsigned int NUM_OF_INPUTS = 1, ARGV_RUNNING_TIME_INDEX = 1;

    pid_t printer_pid = 0, game_pid = 0;
    char printer_pid_str[MAX_PID_DIGITS] = {0};

    char *printer_args[] = {PRINTER_PROCESS_PATH, NULL};
    char *game_args[] = {GAME_PROCESS_PATH, NULL, NULL};

    int running_time = 0;

    // Check number of inputs.
    if(argc != NUM_OF_INPUTS + 1)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, NOF_INPUTS_ERROR, sizeof(NOF_INPUTS_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    // Get the game's running time (in seconds).
    running_time = atoi(argv[ARGV_RUNNING_TIME_INDEX]);

    // Create pipe.
    createPipe();

    // Run the printer process.
    printer_pid = runPrinterProcess(printer_args);

    // Wait for the printer to initialize.
    sleep(1);

    // Convert the printer's pid into a string.
    sprintf(printer_pid_str, "%d", printer_pid);
    game_args[1] = printer_pid_str;

    // Run the game process.
    game_pid = runGameProcess(game_args);

    // Parent closes pipe [doesn't use it].
    closePipe(Game_To_Printer_Pipe_Fds);

    sleep(running_time);

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

    sleep(1);

    return EXIT_OK_CODE;
}
