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
#include "data_structure/red_black_tree.h"
#include <assert.h>
#include <stdlib.h>

const static unsigned DEFAULT_INITIAL_CAPACITY = 1 << 4;
const static float DEFAULT_LOAD_FACTOR = 0.75;

typedef struct {
  void *key;
  void *val;
} Node;

Ordering node_cmp(void *a, void *b) {
  assert(a && b && "cannot compare null nodes");
  return less_than_cmp(((Node *)a)->key, ((Node *)b)->key);
}

static void validate(Hash *hash) {
  assert(hash && "hash expected to be non-null");
  assert(hash->data && "hash's data should always be initialized");
  assert(hash->capacity > 0 && "hash's capacity should always be greater than zero");
  assert(hash->size < hash->capacity * hash->load_factor && "hash should have been resized");
}

static Node *node_new(void *key, void *pair) {
  Node *n = malloc(sizeof(Node));
  *n = (Node){
      .key = key,
      .val = pair,
  };
  return n;
}

// Used for temporary nodes that will not be inserted i.e. search, deletion.
Node key_node_new(void *key) { return (Node){.key = key}; }

Hash *hash_new() { return hash_newf(pointer_hashfunc); }

Hash *hash_newf(hashfunc_t f) {
  Hash *hash = malloc(sizeof(Hash));
  *hash = (Hash){
      .data = calloc(sizeof(RedBlackTree *), DEFAULT_INITIAL_CAPACITY),
      .hashfunc = f,
      .size = 0,
      .capacity = DEFAULT_INITIAL_CAPACITY,
      .load_factor = DEFAULT_LOAD_FACTOR,
  };
  return hash;
}

void hash_free(Hash *hash) {
  validate(hash);
  for (unsigned i = 0; i < hash->capacity; ++i) {
    RedBlackTree *tree = hash->data[i];
    if (tree) {
      Node **elements = (Node **)red_black_tree_elements(tree);
      for (unsigned i = 0; i < tree->size; ++i) {
        free(elements[i]);
      }
      free(elements);
      red_black_tree_free(tree);
    }
  }
  free(hash->data);
  free(hash);
}

static unsigned bucket_at(Hash *hash, void *key) {
  unsigned hashcode = hash->hashfunc(key);
  assert(hash->capacity > 0 && "capacity must be greater than zero");
  return hashcode % hash->capacity;
}

static RedBlackTree *tree_at(Hash *hash, void *key) {
  return hash->data[bucket_at(hash, key)];
}

// Doubles the size of hash and copies over data from the old data buffer to the
// new data buffer.
static void hash_resize(Hash *hash) {
  // Initialize a new vector and copy over the nodes. We cannot realloc the
  // data because it would be hard to move nodes to the right place.
  RedBlackTree **old_data = hash->data;
  unsigned old_capacity = hash->capacity;
  unsigned new_capacity = old_capacity * 2;
  hash->data = calloc(sizeof(RedBlackTree *), new_capacity);
  hash->size = 0;
  hash->capacity = new_capacity;

  for (unsigned i = 0; i < old_capacity; ++i) {
    RedBlackTree *tree = old_data[i];
    if (!tree) {
      continue;
    }
    Node **elements = (Node **)red_black_tree_elements(tree);
    for (unsigned i = 0; i < tree->size; ++i) {
      Node *n = elements[i];
      hash_insert_pair(hash, n->key, n->val);
      free(n);
    }
    free(elements);
    red_black_tree_free(tree);
  }

  validate(hash);
  free(old_data);
}

bool hash_insert(Hash *hash, void *key) {
  return hash_insert_pair(hash, key, NULL);
}

bool hash_insert_pair(Hash *hash, void *key, void *val) {
  validate(hash);
  unsigned bucket = bucket_at(hash, key);
  RedBlackTree *tree = hash->data[bucket];
  if (!tree) {
    tree = red_black_tree_newc(node_cmp);
    hash->data[bucket] = tree;
  }
  if (red_black_tree_insert(tree, node_new(key, val))) {
    hash->size += 1;
    if (hash->size >= hash->capacity * hash->load_factor) {
      hash_resize(hash);
    }
    return true;
  }
  return false;
}

void *hash_delete(Hash *hash, void *key) {
  validate(hash);
  RedBlackTree *tree = tree_at(hash, key);
  if (!tree) {
    return NULL;
  }
  Node key_node = key_node_new(key);
  Node *deleted_node = red_black_tree_delete(tree, &key_node);
  if (deleted_node) {
    void *val = deleted_node->val;
    free(deleted_node);
    hash->size -= 1;
    return val;
  }
  return NULL;
}

bool hash_contains(Hash *hash, void *key) {
  validate(hash);
  RedBlackTree *tree = tree_at(hash, key);
  if (!tree) {
    return false;
  }
  Node key_node = key_node_new(key);
  return red_black_tree_contains(tree, &key_node);
}
