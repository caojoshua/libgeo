// Red black tree. A balanced binary search tree that supports search,
// insertion, and deletion in O(log(n)) time.
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
// This file contains many diagrams with the following labels
// N - corresponds the the variable `n`
// C - N's child
// P - N's parent
// S - N's sibling
// SC - N's sibling's child
// U - N's uncle
// GP - N's grandparent
// GPP - N's great-grandparent
// [a-z] - miscellaneous nodes to show what rotations look like. can be NULL.
//
// Nodes end in `.b` if they are colored black, `.r` if they are
// colored red, or `.db` if they are double black (see "resolve_double_black",
// used for deletion).
//
#include "data_structure/red_black_tree.h"
#include "data_structure/util.h"
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

void rb_tree_init(RedBlackTree *tree) {
  rb_tree_initc(tree, less_than_cmp);
}

void rb_tree_initc(RedBlackTree *tree, cmp_t cmp) {
  *tree = (RedBlackTree){
      .root = nullptr,
      .size = 0,
      .cmp = cmp,
  };
}

void node_free(Node *n) {
  if (!n) {
    return;
  }
  node_free(n->left);
  node_free(n->right);
  free(n);
}

void rb_tree_free(RedBlackTree *tree) {
  rb_tree_validate(tree);
  node_free(tree->root);
}

Direction node_parent_direction(Node *n) {
  assert(n && n->parent);
  if (n->parent->left == n) {
    return LEFT;
  }
  assert(n->parent->right == n && "mismatched parent-child relationship");
  return RIGHT;
}

Node *node_child(Node *n, Direction d) {
  assert(n);
  return d == LEFT ? n->left : n->right;
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

// Rotate about node `n`(N in the diagram). When rotating in n about a
// direction, the child in the opposite direction must not be null, and it will
// become the new parent of `n`. Sets C and the root of the tree if P was
// originally the root.
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
void node_rotate(RedBlackTree *tree, Node *n, Direction d) {
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
    tree->root = child;
    child->parent = NULL;
  }
}

// Node n might be violating red black tree properties. Fixup rotates and
// re-colors the tree to restore correctness of the properties.
//
// After transformations, the labels do not change to demonstrate where nodes
// end up. The colors may change to demonstrate recoloring.
//
void insert_fixup(RedBlackTree *tree, Node *n) {
  assert(n && "cannot fixup null node");
  assert(n->color == RED && "only fixup RED nodes");

  Node *parent = n->parent;
  if (!parent) {
    // if n is the root, color it black and return
    n->color = BLACK;
    tree->root = n;
    return;
  }

  Node *grandparent = parent->parent;
  if (!grandparent || parent->color == BLACK) {
    // If parent is the root or the parent is black, no fixups are needed
    return;
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
    insert_fixup(tree, grandparent);
    return;
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
    node_rotate(tree, parent, grandparent_direction);
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
  node_rotate(tree, grandparent, opposite_direction(grandparent_direction));

  grandparent->color = RED;
  parent->color = BLACK;
  n->color = RED;
}

bool node_insert(RedBlackTree *tree, Node *n, void *val) {
  assert(n && "RBTree: cannot insert to null node");

  Ordering comparison = tree->cmp(val, n->val);
  if (comparison == EQUALS) {
    // Element already exists. Nothing to insert.
    return false;
  }

  Direction direction = comparison == LESS ? LEFT : RIGHT;
  Node **child = direction == LEFT ? &n->left : &n->right;

  if (*child) {
    // Child node exists. Continue recursing into the child.
    return node_insert(tree, *child, val);
  }

  // Child does not exist. Insert the node and fixup.
  *child = node_new(val, RED);
  node_adopt(n, *child, direction);
  tree->size += 1;
  insert_fixup(tree, *child);
  return true;
}

bool rb_tree_insert(RedBlackTree *tree, void *val) {
  rb_tree_validate(tree);
  if (!tree->root) {
    tree->root = node_new(val, BLACK);
    tree->size = 1;
    return true;
  }
  return node_insert(tree, tree->root, val);
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

// A double black is a node that counts for 2 black nodes in a path. We create a
// double black node when we delete a black node. Deleting a black node can
// violate the property that all paths have an equal number of black nodes.
void resolve_double_black(RedBlackTree *tree, Node *n) {
  assert(n);
  assert(n->color == BLACK && "double black node must be colored black");

  Node *parent = n->parent;
  if (!parent) {
    // If n is the root, we know the double black is resolved
    tree->root = n;
    return;
  }

  Direction parent_direction = node_parent_direction(n);
  Direction opposite = opposite_direction(parent_direction);
  Node *sibling = node_sibling(n);

  // Case 1: sibling is red
  //     P.b
  //    /   \
  //   S.r   N.db
  //  /  \  /  \
  // a   b  c   d
  //
  // right rotate about P, and recolor P and S:
  //     S.b
  //    /   \
  //   a    P.r
  //        /  \
  //       b    N.db
  //           /  \
  //          c    d
  //
  // And repeat resolving double black for N. Note that `b` is guaranteed to be
  // black since it was originally a child of a red node. Since `b` becomes the
  // new sibling of `N`, we can resolve double black with one of case 2 or 3.
  if (sibling->color == RED) {
    node_rotate(tree, parent, parent_direction);
    sibling->color = BLACK;
    parent->color = RED;
    sibling = node_sibling(n);
    assert(sibling && sibling->color == BLACK);
  }

  // Case 2: sibling has a red child
  Node *sibling_outer_child = node_child(sibling, opposite);
  if (sibling_outer_child && sibling_outer_child->color == RED) {
    // Case 2.a: The outer child is red
    //     P.r
    //    /   \
    //   S.b   N.db
    //  /   \
    // SC.r  a
    //
    // After rotations/recoloring:
    //     S.r
    //    /   \
    //   SC.b  P.b
    //         / \
    //        a   N.b
    //
    // In the before diagram, P can be red or black. S is assigned P's initial
    // color.
    node_rotate(tree, parent, parent_direction);
    sibling->color = parent->color;
    parent->color = BLACK;
    sibling_outer_child->color = BLACK;
    return;
  }

  Node *sibling_inner_child = node_child(sibling, parent_direction);
  if (sibling_inner_child && sibling_inner_child->color == RED) {
    // Case 2.b: The inner child is red, and the outer child is not red
    //     P.b
    //    /   \
    //   S.b   N.db
    //     \
    //    SC.r
    //
    // After rotating about sibling, we can resolve double black for N with
    // case 2.a
    //     P.b
    //    /   \
    //  SC.b   N.db
    //  /
    // S.r
    node_rotate(tree, sibling, opposite);
    sibling->color = RED;
    sibling_inner_child->color = BLACK;

    // Recursive call always resolves to case 2.a
    resolve_double_black(tree, n);
    return;
  }

  // Case 3: sibling is black and sibling's children are both black
  assert(sibling->color == BLACK &&
         ((!sibling->left && !sibling->right) ||
          (sibling->left->color == BLACK && sibling->right->color == BLACK)) &&
         "sibling should be a black node with only black children");
  sibling->color = RED;
  if (parent->color == RED) {
    // Case 3.a: parent is red.
    //     P.r
    //    /   \
    //   S.b   N.db
    //
    // Recolors into
    //     P.b
    //    /   \
    //   S.r   N.b
    parent->color = BLACK;
  } else {
    // Case 3.b: parent is black.
    //     P.b
    //    /   \
    //   S.b   N.db
    //
    // Recolors into
    //     P.db
    //    /   \
    //   S.b   N.b
    resolve_double_black(tree, parent);
  }
}

void node_delete(RedBlackTree *tree, Node *n) {
  assert(n);
  // Case 1: n only has one child. Delete n and promote the child.
  if (n->left && !n->right || !n->left && n->right) {
    // Case 1.a: N is red and the child is black. Promote the child.
    // N.r
    //  \
    //  C.b
    //  / \
    // a   b
    //
    // After promotion:
    //   C.b
    //  /   \
    // a     b
    //
    Node *single_child = n->left ? n->left : n->right;
    if (n->parent) {
      node_adopt(n->parent, single_child, node_parent_direction(n));
    } else {
      // n is the root node. promote its child as the new root.
      single_child->parent = NULL;
      tree->root = single_child;
    }

    if (n->color == BLACK) {
      if (single_child->color == RED) {
        // Case 1.b: N is black and child is red. Promote the child and color it
        // black.
        // N.b
        //  \
        //  C.r
        //  / \
        // a   b
        //
        // After promotion:
        //   C.b
        //  /   \
        // a     b
        single_child->color = BLACK;
      } else {
        // Case 1.c: N is black and child is black. Promote the child. This
        // invalidates the property that all paths should have the same number
        // of black nodes. We treat C as a double black and resolve it.
        // N.b
        //  \
        //  C.b
        //  / \
        // a   b
        //
        // After promotion:
        //   C.db
        //  /   \
        // a     b
        resolve_double_black(tree, single_child);
      }
    }
    free(n);
    return;
  }

  // Case 2: n has no children. If n is red, we can delete it without violating
  // properties. If n is black, we treat it as a double black, resolve it, and
  // then delete the node.
  if (!n->left && !n->right) {
    if (n->color == BLACK) {
      resolve_double_black(tree, n);
    }
    // After double black is resolved, we can safely delete the node without
    // violating any properties.
    if (n == tree->root) {
      tree->root = NULL;
    } else {
      if (node_parent_direction(n) == LEFT) {
        n->parent->left = NULL;
      } else {
        n->parent->right = NULL;
      }
    }
    free(n);
    return;
  }

  // Case 3: n has two children
  // n's predecessor is the rightmost node of n's left subtree
  // First replace the entries of n and its inorder predecessor. By replacing
  // only the entries, we maintain parent-child relationships and colors. Note
  // that pred is not guaranteed to be a right child because it could be the
  // left child of n.
  //
  // After swapping, we recursively delete n. Since n is swapped into a
  // rightmost place, it cannot have any right children, so the recursive call
  // will be handled with case 1 or 2.
  //
  //     N
  //    /
  //   a
  //  / \
  // b   c
  //
  // After swapping:
  //     c
  //    /
  //   a
  //  / \
  // b   N
  //
  assert(n->left && n->right);
  Node *pred = node_rightmost(n->left);
  assert(pred->parent &&
         "n's predecessor cannot be the root and must have a parent");

  swap(&n->val, &pred->val);

  node_delete(tree, pred);
}

void *rb_tree_delete(RedBlackTree *tree, void *val) {
  rb_tree_validate(tree);
  Node *n = node_search(tree->root, val, tree->cmp);
  if (!n) {
    return NULL;
  }
  tree->size -= 1;
  void *deleted_val = n->val;
  node_delete(tree, n);
  return deleted_val;
}

void *rb_tree_get(RedBlackTree *tree, void *val) {
  rb_tree_validate(tree);
  Node *n = node_search(tree->root, val, tree->cmp);
  return n ? n->val : NULL;
}

bool rb_tree_contains(RedBlackTree *tree, void *val) {
  rb_tree_validate(tree);
  return node_search(tree->root, val, tree->cmp);
}

Optional rb_tree_min(RedBlackTree *tree) {
  rb_tree_validate(tree);
  if (!tree->root) {
    return optional_null();
  }
  return optional(node_leftmost(tree->root)->val);
}

Optional rb_tree_max(RedBlackTree *tree) {
  rb_tree_validate(tree);
  if (!tree->root) {
    return optional_null();
  }
  return optional(node_rightmost(tree->root)->val);
}

// Find the nearest parent of n that is a predecessor/successor of val.
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

Optional rb_tree_pred(RedBlackTree *tree, void *val) {
  rb_tree_validate(tree);
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

Optional rb_tree_succ(RedBlackTree *tree, void *val) {
  rb_tree_validate(tree);
  if (!tree->root) {
    return optional_null();
  }
  return node_succ(tree->root, val, tree->cmp);
}

void **rb_tree_elements(RedBlackTree *tree) {
  rb_tree_validate(tree);
  if (tree->size == 0) {
    return NULL;
  }
  void **elements = malloc(tree->size * sizeof(void *));
  void **elements_iter = elements;
  Optional iter = rb_tree_min(tree);
  assert(iter.present && "tree with size zero should have a minimum");
  do {
    *elements_iter = iter.val;
    ++elements_iter;
    iter = rb_tree_succ(tree, iter.val);
  } while (iter.present);
  return elements;
}

// Validates property 3 as listed at the top of the file, binary tree
// ordering properties, and that the tree is balanced.
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
    assert(order == GREATER && "parent node must be >= left child node");
    left_height = 1 + node_validate(n->left, cmp);
  }
  if (n->right) {
    Ordering order = cmp(n->val, n->right->val);
    assert(order == LESS && "parent node must be <= right child node");
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
  assert(taller_height <= shorter_height * 2 + 1 &&
         "taller subtree's height should be bounded by twice the height of "
         "the shorter subtree's height");

  return MAX(left_height, right_height);
}

// Validates tree. Cannot be called in the middle of tree transformations i.e
// insertion.
void rb_tree_validate(RedBlackTree *tree) {
  assert(tree);
  if (tree->root) {
    assert(!tree->root->parent && "root's parent should be NULL");
    assert(tree->root->color == BLACK && "root's color should be black");
  } else {
    assert(tree->size == 0);
  }
}

void rb_tree_validate_expensive(RedBlackTree *tree) {
  rb_tree_validate(tree);
  if (tree->root) {
    node_validate(tree->root, tree->cmp);
  }
}
