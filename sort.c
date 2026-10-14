#include "sort.h"
#include <stdlib.h>
#include <string.h>

// Сравнение двух записей по указанному полю
static int compare_records(Record* a, Record* b, int field_idx) {
    if (field_idx >= a->field_count || field_idx >= b->field_count) {
        return 0;
    }

    Field* f1 = &a->fields[field_idx];
    Field* f2 = &b->fields[field_idx];

    if (f1->type == TYPE_NUMERIC && f2->type == TYPE_NUMERIC) {
        if (f1->value.num_val < f2->value.num_val) return -1;
        if (f1->value.num_val > f2->value.num_val) return 1;
        return 0;
    } else {
        // Если хотя бы одно поле строковое, сравниваем как строки
        const char* s1 = (f1->type == TYPE_STRING) ? f1->value.str_val : "";
        const char* s2 = (f2->type == TYPE_STRING) ? f2->value.str_val : "";
        return strcmp(s1, s2);
    }
}

// Слияние для сортировки
static void merge(Record** arr, int left, int mid, int right, int field_idx) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Record** L = malloc(n1 * sizeof(Record*));
    Record** R = malloc(n2 * sizeof(Record*));

    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2) {
        if (compare_records(L[i], R[j], field_idx) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

// Рекурсивная сортировка слиянием
static void merge_sort(Record** arr, int left, int right, int field_idx) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid, field_idx);
        merge_sort(arr, mid + 1, right, field_idx);
        merge(arr, left, mid, right, field_idx);
    }
}

// Публичная функция сортировки
void sort_records(Record** records, int count, int field_idx) {
    if (count > 1) {
        merge_sort(records, 0, count - 1, field_idx);
    }
}