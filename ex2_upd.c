// TODO:
// 1. check if the matrix has free space.
// 2. printf->write(STDOUT)
// 3. kill() return address
// 4. take care of new spawn.

/********************************/

// Includes:

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


/********************************/

// Defines:

#define TRUE                      (1)
#define FALSE                     (0)

#define BOARD_ROW_SIZE            (4)
#define BOARD_COL_SIZE            (4)
#define BOARD_SIZE                (BOARD_ROW_SIZE * BOARD_COL_SIZE)

#define MIN_SPAWN_TIME_IN_SEC     (2)
#define MAX_SPAWN_TIME_IN_SEC     (4)

#define FREE_SLOT                 (0)

#define CREATED_SLOT_VALUE        (2)


// Max. number to print to screen is '2048' which is 4 byte long.
#define NUM_OF_BYTES_TO_WRITE     (10)

// Keys:
#define UP_KEY                    ('W')
#define DOWN_KEY                  ('X')
#define LEFT_KEY                  ('A')
#define RIGHT_KEY                 ('D')
#define NEW_GAME_KEY              ('S')

#define WRITE_ERROR               ("write() failed.\n")
#define KILL_ERROR                ("kill() failed.\n")
#define READ_ERROR                ("read() failed.\n")
#define SIGACTION_ERROR           ("sigaction() failed.\n")
#define SIGFILLSET_ERROR          ("sigfillset() failed.\n")
#define NOF_INPUTS_ERROR          ("Wrong number of inputs.\n")

#define EXIT_ERROR_CODE           (-1)

/********************************/

// Static Variables:

static pid_t Printer_Pid;


static unsigned int Game_Board[BOARD_ROW_SIZE][BOARD_COL_SIZE];
static unsigned int Spawn_Time;

/********************************/

// Static Declarations:

static void tryToWriteToStdout(const char *p_message, unsigned int message_len);

static unsigned int getRandomSpawnTime(void);
static unsigned int getRandomFreeIndex(void);

static void printBoardAsLine(void);

static int isBoardFull(void);

static void startNewGame(void);

static char readDirectionFromUser(void);
static char isDirectionValid(char direction);

static void updateSpawnTime(void);

static void handleGame(void);

static void runGameAlgorithm(unsigned int line[], int start, int end, int direction);

static void runGameAlgorithmForRows(int start, int end, int direction);
static void runGameAlgorithmForCols(int start, int end, int direction);

static void handleMove(char direction);

static void sigalrm_handler(int sig);

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

static unsigned int getRandomSpawnTime(void)
{
    unsigned int limit_diff = MAX_SPAWN_TIME_IN_SEC - MIN_SPAWN_TIME_IN_SEC;

    // Return a random number within the possible limits.
    return (rand() % (limit_diff + 1)) + MIN_SPAWN_TIME_IN_SEC;
}

// Note: MUST assure that the Matrix has free space.
static unsigned int getRandomFreeIndex(void)
{
    // Generate a random index within the possible range.
    unsigned int random_index = rand() % BOARD_SIZE;

    // Keep generating until the index points to a free slot.
    while(*(Game_Board[0] + random_index) != FREE_SLOT)
    {
        random_index = rand() % BOARD_SIZE;
    }

    return random_index;
}

static void printBoardAsLine(void)
{
    const char SEPERATOR[] = ",";
    const char NEWLINE[] = "\n";

    unsigned int row = 0, col = 0, string_len = 0;
    char convert_to_string[NUM_OF_BYTES_TO_WRITE] = {0};

    if(kill(Printer_Pid, SIGUSR1) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, KILL_ERROR, sizeof(KILL_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    for(row = 0; row < BOARD_ROW_SIZE; ++row)
    {
        for(col = 0; col < BOARD_COL_SIZE; ++col)
        {
            // Convert to string format.
            sprintf(convert_to_string, "%d", Game_Board[row][col]);
            string_len = strlen(convert_to_string);

            // Print to screen.
            tryToWriteToStdout(convert_to_string, string_len);

            // Clear converter.
            memset(convert_to_string, 0, sizeof(convert_to_string));

            // Newline is printed after the last character.
            if((row == BOARD_ROW_SIZE - 1) && (col == BOARD_COL_SIZE - 1))
            {
                tryToWriteToStdout(NEWLINE, sizeof(NEWLINE) - 1);
            }

            // Otherwise print a comma.
            else
            {
                tryToWriteToStdout(SEPERATOR, sizeof(SEPERATOR) - 1);
            }
        }
    }
}

static int isBoardFull(void)
{
    int row = 0, col = 0;
    for(row = 0; row < BOARD_ROW_SIZE; ++row)
    {
        for(col = 0; col < BOARD_COL_SIZE; ++col)
        {
            if(Game_Board[row][col] == FREE_SLOT)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static void startNewGame(void)
{
    int i = 0, slot_index = 0;
    const int NUM_OF_NEW_SLOTS_TO_CREATE = 2;

    // Clear game board.
    memset(Game_Board, 0, sizeof(Game_Board));

    updateSpawnTime();

    // Choose slots to be the first initialized slots.
    for(i = 0; i < NUM_OF_NEW_SLOTS_TO_CREATE; ++i)
    {
        slot_index = getRandomFreeIndex();
        *(Game_Board[0] + slot_index) = CREATED_SLOT_VALUE;
    }

    fprintf(stderr, "%s", "UPD:: Printing Line.\n");
    fflush(stderr);

    printBoardAsLine();
}

static char readDirectionFromUser(void)
{
    char direction = 0;

    // Read one character from STDIN.
    if(read(STDIN_FILENO, &direction, sizeof(direction)) <= 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, READ_ERROR, sizeof(READ_ERROR));
        exit(EXIT_ERROR_CODE);
    }

    return direction;
}

static char isDirectionValid(char direction)
{
    return ((direction == UP_KEY) || (direction == DOWN_KEY) || \
            (direction == LEFT_KEY) || (direction == RIGHT_KEY) || \
            (direction == NEW_GAME_KEY));
}

static void updateSpawnTime(void)
{
    // Get a random waiting time.
    Spawn_Time = getRandomSpawnTime();

    // Set alarm.
    alarm(Spawn_Time);
}

static void handleGame(void)
{
    char direction = 0;
    startNewGame();

    while(1)
    {
        if(direction != '\n')
        {
            fprintf(stderr, "Enter direction...\n");
            fflush(stderr);
        }

        direction = readDirectionFromUser();

        if(!isDirectionValid(direction))
        {
            if(direction != '\n')
            {
                fprintf(stderr, "Invalid direction: %c\n", direction);
                fflush(stderr);
            }
            continue;
        }

        fprintf(stderr, "Got direction: %c\n", direction);
        fflush(stderr);

        handleMove(direction);
        if(direction != 'S')
        {
            printBoardAsLine();
        }
    }
}



static void runGameAlgorithm(unsigned int line[], int start, int end, int direction)
{
    unsigned int already_unified_flags[BOARD_ROW_SIZE] = {FALSE};

    // Starting from the slot before the last slot.
    int index = end - direction;

    // Try to move each slot to the desired direction.
    while (index != (start - direction))
    {
        // 'runner' is the current slot to move.
        int runner = index;

        while(runner != end)
        {
            // 'next' is the next slot in the desired direction after 'runner'.
            int next = runner + direction;

            // Free space to move
            if(line[next] == FREE_SLOT)
            {
                // Swap and continue
                line[next] = line[runner];
                line[runner] = FREE_SLOT;

                runner = runner + direction;
                continue;
            }

            // Identical slots
            if(line[next] == line[runner])
            {
                // Unify only if this slot hasn't been unified already!
                if(!already_unified_flags[next])
                {
                    // Unify and update flags.
                    line[next] = line[next] + line[runner];
                    line[runner] = FREE_SLOT;
                    already_unified_flags[next] = TRUE;
                }
            }

            // We got here if there was already a union or no place to go.
            break;
        }

        index = index - direction;
    }
}

static void runGameAlgorithmForRows(int start, int end, int direction)
{
    int row = 0, col = 0;

    unsigned int line[BOARD_ROW_SIZE] = {0};
    for(row = 0; row < BOARD_ROW_SIZE; ++row)
    {
        // Copy the row
        for(col = 0; col <BOARD_COL_SIZE; ++col)
        {
            line[col] = Game_Board[row][col];
        }

        // Run the algorithm
        runGameAlgorithm(line, start, end, direction);

        // Copy back
        for(col = 0; col <BOARD_COL_SIZE; ++col)
        {
            Game_Board[row][col] = line[col];
        }
    }
}

static void runGameAlgorithmForCols(int start, int end, int direction)
{
    unsigned int line[BOARD_COL_SIZE] = {0};
    int row = 0, col = 0;

    for(col = 0; col <BOARD_COL_SIZE; ++col)
    {
        // Copy the col
        for(row = 0; row < BOARD_ROW_SIZE; ++row)
        {
            line[row] = Game_Board[row][col];
        }

        // Run the algorithm
        runGameAlgorithm(line, start, end, direction);

        // Copy back
        for(row = 0; row < BOARD_ROW_SIZE; ++row)
        {
            Game_Board[row][col] = line[row];
        }
    }
}

static void handleMove(char direction)
{
    const int ONWARDS = 1, BACKWARDS = -1;
    const unsigned int FIRST_INDEX = 0;

    switch(direction)
    {
        case NEW_GAME_KEY:
            startNewGame();
            return;

        case UP_KEY:
            runGameAlgorithmForCols(BOARD_COL_SIZE - 1, FIRST_INDEX, BACKWARDS);
            break;

        case DOWN_KEY:
            runGameAlgorithmForCols(FIRST_INDEX, BOARD_COL_SIZE - 1, ONWARDS);
            break;

        case LEFT_KEY:
            runGameAlgorithmForRows(BOARD_ROW_SIZE - 1, FIRST_INDEX, BACKWARDS);
            break;

        case RIGHT_KEY:
            runGameAlgorithmForRows(FIRST_INDEX, BOARD_ROW_SIZE - 1, ONWARDS);
            break;

        default:
            break;
    }

    updateSpawnTime();
    printBoardAsLine();
}

static void sigalrm_handler(int sig)
{
    // Add a new value.
    if(!isBoardFull())
    {
        unsigned int slot_index = getRandomFreeIndex();
        *(Game_Board[0] + slot_index) = CREATED_SLOT_VALUE;
    }

    updateSpawnTime();
    printBoardAsLine();
}

/********************************/

// Main:

int main(int argc, char *argv[])
{
    const unsigned int NUM_OF_INPUTS = 1;
    const unsigned int ARGV_PRINTER_PID_INDEX = 1;

    struct sigaction usr_action;
    sigset_t block_mask;

    fprintf(stderr, "%s", "UPD:: In Upd.\n");
    fflush(stderr);

    // Check number of inputs.
    if(argc != NUM_OF_INPUTS + 1)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, NOF_INPUTS_ERROR, sizeof(NOF_INPUTS_ERROR) - 1);
        exit(EXIT_ERROR_CODE);
    }

    // Initialize the rand() seed.
    srand(time(NULL));

    // Get the printer's pid.
    Printer_Pid = atoi(argv[ARGV_PRINTER_PID_INDEX]);

    // Prepare the alarm handler
    if(sigfillset(&block_mask) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, SIGFILLSET_ERROR, sizeof(SIGFILLSET_ERROR));
        exit(EXIT_ERROR_CODE);
    }

    // Establish the SIGALRM signal handler.
    usr_action.sa_handler = sigalrm_handler;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    if(sigaction(SIGALRM, &usr_action, NULL) < 0)
    {
        // No checking needed, exits with error code.
        write(STDERR_FILENO, SIGACTION_ERROR, sizeof(SIGACTION_ERROR));
        exit(EXIT_ERROR_CODE);
    }

    handleGame();

    return 0;
}
