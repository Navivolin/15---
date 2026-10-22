#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "file_io.h"
#include "sort.h"

int main(int argc, char* argv[]) {
    int field_count;
    int choice;
    Database db;
    Record** ptr_array;
    const char* filename;
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл_базы_данных>\n", argv[0]);
        return 1;
    }

    filename = argv[1];

    printf("Загрузка базы данных из '%s'...\n", filename);
    if (load_database(filename, &db) != 0) {
        return 1;
    }

    if (db.record_count == 0) {
        printf("База данных пуста.\n");
        free_database(&db);
        return 0;
    }

    field_count = db.records[0].field_count;
    printf("Найдено записей: %d, полей в записи: %d\n", db.record_count, field_count);

    printf("\nДоступные поля для сортировки:\n");
    for (int i = 0; i < field_count; i++) {
        printf("  %d - Поле %d", i, i + 1);
        if (db.records[0].fields[i].type == TYPE_NUMERIC) {
            printf(" (числовое)\n");
        } else {
            printf(" (строковое)\n");
        }
    }

    printf("\nВыберите номер поля для сортировки (0-%d): ", field_count - 1);
    if (scanf("%d", &choice) != 1 || choice < 0 || choice >= field_count) {
        fprintf(stderr, "Некорректный ввод.\n");
        free_database(&db);
        return 1;
    }

    ptr_array = malloc(sizeof(Record*) * db.record_count);
    for (int i = 0; i < db.record_count; i++) {
        ptr_array[i] = &db.records[i];
    }

    printf("Сортировка...\n");
    sort_records(ptr_array, db.record_count, choice);

    printf("Сохранение в '%s'...\n", filename);
    if (save_database(filename, &db, ptr_array) != 0) {
        free(ptr_array);
        free_database(&db);
        return 1;
    }

    printf("Готово!\n");
    
    free(ptr_array);
    free_database(&db);

    return 0;
}
