/********************************/

// Includes:

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/********************************/

// Defines:

#define BOARD_SIZE                (4)

#define MIN_SPAWN_TIME_IN_SEC     (2)
#define MAX_SPAWN_TIME_IN_SEC     (4)

/********************************/

// Static Declarations:

static unsigned int getRandomSpawnTime(void);
static unsigned int getRandomFreeIndex(void);

/********************************/

// Functions:

static unsigned int getRandomSpawnTime(void)
{
    unsigned int limit_diff = MAX_SPAWN_TIME_IN_SEC - MIN_SPAWN_TIME_IN_SEC;

    // Return a random number within the possible limits.
    return (rand() % (limit_diff + 1)) + MIN_SPAWN_TIME_IN_SEC;
}

static unsigned int getRandomFreeIndex(void)
{
    return 0;
}

/********************************/

// Main:

int main()
{
    int i = 0;

    // Create the game's board.
    unsigned int game_board[BOARD_SIZE][BOARD_SIZE];

    // Initialize the game board.
    memset(game_board, 0, sizeof(game_board));

    // Initialize the rand() seed.
    srand (time(NULL));

    return 0;
}
