#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include "data_structure/comparator.h"
#include "data_structure/optional.h"

typedef struct RedBlackNode RedBlackNode;

typedef struct {
  RedBlackNode *root;
  unsigned size;
  cmp_t cmp;
} RedBlackTree;

RedBlackTree *rb_tree_new();
RedBlackTree *rb_tree_newc(cmp_t cmp);
void rb_tree_init(RedBlackTree *tree);
void rb_tree_initc(RedBlackTree *tree, cmp_t cmp);
void rb_tree_free(RedBlackTree *tree);
bool rb_tree_insert(RedBlackTree *tree, void *val);
void *rb_tree_delete(RedBlackTree *tree, void *val);
void *rb_tree_get(RedBlackTree *tree, void *val);
bool rb_tree_contains(RedBlackTree *tree, void *val);
Optional rb_tree_min(RedBlackTree *tree);
Optional rb_tree_max(RedBlackTree *tree);
Optional rb_tree_pred(RedBlackTree *tree, void *val);
Optional rb_tree_succ(RedBlackTree *tree, void *val);
void **rb_tree_elements(RedBlackTree *tree);

void rb_tree_validate(RedBlackTree *tree);
void rb_tree_validate_expensive(RedBlackTree *tree);

#endif
