// Red black tree. A balanced binary search tree that supports search,
// insertion, and deletion(TODO) in O(log(n)) time.
//
// Each node is colored either red or black. For a red black tree, the following
// properties must hold:
// 1. The root is black
// 2. Leaf nodes are black
// 3. The children of a red node are black
// 4. All leaf nodes have the same black depth
//
// Inserted nodes are always red. After tree transformations, the tree might not
// hold the properties anymore. The implementation is responsible for
// re-coloring and rotating nodes such that the properties hold.
//
// Red black trees and AVL trees have the same space complexity for the data
// structure, and time complexity for the major operations listed above. The
// advantage of red black trees is that they tend to have less rotations that do
// not need to bubble up the tree as much. This results in faster insertions and
// deletions.
//
// AVL trees have stronger balancing than red black trees. AVL trees hold
// that for a given node, the difference in heights of the left and right
// subtree is <= 1.
//
// In red black trees, for a given node, one of subtrees can be at most twice as
// large as the other. In this worst case, its shorter subtree's max height path
// is only black nodes, and the other subtree's max height path has alternating
// black and red nodes. Properties 3 and 4 bound this worst case.
// black and red nodes. Properties 3 and 4 bound this worst case. Despite this
// difference, the worst case complexity of height is the same between red black
// trees and AVL trees.
//
#include "data_structure/red_black_tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

typedef enum {
  BLACK,
  RED,
} Color;

typedef enum { LEFT, RIGHT } Direction;

struct RedBlackNode {
  void *val;
  Color color;
  RedBlackNode *parent;
  RedBlackNode *left;
  RedBlackNode *right;
};

typedef RedBlackNode Node;

Direction opposite_direction(Direction d) {
  if (d == LEFT) {
    return RIGHT;
  }
  assert(d == RIGHT && "direction must be LEFT or RIGHT");
  return LEFT;
}

Node *node_new(void *val, Color color) {
  Node *n = malloc(sizeof(Node));
  *n = (Node){
      .val = val,
      .color = color,
      .parent = NULL,
      .left = NULL,
      .right = NULL,
  };
  return n;
}

RedBlackTree *red_black_tree_new() {
  RedBlackTree *tree = malloc(sizeof(RedBlackTree));
  *tree = (RedBlackTree){
      .root = nullptr,
      .size = 0,
      .cmp = less_than_cmp,
  };
  return tree;
}

Direction node_parent_direction(Node *n) {
  assert(n && n->parent);
  if (n->parent->left == n) {
    return LEFT;
  }
  assert(n->parent->right == n && "mismatched parent-child relationship");
  return RIGHT;
}

Node *node_sibling(Node *n) {
  return node_parent_direction(n) == LEFT ? n->parent->right : n->parent->left;
}

void node_adopt(Node *parent, Node *child, Direction direction) {
  assert(parent && "null parent cannot adopt");
  if (direction == LEFT) {
    parent->left = child;
  } else {
    parent->right = child;
  }
  if (child) {
    child->parent = parent;
  }
}

// Rotate about node `n`(N in the diagram). When rotating in n about a direction,
// the child in the opposite direction must not be null, and it will become the
// new parent of `n`.
//
// Example left rotation about N
// P
//  \
//   N
//  / \
// a   C
//    / \
//   b   c
//
// Rotates into:
//   P
//    \
//     C
//    / \
//   N   c
//  / \
// a   b
//
void node_rotate(Node *n, Direction d) {
  assert(n);
  Node *parent = n->parent;
  Direction parent_direction;
  if (parent) {
    parent_direction = node_parent_direction(n);
  }

  Node *child;
  if (d == LEFT) {
    child = n->right;
    assert(child &&
           "When rotating about `n` left, `n`'s right child must exist");
    node_adopt(n, child->left, RIGHT);
  } else {
    child = n->left;
    assert(child &&
           "When rotating about `n` right, `n`'s left child must exist");
    node_adopt(n, child->right, LEFT);
  }

  node_adopt(child, n, d);
  if (parent) {
    node_adopt(parent, child, parent_direction);
  } else {
    child->parent = NULL;
  }
}

// Node n might be violating red black tree properties. Fixup rotates and
// re-colors the tree to restore correctness of the properties.
//
// This function contains diagrams with the following labels:
// N - The node we are fixing up
// P - N's parent
// U - N's uncle
// GP - N's grandparent
// GPP - N's great-grandparent
// [a-z] - miscellaneous nodes to show what rotations look like. can be NULL.
//
// Uppercase nodes end in `.b` if they are colored black, or `.r` if they are
// colored red.
//
// After transformations, the labels do not change to demonstrate where nodes
// end up. The colors may change to demonstrate recoloring.
//
Node *insert_fixup(Node *n) {
  assert(n && "cannot fixup null node");
  assert(n->color == RED && "only fixup RED nodes");

  Node *parent = n->parent;
  if (!parent) {
    // if n is the root, color it black and return
    n->color = BLACK;
    return n;
  }

  Node *grandparent = parent->parent;
  if (!grandparent || parent->color == BLACK) {
    // If parent is the root or the parent is black, no fixups are needed
    return NULL;
  }

  // Recolor if uncle is red. No rotations are needed.
  //   GP.b
  //  /  \
  // U.r  P.r
  //     /
  //    N.r
  //
  // Transforms into:
  //   GP.r
  //  /  \
  // U.b  P.b
  //     /
  //    N.r
  //
  // Then we need to fixup GP because its parent might be red.
  Node *uncle = node_sibling(parent);
  if (uncle && uncle->color == RED) {
    grandparent->color = RED;
    parent->color = BLACK;
    uncle->color = BLACK;
    return insert_fixup(grandparent);
  }

  // GP, P and N form a triangle. Transform it into a line. It then undergoes
  // the straight line transformation.
  //   GP.b
  //  /  \
  // U.b  P.r
  //     / \
  //    N.r c
  //   / \
  //  a   b
  //
  // Rotates into (no color changes):
  //   GP.b
  //  /  \
  // U.b  N.r
  //     / \
  //    a   P.r
  //       / \
  //      b   c
  Direction parent_direction = node_parent_direction(n);
  Direction grandparent_direction = node_parent_direction(parent);
  if (parent_direction != grandparent_direction) {
    // If directions are opposite, then it is a triangle
    node_rotate(parent, grandparent_direction);
    // Swap n and parent in preparation for the straight line transformation
    Node *t = n;
    n = parent;
    parent = t;
  }

  // GP, P, and N form a straight line.
  // GPP
  //  \
  //   GP.b
  //  /  \
  // U.b  P.r
  //     / \
  //    a  N.r
  //
  // Rotates into:
  //   GPP
  //     \
  //      P.b
  //     /  \
  //    GP.r  N.r
  //   / \
  // U.b  a
  //
  node_rotate(grandparent, opposite_direction(grandparent_direction));

  grandparent->color = RED;
  parent->color = BLACK;
  n->color = RED;

  // Return parent if it is the new root node
  return parent->parent ? NULL : parent;
}

Node *node_insert(Node *n, void *val, cmp_t cmp) {
  assert(n && "RBTree: cannot insert to null node");
  Direction direction = cmp(val, n->val) == LESS ? LEFT : RIGHT;
  Node **child = direction == LEFT ? &n->left : &n->right;

  if (*child) {
    // Child node exists. Continue recursing into the child.
    return node_insert(*child, val, cmp);
  }

  // Child does not exist. Insert the node and fixup.
  *child = node_new(val, RED);
  node_adopt(n, *child, direction);
  return insert_fixup(*child);
}

void red_black_tree_insert(RedBlackTree *tree, void *val) {
  red_black_tree_validate(tree);
  tree->size += 1;
  if (!tree->root) {
    tree->root = node_new(val, BLACK);
    return;
  }
  Node *new_root = node_insert(tree->root, val, tree->cmp);
  if (new_root) {
    tree->root = new_root;
  }
}

Node *node_search(Node *n, void *val, cmp_t cmp) {
  if (!n) {
    return NULL;
  }
  if (cmp(val, n->val) == EQUALS) {
    return n;
  }
  Node *nextn = cmp(val, n->val) == LEFT ? n->left : n->right;
  return node_search(nextn, val, cmp);
}

bool red_black_tree_contains(RedBlackTree *tree, void *val) {
  red_black_tree_validate(tree);
  return node_search(tree->root, val, tree->cmp);
}

Node *node_leftmost(Node *n) {
  assert(n);
  while (n->left) {
    n = n->left;
  }
  return n;
}

// Finds the right most node in the subtree rooted at n
Node *node_rightmost(Node *n) {
  assert(n);
  while (n->right) {
    n = n->right;
  }
  return n;
}

Optional red_black_tree_min(RedBlackTree *tree) {
  red_black_tree_validate(tree);
  if (!tree->root) {
    return optional_null();
  }
  return optional(node_leftmost(tree->root)->val);
}

Optional red_black_tree_max(RedBlackTree *tree) {
  red_black_tree_validate(tree);
  if (!tree->root) {
    return optional_null();
  }
  return optional(node_rightmost(tree->root)->val);
}

// Find the nearest parent of n that is a predecessor/successor of val.
// TODO: this is weird with duplicates values. should just not support it.
Optional node_parent_predsucc(Node *n, void *val, cmp_t cmp, Ordering ord) {
  n = n->parent;
  while (n) {
    if (cmp(n->val, val) == ord) {
      return optional(n->val);
    }
    n = n->parent;
  }
  return optional_null();
}

Optional node_pred(Node *n, void *val, cmp_t cmp) {
  assert(n && "cannot find pred from null node");

  Ordering comparison = cmp(val, n->val);
  if (comparison == EQUALS) {
    // Found the node for val
    if (n->left) {
      // n has a left node. The pred is the rightmost node in the
      // subtree rooted at the left subnode.
      return optional(node_rightmost(n->left)->val);
    }
    // n does not have a left node. Find the nearest parent that is a pred.
    return node_parent_predsucc(n, val, cmp, LESS);
  }

  if (comparison == LESS) {
    if (n->left) {
      return node_pred(n->left, val, cmp);
    }
    // Child does not exist. Find the nearest parent that is a pred.
    return node_parent_predsucc(n, val, cmp, LESS);
  }

  // comparison == GREATER
  if (n->right) {
    return node_pred(n->right, val, cmp);
  }
  // Child does not exist. The pred is n itself
  return optional(n->val);
}

Optional red_black_tree_pred(RedBlackTree *tree, void *val) {
  red_black_tree_validate(tree);
  if (!tree->root) {
    return optional_null();
  }
  return node_pred(tree->root, val, tree->cmp);
}

Optional node_succ(Node *n, void *val, cmp_t cmp) {
  assert(n && "cannot find succ from null node");

  Ordering comparison = cmp(val, n->val);
  if (comparison == EQUALS) {
    // Found the node for val
    if (n->right) {
      // n has a left node. The succ is the leftmost node in the
      // subtree rooted at the left subnode.
      return optional(node_leftmost(n->right)->val);
    }
    // n does not have a left node. Find the nearest parent that is a succ.
    return node_parent_predsucc(n, val, cmp, GREATER);
  }

  if (comparison == GREATER) {
    if (n->right) {
      return node_succ(n->right, val, cmp);
    }
    // Child does not exist. Find the nearest parent that is a succ.
    return node_parent_predsucc(n, val, cmp, GREATER);
  }

  // comparison == LESS
  if (n->left) {
    return node_succ(n->left, val, cmp);
  }
  // Child does not exist. The succ is n itself
  return optional(n->val);
}

Optional red_black_tree_succ(RedBlackTree *tree, void *val) {
  red_black_tree_validate(tree);
  if (!tree->root) {
    return optional_null();
  }
  return node_succ(tree->root, val, tree->cmp);
}

// Validates property 3 as listed at the top of the file, binary tree
// ordering properties, and that the tree is balanced.
// TODO: We allow equivalent nodes. C++'s sets are just RBTrees that do not
// allow equivalent elements. Need to think about our use cases.
unsigned node_validate(Node *n, cmp_t cmp) {
  assert(n && "cannot validate null node");
  if (n->color == RED) {
    assert(!n->left ||
           n->left->color == BLACK && "red node's left child should be black");
    assert(!n->right || n->right->color == BLACK &&
                            "red node's right child should be black");
  }

  unsigned left_height = 0;
  unsigned right_height = 0;
  if (n->left) {
    Ordering order = cmp(n->val, n->left->val);
    assert((order == GREATER || order == EQUALS) &&
           "parent node must be >= left child node");
    left_height = 1 + node_validate(n->left, cmp);
  }
  if (n->right) {
    Ordering order = cmp(n->val, n->right->val);
    assert(order == LESS ||
           order == EQUALS && "parent node must be <= right child node");
    right_height = 1 + node_validate(n->right, cmp);
  }

  unsigned shorter_height;
  unsigned taller_height;
  if (left_height > right_height) {
    shorter_height = right_height;
    taller_height = left_height;
  } else {
    shorter_height = left_height;
    taller_height = right_height;
  }
  assert(taller_height + 1 <= (shorter_height + 1) * 2 &&
         "taller subtree's height should be bounded by twice the height of "
         "the shorter subtree's height");

  return MAX(left_height, right_height);
}

// Validates tree. Cannot be called in the middle of tree transformations i.e
// insertion.
void red_black_tree_validate(RedBlackTree *tree) {
  assert(tree);
  if (tree->root) {
    assert(!tree->root->parent && "root's parent should be NULL");
    assert(tree->root->color == BLACK && "root's color should be black");
  } else {
    assert(tree->size == 0);
  }
}

void red_black_tree_validate_expensive(RedBlackTree *tree) {
  red_black_tree_validate(tree);
  if (tree->root) {
    node_validate(tree->root, tree->cmp);
  }
}
