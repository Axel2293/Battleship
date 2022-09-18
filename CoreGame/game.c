#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>


// Global vars for the board size
int Rows=10;
int Columns=10;
int types_sizes[5]={5,4,3,2,1};
char types_names[5]={'A','V','S','C','B'};

// Porcentage of ships ocupation in the board
#define defaultSO 30

// Default game mode : Didactico
#define DF_GM 0

typedef struct cell{
    // Occupied or not
    short cell_state;
    // Link with a ship in the given cell
    int ship_id;
    // Cell recieved a hit or not
    short hit; 
}cell;

typedef struct ship{
    // Aircraft carrier (AC), Vessel (VS), Submarine(SM), Cruise (CR), Boat (BT)
    char ship_type;
    // Vertical or Hirizontal
    char orientation;
    // Sunk - not sunk
    short sunk;
    // Link with cells
    int ID;
}ship;

void gameInit(int *, int *, short *);

void boardInt(char (*)[], int *, int *);

void drawBoard(char (*)[], int *, int *);

void shipsInit(char (*)[Columns], cell (*)[], int *, int *, int *);

void gameLoop(char (*)[Columns], char (*)[Columns] , int *, int *);

void register_ships(int *, cell (*)[], ship *, char (*)[Columns]);

int total_amount_ships(int *, int *);

void initialice_cells_list(cell (*CB)[Columns]);
void initialize_ships_list(ship *ships, int *);

void total_per_type(int *amt_ships, int *ships_size, int *total_pertype);

void print_ships(ship *ships, int *len);

void gameInit(int *userX, int *userY, short *game_mode)
{
    if (*userX && *userY != 0)
    {
        // USER BOARD
        Rows=*userY;
        Columns=*userX;
        char board[Rows][Columns];
        boardInt(board, userX, userY);
        // drawBoard(board, userX, userY);

        // Bidimensional array of strructs with the info of each cell
        cell CB[Rows][Columns];
        initialice_cells_list(CB);

        int total_pertype[5], total_ships=0;
        shipsInit(board, CB, userX, userY, total_pertype);
        total_ships=*(total_pertype) + *(total_pertype+1) + *(total_pertype+2) + *(total_pertype+3) + *(total_pertype+4);
        
        ship usr_ships[total_ships];
        printf("Total ships: %d \n", total_ships);
        initialize_ships_list(usr_ships, &total_ships);
        register_ships(total_pertype, CB, usr_ships, board);


        print_ships(usr_ships, &total_ships);
        drawBoard(board, userX, userY);
        // PC board

    }
    else
    {
        // USER BOARD
        // Default dimentions of the game board 10x10
        char board[Rows][Columns];
        boardInt(board, &Columns, &Rows);
        // drawBoard(board, &Columns, &Rows);

        // Bidimensional array of strructs with the info of each cell
        cell CB[Rows][Columns];
        initialice_cells_list(CB);

        int total_pertype[5];
        shipsInit(board,CB,&Columns, &Rows, total_pertype);

        int total_ships=*(total_pertype) + *(total_pertype+1) + *(total_pertype+2) + *(total_pertype+3) + *(total_pertype+4);

        ship usr_ships[total_ships];
        printf("Total ships: %d \n", total_ships);
        initialize_ships_list(usr_ships, &total_ships);
        register_ships(total_pertype, CB, usr_ships, board);

        print_ships(usr_ships, &total_ships);
        drawBoard(board, userX, userY);
        // PC BOARD
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

// Define the number and type of ships
void shipsInit(char (*board)[Columns],  cell (*CB)[Columns] ,int *x, int *y , int *ships_per_type)
{
    int cells_for_ships= ((*x) * (*y)) *.3;
    printf("Total of cells for ships %d\n", cells_for_ships);

    // Types of ships
            // Aircraft carrier (AC), Vessel (VS), Submarine(SM), Cruise (CR), Boat (BT)
    int temp=cells_for_ships+1;

    // Amount of cells designated to every ship type
    // [0]=AC, [1]=VS, [2]=SM, [3]=CR, [4]=BT
    int ships[5];

    // Cycle every type of ship so we can get the random quantity of each on the board
    for(int i=0; i<5; i++)
    {
        *(ships+i)=rand() %temp;
        temp-=*(ships+i);
    }

    // Ships have fixed sizes and the random number could leave some unused cells
    int fixed_cells=0;
    int restantcells=0;
    for(int i=0; i<5;i++)
    {
        (*(ships+i))+=restantcells;
        fixed_cells= (*(ships+i))/ *(types_sizes+i);
        restantcells = (*(ships+i)) - (fixed_cells * *(types_sizes+i));
        (*(ships+i))-=restantcells;
    }
    int total_cells_used=*(ships) + *(ships+1) + *(ships+2) + *(ships+3) + *(ships+4);

    // Compensate if more than 5 percent of the cells are not assigned
    while ((total_cells_used - cells_for_ships) > (cells_for_ships * .05))
    {
        // Compensate with adding more boats
        *(ships+4)+=1;
    }


    // Cells available for every type of ship
    for(int i=0; i<5; i++)
    {
        printf("%d -- %d\n", i, *(ships+i));
    }

    // TOTAL OF SHIPS PER TYPE
    total_per_type(ships, types_sizes, ships_per_type);
}

// Create ships on the board and link with cells
void register_ships(int *amt_ships, cell (*CB)[Columns], ship *ships_list, char (*board)[Columns])
{

    // ID will create starting on 1
    int ID_temp=1, ship_list_pos=0;
    // Cycle in every type of ship and create the amount desired
    for(int type=0; type<5; type++)
    {   
        printf("TYPE: %d\n", type);

        while(*(amt_ships+type) !=0)
        {
            // Register Ship on cell and create ship ID linked with cells
                // Random cell in the board
            printf("Searching space for boat\n");
            int conf_of_registry=0;
            while(conf_of_registry==0)
            {

                // COORDINATES OF THE SHIP
                int cellY = rand() %Rows, cellX= rand() %Columns;
                while(  (*(CB+cellY)+cellX)->cell_state != 0)
                {
                    cellY = rand() %Rows; cellX= rand() %Columns;
                }
                printf("\tCells (x=%d, y=%d)\n", cellX, cellY);
                Sleep(2);
            

                int verf_x=cellX, verf_y=cellY;
                int direction=0;
                int free_cells=0;
                // Orientation
                if(type !=4)
                {
                    // Vertical (Rows - Y)
                        // Up the cell
                    printf("  UP\n");
                    while(  (verf_y<Rows) && (verf_y>=0) && (free_cells<(types_sizes[type]))  )
                    {
                        if((*(CB+verf_y)+verf_x)->cell_state == 0)
                        {
                            free_cells++;
                            printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                            if(free_cells==(types_sizes[type]))
                            {
                                printf("\t\tOrientation V\n");
                                Sleep(2);
                                direction=2;
                                break;
                            }
                            verf_y--;
                        }
                        else
                        {
                            break;
                        }
                    }
                    verf_x=cellX; verf_y=cellY; free_cells=0;

                        // Down the cell
                    if(direction==0){
                        printf("  DOWN\n");
                        while(  (verf_y<Rows) && (verf_y>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                                free_cells++;
                                if(free_cells==(types_sizes[type]))
                                {
                                    printf("\t\tOrientation: V\n");
                                    Sleep(2);
                                    direction=-2;
                                    break;
                                }
                                verf_y++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        verf_x=cellX; verf_y=cellY; free_cells=0;
                    }

                    // Horizontal (Columns - X)
                        // Left
                    if(direction==0){
                        printf("  Left\n");
                        while(  (verf_x<Columns) && (verf_x>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                                free_cells++;
                                if(free_cells==(types_sizes[type]))
                                {
                                    printf("\t\tOrientation H\n");
                                    Sleep(2);
                                    direction=-1;
                                    break;
                                }
                                verf_x--;
                            }
                            else
                            {
                                break;
                            }
                        }
                        verf_x=cellX; verf_y=cellY; free_cells=0;
                    }

                        // Right
                    if(direction==0){
                        printf("  RIGHT\n" );
                        while(  (verf_x<Columns) && (verf_x>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                                free_cells++;
                                if(free_cells==(types_sizes[type]))
                                {
                                    direction=1;
                                    printf("\t\tOrientation H\n");
                                    Sleep(2);
                                    break;
                                }
                                verf_x++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        verf_x=cellX; verf_y=cellY; free_cells=0;
                    }

                    if(direction != 0)
                    {
                        if(direction==2)
                        {
                            (ships_list+ship_list_pos)->ID= ID_temp;
                            (ships_list+ship_list_pos)->sunk= 0;
                            (ships_list+ship_list_pos)->orientation='V';
                            (ships_list+ship_list_pos)->ship_type=*(types_names+type);
                            

                            for(int i=0; i< (*(types_sizes+type)); i++){
                                (*(CB+verf_y)+verf_x)->cell_state=1;
                                (*(CB+verf_y)+verf_x)->ship_id=ID_temp;
                                *(*(board+verf_y)+verf_x)=*(types_names+type);
                                verf_y--;
                            }
                            ID_temp++;
                            printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            printf("\tBarcos restantes: %d\n", *(amt_ships+type));
                            ship_list_pos++;

                        }

                        else if(direction==-2)
                        {
                            (ships_list+ship_list_pos)->ID= ID_temp;
                            (ships_list+ship_list_pos)->sunk= 0;
                            (ships_list+ship_list_pos)->orientation='V';
                            (ships_list+ship_list_pos)->ship_type=*(types_names+type);

                            for(int i=0; i< (*(types_sizes+type)); i++){
                                (*(CB+verf_y)+verf_x)->cell_state=1;
                                (*(CB+verf_y)+verf_x)->ship_id=ID_temp;
                                *(*(board+verf_y)+verf_x)=*(types_names+type);
                                verf_y++;
                            }
                            ID_temp++;
                            printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            printf("\tBarcos restantes: %d\n", *(amt_ships+type));
                            ship_list_pos++;
                        }

                        else if(direction==-1)
                        {
                            (ships_list+ship_list_pos)->ID= ID_temp;
                            (ships_list+ship_list_pos)->sunk= 0;
                            (ships_list+ship_list_pos)->orientation='H';
                            (ships_list+ship_list_pos)->ship_type=*(types_names+type);

                            for(int i=0; i< (*(types_sizes+type)); i++){
                                (*(CB+verf_y)+verf_x)->cell_state=1;
                                (*(CB+verf_y)+verf_x)->ship_id=ID_temp;
                                *(*(board+verf_y)+verf_x)=*(types_names+type);
                                verf_x--;
                            }
                            ID_temp++;
                            printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            printf("\tBarcos restantes: %d\n", *(amt_ships+type));
                            ship_list_pos++;
                        }
                        else if(direction==1)
                        {
                            (ships_list+ship_list_pos)->ID= ID_temp;
                            (ships_list+ship_list_pos)->sunk= 0;
                            (ships_list+ship_list_pos)->orientation='H';
                            (ships_list+ship_list_pos)->ship_type=*(types_names+type);

                            for(int i=0; i< (*(types_sizes+type)); i++){
                                (*(CB+verf_y)+verf_x)->cell_state=1;
                                (*(CB+verf_y)+verf_x)->ship_id=ID_temp;
                                *(*(board+verf_y)+verf_x)=*(types_names+type);
                                verf_x++;
                            }
                            ID_temp++;
                            printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            printf("\tBarcos restantes: %d\n", *(amt_ships+type));
                            ship_list_pos++;

                        }
                    }


                    
                }
                else
                {
                    (ships_list+ship_list_pos)->ID= ID_temp;
                    (ships_list+ship_list_pos)->sunk= 0;
                    (ships_list+ship_list_pos)->orientation='B';
                    (ships_list+ship_list_pos)->ship_type=*(types_names+type);

                    (*(CB+verf_y)+verf_x)->cell_state=1;
                    (*(CB+verf_y)+verf_x)->ship_id=ID_temp;

                    *(*(board+verf_y)+verf_x)='B';
                        
                    ID_temp++;
                    printf("\tBarcos antes: %d\n", *(amt_ships+type));
                    (*(amt_ships+type))--;
                    conf_of_registry=1;
                    printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                    printf("\tBarcos restantes: %d\n", *(amt_ships+type));
                    ship_list_pos++;
                }
            }


            


        }
    }
}

// Total integer of ships
int total_amount_ships(int *amt_ships, int *ships_size)
{
    int total=0;
    for(int i=0; i<5; i++)
    {
        total+= *(amt_ships+i) / *(ships_size+i);
    }
    return total;
}

// Gives the total of ships per type
void total_per_type(int *amt_ships, int *ships_size, int *total_pertype)
{
    for(int i=0; i<5; i++)
    {
        *(total_pertype+i)=*(amt_ships+i) / *(ships_size+i);
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

void initialice_cells_list(cell (*CB)[Columns])
{
    for(int rows=0; rows<Rows; rows++)
    {
        for (int columns=0; columns<Columns; columns++)
        {
            (*(CB+rows)+columns)->cell_state=0;
            (*(CB+rows)+columns)->ship_id=0;
            (*(CB+rows)+columns)->hit=0;
        }
    }
}

void initialize_ships_list(ship *ships, int *len)
{
    for(int i=0; i<*len; i++)
    {
        (ships+i)->ID = 0;
        (ships+i)->sunk=0;
        (ships+i)->orientation='0';
    }
}

void gameLoop(char (*plyr_B)[Columns], char (*IA)[Columns] , int *x, int *y)
{

}

void print_ships(ship *ships, int *len)
{
    for(int i=0; i< *len; i++)
    {
        printf("ID : %d\n", (ships+i)->ID);
        printf("\tOrientation : %c\n", (ships+i)->orientation);
        printf("\tSunk : %d\n", (ships+i)->sunk);
        printf("\tType : %c\n", (ships+i)->ship_type);
    }
}

void print_cells(cell (*cells)[Columns])
{
    for(int row=0; row<Rows; row++)
    {
        for(int colm=0; colm<Columns; colm++)
        {
            printf("Cell: (%d, %d)\n", colm, row);
            printf("\tCell state: %d\n", (*(cells+row)+colm)->cell_state);
            printf("\tCell ship ID: %d\n", (*(cells+row)+colm)->ship_id);
        }
    }
}

//TEST 2 