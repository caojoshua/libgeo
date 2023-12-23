#ifndef COMPARATOR_H
#define COMPARATOR_H

typedef enum { LESS, EQUALS, GREATER } Ordering;

typedef Ordering (*cmp_t)(void *, void *);

static Ordering less_than_cmp(void *a, void *b) {
  if (a < b) {
    return LESS;
  } else if (a == b) {
    return EQUALS;
  }
  return GREATER;
}

static Ordering greater_than_cmp(void *a, void *b) {
  if (a > b) {
    return LESS;
  } else if (a == b) {
    return EQUALS;
  }
  return GREATER;
}

#endif
