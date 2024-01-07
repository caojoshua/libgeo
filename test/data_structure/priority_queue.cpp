extern "C" {
#include "data_structure/priority_queue.h"
}
#include <gtest/gtest.h>

void pq_test_pop_order(PriorityQueue *pq, unsigned n) {
  for (unsigned long i = 0; i < n; ++i) {
    ASSERT_EQ((long)pq_pop(pq), i);
    ASSERT_EQ(pq->vec.size, n - i - 1);
  }
  pq_free(pq);
}

void pq_test_increasing(unsigned n) {
  PriorityQueue pq;
  pq_init(&pq);
  for (unsigned long i = 0; i < n; ++i) {
    pq_push(&pq, (void *)i);
    ASSERT_EQ(pq.vec.size, i + 1);
  }
  pq_test_pop_order(&pq, n);
}

void pq_test_decreasing(unsigned n) {
  PriorityQueue pq;
  pq_init(&pq);
  for (unsigned long i = n; i > 0; --i) {
    pq_push(&pq, (void *)(i - 1));
    ASSERT_EQ(pq.vec.size, n - i + 1);
  }
  pq_test_pop_order(&pq, n);
}

void pq_test_random(unsigned n) {
  PriorityQueue pq;
  pq_init(&pq);
  for (unsigned long i = 0; i < n; ++i) {
    pq_push(&pq, (void *)(long)rand());
    ASSERT_EQ(pq.vec.size, i + 1);
  }
  void *prev = pq_pop(&pq);
  for (unsigned i = 1; i < n; ++i) {
    void *curr = pq_pop(&pq);
    ASSERT_TRUE(prev <= curr);
    ASSERT_EQ(pq.vec.size, n - i - 1);
    prev = curr;
  }
  pq_free(&pq);
}

void pq_test_length(unsigned n) {
  srand(time(NULL));
  pq_test_increasing(n);
  pq_test_decreasing(n);
  pq_test_random(n);
}

TEST(PriorityQueueTest, Length0) {
  PriorityQueue pq;
  pq_init(&pq);
  ASSERT_FALSE(pq_pop(&pq));
  pq_free(&pq);
}

TEST(PriorityQueueTest, Length1) {
  PriorityQueue pq;
  pq_init(&pq);
  pq_push(&pq, (void *)1);
  ASSERT_EQ((long)pq_pop(&pq), 1);
  pq_free(&pq);
}

TEST(PriorityQueueTest, Length2) {
  PriorityQueue pq;
  pq_init(&pq);
  pq_push(&pq, (void *)1);
  pq_push(&pq, (void *)2);
  ASSERT_EQ((long)pq_pop(&pq), 1);
  ASSERT_EQ((long)pq_pop(&pq), 2);
  pq_free(&pq);
}

TEST(PriorityQueueTest, opposite) {
  PriorityQueue pq;
  pq_init(&pq);
  pq_push(&pq, (void *)2);
  pq_push(&pq, (void *)1);
  ASSERT_EQ((long)pq_pop(&pq), 1);
  ASSERT_EQ((long)pq_pop(&pq), 2);
  pq_free(&pq);
}

TEST(PriorityQueueTest, Length3) { pq_test_length(3); }
TEST(PriorityQueueTest, Length4) { pq_test_length(4); }
TEST(PriorityQueueTest, Length8) { pq_test_length(8); }
TEST(PriorityQueueTest, Length128) { pq_test_length(128); }
TEST(PriorityQueueTest, LengthBar) { pq_test_length(0xBA5); }
TEST(PriorityQueueTest, LengthFoo) { pq_test_length(0xF00); }
