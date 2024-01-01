#include "geometry/structure/line.h"
#include "geometry/util.h"
#include <math.h>

Point line_intersection_point(Line l0, Line l1) {
  // TODO: Need to consider some edge cases. What if the lines are the same?
  // What if one has vertical slope? Should we just compare segments using the
  // algorithm in CLRS?
  double slope_difference = l0.slope - l1.slope;
  if (slope_difference == 0) {
    // Lines have the same slope. There is either no intersection, or infinite
    // intersections.
    return (Point){
        .x = NAN,
        .y = NAN,
    };
  }

  double x = (l1.y_intercept - l0.y_intercept) / slope_difference;
  double y = x * l0.slope + l0.y_intercept;
  return (Point){
      .x = x,
      .y = y,
  };
}

bool point_lies_on_line(Point p, Line l) {
  return tolerant_equals(p.y - (l.slope * p.x + l.y_intercept), 0);
}

bool line_equals(Line l0, Line l1) {
  return (tolerant_equals(l0.slope, l1.slope) ||
          isnan(l0.slope) && isnan(l1.slope)) &&
         tolerant_equals(l0.y_intercept, l1.y_intercept);
}
