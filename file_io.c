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
    if (!db || !db->records) return;
    for (int i = 0; i < db->record_count; i++) {
        Record* rec = &db->records[i];
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
    rec->fields = realloc(rec->fields, sizeof(Field) * (rec->field_count + 1));
    Field* f = &rec->fields[rec->field_count];
    
    if (is_numeric(token)) {
        f->type = TYPE_NUMERIC;
        f->value.num_val = strtod(token, NULL);
    } else {
        f->type = TYPE_STRING;
        f->value.str_val = strdup(token);
    }
    rec->field_count++;
}

static void parse_line_into_record(Record* rec, char* line) {
    char* token = strtok(line, ",");
    while (token != NULL) {
        while (isspace((unsigned char)*token)) token++;
        char* end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }
        add_field_to_record(rec, token);
        token = strtok(NULL, ",");
    }
}

int load_database(const char* filename, Database* db) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка открытия файла");
        return -1;
    }

    db->records = NULL;
    db->record_count = 0;

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    Record current_record = {NULL, 0};
    int in_record = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
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

    if (in_record) {
        db->records = realloc(db->records, sizeof(Record) * (db->record_count + 1));
        db->records[db->record_count] = current_record;
        db->record_count++;
    }

    free(line);
    fclose(file);
    return 0;
}

int save_database(const char* filename, Database* db, Record** sorted_records) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Ошибка открытия файла для записи");
        return -1;
    }

    for (int i = 0; i < db->record_count; i++) {
        Record* rec = sorted_records[i];
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
