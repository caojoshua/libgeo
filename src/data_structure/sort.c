#include "data_structure/sort.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void merge_sortr(void **data, void **data_copy, unsigned n) {
  if (n <= 1) {
    return;
  }

  unsigned mid_index = n / 2;
  merge_sortr(data, data_copy, mid_index);
  merge_sortr(data + mid_index, data_copy, n - mid_index);

  unsigned i = 0;
  unsigned j = mid_index;
  unsigned k = 0;
  while (i < mid_index && j < n) {
    if (data[i] < data[j]) {
      data_copy[k] = data[i];
      ++i;
    } else {
      data_copy[k] = data[j];
      ++j;
    }
    ++k;
  }

  while (i < mid_index) {
    data_copy[k] = data[i];
    ++k;
    ++i;
  }

  while (j < n) {
    data_copy[k] = data[j];
    ++k;
    ++j;
  }

  memcpy(data, data_copy, sizeof(void *) * n);
  assert(k == n && "should have copied n elements into the data_copy");
}

void **merge_sort(void **data, unsigned n) {
  assert(data && "cannot sort NULL data");
  void **data_copy = malloc(sizeof(void *) * n);
  merge_sortr(data, data_copy, n);
  free(data_copy);
  return data;
}

void **quick_sort(void **data, unsigned n) {
  assert(data && "cannot sort NULL data");
  if (n <= 1) {
    return data;
  }

  void *pivot = data[0];
  unsigned num_left = 0;
  unsigned less_than_idx = 1;
  for (unsigned i = 1; i < n; ++i) {
    if (data[i] < pivot) {
      ++num_left;
      void *t = data[num_left];
      data[num_left] = data[i];
      data[i] = t;
    }
  }

  data[0] = data[num_left];
  data[num_left] = pivot;
  quick_sort(data, num_left);
  quick_sort(data + num_left + 1, n - num_left - 1);
  return data;
}

void **selection_sort(void **data, unsigned n) {
  if (n <= 1) {
    return data;
  }
  for (unsigned i = 0; i < n; ++i) {
    unsigned min = i;
    for (unsigned j = i + 1; j < n; ++j) {
      if (data[j] < data[min]) {
        min = j;
      }
    }
    void *t = data[min];
    data[min] = data[i];
    data[i] = t;
  }
  return data;
}
