// TODO:
// 1. check if the matrix has free space.
// 2. printf->write(STDOUT)
/********************************/

// Includes:

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/********************************/

// Defines:

#define BOARD_ROW_SIZE            (4)
#define BOARD_COL_SIZE            (4)
#define BOARD_SIZE                (BOARD_ROW_SIZE * BOARD_COL_SIZE)

#define MIN_SPAWN_TIME_IN_SEC     (2)
#define MAX_SPAWN_TIME_IN_SEC     (4)

#define FREE_SLOT                 (0)

/********************************/

// Static Variables:

static unsigned int Game_Board[BOARD_ROW_SIZE][BOARD_COL_SIZE];
static unsigned int Spawn_Time;
/********************************/

// Static Declarations:

static unsigned int getRandomSpawnTime(void);
static unsigned int getRandomFreeIndex(void);

static void printBoardAsLine(void);

static int isBoardEmpty(void);

static void startNewGame(void);

/********************************/

// Functions:

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
    unsigned int row = 0, col = 0;
    for(row = 0; row < BOARD_ROW_SIZE; ++row)
    {
        for(col = 0; col < BOARD_COL_SIZE; ++col)
        {
            printf("%d", Game_Board[row][col]);

            // Newline is printed after the last character.
            if((row == BOARD_ROW_SIZE - 1) && (col == BOARD_COL_SIZE - 1))
            {
                printf("\n");
            }

            // Otherwise print a comma.
            else
            {
                printf(",");
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
