typedef struct {
  void **data;
  unsigned capacity;
  unsigned size;
} Vector;

Vector *vector_new();
Vector *vector_newn(int n);

void vector_push(Vector *vec, void *val);
bool vector_contains(Vector *vec, void *val);
void vector_resize(Vector *vec, int new_capacity);
void vector_free(Vector *vec);
