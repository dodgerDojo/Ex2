// TODO:
// 2. coding style.
// 3. ALL return values.
// 4. remove garbage (dead comments)
// 5. coding style.
/********************************/

// Includes:

#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <stdlib.h>

/********************************/

// Defines:

#define BOARD_SIZE          (4)
#define MAX_GAME_LINE_LEN   (1000)

#define READ_ERROR          ("read() failed.\n")
#define WRITE_ERROR         ("write() failed.\n")
#define SIGACTION_ERROR     ("sigaction() failed.\n")
#define SIGDELSET_ERROR     ("sigdelset() failed.\n")
#define SIGFILLSET_ERROR    ("sigfillset() failed.\n")
#define CLOSE_ERROR         ("close() failed.\n")

#define EXIT_MESSAGE        ("BYE BYE\n")

#define EXIT_ERROR_CODE     (1)
#define EXIT_OK_CODE        (0)

// Max. number to print to screen is '2048' which is 4 byte long.
#define NUM_OF_BYTES_TO_WRITE   (10)


/********************************/

// Static Variables:

volatile sig_atomic_t gotsignal = 0;

/********************************/

// Static Declarations:

static void printGameBoard(char *p_board_line);

static void sigusr1_handler(int sig);
static void sigint_handler(int sig);

static void tryToWriteToStdout(const char *p_message, unsigned int message_len);

/********************************/

// Functions:

static void tryToWriteToStdout(const char *p_message, unsigned int message_len)
{
    if(write(STDOUT_FILENO, p_message, message_len) != message_len)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, WRITE_ERROR, sizeof(WRITE_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }
}

static void printGameBoard(char *p_board_line)
{
    const char SEPERATOR[] = ",";
    const char EMPTY_SLOT[] = "|      ";
    const char END_OF_LINE[] = "|\n";
    char *value = NULL;
    int row = 0, col = 0, value_as_int = 0;
    char str_to_print[NUM_OF_BYTES_TO_WRITE] = {0};

    // get the first value.
    value = strtok(p_board_line, SEPERATOR);

    for(row = 0; row < BOARD_SIZE; ++row)
    {
        for(col = 0; col < BOARD_SIZE; ++col)
        {
            if(value != NULL)
            {
                value_as_int = atoi(value);

                if(value_as_int != 0)
                {
                    sprintf(str_to_print, "| %04d ", value_as_int);
                    tryToWriteToStdout(str_to_print, strlen(str_to_print));
                    memset(str_to_print, 0, sizeof(str_to_print) - 1);
                }

                else
                {
                    tryToWriteToStdout(EMPTY_SLOT, sizeof(EMPTY_SLOT) - 1);
                }

                value = strtok(NULL, SEPERATOR);
            }
        }
        tryToWriteToStdout(END_OF_LINE, sizeof(END_OF_LINE) - 1);
    }
}

static void sigusr1_handler(int sig)
{
    gotsignal = 1;
}

static void sigint_handler(int sig)
{
    tryToWriteToStdout(EXIT_MESSAGE, sizeof(EXIT_MESSAGE) - 1);
    exit(EXIT_OK_CODE);
}

/********************************/

// Main:

int main(void)
{
    // There is an upper bound for the line sent.
    // 16 values * 4 digits + commas < MAX_GAME_LINE_LEN
    char line_buffer[MAX_GAME_LINE_LEN] = {0};

    char read_ch = 0;
    const char ENDLINE = '\n';
    unsigned int line_buffer_index = 0;

    struct sigaction usr_action;
    sigset_t block_mask;

    // Unblock SIGINT
    if(sigfillset(&block_mask) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, SIGFILLSET_ERROR, sizeof(SIGFILLSET_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    if(sigdelset(&block_mask, SIGINT) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, SIGDELSET_ERROR, sizeof(SIGDELSET_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    // Establish the SIGINT signal handler.
    usr_action.sa_handler = sigint_handler;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    if(sigaction(SIGINT, &usr_action, NULL) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, SIGACTION_ERROR, sizeof(SIGACTION_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    // Establish the SIGUSR1 signal handler.
    usr_action.sa_handler = sigusr1_handler;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    if(sigaction(SIGUSR1, &usr_action, NULL) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, SIGACTION_ERROR, sizeof(SIGACTION_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    while(1)
    {
        // Wait for SIGUSR1.
        while(!gotsignal);

        // Read a full line from user.
        while(read_ch != ENDLINE)
        {
            // Read one character from STDIN.
            if(read(STDIN_FILENO, &read_ch, sizeof(read_ch)) <= 0)
            {
                // No checking needed, exits with error code.
                write(STDERR_FILENO, READ_ERROR, sizeof(READ_ERROR) - 1);
                exit(EXIT_ERROR_CODE);
            }

            if(read_ch != ENDLINE)
            {
                // Insert the read character into the line's buffer.
                line_buffer[line_buffer_index++] = read_ch;
            }
        }

        printGameBoard(line_buffer);

        // Reset all variables:
        memset(line_buffer, 0, MAX_GAME_LINE_LEN);
        read_ch = 0;
        gotsignal = 0;
        line_buffer_index = 0;
    }

    return EXIT_OK_CODE;
}
