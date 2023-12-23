#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

typedef bool (*cmp_t)(void *, void *);
static bool less_than_cmp(void *a, void *b) { return a < b; }

typedef struct RedBlackNode RedBlackNode;

typedef struct {
  RedBlackNode *root;
  unsigned size;
  cmp_t cmp;
} RedBlackTree;

RedBlackTree *red_black_tree_new();
void red_black_tree_insert(RedBlackTree *tree, void *val);
bool red_black_tree_contains(RedBlackTree *tree, void *val);
void red_black_tree_validate(RedBlackTree *tree);
void red_black_tree_validate_expensive(RedBlackTree *tree);

#endif
