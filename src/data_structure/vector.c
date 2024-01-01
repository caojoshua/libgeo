#include "data_structure/vector.h"
#include "assert.h"
#include "stdlib.h"

const int DEFAULT_INITIAL_CAPACITY = 16;

void vector_init(Vector *vec) {
  vector_initn(vec, DEFAULT_INITIAL_CAPACITY);
}

void vector_initn(Vector *vec, unsigned n) {
  *vec = (Vector){
      .data = malloc(sizeof(void *) * n),
      .size = 0,
      .capacity = n,
  };
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

void *vector_pop_back(Vector *vec) {
  vector_validate(vec);
  if (vec->size == 0) {
    return nullptr;
  }
  void *back = vec->data[vec->size - 1];
  vec->size -= 1;
  return back;
}

bool vector_contains(Vector *vec, void *val) {
  vector_validate(vec);
  for (int i = 0; i < vec->size; ++i) {
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
}

void vector_validate(Vector *vec) {
  assert(vec && "vec must not be null");
  assert(vec->data && "vec data must not be null");
  assert(vec->capacity >= vec->size && "vec capacity must be >= size");
}

