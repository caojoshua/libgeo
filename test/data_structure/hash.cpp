extern "C" {
#include "data_structure/hash.h"
}
#include <gtest/gtest.h>

static const unsigned STRIDE = 5;

static void test_get_true(Hash *hash, unsigned long i) {
  ASSERT_TRUE(hash_contains(hash, (void *)(long)i));
  ASSERT_EQ(hash_get(hash, (void *)i), (void *)i);
}

static void test_get_false(Hash *hash, unsigned long i) {
  ASSERT_FALSE(hash_contains(hash, (void *)(long)i));
  ASSERT_FALSE(hash_get(hash, (void *)i));
  ASSERT_FALSE(hash_delete(hash, (void *)(long)i));
}

void hash_test_length_increment(unsigned n) {
  Hash hash;
  hash_init(&hash, NULL, NULL);
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_TRUE(hash_insert_pair(&hash, (void *)i, (void *)i));
    ASSERT_FALSE(hash_insert_pair(&hash, (void *)i, (void *)i));
  }
  for (unsigned i = 0; i < n; ++i) {
    test_get_true(&hash, i);
  }
  test_get_false(&hash, n);

  for (unsigned long i = 0; i < n; i += STRIDE) {
    ASSERT_EQ(hash_delete(&hash, (void *)i), (void *)i);
    test_get_false(&hash, i);
  }

  for (unsigned i = 0; i < n; ++i) {
    if (i % STRIDE == 0) {
      test_get_false(&hash, i);
    } else {
      test_get_true(&hash, i);
    }
  }
  hash_free(&hash);
}

void hash_test_length_strided(unsigned n) {
  Hash hash;
  hash_init(&hash, NULL, NULL);
  for (unsigned long i = 0; i < n; i += STRIDE) {
    ASSERT_TRUE(hash_insert_pair(&hash, (void *)i, (void *)i));
    ASSERT_FALSE(hash_insert_pair(&hash, (void *)i, (void *)i));
  }
  test_get_false(&hash, n);

  for (unsigned i = 0; i < n; ++i) {
    if (i % STRIDE == 0) {
      test_get_true(&hash, i);
    } else {
      test_get_false(&hash, i);
    }
  }
  hash_free(&hash);
}

void hash_test_length(unsigned n) {
  hash_test_length_increment(n);
  hash_test_length_strided(n);
}

TEST(Hash, Length1) { hash_test_length(1); }
TEST(Hash, Length2) { hash_test_length(2); }
TEST(Hash, Length3) { hash_test_length(3); }
TEST(Hash, Length4) { hash_test_length(4); }
TEST(Hash, Length5) { hash_test_length(5); }
TEST(Hash, Length6) { hash_test_length(6); }
TEST(Hash, Length8) { hash_test_length(8); }
TEST(Hash, Length16) { hash_test_length(16); }
TEST(Hash, Length128) { hash_test_length(128); }
TEST(Hash, Length500) { hash_test_length(500); }
TEST(Hash, Length1234) { hash_test_length(1234); }
TEST(Hash, LengthABC) { hash_test_length(0xABC); }
TEST(Hash, LengthBar) { hash_test_length(0xBA5); }
TEST(Hash, LengthCao) { hash_test_length(0xCA0); }
TEST(Hash, LengthFoo) { hash_test_length(0xF00); }

