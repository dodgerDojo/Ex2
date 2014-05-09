// TODO:
// 1. check if the matrix has free space.
// 2. printf->write(STDOUT)
/********************************/

// Includes:

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/********************************/

// Defines:

#define BOARD_ROW_SIZE            (4)
#define BOARD_COL_SIZE            (4)
#define BOARD_SIZE                (BOARD_ROW_SIZE * BOARD_COL_SIZE)

#define MIN_SPAWN_TIME_IN_SEC     (2)
#define MAX_SPAWN_TIME_IN_SEC     (4)

#define FREE_SLOT                 (0)

// Max. number to print to screen is '2048' which is 4 byte long.
#define NUM_OF_BYTES_TO_WRITE     (10)

#define WRITE_ERROR               ("write() failed.\n")

#define EXIT_ERROR_CODE           (-1)

/********************************/

// Static Variables:

static unsigned int Game_Board[BOARD_ROW_SIZE][BOARD_COL_SIZE];
static unsigned int Spawn_Time;

/********************************/

// Static Declarations:

static void tryToWriteToStdout(const char *p_message, unsigned int message_len);

static unsigned int getRandomSpawnTime(void);
static unsigned int getRandomFreeIndex(void);

static void printBoardAsLine(void);

static int isBoardEmpty(void);

static void startNewGame(void);

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

// Note: MUST assure that the Matrix has free space.
static void printBoardAsLine(void)
{
    const char SEPERATOR[] = ",";
    const char NEWLINE[] = "\n";

    unsigned int row = 0, col = 0, string_len = 0;
    char convert_to_string[NUM_OF_BYTES_TO_WRITE] = {0};

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

static int isBoardEmpty(void)
{
    int row = 0, col = 0;
    for(row = 0; row < BOARD_ROW_SIZE; ++row)
    {
        for(col = 0; col < BOARD_COL_SIZE; ++col)
        {
            if(Game_Board[row][col] != 0)
            {
                return 0;
            }
        }
    }

    return 1;
}

static void startNewGame(void)
{
    int i = 0, slot_index = 0;
    const int NUM_OF_NEW_SLOTS_TO_CREATE = 2;
    const int CREATED_SLOT_VALUE = 2;

    // Clear game board.
    memset(Game_Board, 0, sizeof(Game_Board));

    // Get a random waiting time.
    Spawn_Time = getRandomSpawnTime();

    // Choose slots to be the first initialized slots.
    for(i = 0; i < NUM_OF_NEW_SLOTS_TO_CREATE; ++i)
    {
        slot_index = getRandomFreeIndex();
        *(Game_Board[0] + slot_index) = CREATED_SLOT_VALUE;
    }

}

/********************************/

// Main:

int main()
{
    // Initialize the rand() seed.
    srand(time(NULL));

    startNewGame();

    printBoardAsLine();

    return 0;
}
