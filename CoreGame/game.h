#define white (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED)

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
    int ID;
}ship;


void gameInit(int *userX, int *userY, short *game_mode);

void boardInt(char (*board)[], int *x, int *y);

void drawBoard(char (*board)[], int *x, int *y);

