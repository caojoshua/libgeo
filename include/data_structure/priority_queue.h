#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "vector.h"

typedef bool(*cmp_t)(void *, void *);
static bool less_than_cmp(void *a, void *b) {
  return a < b;
}
static bool greater_than_cmp(void *a, void *b) {
  return a > b;
}

typedef struct {
  Vector *vec;
  cmp_t cmp;
} PriorityQueue;

PriorityQueue *priority_queue_new();
PriorityQueue *priority_queue_newc(cmp_t cmp);
PriorityQueue *priority_queue_newn(unsigned n);
PriorityQueue *priority_queue_newcn(cmp_t cmp, unsigned n);

void priority_queue_push(PriorityQueue *pq, void *val);
void *priority_queue_pop(PriorityQueue *pq);
void priority_queue_free(PriorityQueue *pq);
void priority_queue_validate(PriorityQueue *pq);

#endif
