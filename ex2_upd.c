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

#define FREE_SLOT                (0)

/********************************/

// Static Declarations:

static unsigned int getRandomSpawnTime(void);
static unsigned int getRandomFreeIndex(const unsigned int board[][BOARD_COL_SIZE]);
static void printBoardAsLine(const unsigned int board[][BOARD_COL_SIZE]);

/********************************/

// Functions:

static unsigned int getRandomSpawnTime(void)
{
    unsigned int limit_diff = MAX_SPAWN_TIME_IN_SEC - MIN_SPAWN_TIME_IN_SEC;

    // Return a random number within the possible limits.
    return (rand() % (limit_diff + 1)) + MIN_SPAWN_TIME_IN_SEC;
}

// Note: MUST assure that the Matrix has free space.
static unsigned int getRandomFreeIndex(const unsigned int board[][BOARD_COL_SIZE])
{
    // Generate a random index within the possible range.
    unsigned int random_index = rand() % BOARD_SIZE;

    // Keep generating until the index points to a free slot.
    while(*(board[0] + random_index) != FREE_SLOT)
    {
        random_index = rand() % BOARD_SIZE;
    }

    return random_index;
}

// Note: MUST assure that the Matrix has free space.
static void printBoardAsLine(const unsigned int board[][BOARD_COL_SIZE])
{
    unsigned int row = 0, col = 0;
    for(row = 0; row < BOARD_ROW_SIZE; ++row)
    {
        for(col = 0; col < BOARD_COL_SIZE; ++col)
        {
            printf("%d", board[row][col]);

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

/********************************/

// Main:

int main()
{
    int i = 0;

    unsigned int random_index = 0;

    // Create the game's board.
    unsigned int game_board[BOARD_ROW_SIZE][BOARD_COL_SIZE];

    // Initialize the game board.
    memset(game_board, 0, sizeof(game_board));

    // Initialize the rand() seed.
    srand(time(NULL));

    for(i = 1; i < BOARD_SIZE + 1; ++i)
    {
        // The weird cast is for the compiler (passing non-const to const argument).
        random_index = getRandomFreeIndex((const unsigned int (*)[BOARD_ROW_SIZE])game_board);
        *(game_board[0] + random_index) = i;
    }

    printBoardAsLine((const unsigned int (*)[BOARD_ROW_SIZE])game_board);

    return 0;
}
