#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Global vars for the board size
int Rows=10;
int Columns=10;
// Global data 
int types_sizes[5]={5,4,3,2,1};
char types_names[5]={'A','V','S','C','B'};



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
    char type_name[15];
    // Aircraft carrier (AC), Vessel (VS), Submarine(SM), Cruise (CR), Boat (BT)
    short ship_type;
    // Vertical or Horizontal
    char orientation;
    // Cells that got hit
    short sunk;
    // Link with cells
    int ID;
}ship;

// Prototypes of functions
void gameInit(int *, int *, short *);
void boardInt(char (*)[Columns]);
void drawBoard(char (*)[Columns], short);
void shipsQuantity(char (*)[Columns], cell (*)[], int *);
void gameloop (char (*)[Columns],ship *, cell (*)[Columns], char (*)[Columns] , ship *, cell (*)[Columns]);
void register_ships(int *, cell (*)[], ship *, char (*)[Columns]);
void registerName(char *, const short *);
void str_cpy(char *, char *);
int total_amount_ships(int *, int *);
void cellsInit(cell (*)[Columns]);
void shipsInit(ship *, int *);
void total_per_type(int *, int *, int *);
void gameLoop(short *, char (*)[Columns],ship *, cell (*)[Columns],int *, char (*)[Columns] , ship *, cell (*)[Columns], int *);
int verify_win(ship *, int *);


// Creates needed structs and matrix boards, also starts the game
void gameInit(int *userX, int *userY, short *game_mode)
{

    if(*userX!=0 && *userY!=0)
    {
        Rows=*userY;
        Columns=*userX;
    }

    // USER INIT
        // Board
    char board_usr[Rows][Columns];
    boardInt(board_usr);

        // Cells
    cell CB_usr[Rows][Columns];
    cellsInit(CB_usr);
        // Ships
    int total_pertype_usr[5], total_ships_usr=0;
    shipsQuantity(board_usr, CB_usr, total_pertype_usr);
    total_ships_usr=*(total_pertype_usr) + *(total_pertype_usr+1) + *(total_pertype_usr+2) + *(total_pertype_usr+3) + *(total_pertype_usr+4);
    ship usr_ships[total_ships_usr];
    shipsInit(usr_ships, &total_ships_usr);
    register_ships(total_pertype_usr, CB_usr, usr_ships, board_usr);

    // PC INIT
        // Board
    char board_pc[Rows][Columns];
    boardInt(board_pc);
        // Cells
    cell CB_pc[Rows][Columns];
    cellsInit(CB_pc);
        // Ships
    int total_pertype_pc[5], total_ships_pc=0;
    shipsQuantity(board_pc, CB_pc, total_pertype_pc);
    total_ships_pc=*(total_pertype_pc) + *(total_pertype_pc+1) + *(total_pertype_pc+2) + *(total_pertype_pc+3) + *(total_pertype_pc+4);
    ship pc_ships[total_ships_pc];
    shipsInit(pc_ships, &total_ships_pc);
    register_ships(total_pertype_pc, CB_pc, pc_ships, board_pc);

    // Initialize game
    gameLoop(game_mode,board_usr, usr_ships, CB_usr, &total_ships_usr, board_pc, pc_ships, CB_pc, &total_ships_pc);
}
// Fill trash data with ' ' space character
void boardInt(char (*board)[Columns])
{
    // Rows (Vertical)
    for(int i=0; i<(Rows); i++)
    {
        // Columns (Horizontal)
        for(int j=0; j<(Columns); j++)
        {
            *(*(board+i)+j)=' ';
        }
    }
}
// Define the number and type of ships
void shipsQuantity(char (*board)[Columns],  cell (*CB)[Columns] ,int *ships_per_type)
{
    int cells_for_ships= ((Rows) * (Columns)) *.3;
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
    float percent5=(float)cells_for_ships * (.05), remanant=cells_for_ships-total_cells_used ;
    while ((remanant) > percent5)
    {
        // Compensate with adding more boats
        total_cells_used=*(ships) + *(ships+1) + *(ships+2) + *(ships+3) + *(ships+4);
        remanant=cells_for_ships-total_cells_used;
        *(ships+4)+=1;
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
                                if(free_cells==(types_sizes[type]))
                                {
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
                            while(  (verf_y<Rows) && (verf_y>=0) && (free_cells<(types_sizes[type]))  )
                            {
                                if((*(CB+verf_y)+verf_x)->cell_state == 0)
                                {
                                    free_cells++;
                                    if(free_cells==(types_sizes[type]))
                                    {
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
                        while(  (verf_x<Columns) && (verf_x>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                free_cells++;
                                if(free_cells==(types_sizes[type]))
                                {
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
                        while(  (verf_x<Columns) && (verf_x>=0) && (free_cells<(types_sizes[type]))  )
                        {
                            if((*(CB+verf_y)+verf_x)->cell_state == 0)
                            {
                                free_cells++;
                                if(free_cells==(types_sizes[type]))
                                {
                                    direction=1;
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
                        //Register data on the given direction depending on the type
                        (ships_list+ship_list_pos)->ID= ID_temp;
                        (ships_list+ship_list_pos)->sunk= 0;
                        if(direction==2 || direction==-2)
                        {
                            (ships_list+ship_list_pos)->orientation='V';                            
                        }
                        else if(direction==-1 || direction==1)
                        {
                            (ships_list+ship_list_pos)->orientation='H';
                        }
                        (ships_list+ship_list_pos)->ship_type=*(types_sizes+type);
                        registerName((ships_list+ship_list_pos)->type_name, &(ships_list+ship_list_pos)->ship_type);


                        for(int i=0; i< (*(types_sizes+type)); i++){
                            (*(CB+verf_y)+verf_x)->cell_state=1;
                            (*(CB+verf_y)+verf_x)->ship_id=ID_temp;
                            *(*(board+verf_y)+verf_x)=*(types_names+type);
                            // There are 4 types of directions, every one moves diferently on the board
                            switch (direction)
                            {
                                case 2:
                                    verf_y--;
                                    break;
                                case -2:
                                    verf_y++;
                                    break;    
                                case -1:
                                    verf_x--;
                                    break;
                                case 1:
                                    verf_x++;
                                    break;
                                default:
                                    break;
                            }                   
                        }
                        ID_temp++;
                        (*(amt_ships+type))--;
                        conf_of_registry=1;
                        ship_list_pos++;
                    }  
                    
                }
                    // Boats only need one cell
                else
                {
                    (ships_list+ship_list_pos)->ID= ID_temp;
                    (ships_list+ship_list_pos)->sunk= 0;
                    (ships_list+ship_list_pos)->orientation='B';
                    (ships_list+ship_list_pos)->ship_type=*(types_sizes+type);
                    registerName((ships_list+ship_list_pos)->type_name, &(ships_list+ship_list_pos)->ship_type);

                    (*(CB+verf_y)+verf_x)->cell_state=1;
                    (*(CB+verf_y)+verf_x)->ship_id=ID_temp;

                    *(*(board+verf_y)+verf_x)='B';
                        
                    ID_temp++;
                    (*(amt_ships+type))--;
                    conf_of_registry=1;
                    ship_list_pos++;
                }
            }
        }
    }
}
// Registers the full name of the ship in the struct
void registerName(char *shipName, const short *type)
{   
    switch ( *type)
    {
    case 5:
        str_cpy(shipName, "Portaviones");
        break;
    case 4:
        str_cpy(shipName, "Buque");
        break;
    case 3:
        str_cpy(shipName, "Submarino");
        break;
    case 2:
        str_cpy(shipName, "Crucero");
        break;
    case 1:
        str_cpy(shipName, "Lancha");
        break;
    default:
        break;
    }
}
// Copy the secnd str on to the first str
void str_cpy(char *ship_name, char *name)
{
    for(int i=0; i<15; i++)
    {
        *(ship_name+i)=*(name+i);
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
//Tableros completos
void drawBoard(char (*board)[Columns], short PC_USR)
{

    if(PC_USR == 0){

        printf("  \x1b[40;37mTABLERO: JUGADOR\x1b[0m  ");
    }
    else if(PC_USR == 1){
        printf("  \x1b[40;37mTABLERO: COMPUTADORA\x1b[0m  ");  
    }

    // Upper char A-J guide points
    printf("\n| \x1b[40;37mB-S  \x1b[0m");
    for(int k=0; k<(Columns); k++)
    {
        printf("|\x1b[40;37m%3.d\x1b[0m", k);
    }
    printf("|\n");

    // InterLines
    printf("------");
    for(int k=0; k<(Columns); k++)
    {
        printf("----");
    }
    printf("-\n");


    // Move through Lines (Vertical)
    for(int i=0; i<(Rows); i++)
    {
        printf("| \x1b[40;37m%3.d  \x1b[0m", i);
        // Columns (Horizontal)
        for(int j=0; j<(Columns); j++)
        {
            printf("|");

            switch (*(*(board+i)+j))
            {
            case 'A':
                printf("\x1b[42;37m");
                break;
            case 'V':
                printf("\x1b[43;37m");
                break;
            case 'S':
                printf("\x1b[44;37m");
                break;
            case 'C':
                printf("\x1b[45;37m");
                break;
            case 'B':
                printf("\x1b[46;37m");
                break;
            case 'X':
                printf("\x1b[41;37m");
                break;
            case 2:
                break;
            default:
                break;
            }
            printf(" %c \x1b[0m", *(*(board+i)+j));
        }
        printf("|\n");

        // InterLines
        printf("------");
        for(int l=0; l<(Columns); l++)
        {
            printf("----");
        }
        printf("-\n");
    }


}
//Tablero sin barcos
void didacticBoard(char (*board)[Columns])
{
    
    printf("  TABLERO: COMPUTADORA  ");
    // Upper char A-J guide points
    printf("\n| B-S ");
    for(int k=0; k<(Columns); k++)
    {
        printf("|%3.d", k);
    }
    printf("|\n");

    // InterLines
    printf("------");
    for(int k=0; k<(Columns); k++)
    {
        printf("----");
    }
    printf("-\n");


    // Move through Lines (Vertical)
    for(int i=0; i<(Rows); i++)
    {
        printf("| %3.d ", i);
        // Columns (Horizontal)
        for(int j=0; j<(Columns); j++)
        {
            printf("|");
            switch (*(*(board+i)+j))
            {
            case 'X':
                printf(" %c ", *(*(board+i)+j));
                break;
            case 2:
                printf(" %c ", *(*(board+i)+j));
                break;
            default:
                printf("   ");
                break;
            }
        }
        printf("|\n");

        // InterLines
        printf("------");
        for(int k=0; k<(Columns); k++)
        {
            printf("----");
        }
        printf("-\n");
    }
}
// Inicializar la estructura con los datos de cada celda
void cellsInit(cell (*CB)[Columns])
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
// Inicializar la lists de barcos y sus datos
void shipsInit(ship *ships, int *len)
{
    for(int i=0; i<*len; i++)
    {
        (ships+i)->ID = 0;
        (ships+i)->sunk=0;
        (ships+i)->orientation='0';
    }
}
//Prints the boards depending on the gamemode
void gamemode_print(char (*board_1)[Columns], char (*board_2)[Columns], short *gamemode)
{
    system("clear");
    drawBoard(board_1, 0);
    if(*gamemode==1){
        drawBoard(board_2, 1);
    }
    else if(*gamemode==2){
        didacticBoard(board_2);
    }
}
// Logica del juego y ciclo principal del mismo
void gameLoop(short *gamemode, char (*player)[Columns],ship *usr_ships, cell (*usr_cells)[Columns],int *usr_len, char (*pc)[Columns] , ship *pc_ships, cell (*pc_cells)[Columns], int *pc_len)
{
    
    int win_usr=0, win_pc=0;
    short turno=rand()%2;

    while(win_usr == 0 && win_pc == 0)
    {

        if(turno==0){
            //Turno del jugador
            int x_usr=0, y_usr=0, attack_failed=0;
            while(attack_failed ==0  && win_usr==0 && win_pc ==0)
            {

                gamemode_print(player, pc, gamemode);
                printf("Turno: Jugador\n");
                printf("\nIngresa tus cordenadas para disparar x,y (separadas por coma)\n");
                scanf("%d,%d", &x_usr, &y_usr);

                // Verify if selection is within the rows and columns values
                while ((x_usr >= Columns || x_usr < 0) || (y_usr >= Rows || y_usr <0))
                {
                    printf("\nTus coordenadas salen del tablero :(\n");
                    printf("Ingresa otras coordenadas:");
                    scanf("%d,%d", &x_usr, &y_usr);
                }
                

                // Verify if cell is occupied and mark a hit
                if((*(pc_cells+y_usr)+x_usr)->cell_state==1)
                {
                    if((*(pc_cells+y_usr)+x_usr)->hit==0)
                    {
                        //Mark Cell as hit
                        (*(pc_cells+y_usr)+x_usr)->hit=1;
                        //Draw in the board the hit on the cell
                        *(*(pc+y_usr)+x_usr)='X';
                        //Register the hit on the boat struct
                        int id=(*(pc_cells+y_usr)+x_usr)->ship_id;
                        (pc_ships+(id-1))->sunk +=1;
                        gamemode_print(player, pc, gamemode);
                        printf("Turno: Jugador\n");
                        printf("Impacto en (%d,%d)\n", x_usr, y_usr);

                    }
                    // Already attacked that cell
                    else
                    {
                        printf("Turno: Jugador\n");
                        printf("Ya atacaste esa coordenada\n");
                    }
                }
                else if((*(pc_cells+y_usr)+x_usr)->cell_state==0)
                {
                    // Draw board cell as happy face
                    *(*(pc+y_usr)+x_usr)=2;
                    // Mark cell info as hit
                    (*(pc_cells+y_usr)+x_usr)->hit=1;
                    gamemode_print(player, pc, gamemode);
                    printf("Turno: Jugador\n");
                    printf("No hay enemigo en la celda seleccionada\n");
                    turno=1;
                    attack_failed=1;
                    break;
                }

                //Verify if user wins
                win_usr=verify_win(pc_ships, pc_len);
                if(win_usr)
                {
                    printf("\n¡¡Jugador gana!!\n");
                    break;
                }
            }
        }
        else if(turno==1)
        {
            //Turno de PC
            int pc_x=0, pc_y=0; 
            int attack_failed=0;
            while(attack_failed ==0  && win_usr==0 && win_pc ==0)
            {
                //Random cell to attack
                pc_x=rand()%Columns; pc_y=rand()%Rows;
                while((*(usr_cells+pc_y)+pc_x)->hit == 1)
                {
                    pc_x=rand()%Columns; pc_y=rand()%Rows;
                }

                // Verify if cell is occupied and mark a hit
                if((*(usr_cells+pc_y)+pc_x)->cell_state==1)
                {
                    if((*(usr_cells+pc_y)+pc_x)->hit==0)
                    {
                        //Mark Cell as hit
                        (*(usr_cells+pc_y)+pc_x)->hit=1;
                        //Draw in the board the hit on the cell
                        *(*(player+pc_y)+pc_x)='X';
                        //Register the hit on the boat struct
                        int id=(*(usr_cells+pc_y)+pc_x)->ship_id;
                        (usr_ships+(id-1))->sunk +=1;
                        gamemode_print(player, pc, gamemode);
                        printf("Turno: Computadora\n");
                        printf("Impacto de la coputadora en (%d,%d)\n", pc_x, pc_y);
                    }
                }
                else if((*(usr_cells+pc_y)+pc_x)->cell_state==0)
                {
                    // Draw board cell as happy face
                    *(*(player+pc_y)+pc_x)=2;
                    // Mark cell info as hit
                    (*(usr_cells+pc_y)+pc_x)->hit=1;
                    gamemode_print(player, pc, gamemode);
                    printf("Turno: Computadora\n");
                    printf("La computadora fallo\n");
                    turno=0;
                    attack_failed=1;
                    break;

                }

                //Verify if usr wins
                win_pc=verify_win(usr_ships, usr_len);
                if(win_pc)
                {
                    printf("\n¡¡Computadora gana!!\n");
                    break;
                }
            }
        }
    }
}
// Verificar si los datos dados significan que alguien gano
int verify_win(ship *ships, int *ships_len)
{
    int sunk_total=0;
    int type_sizes[5]={5,4,3,2,1};
    //Verify win of u
    for(int i=0; i<*ships_len; i++)
    {
        // Si la cantidad de golpes en el barco es igual a la longitud de su tipo, marcalo como hundido
        if((ships+i)->sunk == (ships+i)->ship_type)
        {
            sunk_total++;
        }
    }
    // Si los barcos hundidos es igual a la cantidad de barcos totales entonces alguien gano
    if (sunk_total== *ships_len)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

