#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Menu/menu.h"
#include "CoreGame/game.h"



int main(void)
{
    // Test variables 
    int x=4, y=4; 
    short gamemode=0;
    // Menu needs functionality 
    // menu();
    srand(time(NULL));
    //Board and game Init works fine with SxS given board sizes, 
    //  preferably use s<100 as I left 3. spaces extra for big numbers on the table on the Y axis (Rows)
    gameInit( &x,&y, &gamemode);
    return 0;
}

//EDIT