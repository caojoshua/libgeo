#include "data_structure/vector.h"
#include "stdlib.h"
#include "assert.h"
#include "stdio.h"
#include <string.h>

const int DEFAULT_INITIAL_CAPACITY = 16;

void vector_validate(Vector *vec) {
  assert(vec && "vec must not be null");
  assert(vec->data && "vec data must not be null");
  assert(vec->capacity >= vec->size && "vec capacity must be >= size");
}

Vector *vector_new() {
  return vector_newn(DEFAULT_INITIAL_CAPACITY);
}

Vector *vector_newn(int n) {
  Vector *vec = malloc(sizeof(Vector));
  *vec = (Vector){
      .data = malloc(sizeof(void *) * n),
      .size = 0,
      .capacity = n,
  };
  return vec;
}

void vector_push(Vector *vec, void *val) {
  vector_validate(vec);
  if (vec->size == vec->capacity) {
    vector_resize(vec, vec->capacity == 0 ? DEFAULT_INITIAL_CAPACITY
                                          : vec->capacity * 2);
  }
  vec->data[vec->size] = val;
  vec->size += 1;
}

bool vector_contains(Vector *vec, void *val) {
  printf("vec contains: %p\n", val);
  vector_validate(vec);
  for (int i = 0; i < vec->size; ++i) {
    printf("%i: %p\n", i, vec->data[i]);
    if (vec->data[i] == val) {
      return true;
    }
  }
  return false;
}

void vector_resize(Vector *vec, int new_capacity) {
  vector_validate(vec);
  vec->capacity = new_capacity;
  vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
}

void vector_free(Vector *vec) {
  vector_validate(vec);
  free(vec->data);
  free(vec);
}
