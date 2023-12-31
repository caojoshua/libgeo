#ifndef POINT_H
#define POINT_H

#include <math.h>

typedef struct {
  double x;
  double y;
} Point;

double point_distance(Point p0, Point p1);
bool point_equals(Point p0, Point p1);

Point nan_point();
bool is_nan_point(Point p);

#endif
