extern "C" {
#include "data_structure/priority_queue.h"
}
#include <gtest/gtest.h>

void test_pop_order(PriorityQueue *pq, unsigned n) {
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_EQ((long)priority_queue_pop(pq), i);
    ASSERT_EQ(pq->vec->size, n - i - 1);
  }
  priority_queue_free(pq);
}

void test_increasing(unsigned n) {
  PriorityQueue *pq = priority_queue_new();
  for (unsigned long i = 0; i < n; ++i) {
    priority_queue_push(pq, (void *)i);
    ASSERT_EQ(pq->vec->size, i + 1);
  }
  test_pop_order(pq, n);
}

void test_decreasing(unsigned n) {
  PriorityQueue *pq = priority_queue_new();
  for (unsigned long i = n; i > 0; --i) {
    priority_queue_push(pq, (void *)(i - 1));
    ASSERT_EQ(pq->vec->size, n - i + 1);
  }
  test_pop_order(pq, n);
}

void test_random(unsigned n) {
  PriorityQueue *pq = priority_queue_new();
  for (unsigned long i = 0; i < n; ++i) {
    priority_queue_push(pq, (void *)(long)rand());
    ASSERT_EQ(pq->vec->size, i + 1);
  }
  void *prev = priority_queue_pop(pq);
  for (unsigned i = 1; i < n; ++i) {
    void *curr = priority_queue_pop(pq);
    ASSERT_TRUE(prev <= curr);
    ASSERT_EQ(pq->vec->size, n - i - 1);
    prev = curr;
  }
  priority_queue_free(pq);
}

void test_length(unsigned n) {
  srand(time(NULL));
  test_increasing(n);
  test_decreasing(n);
  test_random(n);
}

TEST(PriorityQueueTest, Length0) {
  PriorityQueue *pq = priority_queue_new();
  ASSERT_FALSE(priority_queue_pop(pq));
  priority_queue_free(pq);
}

TEST(PriorityQueueTest, Length1) {
  PriorityQueue *pq = priority_queue_new();
  priority_queue_push(pq, (void *)1);
  ASSERT_EQ((long)priority_queue_pop(pq), 1);
  priority_queue_free(pq);
}

TEST(PriorityQueueTest, Length2) {
  PriorityQueue *pq = priority_queue_new();
  priority_queue_push(pq, (void *)1);
  priority_queue_push(pq, (void *)2);
  ASSERT_EQ((long)priority_queue_pop(pq), 1);
  ASSERT_EQ((long)priority_queue_pop(pq), 2);
  priority_queue_free(pq);
}

TEST(PriorityQueueTest, opposite) {
  PriorityQueue *pq = priority_queue_new();
  priority_queue_push(pq, (void *)2);
  priority_queue_push(pq, (void *)1);
  ASSERT_EQ((long)priority_queue_pop(pq), 1);
  ASSERT_EQ((long)priority_queue_pop(pq), 2);
  priority_queue_free(pq);
}

TEST(PriorityQueueTest, Length3) { test_length(3); }
TEST(PriorityQueueTest, Length4) { test_length(4); }
TEST(PriorityQueueTest, Length8) { test_length(8); }
TEST(PriorityQueueTest, Length128) { test_length(128); }
