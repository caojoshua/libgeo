extern "C" {
#include "data_structure/hash.h"
}
#include <gtest/gtest.h>

static const unsigned STRIDE = 5;

void hash_test_length_increment(unsigned n) {
  Hash *hash = hash_new();
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_TRUE(hash_insert_pair(hash, (void *)i, (void *)i));
    ASSERT_FALSE(hash_insert_pair(hash, (void *)i, (void *)i));
  }
  for (unsigned i = 0; i < n; ++i) {
    ASSERT_TRUE(hash_contains(hash, (void *)(long)i));
  }
  ASSERT_FALSE(hash_contains(hash, (void *)(long)n));
  ASSERT_FALSE(hash_delete(hash, (void *)(long)n));

  for (unsigned long i = 0; i < n; i += STRIDE) {
    ASSERT_EQ(hash_delete(hash, (void *)i), (void *)i);
    ASSERT_FALSE(hash_delete(hash, (void *)i));
  }

  for (unsigned i = 0; i < n; ++i) {
    if (i % STRIDE == 0) {
      ASSERT_FALSE(hash_contains(hash, (void *)(long)i));
    } else {
      ASSERT_TRUE(hash_contains(hash, (void *)(long)i));
    }
  }
  hash_free(hash);
}

void hash_test_length_strided(unsigned n) {
  Hash *hash = hash_new();
  for (unsigned long i = 0; i < n; i += STRIDE) {
    ASSERT_TRUE(hash_insert_pair(hash, (void *)i, (void *)i));
    ASSERT_FALSE(hash_insert_pair(hash, (void *)i, (void *)i));
  }
  ASSERT_FALSE(hash_contains(hash, (void *)(long)n));
  ASSERT_FALSE(hash_delete(hash, (void *)(long)n));

  for (unsigned i = 0; i < n; ++i) {
    if (i % STRIDE == 0) {
      ASSERT_TRUE(hash_contains(hash, (void *)(long)i));
    } else {
      ASSERT_FALSE(hash_contains(hash, (void *)(long)i));
    }
  }
  hash_free(hash);
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

