#include <stdio.h>


// Global vars for the board size
int Rows=10;
int Columns=10;

// Porcentage of ships ocupation in the board
#define defaultSO 30

// Default game mode : Didactico
#define DF_GM 0

void gameInit(int *userX, int *userY, short *game_mode);

void boardInt(char (*board)[], int *x, int *y);

void drawBoard(char (*board)[], int *x, int *y);


typedef struct cell
{
    // Occupied or not
    short cell_state;
    // Link with a ship in the given cell
    int ship_id;
    // Cell recieved a hit or not
    short hit; 
}cell;

typedef struct ship
{
    // Aircraft carrier (AC), Vessel (VS), Submarine(SM), Cruise (CR), Boat (BT)
    char ship_type[3];
    // Vertical or Hirizontal
    char orientation;
    // Sunk - not sunk
    short sunk;
}ship;

void gameInit(int *userX, int *userY, short *game_mode)
{
    if (userX && userY != 0)
    {
        Rows=*userY;
        Columns=*userX;
        char board[Rows][Columns];
        boardInt(board, userX, userY);
        drawBoard(board, userX, userY);
    }
    else
    {
        // Default dimentions of the game board 10x10
        char board[Rows][Columns];
        boardInt(board, &Columns, &Rows);
        drawBoard(board, &Columns, &Rows);
    }

    
}

// Fill trash data with ' ' space character
void boardInt(char (*board)[Columns], int *x, int *y)
{
    // Rows (Vertical)
    for(int i=0; i<(*y); i++)
    {
        // Columns (Horizontal)
        for(int j=0; j<(*x); j++)
        {
            *(*(board+i)+j)=' ';
        }
    }
}

void drawBoard(char (*board)[Columns], int *x, int *y)
{
    // Upper char A-J guide points
    printf("\n| B-S ");
    for(int k=0; k<(*x); k++)
    {
        printf("| %c ", ('A'+k));
    }
    printf("|\n");

    // InterLines
    printf("-------");
    for(int k=0; k<(*x)-1; k++)
    {
        printf("-----");
    }
    printf("\n");


    // Move through Lines (Vertical)
    for(int i=0; i<(*y); i++)
    {
        printf("| %3.d ", i+1);
        // Columns (Horizontal)
        for(int j=0; j<(*x); j++)
        {
           printf("| %c ", *(*(board+i)+j));
        }
        printf("|\n");

        // InterLines
        printf("-------");
        for(int k=0; k<(*x)-1; k++)
        {
            printf("-----");
        }
        printf("\n");
    }


}



