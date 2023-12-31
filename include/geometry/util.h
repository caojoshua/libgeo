#ifndef GEOUTILS_H
#define GEOUTILS_H

#include <math.h>

// Allow for some tolerance for floating point error when comparing points
static const double TOLERANCE = 0.0001;

static bool tolerant_equals(double a, double b) {
  return fabs(a - b) < TOLERANCE;
}

static bool tolerant_zero(double d) {
  return tolerant_equals(d, 0);
}

#endif
