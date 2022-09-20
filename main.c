#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Menu/menu.h"
#include "CoreGame/game.h"



int main(void)
{
    // Test variables 
    int x=10, y=10; 
    short gamemode=0;
    // Menu needs functionality 
    // menu();
    srand(time(NULL));
    //Board and game Init works fine with SxS given board sizes, 
    gameInit( &x,&y, &gamemode);
    return 0;
}

//EDIT