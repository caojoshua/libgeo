#ifndef HASH_H
#define HASH_H

typedef void **(*sort_t)(void **, unsigned);

void **merge_sort(void **data, unsigned n);
void **quick_sort(void **data, unsigned n);

#endif
