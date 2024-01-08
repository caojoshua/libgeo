extern "C" {
#include "data_structure/hash.h"
#include "data_structure/red_black_tree.h"
#include "data_structure/vector.h"
}
#include <gtest/gtest.h>

static const unsigned STRIDE = 5;

static void test_get_true(RedBlackTree *tree, unsigned long i) {
  ASSERT_TRUE(rb_tree_contains(tree, (void *)i));
  ASSERT_EQ(rb_tree_get(tree, (void *)i), (void *)i);
}

static void test_get_false(RedBlackTree *tree, long i) {
  ASSERT_FALSE(rb_tree_contains(tree, (void *)i));
  ASSERT_FALSE(rb_tree_get(tree, (void *)i));
  ASSERT_FALSE(rb_tree_delete(tree, (void *)i));
}

void test_contains_range(RedBlackTree *tree, unsigned n) {
  ASSERT_GT(n, 0);
  EXPECT_EQ(tree->size, n);
  void **elements = rb_tree_elements(tree);
  for (unsigned long i = 0; i < n; ++i) {
    test_get_true(tree, i);
    ASSERT_EQ((long)elements[i], i);
    if (i > 0) {
      Optional pred = rb_tree_pred(tree, (void *)i);
      ASSERT_TRUE(pred.present);
      ASSERT_EQ((long)pred.val, (i - 1));
    }
    if (i < n - 1) {
      Optional succ = rb_tree_succ(tree, (void *)i);
      ASSERT_TRUE(succ.present);
      ASSERT_EQ((long)succ.val, (i + 1));
    }
  }
  free(elements);

  test_get_false(tree, -1);
  test_get_false(tree, n);

  Optional min = rb_tree_min(tree);
  EXPECT_TRUE(min.present);
  EXPECT_EQ((long)min.val, 0);
  Optional max = rb_tree_max(tree);
  EXPECT_TRUE(max.present);
  EXPECT_EQ((long)max.val, n - 1);

  EXPECT_FALSE(rb_tree_pred(tree, NULL).present);
  Optional pred = rb_tree_pred(tree, (void *)(long)(n));
  EXPECT_TRUE(pred.present);
  EXPECT_EQ((long)pred.val, (long)(n - 1));
}

void test_delete_range(RedBlackTree *tree, unsigned n) {
  ASSERT_GT(n, 0);
  for (unsigned long i = 0; i < n; i += STRIDE) {
    ASSERT_EQ(rb_tree_delete(tree, (void *)i), (void *)i);
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
  RedBlackTree tree;
  rb_tree_init(&tree);
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_TRUE(rb_tree_insert(&tree, (void *)i));
    ASSERT_EQ(tree.size, i + 1);
  }
  test_contains_range(&tree, n);
  test_delete_range(&tree, n);
  rb_tree_validate_expensive(&tree);
  rb_tree_free(&tree);
}

void rb_tree_test_decreasing(unsigned n) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  for (unsigned long i = n; i > 0; --i) {
    ASSERT_TRUE(rb_tree_insert(&tree, (void *)(i - 1)));
    ASSERT_EQ(tree.size, n - i + 1);
  }
  test_contains_range(&tree, n);
  test_delete_range(&tree, n);
  rb_tree_validate_expensive(&tree);
  rb_tree_free(&tree);
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
      Optional pred = rb_tree_pred(tree, (void *)j);
      ASSERT_TRUE(pred.present);
      ASSERT_EQ((long)pred.val, i);

      Optional succ = rb_tree_succ(tree, (void *)(j - STRIDE - 1));
      EXPECT_TRUE(succ.present);
      EXPECT_EQ((long)succ.val, i);
    }
  }

  test_get_false(tree, -1);
  test_get_false(tree, max_val + 1);
  test_get_false(tree, n + STRIDE);

  EXPECT_FALSE(rb_tree_pred(tree, NULL).present);
  Optional pred = rb_tree_pred(tree, (void *)(long)(n + STRIDE));
  ASSERT_TRUE(pred.present);

  Optional min = rb_tree_min(tree);
  EXPECT_TRUE(min.present);
  EXPECT_EQ((long)min.val, STRIDE);
  Optional max = rb_tree_max(tree);
  EXPECT_TRUE(max.present);
  EXPECT_EQ((long)max.val, max_val);
}

void rb_tree_test_increasing_stride(unsigned n) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  for (unsigned long i = STRIDE; i < n; i += STRIDE) {
    ASSERT_TRUE(rb_tree_insert(&tree, (void *)i));
  }
  test_contains_range_stride(&tree, n);
  rb_tree_validate_expensive(&tree);
  rb_tree_free(&tree);
}

void rb_tree_test_random(unsigned n) {
  srand(0);
  RedBlackTree tree;
  rb_tree_init(&tree);
  // Maintain a vector to keep track of which element is at each index. Maintain
  // a hash structure to check that there are no duplicates.
  Vector numbers_vec;
  vector_initn(&numbers_vec, n);
  Hash numbers_hash;
  hash_init(&numbers_hash, NULL, NULL);
  for (unsigned i = 0; i < n; ++i) {
    void *r;
    do {
      r = (void *)(long)rand();
    } while (hash_contains(&numbers_hash, r));
    rb_tree_insert(&tree, (void *)(long)r);
    vector_push(&numbers_vec, r);
    hash_insert(&numbers_hash, r);
    rb_tree_validate_expensive(&tree);
    ASSERT_EQ(tree.size, i + 1);
  }
  hash_free(&numbers_hash);

  for (unsigned i = 0; i < n; ++i) {
    test_get_true(&tree, (long)numbers_vec.data[i]);
  }

  for (unsigned i = 0; i < n; i += STRIDE) {
    void *element = (void *)(long)numbers_vec.data[i];
    ASSERT_EQ(rb_tree_delete(&tree, element), element);
  }
  for (unsigned i = 0; i < n; ++i) {
    if (i % STRIDE == 0) {
      test_get_false(&tree, (long)numbers_vec.data[i]);
    } else {
      test_get_true(&tree, (long)numbers_vec.data[i]);
    }
  }

  vector_free(&numbers_vec);
  rb_tree_validate_expensive(&tree);
  rb_tree_free(&tree);
}

void rb_tree_test_length(unsigned n) {
  rb_tree_test_increasing(n);
  rb_tree_test_decreasing(n);
  rb_tree_test_increasing_stride(n);
  rb_tree_test_random(n);
}

TEST(RedBlackTree, Empty) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  EXPECT_FALSE(tree.root);
  EXPECT_EQ(tree.size, 0);
  EXPECT_FALSE(rb_tree_min(&tree).present);
  EXPECT_FALSE(rb_tree_max(&tree).present);
  test_get_false(&tree, 0);
  test_get_false(&tree, 0xBA5);
  test_get_false(&tree, 0xF00);
  rb_tree_validate_expensive(&tree);
  rb_tree_free(&tree);
}


TEST(RedBlackTree, TriangleRotationRight) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  rb_tree_insert(&tree, NULL);
  rb_tree_insert(&tree, (void *)2);
  rb_tree_insert(&tree, (void *)1);
  test_contains_range(&tree, 3);
  rb_tree_free(&tree);
}

TEST(RedBlackTree, TriangleRotationLeft) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  rb_tree_insert(&tree, (void *)2);
  rb_tree_insert(&tree, NULL);
  rb_tree_insert(&tree, (void *)1);
  test_contains_range(&tree, 3);
  rb_tree_free(&tree);
}

TEST(RedBlackTree, RecolorRight) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  rb_tree_insert(&tree, (void *)1);
  rb_tree_insert(&tree, NULL);
  rb_tree_insert(&tree, (void *)2);
  rb_tree_insert(&tree, (void *)3);
  test_contains_range(&tree, 4);
  rb_tree_free(&tree);
}

TEST(RedBlackTree, RecolorLeft) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  rb_tree_insert(&tree, (void *)2);
  rb_tree_insert(&tree, (void *)3);
  rb_tree_insert(&tree, NULL);
  rb_tree_insert(&tree, (void *)1);
  test_contains_range(&tree, 4);
  rb_tree_free(&tree);
}

TEST(RedBlackTree, Repeat) {
  RedBlackTree tree;
  rb_tree_init(&tree);
  ASSERT_TRUE(rb_tree_insert(&tree, NULL));
  ASSERT_FALSE(rb_tree_insert(&tree, NULL));
  ASSERT_FALSE(rb_tree_insert(&tree, NULL));
  test_contains_range(&tree, 1);
  rb_tree_free(&tree);
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
