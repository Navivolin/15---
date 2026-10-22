#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int is_numeric(const char* str) {
    char* endptr;
    strtod(str, &endptr);
    if (endptr == str) return 0;
    while (*endptr) {
        if (!isspace((unsigned char)*endptr)) return 0;
        endptr++;
    }
    return 1;
}

void free_database(Database* db) {

    Record* rec;
    if (!db || !db->records) return;
    for (int i = 0; i < db->record_count; i++) {
        rec = &db->records[i];
        for (int j = 0; j < rec->field_count; j++) {
            if (rec->fields[j].type == TYPE_STRING) {
                free(rec->fields[j].value.str_val);
            }
        }
        free(rec->fields);
    }
    free(db->records);
    db->records = NULL;
    db->record_count = 0;
}

static void add_field_to_record(Record* rec, const char* token) {

    Field* f;
    rec->fields = realloc(rec->fields, sizeof(Field) * (rec->field_count + 1));
    f = &rec->fields[rec->field_count];
    
    if (is_numeric(token)) {
        f->type = TYPE_NUMERIC;
        f->value.num_val = strtod(token, NULL);
    } else {
        f->type = TYPE_STRING;
        f->value.str_val = malloc(strlen(token) + 1);
        strcpy(f->value.str_val, token);
    }
    rec->field_count++;
}

static void parse_line_into_record(Record* rec, char* line) {
    char* token;

    char* end;
    token = strtok(line, ",");
    while (token != NULL) {
        while (isspace((unsigned char)*token)) token++;
        end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }
        add_field_to_record(rec, token);
        token = strtok(NULL, ",");
    }
}

int load_database(const char* filename, Database* db) {
    FILE* file;
    char line[1024];
    Record current_record;
    int in_record;
    file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return -1;
    }

    db->records = NULL;
    db->record_count = 0;

    
 //   current_record = {NULL, 0};
    current_record.fields = NULL;
    current_record.field_count = 0;
    in_record = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (strlen(line) == 0) {
            if (in_record) {
                db->records = realloc(db->records, sizeof(Record) * (db->record_count + 1));
                db->records[db->record_count] = current_record;
                db->record_count++;
                current_record.fields = NULL;
                current_record.field_count = 0;
                in_record = 0;
            }
        } else {
            parse_line_into_record(&current_record, line);
            in_record = 1;
        }
    }
    fclose(file);
    return 0;
}

int save_database(const char* filename, Database* db, Record** sorted_records) {
    FILE* file;
    Record* rec;
    file = fopen(filename, "w");
    if (!file) {
        perror("Ошибка открытия файла для записи");
        return -1;
    }

    for (int i = 0; i < db->record_count; i++) {
        
        rec = sorted_records[i];
        for (int j = 0; j < rec->field_count; j++) {
            if (rec->fields[j].type == TYPE_STRING) {
                fprintf(file, "%s", rec->fields[j].value.str_val);
            } else {
                fprintf(file, "%g", rec->fields[j].value.num_val);
            }
            if (j < rec->field_count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n\n");
    }

    fclose(file);
    return 0;
}
