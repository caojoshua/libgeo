#include "data_structure/priority_queue.h"
#include "data_structure/util.h"
#include <assert.h>

const unsigned DEFAUL_INITIAL_CAPACITY = 16;

// Recursive routine after pushing an element onto the bottom of the queue. We
// need to bubble the new element to the right index.
void bubble_up(PriorityQueue *pq, unsigned index) {
  priority_queue_validate(pq);
  assert(index <= pq->vec.size &&
         "PriorityQueue::bubble_up fix up out of bounds index");

  if (index == 0) {
    // reached parent node
    return;
  }

  unsigned parent_index = (index - 1) / 2;
  void **data = pq->vec.data;
  if (pq->cmp(data[index], data[parent_index]) == LESS) {
    swap(&data[index], &data[parent_index]);
    bubble_up(pq, parent_index);
  }
}

// Recursive routine after popping an element from the top of the queue, and the
// previously bottom element is moved to the top. We need to bubble the top
// element down to the right index.
void bubble_down(PriorityQueue *pq, unsigned index) {
  priority_queue_validate(pq);
  assert(index <= pq->vec.size &&
         "PriorityQueue::bubble_down out of bounds index");

  void **data = pq->vec.data;
  unsigned left_index = index * 2 + 1;

  if (left_index > pq->vec.size - 1) {
    // reached a leaf node
    return;
  }

  if (left_index == pq->vec.size - 1) {
    // left child is the last element in the queue
    if (pq->cmp(data[left_index], data[index]) == LESS) {
      swap(&data[index], &data[left_index]);
    }
    return;
  }

  unsigned right_index = left_index + 1;
  unsigned min_index = pq->cmp(data[left_index], data[right_index]) == LESS
                           ? left_index
                           : right_index;
  if (pq->cmp(data[min_index], data[index]) == LESS) {
    swap(&data[index], &data[min_index]);
    bubble_down(pq, min_index);
  }
}

void pq_init(PriorityQueue *pq) {
  pq_initc(pq, less_than_cmp);
}

void pq_initc(PriorityQueue *pq, cmp_t cmp) {
  pq_initcn(pq, less_than_cmp, DEFAUL_INITIAL_CAPACITY);
}

void pq_initn(PriorityQueue *pq, unsigned n) {
  pq_initcn(pq, less_than_cmp, n);
}

void pq_initcn(PriorityQueue *pq, cmp_t cmp, unsigned n) {
  *pq = (PriorityQueue){
      .cmp = cmp,
  };
  vector_initn(&pq->vec, n);
}

void priority_queue_push(PriorityQueue *pq, void *val) {
  priority_queue_validate(pq);
  if (pq->vec.size == 0) {
    vector_push(&pq->vec, val);
    return;
  }

  vector_push(&pq->vec, val);
  bubble_up(pq, pq->vec.size - 1);
}

void *priority_queue_pop(PriorityQueue *pq) {
  priority_queue_validate(pq);
  if (pq->vec.size == 0) {
    return nullptr;
  }
  if (pq->vec.size == 1) {
    return vector_pop_back(&pq->vec);
  }

  void **data = pq->vec.data;
  void *front = data[0];
  data[0] = vector_pop_back(&pq->vec);
  bubble_down(pq, 0);
  return front;
}

void priority_queue_free(PriorityQueue *pq) {
  priority_queue_pop(pq);
  vector_free(&pq->vec);
}

void priority_queue_validate(PriorityQueue *pq) {
  assert(pq && "pq should not be null");
  vector_validate(&pq->vec);
}
