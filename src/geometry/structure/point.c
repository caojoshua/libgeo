#include "geometry/structure/point.h"
#include "geometry/util.h"
#include <math.h>

static double power2(double a) { return a * a; }

double point_distance(Point p0, Point p1) {
  return sqrt(power2(p0.x - p1.x) + power2(p0.y - p1.y));
}

bool point_equals(Point p0, Point p1) {
  return tolerant_equals(p0.x, p1.x) && tolerant_equals(p0.y, p1.y);
}

Point nan_point() { return (Point){.x = NAN, .y = NAN}; }
bool is_nan_point(Point p) { return isnan(p.x) && isnan(p.y); }
