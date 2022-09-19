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

// Colors
#define white (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED)

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
    // Vertical or Horizontal
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

void gameloop (char (*)[Columns],ship *, cell (*)[Columns], char (*)[Columns] , ship *, cell (*)[Columns]);

void register_ships(int *, cell (*)[], ship *, char (*)[Columns]);

int total_amount_ships(int *, int *);

void initialice_cells_list(cell (*CB)[Columns]);
void initialize_ships_list(ship *ships, int *);

void total_per_type(int *amt_ships, int *ships_size, int *total_pertype);

void print_ships(ship *ships, int *len);

void gameInit(int *userX, int *userY, short *game_mode)
{

    if(*userX && *userY != 0)
    {
        Rows=*userY;
        Columns=*userX;
    }

    // USER INIT
    char board_usr[Rows][Columns];
    boardInt(board_usr, &Columns, &Rows);

    // Bidimensional array of strructs with the info of each cell
    cell CB_usr[Rows][Columns];
    initialice_cells_list(CB_usr);

    int total_pertype_usr[5], total_ships_usr=0;
    shipsInit(board_usr, CB_usr, &Columns, &Rows, total_pertype_usr);
    total_ships_usr=*(total_pertype_usr) + *(total_pertype_usr+1) + *(total_pertype_usr+2) + *(total_pertype_usr+3) + *(total_pertype_usr+4);
        
    ship usr_ships[total_ships_usr];
    // printf("Total ships: %d \n", total_ships_usr);
    initialize_ships_list(usr_ships, &total_ships_usr);
    register_ships(total_pertype_usr, CB_usr, usr_ships, board_usr);


    // print_ships(usr_ships, &total_ships_usr);
    drawBoard(board_usr, &Columns, &Rows);

    // PC INIT
    char board_pc[Rows][Columns];
    boardInt(board_pc, &Columns, &Rows);

    // Bidimensional array of strructs with the info of each cell
    cell CB_pc[Rows][Columns];
    initialice_cells_list(CB_pc);

    int total_pertype_pc[5], total_ships_pc=0;
    shipsInit(board_pc, CB_pc, &Columns, &Rows, total_pertype_pc);
    total_ships_pc=*(total_pertype_pc) + *(total_pertype_pc+1) + *(total_pertype_pc+2) + *(total_pertype_pc+3) + *(total_pertype_pc+4);
    
    ship pc_ships[total_ships_pc];
    initialize_ships_list(pc_ships, &total_ships_pc);
    register_ships(total_pertype_pc, CB_pc, pc_ships, board_pc);

    drawBoard(board_pc, &Columns, &Rows);

    //Impresion de tableros acorde al modo de juego: 
    //Lo hacemos en esta funcion o en gameloop?

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

    printf("Before compensation\n");
    total_per_type(ships, types_sizes, ships_per_type); 
    for(int i=0; i<5; i++)
    {
        printf("%d -- %d\n", (5-i), *(ships_per_type+i));
    }

    
    // Compensate if more than 5 percent of the cells are not assigned
    float percent5=(float)cells_for_ships * (.05), remanant=cells_for_ships-total_cells_used ;
    printf("5 percent: %f\n Restante: %f\n", percent5, remanant);
    while ((remanant) > percent5)
    {
        // Compensate with adding more boats
        total_cells_used=*(ships) + *(ships+1) + *(ships+2) + *(ships+3) + *(ships+4);
        remanant=cells_for_ships-total_cells_used;
        printf("Adding BOAT\n");
        *(ships+4)+=1;
    }

    // TOTAL OF SHIPS PER TYPE
    printf("After compensation\n");
    total_per_type(ships, types_sizes, ships_per_type);
    for(int i=0; i<5; i++)
    {
        printf("%d -- %d\n", (5-i), *(ships_per_type+i));
    }
}

// Create ships on the board and link with cells
void register_ships(int *amt_ships, cell (*CB)[Columns], ship *ships_list, char (*board)[Columns])
{

    // ID will create starting on 1
    int ID_temp=1, ship_list_pos=0;
    // Cycle in every type of ship and create the amount desired
    for(int type=0; type<5; type++)
    {   
        //printf("TYPE: %d\n", type);

        while(*(amt_ships+type) !=0)
        {
            // Register Ship on cell and create ship ID linked with cells
                // Random cell in the board
            //printf("Searching space for boat\n");

            //Funcionaria con un do-while
            int conf_of_registry=0;
            while(conf_of_registry==0)
            {

                // COORDINATES OF THE SHIP
                // Ubicacion aleatoria del barco -> Confirmacion de coordenadas
                int cellY = rand() %Rows, cellX= rand() %Columns;
                while(  (*(CB+cellY)+cellX)->cell_state != 0)
                {
                    cellY = rand() %Rows; cellX= rand() %Columns;
                }
                // printf("\tCells (x=%d, y=%d)\n", cellX, cellY);
            

                int verf_x=cellX, verf_y=cellY;
                int direction=0;
                int free_cells=0;
                // Orientation
                if(type !=4)
                {

                    //Decidir si la posicion del barco sera H o V
                    int vertical=0, horizontal=0;
                    vertical= rand()%11;
                    horizontal= rand()%11;
                    while(vertical==horizontal)
                    {
                        vertical= rand()%11;
                        horizontal= rand()%11;
                    }

                    if(vertical > horizontal)
                    {
                        // Vertical (Rows - Y)
                            // Up the cell
                        // printf("  UP\n");
                        while(  (verf_y<Rows) && (verf_y>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                free_cells++;
                                // printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                                if(free_cells==(types_sizes[type]))
                                {
                                    //printf("\t\tOrientation V\n");
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
                            //printf("  DOWN\n");
                            while(  (verf_y<Rows) && (verf_y>=0) && (free_cells<(types_sizes[type]))  )
                            {
                                if((*(CB+verf_y)+verf_x)->cell_state == 0)
                                {
                                    //printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                                    free_cells++;
                                    if(free_cells==(types_sizes[type]))
                                    {
                                        //printf("\t\tOrientation: V\n");
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
                    }

                    if(horizontal>vertical){
                        // Horizontal (Columns - X)
                            // Left
                        //printf("  Left\n");
                        while(  (verf_x<Columns) && (verf_x>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                //printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                                free_cells++;
                                if(free_cells==(types_sizes[type]))
                                {
                                    //printf("\t\tOrientation H\n");
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

                        // Right
                        //printf("  RIGHT\n" );
                        while(  (verf_x<Columns) && (verf_x>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                //printf("\tcell is free (%d, %d)\n", verf_x, verf_y);
                                free_cells++;
                                if(free_cells==(types_sizes[type]))
                                {
                                    direction=1;
                                    //printf("\t\tOrientation H\n");
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
                            //printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            //printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            //printf("\tBarcos restantes: %d\n", *(amt_ships+type));
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
                            //printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            //printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            //printf("\tBarcos restantes: %d\n", *(amt_ships+type));
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
                            //printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            //printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            //printf("\tBarcos restantes: %d\n", *(amt_ships+type));
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
                            //printf("\tBarcos antes: %d\n", *(amt_ships+type));
                            (*(amt_ships+type))--;
                            conf_of_registry=1;
                            //printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                            //printf("\tBarcos restantes: %d\n", *(amt_ships+type));
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
                    //printf("\tBarcos antes: %d\n", *(amt_ships+type));
                    (*(amt_ships+type))--;
                    conf_of_registry=1;
                    //printf("\tBarco registrado: %d\n ", (ships_list+ship_list_pos)->ID);
                    //printf("\tBarcos restantes: %d\n", *(amt_ships+type));
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
    //¿Que son estos structs?
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    // Current atributes
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;


    // Upper char A-J guide points
    printf("\n| B-S ");
    for(int k=0; k<(*x); k++)
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN );
        printf("| %c ", ('A'+k));
    }
    printf("|\n");

    SetConsoleTextAttribute(hConsole, saved_attributes);
    // InterLines
    printf("-------");
    for(int k=0; k<(Columns); k++)
    {
        printf("-----");
    }
    printf("\n");


    // Move through Lines (Vertical)
    for(int i=0; i<(*y); i++)
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
        printf("| %3.d ", i);
        SetConsoleTextAttribute(hConsole, saved_attributes);
        // Columns (Horizontal)
        for(int j=0; j<(*x); j++)
        {
            printf("|");
            switch (*(*(board+i)+j))
            {
            case 'A':
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | white);
                break;
            case 'V':
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | white);
                break;
            case 'S':
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | white);
                break;
            case 'C':
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | white);
                break;
            case 'B':
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | white);
                break;
            default:
                break;
            }
           printf(" %c ", *(*(board+i)+j));
           SetConsoleTextAttribute(hConsole, saved_attributes);
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

void gameLoop(char (*player)[Columns],ship *usr_ships, cell (*usr_cells)[Columns], char (*pc)[Columns] , ship *pc_ships, cell (*pc_cells)[Columns])
{

    
}


// Funciones para verificar el contenido de cada una de las estructuras
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