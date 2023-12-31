#include "geometry/util.h"
extern "C" {
#include "geometry/structure/segment.h"
}
#include <gtest/gtest.h>

static void test_segments_equal(Segment s0, Segment s1) {
  ASSERT_TRUE(segment_equals(s0, s1));
  ASSERT_TRUE(segment_equals(s1, s0));

  Line l0 = segment_to_line(s0);
  Line l1 = segment_to_line(s1);
  ASSERT_TRUE(line_equals(l0, l1));
  ASSERT_TRUE(line_equals(l1, l0));
}

static void test_same_segment_inner(double x0, double y0, double x1, double y1) {
  Segment s0 = segment_from_coords(x0, y0, x1, y1);
  Segment s1 = segment_from_coords(x1, y1, x0, y0);
  test_segments_equal(s0, s0);
  test_segments_equal(s0, s1);
}

static void test_same_segment(double x0, double y0, double x1, double y1) {
  test_same_segment_inner(x0, y0, x1, y1);
  test_same_segment_inner(-x0, y0, x1, y1);
  test_same_segment_inner(x0, -y0, x1, y1);
  test_same_segment_inner(x0, y0, -x1, y1);
  test_same_segment_inner(x0, y0, x1, -y1);
  test_same_segment_inner(-x0, -y0, x1, y1);
  test_same_segment_inner(x0, y0, -x1, -y1);
  test_same_segment_inner(-x0, -y0, -x1, -y1);
}

static void test_segment_intersection(double x0, double y0, double x1,
                                      double y1, double x2, double y2,
                                      double x3, double y3, double x_intersect,
                                      double y_intersect) {
  Point intersect_point = (Point){.x = x_intersect, .y = y_intersect};
  Segment s0 = segment_from_coords(x0, y0, x1, y1);
  Segment s1 = segment_from_coords(x2, y2, x3, y3);
  ASSERT_TRUE(segment_intersects(s0, s1));
  ASSERT_TRUE(segment_intersects(s1, s0));
  ASSERT_TRUE(point_equals(segment_intersection_point(s0, s1), intersect_point));
  ASSERT_TRUE(point_equals(segment_intersection_point(s1, s0), intersect_point));
}

static void test_segment_no_intersection(double x0, double y0, double x1,
                                      double y1, double x2, double y2,
                                      double x3, double y3) {
  Segment s0 = segment_from_coords(x0, y0, x1, y1);
  Segment s1 = segment_from_coords(x2, y2, x3, y3);
  ASSERT_FALSE(segment_intersects(s0, s1));
  ASSERT_FALSE(segment_intersects(s1, s0));
  ASSERT_TRUE(is_nan_point(segment_intersection_point(s0, s1)));
  ASSERT_TRUE(is_nan_point(segment_intersection_point(s1, s0)));
}

TEST(Segment, SameSegment) {
  test_same_segment(0, 0, 0, 0);
  test_same_segment(1, 2, 3, 4);
  test_same_segment(0xF0, 0x0, 0xBA, 0x5);
}

TEST(Segment, SegmentLength) {
  ASSERT_TRUE(tolerant_equals(segment_length(segment_from_coords(0, 0, 0, 0)), 0));
  ASSERT_TRUE(tolerant_equals(segment_length(segment_from_coords(0, 0, 1, 0)), 1));
  ASSERT_TRUE(tolerant_equals(segment_length(segment_from_coords(0, 0, 1, 1)), 1.4142));
  ASSERT_TRUE(tolerant_equals(segment_length(segment_from_coords(0, 0, 2, 2)), 2.8284));
  ASSERT_TRUE(tolerant_equals(segment_length(segment_from_coords(0, 0, 2, 3)), 3.6055));
}

TEST(Segment, SegmentIntersection) {
  test_segment_intersection(0, 0, 2, 2, -1, 1, 1, 1, 1, 1);
  test_segment_intersection(-1, 4, 5, -2, -3, -6, 2, 4, 1, 2);
}

TEST(Segment, SegmentDoesNotIntersection) {
  test_segment_no_intersection(0, 0, 2, 2, 1, 3, -1, 10);
  test_segment_no_intersection(0, 0, 2, 2, 1, 3, 5, 10);
}

TEST(Segment, SegmentDoesNotIntersectionSameSlope) {
  test_segment_no_intersection(0, 0, 2, 2, 5, 5, 7, 7);
  test_segment_no_intersection(0, 0, 2, 2, 4, 4, 6, 6);
}

TEST(Segment, PointsLieOnHorizontalLine) {
  double left = 0;
  double right = 10;
  double y = 5;
  Segment s = segment_from_coords(left, y, right, y);
  for (double i = left; i <= right; ++i) {
    ASSERT_TRUE(point_lies_on_segment((Point){.x = i, .y = 5}, s));
  }
  ASSERT_FALSE(point_lies_on_segment((Point){.x = left - 1, .y = y}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = right + 1, .y = y}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = left, .y = y + 1}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = left, .y = y - 1}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = right, .y = y + 1}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = right, .y = y - 1}, s));
}

TEST(Segment, PointsLieOnDiagonalLine) {
  double left = 0;
  double right = 10;
  Segment s = segment_from_coords(left, left, right, right);
  for (double i = left; i <= right; ++i) {
    ASSERT_TRUE(point_lies_on_segment((Point){.x = i, .y = i}, s));
  }
  ASSERT_FALSE(point_lies_on_segment((Point){.x = left + 1, .y = right}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = left - 1, .y = right}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = left, .y = right + 1}, s));
  ASSERT_FALSE(point_lies_on_segment((Point){.x = left, .y = right - 1}, s));
}
