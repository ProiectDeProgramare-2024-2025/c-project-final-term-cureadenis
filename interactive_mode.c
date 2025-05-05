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

// scopul culorilor
#define RED "\e[0;31m"    // data unei rezervari || optiune gresita/anulare
#define BLUE "\e[0;34m"   // nr unei mese || optiune generica/alternativa
#define GREEN "\e[0;32m"  // numele persoanei care rezerva || optiune corecta
#define PURPLE "\e[0;35m" // nr de locuri la o masa
#define WHITE "\e[0;37m"  // revenire text normal

// nefolosite
// #define BLACK "\e[0;30m"
// #define YELLOW "\e[0;33m"
// #define CYAN "\e[0;36m"

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
        if (ptrtables[i].is_reserved == 1)
            printf("Masa %s%d%s rezervata de %s%s%s pe data de %s%.10s%s\n",
                   BLUE, ptrtables[i].id, WHITE, GREEN, ptrtables[i].reservation_name, WHITE, RED, ptrtables[i].reservation_date, WHITE);
    }

    printf("\n%s0%s: Meniu principal\n", RED, WHITE);
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
            printf("Masa %s%d%s cu nr locuri %s%d%s\n", BLUE, ptrtables[i].id, WHITE, PURPLE, ptrtables[i].seats, WHITE);
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
        printf("Introduceti %snumele persoanei%s care rezerva (maxim 30 de litere)\n", GREEN, WHITE);
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';
        fflush(stdin);
        printf("Introduceti %sdata%s pentru rezervare (in format %sZZ.LL.AAAA%s)\n", RED, WHITE, RED, WHITE);
        fgets(tmp, sizeof(tmp), stdin);
        tmp[strcspn(tmp, "\n")] = '\0';
        sscanf(tmp, "%d.%d.%d", &date.day, &date.month, &date.year);

        printf("Rezervarea este pe data de %s%d.%d.%d%s, masa %s%d%s pentru %s%s%s. Este rezervarea corecta?\n",
               RED, date.day, date.month, date.year, WHITE, BLUE, table, WHITE, GREEN, name, WHITE);
        printf("%s0%s: Anulare, meniu principal\n%s1%s: Datele sunt corecte\n%s2%s: Datele sunt incorecte\n",
               RED, WHITE, GREEN, WHITE, BLUE, WHITE);
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
    main_menu();
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
            printf("Masa %s%d%s rezervata de %s%s%s pe data de %s%.10s%s\n",
                   BLUE, ptrtables[i].id, WHITE, GREEN, ptrtables[i].reservation_name, WHITE, RED, ptrtables[i].reservation_date, WHITE);
    }

    printf("\n%s0%s: Meniu principal\n%s1%s: Modificare rezervare\n%s2%s: Stergere rezervare\n",
           RED, WHITE, BLUE, WHITE, BLUE, WHITE);
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
        printf("Introduceti %snumarul mesei%s de la rezervare: ", BLUE, WHITE);
        scanf("%d", &table);
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        printf("Introduceti %snumele persoanei%s care rezerva (maxim 30 de litere)\n", GREEN, WHITE);
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';
        fflush(stdin);
        printf("Introduceti %sdata%s pentru rezervare (in format %sZZ.LL.AAAA%s)\n", RED, WHITE, RED, WHITE);
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
        printf("Introduceti %snumarul mesei%s de la rezervare: ", BLUE, WHITE);
        scanf("%d", &table);
        ptrtables[table - 1].is_reserved = 0;
    }

    save_tables(ptrtables, nr_mese);
    clear_screen();
    printf("%s0%s: Meniu principal\n", RED, WHITE);
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
        printf("Masa %s%d%s cu nr locuri %s%d%s\n", BLUE, ptrtables[i].id, WHITE, PURPLE, ptrtables[i].seats, WHITE);
    }

    clear_screen();
    printf("\n%s0%s: Meniu principal\n", RED, WHITE);
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
        printf("Masa %s%d%s cu nr locuri %s%d%s\n", BLUE, ptrtables[i].id, WHITE, PURPLE, ptrtables[i].seats, WHITE);
    }

    printf("\n%s0%s: Meniu principal\n%s1%s: Modificare masa\n%s2%s: Stergere masa\n",
           RED, WHITE, BLUE, WHITE, BLUE, WHITE);
    char op = user_option(2);
    if (op == '0')
        main_menu();
    if (op == '1')
    {
        int table;
        int nr_locuri;
        printf("Introduceti %snumarul mesei%s: ", BLUE, WHITE);
        scanf("%d", &table);
        printf("Introduceti %snumarul de locuri%s a mesei: ", PURPLE, WHITE);
        scanf("%d", &nr_locuri);

        ptrtables[table - 1].seats = nr_locuri;
    }
    if (op == '2')
    {
        int table;
        printf("Introduceti %snumarul mesei%s: ", BLUE, WHITE);
        scanf("%d", &table);
        ptrtables[table - 1].id = -1;
    }
    save_tables(ptrtables, nr_mese);
    // clear_screen();
    printf("\n%s0%s: Meniu principal\n", RED, WHITE);
    op = user_option(0);
    if (op == '0')
        main_menu();
}

void search_tables()
{
    clear_screen();
    printf("--- Cautarea meselor ---\n");

    printf("%s0%s: Meniu principal\n%s1%s: Cautare mese dupa %snumar locuri%s\n%s2%s: Cautare mese %slibere%s\n%s3%s: Cautare mese %socupate%s\n",
           RED, WHITE, BLUE, WHITE, PURPLE, WHITE, BLUE, WHITE, GREEN, WHITE, BLUE, WHITE, BLUE, WHITE);

    char op = user_option(3);
    if (op == '0')
        main_menu();
    if (op == '1')
    {
        int nr_locuri;
        printf("Introduceti %snumarul de locuri%s: ", PURPLE, WHITE);
        scanf("%d", &nr_locuri);

        int nr_mese = get_table_count();
        for (int i = 0; i < nr_mese; i++)
        {
            if (ptrtables[i].seats == nr_locuri)
                printf("Masa %s%d%s\n", BLUE, ptrtables[i].id, WHITE);
        }
    }
    if (op == '2')
    {
        clear_screen();
        int nr_mese = get_table_count();
        for (int i = 0; i < nr_mese; i++)
        {
            if (ptrtables[i].is_reserved == 0)
                printf("Masa %s%d%s cu %s%d%s locuri\n", BLUE, ptrtables[i].id, WHITE, PURPLE, ptrtables[i].seats, WHITE);
        }
    }
    if (op == '3')
    {
        clear_screen();
        int nr_mese = get_table_count();
        for (int i = 0; i < nr_mese; i++)
        {
            if (ptrtables[i].is_reserved == 1)
                printf("Masa %s%d%s rezervata de %s%s%s pe %s%.10s%s\n", BLUE, ptrtables[i].id, WHITE, GREEN, ptrtables[i].reservation_name, WHITE, RED, ptrtables[i].reservation_date, WHITE);
        }
    }

    printf("\n%s0%s: Meniu principal\n", RED, WHITE);
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
    printf("--- Meniu Principal --- \n%s0%s: Iesire din program\n%s1%s: Creare rezervare\n%s2%s: Vizualizare rezervari\n%s3%s: Modificare rezervari\n%s4%s: Vizualizare mese\n%s5%s: Modificare mese\n%s6%s: Cautare masa\n",
           RED, WHITE, BLUE, WHITE, BLUE, WHITE, BLUE, WHITE, BLUE, WHITE, BLUE, WHITE, BLUE, WHITE);

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
