extern "C" {
#include "data_structure/priority_queue.h"
}
#include <gtest/gtest.h>

void priority_queue_test_pop_order(PriorityQueue *pq, unsigned n) {
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_EQ((long)priority_queue_pop(pq), i);
    ASSERT_EQ(pq->vec.size, n - i - 1);
  }
  priority_queue_free(pq);
}

void priority_queue_test_increasing(unsigned n) {
  PriorityQueue *pq = priority_queue_new();
  for (unsigned long i = 0; i < n; ++i) {
    priority_queue_push(pq, (void *)i);
    ASSERT_EQ(pq->vec.size, i + 1);
  }
  priority_queue_test_pop_order(pq, n);
}

void priority_queue_test_decreasing(unsigned n) {
  PriorityQueue *pq = priority_queue_new();
  for (unsigned long i = n; i > 0; --i) {
    priority_queue_push(pq, (void *)(i - 1));
    ASSERT_EQ(pq->vec.size, n - i + 1);
  }
  priority_queue_test_pop_order(pq, n);
}

void priority_queue_test_random(unsigned n) {
  PriorityQueue *pq = priority_queue_new();
  for (unsigned long i = 0; i < n; ++i) {
    priority_queue_push(pq, (void *)(long)rand());
    ASSERT_EQ(pq->vec.size, i + 1);
  }
  void *prev = priority_queue_pop(pq);
  for (unsigned i = 1; i < n; ++i) {
    void *curr = priority_queue_pop(pq);
    ASSERT_TRUE(prev <= curr);
    ASSERT_EQ(pq->vec.size, n - i - 1);
    prev = curr;
  }
  priority_queue_free(pq);
}

void priority_queue_test_length(unsigned n) {
  srand(time(NULL));
  priority_queue_test_increasing(n);
  priority_queue_test_decreasing(n);
  priority_queue_test_random(n);
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

TEST(PriorityQueueTest, Length3) { priority_queue_test_length(3); }
TEST(PriorityQueueTest, Length4) { priority_queue_test_length(4); }
TEST(PriorityQueueTest, Length8) { priority_queue_test_length(8); }
TEST(PriorityQueueTest, Length128) { priority_queue_test_length(128); }
TEST(PriorityQueueTest, LengthBar) { priority_queue_test_length(0xBA5); }
TEST(PriorityQueueTest, LengthFoo) { priority_queue_test_length(0xF00); }
