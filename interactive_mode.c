#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backend.h"

void main_menu(); // declarare mai devreme pentru a accesa functia in alte meniuri
struct table *ptrtables;

#ifdef _WIN32
#define CLEAR_COMMAND "cls"
#else
#define CLEAR_COMMAND "clear"
#endif

void clear_screen()
{
    system(CLEAR_COMMAND);
}

char user_option(int nr_optiuni)
/*
    Ce face: scanează după opțiunea utilizatorului când trebuie aleasă
    o optiune de traversare in meniuri.
    Argumente: nr_optiuni
        - spune funcției câte opțiuni numerice există pentru un meniu
        încât să poată respingă orice cifră mai mare decât numărul
        de opțiuni
*/
{
    char option;
    /* Deoarece option este un char și nu int dacă este introdus un
    număr mai mare de 9 o să se ia in considerare doar prima cifră.
    */
    int ales_deja = 0;
    do
    {
        if (ales_deja)
        {
            printf("\e[1F\e[0J"); // sterge linia deasupra cursorului
            printf("Valoarea aleasa nu este buna! ");
        }
        printf("Introduceti optiunea dorita: ");
        fflush(stdin);
        scanf("%c", &option);

        ales_deja = 1;
    } while (option < '0' || option > ('0' + nr_optiuni));
    // printf("\e[1F\e[0J"); // sterge linia deasupra cursorului
    // 0 este optiunea universală de a merge înapoi
    return option;
}

void show_reservations()
{
    clear_screen();
    printf("--- Rezervarile curente ---\n");

    int nr_mase = get_table_count();
    for (int i = 0; i < nr_mase; i++)
    {
        printf("Masa %d cu nr locuri %d\n", ptrtables[i].id, ptrtables[i].seats);
    }

    printf("\n0: Meniu principal\n");
    char op = user_option(0);
    if (op == '0')
        main_menu();
}

void create_reservation()
{
    clear_screen();
    printf("--- Rezervarea unei mese ---\n"
           "Aceste mese sunt libere, selectati una pentru rezervare:\n");

    int nr_mese = get_table_count();
    for (int i = 0; i < nr_mese; i++)
    {
        if (ptrtables[i].is_reserved == 0)
            printf("Masa %d cu nr locuri %d\n", ptrtables[i].id, ptrtables[i].seats);
    }

    int table;
    int is_ok = 0;
    char op;
    char name[31];
    char tmp[11];
    struct calendar_date date;
    do
    {
        scanf("%d", &table);
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("Introduceti numele persoanei care rezerva (maxim 30 de caractere)\n");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';
        fflush(stdin);
        printf("Introduceti data pentru rezervare (in format ZZ.LL.AAAA)\n");
        fgets(tmp, sizeof(tmp), stdin);
        tmp[strcspn(tmp, "\n")] = '\0';
        sscanf(tmp, "%d.%d.%d", &date.day, &date.month, &date.year);

        printf("Rezervarea este pe data de %d.%d.%d, masa %d pentru %s. Este rezervarea corecta?\n", date.day, date.month, date.year, table, name);
        printf("0: Anulare, meniu principal\n"
               "1: Datele sunt corecte\n"
               "2: Datele sunt incorecte\n");
        op = user_option(2);
        if (op == '0')
            main_menu();
        else if (op == '1')
            is_ok = 1;
        else if (op == '2')
            is_ok = 0;

    } while (is_ok == 0);

    for (int i = 0; i < nr_mese; i++)
    {
        if (ptrtables[i].id == table)
        {
            ptrtables[i].is_reserved = 1;
            strncpy(ptrtables[i].reservation_name, name, 30);
            ptrtables[i].reservation_name[30] = '\0';
            snprintf(ptrtables[i].reservation_date, sizeof(ptrtables[i].reservation_date),
                     "%02d.%02d.%04d", date.day, date.month, date.year);
            break;
        }
    }

    save_tables(ptrtables, nr_mese);
}

void modify_reservations()
{
    clear_screen();
    printf("--- Modificarea unei rezervari ---\n"
           "Acestea sunt rezervarile existente:\n");

    int nr_mese = get_table_count();
    for (int i = 0; i < nr_mese; i++)
    {
        if (ptrtables[i].is_reserved == 1)
            printf("Masa %d rezervata de %s pe data de %.10s\n", ptrtables[i].id, ptrtables[i].reservation_name, ptrtables[i].reservation_date);
    }

    printf("\n0: Meniu principal\n"
           "1: Modificare rezervare\n"
           "2: Stergere rezervare\n");
    char op = user_option(2);
    if (op == '0')
        main_menu();
    if (op == '1')
    {
        int table;
        char name[31];
        char tmp[11];
        struct calendar_date date;
        int c;
        printf("Introduceti numarul mesei de la rezervare: ");
        scanf("%d", &table);
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("Introduceti numele persoanei care rezerva (maxim 30 de caractere)\n");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';
        fflush(stdin);
        printf("Introduceti data pentru rezervare (in format ZZ.LL.AAAA)\n");
        fgets(tmp, sizeof(tmp), stdin);

        strncpy(ptrtables[table - 1].reservation_name, name, 30);
        ptrtables[table - 1].reservation_name[30] = '\0';
        tmp[strcspn(tmp, "\n")] = '\0';
        sscanf(tmp, "%d.%d.%d", &date.day, &date.month, &date.year);
        snprintf(ptrtables[table - 1].reservation_date, sizeof(ptrtables[table - 1].reservation_date),
                 "%02d.%02d.%04d", date.day, date.month, date.year);
    }
    if (op == '2')
    {
        int table;
        printf("Introduceti numarul mesei de la rezervare: ");
        scanf("%d", &table);
        ptrtables[table - 1].is_reserved = 0;
    }

    save_tables(ptrtables, nr_mese);
    printf("0: Meniu principal\n");
    op = user_option(0);
    if (op == '0')
        main_menu();
}

void show_tables()
{
    clear_screen();
    printf("--- Modificarea meselor ---\n"
           "Mesele restaurantului:\n");

    int nr_mase = get_table_count();
    for (int i = 0; i < nr_mase; i++)
    {
        printf("Masa %d cu nr locuri %d\n", ptrtables[i].id, ptrtables[i].seats);
    }

    printf("\n0: Meniu principal\n");
    char op = user_option(0);
    if (op == '0')
        main_menu();
}

void modify_tables()
{
    clear_screen();
    printf("--- Vizualizarea meselor ---\n"
           "Mesele restaurantului:\n");

    int nr_mese = get_table_count();
    for (int i = 0; i < nr_mese; i++)
    {
        printf("Masa %d cu nr locuri %d\n", ptrtables[i].id, ptrtables[i].seats);
    }

    printf("\n0: Meniu principal\n"
           "1: Modificare masa\n"
           "2: Stergere masa\n");
    char op = user_option(2);
    if (op == '0')
        main_menu();
    if (op == '1')
    {
        int table;
        int nr_locuri;
        printf("Introduceti numarul mesei: ");
        scanf("%d", &table);
        printf("Introduceti numarul de locuri a mesei: ");
        scanf("%d", &nr_locuri);

        ptrtables[table - 1].seats = nr_locuri;
    }
    if (op == '2')
    {
        int table;
        printf("Introduceti numarul mesei: ");
        scanf("%d", &table);
        ptrtables[table - 1].id = -1;
    }
    save_tables(ptrtables, nr_mese);
    printf("\n0: Meniu principal\n");
    op = user_option(0);
    if (op == '0')
        main_menu();
}

void search_tables()
{
    clear_screen();
    printf("--- Cautarea meselor ---\n");

    printf("0: Meniu principal\n"
           "1: Cautare mese dupa numar locuri\n"
           "2: Cautare mese libere\n"
           "3: Cautare mese ocupate\n");

    char op = user_option(3);
    if (op == '0')
        main_menu();
    if (op == '1')
    {
        int nr_locuri;
        printf("Introduceti numarul de locuri: ");
        scanf("%d", &nr_locuri);

        int nr_mese = get_table_count();
        for (int i = 0; i < nr_mese; i++)
        {
            if (ptrtables[i].seats == nr_locuri)
                printf("Masa %d\n", ptrtables[i].id);
        }
    }
    if (op == '2')
    {
        clear_screen();
        int nr_mese = get_table_count();
        for (int i = 0; i < nr_mese; i++)
        {
            if (ptrtables[i].is_reserved == 0)
                printf("Masa %d cu %d locuri\n", ptrtables[i].id, ptrtables[i].seats);
        }
    }
    if (op == '3')
    {
        clear_screen();
        int nr_mese = get_table_count();
        for (int i = 0; i < nr_mese; i++)
        {
            if (ptrtables[i].is_reserved == 1)
                printf("Masa %d rezervata de %s pe %.10s\n", ptrtables[i].id, ptrtables[i].reservation_name, ptrtables[i].reservation_date);
        }
    }

    printf("\n0: Meniu principal\n");
    op = user_option(0);
    if (op == '0')
        main_menu();
}

void main_menu()
{
    create_db_if_none();
    ptrtables = (struct table *)malloc(get_table_count() * sizeof(struct table));
    populate_memory(ptrtables);

    clear_screen();
    printf("--- Meniu Principal --- \n"
           "0: Iesire din program\n"
           "1: Creare rezervare\n"
           "2: Vizualizare rezervari\n"
           "3: Modificare rezervari\n"
           "4: Vizualizare mese\n"
           "5: Modificare mese\n"
           "6: Cautare masa\n");

    char op = user_option(6);
    if (op == '0')
        exit(EXIT_SUCCESS);
    else if (op == '1')
        create_reservation();
    else if (op == '2')
        show_reservations();
    else if (op == '3')
        modify_reservations();
    else if (op == '4')
        show_tables();
    else if (op == '5')
        modify_tables();
    else if (op == '6')
        search_tables();
}
