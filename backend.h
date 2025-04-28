#ifndef BACKEND_H
#define BACKEND_H

struct calendar_date
{
    int day;
    int month;
    int year;
};

struct table
{
    int id;
    int seats;
    int is_reserved;
    char reservation_name[31];
    char reservation_date[11];
};

struct table *ptrtables;
void save_tables(struct table *ptrtables, int count);
void create_db_if_none();
void populate_memory(struct table *ptrtables);
int get_table_count();

#endif