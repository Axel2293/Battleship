#include <stdio.h>
#include <stdlib.h>


void print_logo()
{
    printf("\t   ___       __  __  __        __   _     \n"
            "\t  / _ )___ _/ /_/ /_/ /__ ___ / /  (_)__ \n"
            "\t / _  / _ `/ __/ __/ / -_|_-</ _ \\/ / _ \\ \n"
            "\t/____/\\_,_/\\__/\\__/_/\\__/___/_//_/_/ .__/\n"
            "\t                                  /_/    \n");
}


void menu()
{
    system("cls");
    print_logo();

    printf("\n\nA. Modificar dimension del tablero\n");
    printf("B. Seleccionar modo de juego\n");
    printf("\ta.Didactico\n\tb. Facil\n");
    printf("C. Iniciar juego \n");
    printf("D. Salir \n");
}
