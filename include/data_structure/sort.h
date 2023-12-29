#ifndef HASH_H
#define HASH_H

typedef void **(*sort_t)(void **, unsigned);

void **bubble_sort(void **data, unsigned n);
void **insertion_sort(void **data, unsigned n);
void **merge_sort(void **data, unsigned n);
void **quick_sort(void **data, unsigned n);
void **selection_sort(void **data, unsigned n);

#endif
