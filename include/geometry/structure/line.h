#ifndef LINE_H
#define LINE_H

#include "geometry/structure/point.h"

// TODO: Come back to lines at a later point. Most of the immediate algorithms I
// can think of now are relating to segments.

typedef struct {
  double slope;
  double y_intercept;
} Line;

Point line_intersection_point(Line l0, Line l1);
bool point_lies_on_line(Point p, Line l);
bool line_equals(Line l0, Line l1);

#endif
