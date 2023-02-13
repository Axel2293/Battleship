#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "CoreGame/game.h"

void print_logo();
void menu();

int main(void)
{
    menu();
    return 0;
}

void print_logo()
{   
    printf("\x1b[47;30m");
    printf("\t     ___       __  __  __        __   _        \n"
            "\t    / _ )___ _/ /_/ /_/ /__ ___ / /  (_)__     \n"
            "\t   / _  / _ `/ __/ __/ / -_|_-</ _ \\/ / _ \\    \n"
            "\t  /____/\\_,_/\\__/\\__/_/\\__/___/_//_/_/ .__/    \n"
            "\t                                    /_/        \n");
    printf("\x1b[0m");
}

void menu()
{
    int x=0,y=0;
    short gamemode=1;
    short loop=1;

    while (loop)
    {   
        system("clear");
        print_logo();
        printf("\n\n\x1b[37mA.\x1b[0m Modificar dimension del tablero\n");
        printf("\x1b[37mB.\x1b[0m Cambiar modo de juego\n");
        printf("\x1b[37mC.\x1b[0m Iniciar juego \n");
        printf("\x1b[37mD.\x1b[0m Salir \n");

        char seleccion;
        scanf("%c", &seleccion);

        switch (seleccion)
        {
            case 'A':
                printf("Ingresa las dimensiones en el siguiente formato S,S: ");
                scanf("%d,%d", &x, &y);
                getchar();
                break;
            case 'B':
                printf("Selecciona el modo de juedo:\n");
                printf("\t1.Didactico (Por defecto)\n\t2. Facil\n");
                scanf("%hd", &gamemode);
                getchar();
                break;
            case 'C':
                system("clear");
                print_logo();
                printf("\nBienvenido a battleship\n");
                printf("Configuracion: \n\t");
                printf("DImensiones-> %d x %d\n\t", x,y);
                printf("Modo de juego-> ");
                if(gamemode==1)
                {
                    printf(" Didactico\n\t");
                }
                else if(gamemode == 2)
                {
                    printf(" Facil\n\n");
                }
                printf("\nCargando el juego...\n");
                srand(time(NULL));
                gameInit(&x,&y,&gamemode);
                break;
            case 'D':
                printf("\nAdios :)\n");
                loop=0;
                break;
            default:
                break;
        }
    }
}
