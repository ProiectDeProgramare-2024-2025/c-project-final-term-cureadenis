#include <stdio.h>
#include <stdlib.h>
#include "interactive_mode.h"
#include "backend.h"

int main(int argc, char *argv[])
{
    if (argc == 1) // fara argumente porneste meniul interactin
    {
        main_menu();
        exit(EXIT_SUCCESS);
    }
    // cand se termina modul interactiv iesim din program
    // astfel modul CLI nu trebuie tot implementat intr-un bloc else
    printf("Numarul de argumente primite: %d \n", argc);
    printf("Primul argument: %s\n", argv[1]);
}