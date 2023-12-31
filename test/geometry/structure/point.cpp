#include "geometry/util.h"
extern "C" {
#include "geometry/structure/point.h"
}
#include <gtest/gtest.h>
#include <math.h>

static void test_distance_inner(Point p0, Point p1, double distance) {
  ASSERT_FALSE(point_equals(p0, p1));
  ASSERT_FALSE(point_equals(p1, p0));
  ASSERT_LT(fabs(point_distance(p0, p1) - distance), TOLERANCE);
  ASSERT_LT(fabs(point_distance(p1, p0) - distance), TOLERANCE);
}

static void test_distance(double x0, double y0, double x1, double y1,
                          double distance) {
  Point p0 = (Point){.x = x0, .y = y0};
  Point p1 = (Point){.x = x1, .y = y1};
  test_distance_inner(p0, p1, distance);

  p0 = (Point){.x = -x0, .y = -y0};
  p1 = (Point){.x = -x1, .y = -y1};
  test_distance_inner(p0, p1, distance);
}

static void test_equals_inner(Point p0, Point p1) {
  ASSERT_TRUE(point_equals(p0, p1));
  ASSERT_TRUE(point_equals(p1, p0));
  ASSERT_TRUE(tolerant_zero(point_distance(p0, p1)));
  ASSERT_TRUE(tolerant_zero(point_distance(p1, p0)));
}

static void test_equals(double x0, double y0) {
  Point p0 = (Point){.x = x0, .y = y0};
  test_equals_inner(p0, p0);

  double error = TOLERANCE / 2;
  Point p1 = (Point){.x = x0 + error, .y = y0};
  test_equals_inner(p0, p1);
  p1 = (Point){.x = x0 - error, .y = y0};
  test_equals_inner(p0, p1);
  p1 = (Point){.x = x0, .y = y0 + error};
  test_equals_inner(p0, p1);
  p1 = (Point){.x = x0, .y = y0 - error};
  test_equals_inner(p0, p1);
}

static void test_same_point(double x0, double y0) {
  test_equals(x0, y0);
  test_equals(-x0, y0);
  test_equals(x0, -y0);
  test_equals(-x0, -y0);
}

TEST(Point, SamePoint) {
  test_same_point(0, 0);
  test_same_point(1.5, -3.0);
  test_same_point(0xBA5, 0xF00);
}

TEST(Point, UnitDistance) {
  test_distance(0, 0, 1, 0, 1);
  test_distance(0, 0, 0, 1, 1);
  test_distance(0, 0, -1, 0, 1);
  test_distance(0, 0, 0, -1, 1);
}

TEST(Point, Distance) {
  test_distance(0, 0, 5, 5, 7.0711);
  test_distance(1, 2, 3, 4, 2.8284);
}
