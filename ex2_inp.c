/********************************/

// Includes:

#include "string.h"

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
#define READ_ERROR          ("read() failed.")

/********************************/

// Static Variables:

volatile sig_atomic_t gotsignal = 0;

/********************************/

// Static Declarations:

static void printGameBoard(char *p_board_line);

static void signal_hand(int sig);

/********************************/

// Functions:

static void printGameBoard(char *p_board_line)
{
    const char SEPERATOR[] = ",";
    char *value = NULL;
    int row = 0, col = 0, value_as_int = 0;

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
                  printf("|%04d", value_as_int);
                }
                else
                {
                    printf("|%.*s", 4, "    ");
                }
                value = strtok(NULL, SEPERATOR);
            }
        }
        printf("|\n");
    }
}

static void signal_hand(int sig)
{
    gotsignal = 1;
}

/********************************/

// Main:

int main()
{
    // There is an upper bound for the line sent.
    // 16 values * 4 digits + commas < MAX_GAME_LINE_LEN
    char line_buffer[MAX_GAME_LINE_LEN] = {0};

    char read_ch = 0;
    const char ENDLINE = '\n';
    unsigned int line_buffer_index = 0;

    //char str[] = "2,4,0,0,2,2,0,16,0,0,4,0,16,0,16,2048";

    struct sigaction usr_action;
    sigset_t block_mask;

    /* Establish the signal handler. */
    sigfillset (&block_mask);
    usr_action.sa_handler = signal_hand;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    sigaction (SIGUSR1, &usr_action, NULL);

    while(1)
    {
        while(!gotsignal);
        printf("Hello!\n");

        while(read_ch != ENDLINE)
        {
            // Read one character from STDIN.
            if(read(STDIN_FILENO, &read_ch, 1) <= 0)
            {
                write(STDERR_FILENO, READ_ERROR, sizeof(READ_ERROR));
            }
            if(read_ch != ENDLINE)
            {
                // Insert the read character into the line's buffer.
                line_buffer[line_buffer_index++] = read_ch;
            }
        }

        printf("read: %s\n", line_buffer);
        printGameBoard(line_buffer);
        memset(line_buffer, 0, MAX_GAME_LINE_LEN);
        read_ch = 0;
        gotsignal = 0;
        line_buffer_index = 0;
    }

    return 0;
}
