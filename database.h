#ifndef DATABASE_H
#define DATABASE_H

typedef enum {
    TYPE_STRING,
    TYPE_NUMERIC
} FieldType;

typedef struct {
    FieldType type;
    union {
        char* str_val;
        double num_val;
    } value;
} Field;

typedef struct {
    Field* fields;
    int field_count;
} Record;

typedef struct {
    Record* records;
    int record_count;
} Database;

void free_database(Database* db);

#endif // DATABASE_H