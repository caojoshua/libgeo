#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
  void **data;
  unsigned capacity;
  unsigned size;
} Vector;

Vector *vector_new();
Vector *vector_newn(unsigned n);

void vector_push(Vector *vec, void *val);
void *vector_pop_back(Vector *vec);
bool vector_contains(Vector *vec, void *val);
void vector_resize(Vector *vec, int new_capacity);
void vector_free(Vector *vec);
void vector_validate(Vector *vec);

#endif
