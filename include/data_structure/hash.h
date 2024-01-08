#ifndef HASH_H
#define HASH_H

typedef unsigned(*hashfunc_t)(void *);
typedef bool(*eqfunc_t)(void *, void *);

// Hash function for pointers. Often times pointers are word aligned, so rather
// than using the raw address, we use its word address. It gets more complicated
// in theory, but this is fine for now.
static unsigned ptr_hash(void *ptr) {
  return (long)ptr / sizeof(void *);
}

static bool ptr_eq(void *a, void *b) {
  return a == b;
}

typedef struct Node Node;

typedef struct {
  Node **data;
  hashfunc_t hashfunc;
  eqfunc_t eqfunc;
  unsigned size;
  unsigned capacity;
  float load_factor;
} Hash;

void hash_init(Hash *hash, hashfunc_t h, eqfunc_t e);
void hash_free(Hash *hash);

bool hash_insert(Hash *hash, void *key);
bool hash_insert_pair(Hash *hash, void *key, void *val);
void *hash_delete(Hash *hash, void *key);
void *hash_get(Hash *hash, void *key);
bool hash_contains(Hash *hash, void *key);

#endif

