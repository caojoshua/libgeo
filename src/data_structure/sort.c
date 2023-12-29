#include "data_structure/sort.h"
#include "data_structure/red_black_tree.h"
#include "data_structure/util.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void **bubble_sort(void **data, unsigned n) {
  bool changed;
  do {
    changed = false;
    for (unsigned i = 0; i < n - 1; ++i) {
      if (data[i] > data[i + 1]) {
        swap(data + i, data + i + 1);
        changed = true;
      }
    }
  } while (changed);
  return data;
}

void heapify(void **data, unsigned n, unsigned i) {
  unsigned left_index = i * 2 + 1;
  if (left_index >= n) {
    return;
  }

  unsigned right_index = left_index + 1;
  if (right_index == n) {
    if (data[left_index] > data[i]) {
      swap(data + i, data + left_index);
    }
    return;
  }

  unsigned greater_index =
      data[left_index] > data[right_index] ? left_index : right_index;
  if (data[greater_index] > data[i]) {
    swap(data + i, data + greater_index);
    heapify(data, n, greater_index);
  }
}

void heap_pop(void **data, unsigned n) {
  swap(data, data + n - 1);
  heapify(data, n - 1, 0);
}

void **heap_sort(void **data, unsigned n) {
  assert(data && "cannot sort NULL data");
  for (unsigned i = n; i > 0; --i) {
    heapify(data, n, i - 1);
  }

  for (unsigned i = n; i > 0; --i) {
    heap_pop(data, i);
  }

  return data;
}

void **insertion_sort(void **data, unsigned n) {
  for (unsigned i = 0; i < n - 1; ++i) {
    for (unsigned j = i + 1; j > 0; --j) {
      if (data[j] < data[j - 1]) {
        swap(data + j, data + j - 1);
      }
    }
  }
  return data;
}

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
      swap(data + num_left, data + i);
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
    swap(data + i, data + min);
  }
  return data;
}

void **tree_sort(void **data, unsigned n) {
  RedBlackTree *tree = red_black_tree_new();
  for (unsigned i = 0; i < n; ++i) {
    red_black_tree_insert(tree, data[i]);
  }
  Optional iter = red_black_tree_min(tree);
  unsigned i = 0;
  while (i < n) {
    assert(iter.present && "tree should have n values");
    data[i] = iter.val;
    iter = red_black_tree_succ(tree, iter.val);
    ++i;
  }
  return data;
}
