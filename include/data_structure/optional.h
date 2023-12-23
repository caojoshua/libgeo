#ifndef OPTIONAL_H
#define OPTIONAL_H

typedef struct {
  bool present;
  void *val;
} Optional;

static Optional optional(void *val) { return (Optional){.present = true, .val = val}; }
static Optional optional_null() { return (Optional){.present = false}; }

#endif
