extern "C" {
#include "data_structure/red_black_tree.h"
#include "data_structure/vector.h"
}
#include <gtest/gtest.h>

static const unsigned STRIDE = 5;

void test_contains_range(RedBlackTree *tree, unsigned n) {
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_TRUE(red_black_tree_contains(tree, (void *)i));
    if (i > 0) {
      // Test predecessor
      Optional pred = red_black_tree_pred(tree, (void *)i);
      ASSERT_TRUE(pred.present);
      ASSERT_EQ(pred.val, (void *)(i - 1));
    }
    if (i > 0 && i < n - 1) {
      // Test successor
      Optional succ = red_black_tree_succ(tree, (void *)i);
      ASSERT_TRUE(succ.present);
      ASSERT_EQ(succ.val, (void *)(i + 1));
    }
  }

  EXPECT_FALSE(red_black_tree_contains(tree, (void *)-1));
  EXPECT_FALSE(red_black_tree_contains(tree, (void *)(long)(n)));

  EXPECT_FALSE(red_black_tree_pred(tree, NULL).present);
  Optional pred = red_black_tree_pred(tree, (void *)(long)(n));
  EXPECT_TRUE(pred.present);
  EXPECT_EQ(pred.val, (void *)(long)(n - 1));
}

void rb_tree_test_increasing(unsigned n) {
  RedBlackTree *tree = red_black_tree_new();
  for (unsigned long i = 0; i < n; ++i) {
    red_black_tree_insert(tree, (void *)i);
    ASSERT_EQ(tree->size, i + 1);
  }
  test_contains_range(tree, n);
  red_black_tree_validate_expensive(tree);
}

void rb_tree_test_decreasing(unsigned n) {
  RedBlackTree *tree = red_black_tree_new();
  for (unsigned long i = n; i > 0; --i) {
    red_black_tree_insert(tree, (void *)(i - 1));
    ASSERT_EQ(tree->size, n - i + 1);
  }
  test_contains_range(tree, n);
  red_black_tree_validate_expensive(tree);
}

void test_contains_range_stride(RedBlackTree *tree, unsigned n) {
  EXPECT_FALSE(red_black_tree_pred(tree, NULL).present);
  for (unsigned long i = STRIDE; i < n; i += STRIDE) {
    ASSERT_TRUE(red_black_tree_contains(tree, (void *)i));
    for (unsigned j = i + 1; j <= i + STRIDE; ++j) {
      Optional pred = red_black_tree_pred(tree, (void *)(long)j);
      ASSERT_TRUE(pred.present);
      ASSERT_EQ(pred.val, (void *)(i));
    }
  }
  EXPECT_FALSE(red_black_tree_contains(tree, (void *)-1));
  EXPECT_FALSE(red_black_tree_contains(tree, (void *)(long)(n + STRIDE)));
  if (n > STRIDE) {
    Optional pred = red_black_tree_pred(tree, (void *)(long)(n + STRIDE));
    ASSERT_TRUE(pred.present);
  }
}

void rb_tree_test_increasing_stride(unsigned n) {
  RedBlackTree *tree = red_black_tree_new();
  for (unsigned long i = STRIDE; i < n; i += STRIDE) {
    red_black_tree_insert(tree, (void *)i);
  }
  test_contains_range_stride(tree, n);
  red_black_tree_validate_expensive(tree);
}

void rb_tree_test_random(unsigned n) {
  srand(0);
  RedBlackTree *tree = red_black_tree_new();
  Vector *numbers = vector_newn(n);
  for (unsigned i = 0; i < n; ++i) {
    void *r = (void *)(long)rand();
    red_black_tree_insert(tree, (void *)(long)r);
    vector_push(numbers, r);
    red_black_tree_validate_expensive(tree);
    ASSERT_EQ(tree->size, i + 1);
  }
  for (unsigned i = 0; i < n; ++i) {
    ASSERT_TRUE(red_black_tree_contains(tree, numbers->data[i]));
  }
  red_black_tree_validate_expensive(tree);
}

void rb_tree_test_length(unsigned n) {
  rb_tree_test_increasing(n);
  rb_tree_test_decreasing(n);
  rb_tree_test_increasing_stride(n);
  rb_tree_test_random(n);
}

TEST(RedBlackTree, TriangleRotationRight) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, (void *)1);
  test_contains_range(tree, 3);
}

TEST(RedBlackTree, TriangleRotationLeft) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)1);
  test_contains_range(tree, 3);
}

TEST(RedBlackTree, RecolorRight) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, (void *)1);
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, (void *)3);
  test_contains_range(tree, 4);
}

TEST(RedBlackTree, RecolorLeft) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, (void *)3);
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)1);
  test_contains_range(tree, 4);
}

TEST(RedBlackTree, Length1) { rb_tree_test_length(1); }
TEST(RedBlackTree, Length2) { rb_tree_test_length(2); }
TEST(RedBlackTree, StraightRotation) { rb_tree_test_length(3); }
TEST(RedBlackTree, Length4) { rb_tree_test_length(4); }
TEST(RedBlackTree, Length5) { rb_tree_test_length(5); }
TEST(RedBlackTree, Length6) { rb_tree_test_length(6); }
TEST(RedBlackTree, Length8) { rb_tree_test_length(8); }
TEST(RedBlackTree, Length16) { rb_tree_test_length(16); }
TEST(RedBlackTree, Length128) { rb_tree_test_length(128); }
TEST(RedBlackTree, Length1234) { rb_tree_test_length(1234); }
TEST(RedBlackTree, LengthBar) { rb_tree_test_length(0xBA5); }
TEST(RedBlackTree, LengthFoo) { rb_tree_test_length(0xF00); }
