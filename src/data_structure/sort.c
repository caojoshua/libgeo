#include "data_structure/sort.h"

void quick_sort(void **data, unsigned n) {
  if (n <= 1) {
    return;
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
}

