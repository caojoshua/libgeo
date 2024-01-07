#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "comparator.h"
#include "vector.h"

typedef struct {
  Vector vec;
  cmp_t cmp;
} PriorityQueue;

void pq_init(PriorityQueue *pq);
void pq_initc(PriorityQueue *pq, cmp_t cmp);
void pq_initn(PriorityQueue *pq, unsigned n);
void pq_initcn(PriorityQueue *pq, cmp_t cmp, unsigned n);

void pq_push(PriorityQueue *pq, void *val);
void *pq_pop(PriorityQueue *pq);
void pq_free(PriorityQueue *pq);
void pq_validate(PriorityQueue *pq);

#endif
