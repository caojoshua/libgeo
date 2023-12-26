extern "C" {
#include "data_structure/red_black_tree.h"
#include "data_structure/vector.h"
}
#include <gtest/gtest.h>

static const unsigned STRIDE = 5;

static void test_get_true(RedBlackTree *tree, unsigned long i) {
  ASSERT_TRUE(red_black_tree_contains(tree, (void *)i));
  ASSERT_EQ(red_black_tree_get(tree, (void *)i), (void *)i);
}

static void test_get_false(RedBlackTree *tree, long i) {
  ASSERT_FALSE(red_black_tree_contains(tree, (void *)i));
  ASSERT_FALSE(red_black_tree_get(tree, (void *)i));
  ASSERT_FALSE(red_black_tree_delete(tree, (void *)i));
}

void test_contains_range(RedBlackTree *tree, unsigned n) {
  ASSERT_GT(n, 0);
  EXPECT_EQ(tree->size, n);
  void **elements = red_black_tree_elements(tree);
  for (unsigned long i = 0; i < n; ++i) {
    test_get_true(tree, i);
    ASSERT_EQ((long)elements[i], i);
    if (i > 0) {
      Optional pred = red_black_tree_pred(tree, (void *)i);
      ASSERT_TRUE(pred.present);
      ASSERT_EQ((long)pred.val, (i - 1));
    }
    if (i < n - 1) {
      Optional succ = red_black_tree_succ(tree, (void *)i);
      ASSERT_TRUE(succ.present);
      ASSERT_EQ((long)succ.val, (i + 1));
    }
  }
  free(elements);

  test_get_false(tree, -1);
  test_get_false(tree, n);

  Optional min = red_black_tree_min(tree);
  EXPECT_TRUE(min.present);
  EXPECT_EQ((long)min.val, 0);
  Optional max = red_black_tree_max(tree);
  EXPECT_TRUE(max.present);
  EXPECT_EQ((long)max.val, n - 1);

  EXPECT_FALSE(red_black_tree_pred(tree, NULL).present);
  Optional pred = red_black_tree_pred(tree, (void *)(long)(n));
  EXPECT_TRUE(pred.present);
  EXPECT_EQ((long)pred.val, (long)(n - 1));
}

void test_delete_range(RedBlackTree *tree, unsigned n) {
  ASSERT_GT(n, 0);
  for (unsigned long i = 0; i < n; i += STRIDE) {
    ASSERT_EQ(red_black_tree_delete(tree, (void *)i), (void *)i);
  }
  test_get_false(tree, n);
  for (unsigned i = 0; i < n; ++i) {
    if (i % STRIDE == 0) {
      test_get_false(tree, i);
    } else {
      test_get_true(tree, i);
    }
  }
}

void rb_tree_test_increasing(unsigned n) {
  RedBlackTree *tree = red_black_tree_new();
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_TRUE(red_black_tree_insert(tree, (void *)i));
    ASSERT_EQ(tree->size, i + 1);
  }
  test_contains_range(tree, n);
  test_delete_range(tree, n);
  red_black_tree_validate_expensive(tree);
  red_black_tree_free(tree);
}

void rb_tree_test_decreasing(unsigned n) {
  RedBlackTree *tree = red_black_tree_new();
  for (unsigned long i = n; i > 0; --i) {
    ASSERT_TRUE(red_black_tree_insert(tree, (void *)(i - 1)));
    ASSERT_EQ(tree->size, n - i + 1);
  }
  test_contains_range(tree, n);
  test_delete_range(tree, n);
  red_black_tree_validate_expensive(tree);
  red_black_tree_free(tree);
}

void test_contains_range_stride(RedBlackTree *tree, unsigned n) {
  if (n <= STRIDE) {
    return;
  }

  unsigned remainder = n % STRIDE;
  unsigned max_val = remainder == 0 ? n - STRIDE : n - remainder;

  for (unsigned long i = STRIDE; i <= max_val; i += STRIDE) {
    test_get_true(tree, i);
    for (unsigned long j = i + 1; j <= i + STRIDE; ++j) {
      Optional pred = red_black_tree_pred(tree, (void *)j);
      ASSERT_TRUE(pred.present);
      ASSERT_EQ((long)pred.val, i);

      Optional succ = red_black_tree_succ(tree, (void *)(j - STRIDE - 1));
      EXPECT_TRUE(succ.present);
      EXPECT_EQ((long)succ.val, i);
    }
  }

  test_get_false(tree, -1);
  test_get_false(tree, max_val + 1);
  test_get_false(tree, n + STRIDE);

  EXPECT_FALSE(red_black_tree_pred(tree, NULL).present);
  Optional pred = red_black_tree_pred(tree, (void *)(long)(n + STRIDE));
  ASSERT_TRUE(pred.present);

  Optional min = red_black_tree_min(tree);
  EXPECT_TRUE(min.present);
  EXPECT_EQ((long)min.val, STRIDE);
  Optional max = red_black_tree_max(tree);
  EXPECT_TRUE(max.present);
  EXPECT_EQ((long)max.val, max_val);
}

void rb_tree_test_increasing_stride(unsigned n) {
  RedBlackTree *tree = red_black_tree_new();
  for (unsigned long i = STRIDE; i < n; i += STRIDE) {
    ASSERT_TRUE(red_black_tree_insert(tree, (void *)i));
  }
  test_contains_range_stride(tree, n);
  red_black_tree_validate_expensive(tree);
  red_black_tree_free(tree);
}

void rb_tree_test_random(unsigned n) {
  srand(0);
  RedBlackTree *tree = red_black_tree_new();
  Vector *numbers = vector_newn(n);
  for (unsigned i = 0; i < n; ++i) {
    void *r = (void *)(long)rand();
    // TODO: Make sure we have unique numbers by checking if the number exists
    // in a hash structure. We get lucky with our seed that we do not have any
    // random numbers right now with 2^32 possible numbers.
    /* void *r; */
    /* do { */
    /*   r = (void *)(long)rand(); */
    /* } while (vector_contains(numbers, r)); */
    red_black_tree_insert(tree, (void *)(long)r);
    vector_push(numbers, r);
    red_black_tree_validate_expensive(tree);
    ASSERT_EQ(tree->size, i + 1);
  }
  for (unsigned i = 0; i < n; ++i) {
    test_get_true(tree, (long)numbers->data[i]);
  }

  for (unsigned i = 0; i < n; i += STRIDE) {
    void *element = (void *)(long)numbers->data[i];
    ASSERT_EQ(red_black_tree_delete(tree, element), element);
  }
  for (unsigned i = 0; i < n; ++i) {
    if (i % STRIDE == 0) {
      test_get_false(tree, (long)numbers->data[i]);
    } else {
      test_get_true(tree, (long)numbers->data[i]);
    }
  }

  vector_free(numbers);
  red_black_tree_validate_expensive(tree);
  red_black_tree_free(tree);
}

void rb_tree_test_length(unsigned n) {
  rb_tree_test_increasing(n);
  rb_tree_test_decreasing(n);
  rb_tree_test_increasing_stride(n);
  rb_tree_test_random(n);
}

TEST(RedBlackTree, Empty) {
  RedBlackTree *tree = red_black_tree_new();
  EXPECT_FALSE(tree->root);
  EXPECT_EQ(tree->size, 0);
  EXPECT_FALSE(red_black_tree_min(tree).present);
  EXPECT_FALSE(red_black_tree_max(tree).present);
  test_get_false(tree, 0);
  test_get_false(tree, 0xBA5);
  test_get_false(tree, 0xF00);
  red_black_tree_validate_expensive(tree);
  red_black_tree_free(tree);
}


TEST(RedBlackTree, TriangleRotationRight) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, (void *)1);
  test_contains_range(tree, 3);
  red_black_tree_free(tree);
}

TEST(RedBlackTree, TriangleRotationLeft) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)1);
  test_contains_range(tree, 3);
  red_black_tree_free(tree);
}

TEST(RedBlackTree, RecolorRight) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, (void *)1);
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, (void *)3);
  test_contains_range(tree, 4);
  red_black_tree_free(tree);
}

TEST(RedBlackTree, RecolorLeft) {
  RedBlackTree *tree = red_black_tree_new();
  red_black_tree_insert(tree, (void *)2);
  red_black_tree_insert(tree, (void *)3);
  red_black_tree_insert(tree, NULL);
  red_black_tree_insert(tree, (void *)1);
  test_contains_range(tree, 4);
  red_black_tree_free(tree);
}

TEST(RedBlackTree, Repeat) {
  RedBlackTree *tree = red_black_tree_new();
  ASSERT_TRUE(red_black_tree_insert(tree, NULL));
  ASSERT_FALSE(red_black_tree_insert(tree, NULL));
  ASSERT_FALSE(red_black_tree_insert(tree, NULL));
  test_contains_range(tree, 1);
  red_black_tree_free(tree);
}

TEST(RedBlackTree, Length1) { rb_tree_test_length(1); }
TEST(RedBlackTree, Length2) { rb_tree_test_length(2); }
TEST(RedBlackTree, Length3) { rb_tree_test_length(3); }
TEST(RedBlackTree, Length4) { rb_tree_test_length(4); }
TEST(RedBlackTree, Length5) { rb_tree_test_length(5); }
TEST(RedBlackTree, Length6) { rb_tree_test_length(6); }
TEST(RedBlackTree, Length8) { rb_tree_test_length(8); }
TEST(RedBlackTree, Length16) { rb_tree_test_length(16); }
TEST(RedBlackTree, Length128) { rb_tree_test_length(128); }
TEST(RedBlackTree, Length500) { rb_tree_test_length(500); }
TEST(RedBlackTree, Length1234) { rb_tree_test_length(1234); }
TEST(RedBlackTree, LengthABC) { rb_tree_test_length(0xABC); }
TEST(RedBlackTree, LengthBar) { rb_tree_test_length(0xBA5); }
TEST(RedBlackTree, LengthCao) { rb_tree_test_length(0xCA0); }
TEST(RedBlackTree, LengthFoo) { rb_tree_test_length(0xF00); }
