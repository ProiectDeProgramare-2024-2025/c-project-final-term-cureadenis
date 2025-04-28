#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER "# nr masa; nr locuri; este rezervata (0 sau 1); nume rezerare (daca este rezervata); data rezervare (daca este rezervata)\n"

/*
    TODO:
    - functie care verifica daca exista deja
        fisiere necesare pentru functionarea
        programului si daca nu le creaza
    - functii pentru vizualizarea meselor
        si rezervarilor
    - functii pentru modificarea meselor si
        rezervarilor
    - crearea de rezervari si mese
    - formatarea datelor in fisier

*/

// Define the struct table with an id member
struct table
{
    int id;
    int seats;
    int is_reserved;
    char reservation_name[31];
    char reservation_date[11];
};

struct table *ptrtables;

int get_table_count()
{
    FILE *dbptr;
    int line_count = -1; // ultima linie este goala din db.txt si nu o punem la numar
    char ch;
    dbptr = fopen("db.txt", "r");
    while ((ch = fgetc(dbptr)) != EOF)
    {
        if (ch == '\n')
            line_count++;
    }

    fclose(dbptr);

    return line_count;
}

void create_db_if_none()
{
    FILE *dbptr;
    dbptr = fopen("db.txt", "r");
    if (dbptr == NULL)
    {
        // fisierul nu exista, trebuie creat
        dbptr = fopen("db.txt", "w");
        fprintf(dbptr, HEADER);
    }
    fclose(dbptr);
}

void save_tables(struct table *ptrtables, int count)
{
    FILE *dbptr = fopen("db.txt", "w");
    if (!dbptr)
    {
        printf("Nu s-a putut deschide db.txt pentru scriere");
        return;
    }

    fprintf(dbptr, HEADER);
    for (int i = 0; i < count; i++)
    {
        if (ptrtables[i].id != -1)
        {
            fprintf(dbptr, "%d;%d;%d",
                    ptrtables[i].id,
                    ptrtables[i].seats,
                    ptrtables[i].is_reserved);

            if (ptrtables[i].is_reserved == 1)
            {
                fprintf(dbptr, ";%s;%.10s",
                        ptrtables[i].reservation_name,
                        ptrtables[i].reservation_date);
            }

            fprintf(dbptr, "\n");
        }
    }

    fclose(dbptr);
}

void populate_memory(struct table *ptrtables)
{
    FILE *dbptr = fopen("db.txt", "r");
    if (!dbptr)
    {
        perror("Nu s-a putut deschide db.txt");
        return;
    }

    char line[127];
    int i = 0;
    while (fgets(line, sizeof(line), dbptr))
    {
        if (line[0] == '#')
            continue;

        char *token = strtok(line, ";");
        if (!token)
            continue;
        ptrtables[i].id = atoi(token);

        token = strtok(NULL, ";");
        if (!token)
            continue;
        ptrtables[i].seats = atoi(token);

        token = strtok(NULL, ";");
        if (!token)
            continue;
        ptrtables[i].is_reserved = atoi(token);

        if (ptrtables[i].is_reserved == 1)
        {
            token = strtok(NULL, ";");
            if (token)
                strncpy(ptrtables[i].reservation_name, token, 30);
            token = strtok(NULL, ";");
            if (token)
                strncpy(ptrtables[i].reservation_date, token, 11);
        }

        i++;
    }

    fclose(dbptr);
}
