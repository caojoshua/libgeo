extern "C" {
#include "data_structure/sort.h"
}
#include <gtest/gtest.h>

static void test_sorted(void **data, unsigned n) {
  for (unsigned i = 0; i < n - 1; ++i) {
    ASSERT_LT(data[i], data[i + 1]);
  }
  free(data);
}

static void test_random(unsigned n) {
  srand(0);
  void **data = (void **)malloc(sizeof(void *) * n);
  for (unsigned i = 0; i < n; ++i) {
    data[i] = (void *)(long)rand();
  }
  quick_sort(data, n);
  test_sorted(data, n);
}

static void test_length(unsigned n) { test_random(n); }

TEST(Sort, Length1) { test_length(1); }
TEST(Sort, Length2) { test_length(2); }
TEST(Sort, Length3) { test_length(3); }
TEST(Sort, Length4) { test_length(4); }
TEST(Sort, Length5) { test_length(5); }
TEST(Sort, Length6) { test_length(6); }
TEST(Sort, Length8) { test_length(8); }
TEST(Sort, Length16) { test_length(16); }
TEST(Sort, Length128) { test_length(128); }
TEST(Sort, Length500) { test_length(500); }
TEST(Sort, Length1234) { test_length(1234); }
TEST(Sort, LengthABC) { test_length(0xABC); }
TEST(Sort, LengthBar) { test_length(0xBA5); }
TEST(Sort, LengthCao) { test_length(0xCA0); }
TEST(Sort, LengthFoo) { test_length(0xF00); }
