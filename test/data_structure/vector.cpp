extern "C" {
#include "data_structure/vector.h"
}
#include <gtest/gtest.h>

void vector_test_basic(Vector *vec) {
  const unsigned SIZE = 100;
  ASSERT_EQ(vec->size, 0);
  for (long i = 0; i < SIZE; ++i) {
    vector_push(vec, (void *)i);
    ASSERT_EQ(vec->size, i + 1);
  }
  for (long i = 0; i < SIZE; ++i) {
    EXPECT_TRUE(vector_contains(vec, (void *)i));
  }
  vector_free(vec);
}

TEST(VectorTest, Basic) {
  Vector *vec = vector_new();
  vector_test_basic(vec);
}

TEST(VectorTest, InitialCapacity) {
  Vector *vec = vector_newn(22);
  vector_test_basic(vec);
}

TEST(VectorTest, CapacityDoubles) {
  Vector *vec = vector_newn(10);
  for (unsigned i = 0; i < 10; ++i) {
    vector_push(vec, nullptr);
    EXPECT_EQ(vec->size, i + 1);
    EXPECT_EQ(vec->capacity, 10);
  }
  vector_push(vec, nullptr);
  EXPECT_EQ(vec->size, 11);
  EXPECT_EQ(vec->capacity, 20);
  vector_free(vec);
}

TEST(VectorTest, Resize) {
  Vector *vec = vector_newn(10);
  for (unsigned i = 0; i < 5; ++i) {
    vector_push(vec, nullptr);
    EXPECT_EQ(vec->size, i + 1);
    EXPECT_EQ(vec->capacity, 10);
  }
  vector_resize(vec, 25);
  EXPECT_EQ(vec->size, 5);
  EXPECT_EQ(vec->capacity, 25);
  for (unsigned i = 0; i < 10; ++i) {
    vector_push(vec, nullptr);
    EXPECT_EQ(vec->size, i + 6);
    EXPECT_EQ(vec->capacity, 25);
  }

  vector_free(vec);
}
