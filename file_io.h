#ifndef FILE_IO_H
#define FILE_IO_H

#include "database.h"

int load_database(const char* filename, Database* db);
int save_database(const char* filename, Database* db, Record** sorted_records);
void free_database(Database* db);

#endif // FILE_IO_H