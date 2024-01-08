// This file implements a hash data structure. It can function as a hash map, or
// hash set if the keys are ignored.
//
// Each element is sorted into buckets based on their hash code. The elements in
// each bucket are stored in a red black tree. OpenJDK's HashMap stores all
// their elements in a linked list, and once the number of elements reaches the
// "treeify" threshold of 8, the linked list is converted into a red black tree.
// We just use a red black tree here for simplicity.
//
// By default, the hash structure is instantiated with a capacity of 16 buckets
// and a load factor of 0.75. When the number of elements reaches the threshold
// of `capacity * load factor`, the structure doubles the number of buckets and
// moves over all the elements. This scheme is shamelessly taken from OpenJDK's
// HashMap implementation.
//
#include "data_structure/hash.h"
#include <assert.h>
#include <stdlib.h>

const static unsigned DEFAULT_INITIAL_CAPACITY = 1 << 4;
const static float DEFAULT_LOAD_FACTOR = 0.75;

struct Node {
  void *key;
  void *val;
  Node *next;
  Node *prev;
};

typedef struct {
  Node *n;
  bool found; // is true if we found the node we are looking for, false if it is
              // the last node in the same bucket
} NodeGetRes;

static void validate(Hash *hash) {
  assert(hash && "hash expected to be non-null");
  assert(hash->capacity > 0 && "hash's capacity should always be greater than zero");
  assert(hash->size < hash->capacity * hash->load_factor && "hash should have been resized");
}

void hash_init(Hash *hash, hashfunc_t h, eqfunc_t e) {
  *hash = (Hash){
      .data = calloc(DEFAULT_INITIAL_CAPACITY, sizeof(Node *)),
      .hashfunc = h ? h : ptr_hash,
      .eqfunc = e ? e : ptr_eq,
      .size = 0,
      .capacity = DEFAULT_INITIAL_CAPACITY,
      .load_factor = DEFAULT_LOAD_FACTOR,
  };
}

void hash_free(Hash *hash) {
  validate(hash);
  for (unsigned i = 0; i < hash->capacity; ++i) {
    Node *n = hash->data[i];
    while (n) {
      Node *next = n->next;
      free(n);
      n = next;
    }
  }
  free(hash->data);
}

static unsigned bucket_at(Hash *hash, void *key) {
  unsigned hashcode = hash->hashfunc(key);
  assert(hash->capacity > 0 && "capacity must be greater than zero");
  return hashcode % hash->capacity;
}

static Node *node_at(Hash *hash, void *key) {
  return hash->data[bucket_at(hash, key)];
}

// Doubles the size of hash and copies over data from the old data buffer to the
// new data buffer.
static void hash_resize(Hash *hash) {
  // Initialize a new vector and copy over the nodes. We cannot realloc the
  // data because it would be hard to move nodes to the right place.
  Node **old_data = hash->data;
  unsigned old_capacity = hash->capacity;
  unsigned new_capacity = old_capacity * 2;
  hash->data = calloc(new_capacity, sizeof(Node *));
  hash->size = 0;
  hash->capacity = new_capacity;

  for (unsigned i = 0; i < old_capacity; ++i) {
    Node *n = old_data[i];
    while (n) {
      // TODO: we know the bucket. we can insert the nodes directly instead of
      // allocating new nodes / freeing old nodes.
      hash_insert_pair(hash, n->key, n->val);
      Node *next = n->next;
      free(n);
      n = next;
    }
  }

  validate(hash);
  free(old_data);
}

bool hash_insert(Hash *hash, void *key) {
  return hash_insert_pair(hash, key, NULL);
}

NodeGetRes hash_get_node(Hash *hash, void *key) {
  validate(hash);
  unsigned bucket = bucket_at(hash, key);
  Node *n = hash->data[bucket];
  Node *prev = NULL;
  while (n) {
    if (hash->eqfunc(key, n->key)) {
      return (NodeGetRes){.n = n, .found = true};
    }
    prev = n;
    n = n->next;
  }
  return (NodeGetRes){.n = prev, .found = false};
}

bool hash_insert_pair(Hash *hash, void *key, void *val) {
  validate(hash);
  NodeGetRes res = hash_get_node(hash, key);
  if (res.found) {
    return false;
  }
  Node *new_node = malloc(sizeof(Node));
  *new_node = (Node){.key = key, .val = val, .prev = res.n, .next = NULL};
  if (res.n) {
    res.n->next = new_node;
  } else {
    hash->data[bucket_at(hash, key)] = new_node;
  }
  ++hash->size;
  if (hash->size >= hash->capacity * hash->load_factor) {
    hash_resize(hash);
  }
  return true;
}

void *hash_delete(Hash *hash, void *key) {
  validate(hash);
  NodeGetRes res = hash_get_node(hash, key);
  Node *n = res.n;
  if (!res.found) {
    return NULL;
  }
  if (n->prev) {
    n->prev->next = n->next;
  } else {
    hash->data[bucket_at(hash, key)] = n->next;
  }
  if (n->next) {
    n->next->prev = n->prev;
  }
  void *val = n->val;
  free(n);
  --hash->size;
  return val;
}

void *hash_get(Hash *hash, void *key) {
  validate(hash);
  NodeGetRes res = hash_get_node(hash, key);
  return res.found ? res.n->val : NULL;
}

bool hash_contains(Hash *hash, void *key) {
  validate(hash);
  NodeGetRes res = hash_get_node(hash, key);
  return res.found;
}
