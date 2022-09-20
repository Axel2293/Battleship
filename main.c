#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
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
    // Menejo de colores de la consola
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    // Current atributes of color, etc..
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    SetConsoleTextAttribute(hConsole, white );
    printf("\t     ___       __  __  __        __   _        \n"
            "\t    / _ )___ _/ /_/ /_/ /__ ___ / /  (_)__     \n"
            "\t   / _  / _ `/ __/ __/ / -_|_-</ _ \\/ / _ \\    \n"
            "\t  /____/\\_,_/\\__/\\__/_/\\__/___/_//_/_/ .__/    \n"
            "\t                                    /_/        \n");
    SetConsoleTextAttribute(hConsole, saved_attributes);
}

void menu()
{
    int x=0,y=0;
    short gamemode=1;
    short loop=1;

    while (loop)
    {   
        system("cls");
        print_logo();
        printf("\n\nA. Modificar dimension del tablero\n");
        printf("B. Cambiar modo de juego\n");
        printf("C. Iniciar juego \n");
        printf("D. Salir \n");

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
                system("cls");
                print_logo();
                printf("\nBienvenido a battleship\nCargando el juego...\n");
                Sleep(4000);
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
