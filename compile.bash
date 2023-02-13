clear
ECHO 1-Compiling game files
gcc -o CoreGame/game.o -c CoreGame/game.c

ECHO 2-Compiling executable
gcc main.c CoreGame/game.o -o Battleship.exec

ECHO PROCESS COMPLETED
