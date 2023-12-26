#ifndef HASH_H
#define HASH_H

#include "data_structure/red_black_tree.h"
#include "vector.h"

typedef unsigned(*hashfunc_t)(void *);

// Hash function for pointers. Often times pointers are word aligned, so rather
// than using the raw address, we use its word address. It gets more complicated
// in theory, but this is fine for now.
static unsigned pointer_hashfunc(void *ptr) {
  return (long)ptr / sizeof(void *);
}

typedef struct {
  RedBlackTree **data;
  hashfunc_t hashfunc;
  unsigned size;
  unsigned capacity;
  float load_factor;
} Hash;

Hash *hash_new();
Hash *hash_newf(hashfunc_t f);
void hash_free(Hash *hash);

bool hash_insert(Hash *hash, void *key);
bool hash_insert_pair(Hash *hash, void *key, void *val);
void *hash_delete(Hash *hash, void *key);
void *hash_get(Hash *hash, void *key);
bool hash_contains(Hash *hash, void *key);

#endif

