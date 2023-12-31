#include "geometry/structure/segment.h"

Segment segment_from_coords(double x0, double y0, double x1, double y1) {
  Point p0 = (Point){.x = x0, .y = y0};
  Point p1 = (Point){.x = x1, .y = y1};
  return (Segment){.p0 = p0, .p1 = p1};
}

double segment_length(Segment segment) {
  return point_distance(segment.p0, segment.p1);
}

Line segment_to_line(Segment segment) {
  double x_difference = segment.p0.x - segment.p1.x;
  double slope = (segment.p0.y - segment.p1.y) / x_difference;
  // If x's are the same and y's are not, it is a veritcal line with NAN slope.
  // We need to handle to y intercept specially in this case.
  // TODO: how do we handle segment that is just one point
  double y_intercept =
      x_difference == 0 ? segment.p0.y : segment.p0.y - (segment.p0.x * slope);
  return (Line){
      .slope = slope,
      .y_intercept = y_intercept,
  };
}

// Does not check that p lies on s, just that it is within the bounds of s.
static bool segment_bound_contains_point(Segment s, Point p) {
  double lower_x;
  double upper_x;
  if (s.p0.x < s.p1.x) {
    lower_x = s.p0.x;
    upper_x = s.p1.x;
  } else {
    lower_x = s.p1.x;
    upper_x = s.p0.x;
  }

  double lower_y;
  double upper_y;
  if (s.p0.y < s.p1.y) {
    lower_y = s.p0.y;
    upper_y = s.p1.y;
  } else {
    lower_y = s.p1.y;
    upper_y = s.p0.y;
  }

  return p.x >= lower_x && p.x <= upper_x && p.y >= lower_y && p.y <= upper_y;
}

Point segment_intersection_point(Segment s0, Segment s1) {
  Point intersect_point =
      line_intersection_point(segment_to_line(s0), segment_to_line(s1));
  return segment_bound_contains_point(s0, intersect_point) &&
                 segment_bound_contains_point(s1, intersect_point)
             ? intersect_point
             : nan_point();
}

bool segment_intersects(Segment s0, Segment s1) {
  return !is_nan_point(segment_intersection_point(s0, s1));
}

bool segment_line_intersect(Segment s, Line l) {
  // TODO: this is untested. do we need this API?
  Point intersect_point =
      line_intersection_point(segment_to_line(s), l);
  return segment_bound_contains_point(s, intersect_point);
}

bool point_lies_on_segment(Point p, Segment s) {
  return segment_bound_contains_point(s, p) &&
         point_lies_on_line(p, segment_to_line(s));
}

bool segment_equals(Segment s0, Segment s1) {
  return (point_equals(s0.p0, s1.p0) && point_equals(s0.p1, s1.p1)) ||
         (point_equals(s0.p0, s1.p1) && point_equals(s0.p1, s1.p0));
}

