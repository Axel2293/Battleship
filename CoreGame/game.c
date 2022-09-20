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
    //Name
    char type_name[10];
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

void gameLoop(char (*player)[Columns],ship *usr_ships, cell (*usr_cells)[Columns],int *usr_len, char (*pc)[Columns] , ship *pc_ships, cell (*pc_cells)[Columns], int *pc_len);
int verify_win(ship *, int *);
void enter_continue();

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

    gameLoop(board_usr, usr_ships, CB_usr, &total_ships_usr, board_pc, pc_ships, CB_pc, &total_ships_pc);
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

                int verf_x=cellX, verf_y=cellY;
                int direction=0;
                int free_cells=0;
                // Orientation
                if(type !=4)
                {
                    
                    // Decidir si la posicion del barco sera H o V
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
                            (ships_list+ship_list_pos)->ship_type=*(types_sizes+type);
                            

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
                            (ships_list+ship_list_pos)->ship_type=*(types_sizes+type);

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
                            (ships_list+ship_list_pos)->ship_type=*(types_sizes+type);

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
                            (ships_list+ship_list_pos)->ship_type=*(types_sizes+type);

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
                    (ships_list+ship_list_pos)->ship_type=*(types_sizes+type);

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
    // Menejo de colores de la consola
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    // Current atributes of color, etc..
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;


    // Upper char A-J guide points
    printf("\n| B-S ");
    for(int k=0; k<(Columns); k++)
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN );
        printf("| %d ", k);
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
    for(int i=0; i<(Rows); i++)
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
        printf("| %d   ", i);
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

void gameLoop(char (*player)[Columns],ship *usr_ships, cell (*usr_cells)[Columns],int *usr_len, char (*pc)[Columns] , ship *pc_ships, cell (*pc_cells)[Columns], int *pc_len)
{
    
    int win_usr=0, win_pc=0;
    printf("Hello");

    while((win_usr && win_pc) == 0)
    {
        //Turno del jugador
        system("cls");
        drawBoard(player, &Columns, &Rows);
        drawBoard(pc, &Columns, &Rows);

        int x_usr=0, y_usr=0, attack_failed=0;
        
        while(attack_failed == 0)
        {

            system("cls");
            drawBoard(player, &Columns, &Rows);
            drawBoard(pc, &Columns, &Rows);
            printf("\nIngresa tus cordenadas para disparar x,y (separadas por coma)\n");
            scanf("%d,%d", &x_usr, &y_usr);

            // Verify if cell is occupied and mark a hit
            if((*(pc_cells+y_usr)+x_usr)->cell_state==1)
            {
                if((*(pc_cells+y_usr)+x_usr)->hit==0)
                {
                    system("cls");
                    //Mark Cell as hit
                    (*(pc_cells+y_usr)+x_usr)->hit=1;
                    //Draw in the board the hit on the cell
                    *(*(pc+y_usr)+x_usr)='X';
                    //Register the hit on the boat struct
                    int id=(*(pc_cells+y_usr)+x_usr)->ship_id;
                    (pc_ships+(id-1))->sunk +=1;
                    drawBoard(player, &Columns, &Rows);
                    drawBoard(pc, &Columns, &Rows);
                    printf("Impacto en (%d,%d)\n", x_usr, y_usr);
                    enter_continue();
                    getchar();

                }
                else
                {
                    printf("Ya atacaste esa coordenada\n");
                    enter_continue();
                }
            }
            else if((*(pc_cells+y_usr)+x_usr)->cell_state==0)
            {
                *(*(pc+y_usr)+x_usr)=2;
                (*(pc_cells+y_usr)+x_usr)->hit=1;
                system("cls");
                drawBoard(player, &Columns, &Rows);
                drawBoard(pc, &Columns, &Rows);

                printf("No hay nada en la celda atacada\n");
                enter_continue();
                
                attack_failed=1;
            }

            //Verify if usr wins
            win_usr=verify_win(pc_ships, pc_len);
            if(win_usr)
            {
                break;
            }

        }
        if(win_usr)
        {
            printf("¡¡Jugador gana!!\n");
            attack_failed=1;
            break;
        }

        //Turno de PC
        int pc_x=0, pc_y=0; 
        attack_failed=0;
        while(attack_failed == 0)
        {
            //Random cell to attack
            pc_x=rand()%Columns; pc_y=rand()%Rows;
            while((*(usr_cells+pc_y)+pc_x)->hit == 1)
            {
                pc_x=rand()%Columns; pc_y=rand()%Rows;
                printf("New values %d--%d hit? %d", pc_x, pc_y, (*(usr_cells+pc_y)+pc_x)->hit);
            }
            getchar();

            system("cls");
            printf("Computadora atacará %d--%d\n", pc_x, pc_y);
            enter_continue();

            // Verify if cell is occupied and mark a hit
            if((*(usr_cells+pc_y)+pc_x)->cell_state==1)
            {
                if((*(usr_cells+pc_y)+pc_x)->hit==0)
                {
                    system("cls");
                    //Mark Cell as hit
                    (*(usr_cells+pc_y)+pc_x)->hit=1;
                    //Draw in the board the hit on the cell
                    *(*(player+pc_y)+pc_x)='X';
                    //Register the hit on the boat struct
                    int id=(*(usr_cells+pc_y)+pc_x)->ship_id;
                    (usr_ships+(id-1))->sunk +=1;
                    drawBoard(player, &Columns, &Rows);
                    drawBoard(pc, &Columns, &Rows);
                    printf("Impacto de la coputadora en (%d,%d)\n", pc_x, pc_y);
                    enter_continue();

                }
                else
                {
                    //printf("Ya atacaste esa coordenada\n");
                }
            }
            else if((*(usr_cells+pc_y)+pc_x)->cell_state==0)
            {
                system("cls");
                *(*(player+pc_y)+pc_x)=2;
                (*(usr_cells+pc_y)+pc_x)->hit=1;
                drawBoard(player, &Columns, &Rows);
                drawBoard(pc, &Columns, &Rows);
                printf("La computadora fallo\n");
                enter_continue();
                attack_failed=1;

            }

            //Verify if usr wins
            win_pc=verify_win(usr_ships, usr_len);
            if(win_pc)
            {
                break;
            }

        }
        if(win_pc)
        {
            printf("¡¡Computadora gana!!\n");
            attack_failed=1;
            break;
        }

    }
}


int verify_win(ship *ships, int *ships_len)
{
    int sunk_total=0;
    int type_sizes[5]={5,4,3,2,1};
    //Verify win of u
    for(int i=0; i<*ships_len; i++)
    {
        if((ships+i)->sunk == (ships+i)->ship_type)
        {
            sunk_total++;
        }
    }
    if (sunk_total== *ships_len)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

void enter_continue()
{
    printf("\nPresiona enter para continuar..");
    getchar(); 
}



// Funciones para verificar el contenido de las estructuras
void print_ships(ship *ships, int *len)
{
    for(int i=0; i< *len; i++)
    {
        printf("ID : %d\n", (ships+i)->ID);
        printf("\tOrientation : %c\n", (ships+i)->orientation);
        printf("\tSunk : %d\n", (ships+i)->sunk);
        printf("\tType : %d\n", (ships+i)->ship_type);
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