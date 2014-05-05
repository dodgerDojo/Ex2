/********************************/

// Includes:

#include "stdio.h"
#include "string.h"

/********************************/

// Defines:

#define BOARD_SIZE          (4)

/********************************/

// Static Declarations:

static void printGameBoard(char *p_board_line);

/********************************/

// Functions:

static void printGameBoard(char *p_board_line)
{
    const char SEPERATOR[] = ",";
    char *value = NULL;
    int row = 0, col = 0;

    // get the first value.
    value = strtok(p_board_line, SEPERATOR);

    for(row = 0; row < BOARD_SIZE; ++row)
    {
        for(col = 0; col < BOARD_SIZE; ++col)
        {
            if(value != NULL)
            {
                printf("|%04d", atoi(value));
                value = strtok(NULL, SEPERATOR);
            }
        }
        printf("|\n");
    }
}

/********************************/

// Main:

int main()
{
    char str[] = "2,4,0,0,2,2,0,16,0,0,4,0,16,0,16,2048";
    printf("%s\n", str);

    printGameBoard(str);

    char s[] = ",";
    char *token = NULL;
    int val = 0;



   /* walk through other tokens */
   while( token != NULL )
   {
      val = atoi(token);
      printf("%04d\n", val);
      token = strtok(NULL, s);
   }
    printf("my milkshake");
    printf("%04d",12);
    return 0;
}
