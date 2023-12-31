#ifndef SEGMENT_H
#define SEGMENT_H

#include "geometry/structure/line.h"
#include "geometry/structure/point.h"

typedef struct {
  Point p0;
  Point p1;
} Segment;

Segment segment_from_coords(double x0, double y0, double x1, double y1);
double segment_length(Segment segment);
Line segment_to_line(Segment segment);
Point segment_intersection_point(Segment s0, Segment s1);
bool segment_intersects(Segment s0, Segment s1);
bool segment_line_intersect(Segment s, Line l);
bool point_lies_on_segment(Point p, Segment s);
bool segment_equals(Segment s0, Segment s1);

#endif

