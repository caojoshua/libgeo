#ifndef UTIL_H
#define UTIL_H

#include <assert.h>

static void swap(void **a, void **b) {
  assert(a && b && "cannot swap null addresses");
  void *t = *a;
  *a = *b;
  *b = t;
}

#endif
