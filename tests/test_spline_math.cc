#include "doctest.h"
#include "../spline_math.h"

#include <cmath>

TEST_CASE("spline endpoint t=0 passes through first control point") {
  double ctrl[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
  double fx, fy;
  evaluate_spline(ctrl, 0.0, fx, fy);
  CHECK(fx == doctest::Approx(1.0).epsilon(1e-10));
  CHECK(fy == doctest::Approx(2.0).epsilon(1e-10));
}

TEST_CASE("spline endpoint t=1 passes through last control point") {
  double ctrl[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
  double fx, fy;
  evaluate_spline(ctrl, 1.0, fx, fy);
  CHECK(fx == doctest::Approx(7.0).epsilon(1e-10));
  CHECK(fy == doctest::Approx(8.0).epsilon(1e-10));
}

TEST_CASE("spline at t=1/3 passes through second control point") {
  double ctrl[8] = {0.0, 0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0};
  double fx, fy;
  evaluate_spline(ctrl, 1.0 / 3.0, fx, fy);
  CHECK(fx == doctest::Approx(5.0).epsilon(1e-10));
  CHECK(fy == doctest::Approx(10.0).epsilon(1e-10));
}

TEST_CASE("spline at t=2/3 passes through third control point") {
  double ctrl[8] = {0.0, 0.0, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0};
  double fx, fy;
  evaluate_spline(ctrl, 2.0 / 3.0, fx, fy);
  CHECK(fx == doctest::Approx(15.0).epsilon(1e-10));
  CHECK(fy == doctest::Approx(20.0).epsilon(1e-10));
}

TEST_CASE("spline with all control points at origin") {
  double ctrl[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  double fx, fy;
  evaluate_spline(ctrl, 0.5, fx, fy);
  CHECK(fx == doctest::Approx(0.0));
  CHECK(fy == doctest::Approx(0.0));
}

TEST_CASE("spline collinear control points yields points on line") {
  // Control points on the line y = 2x: (0,0), (1,2), (2,4), (3,6)
  double ctrl[8] = {0, 0, 1, 2, 2, 4, 3, 6};
  double fx, fy;

  evaluate_spline(ctrl, 0.25, fx, fy);
  CHECK(fy == doctest::Approx(2.0 * fx).epsilon(1e-10));

  evaluate_spline(ctrl, 0.5, fx, fy);
  CHECK(fy == doctest::Approx(2.0 * fx).epsilon(1e-10));

  evaluate_spline(ctrl, 0.75, fx, fy);
  CHECK(fy == doctest::Approx(2.0 * fx).epsilon(1e-10));
}

TEST_CASE("spline x and y are independent") {
  // Changing only y control points should not affect x
  double ctrl_a[8] = {1, 0, 2, 0, 3, 0, 4, 0};
  double ctrl_b[8] = {1, 10, 2, 20, 3, 30, 4, 40};
  double fxa, fya, fxb, fyb;

  evaluate_spline(ctrl_a, 0.5, fxa, fya);
  evaluate_spline(ctrl_b, 0.5, fxb, fyb);
  CHECK(fxa == doctest::Approx(fxb).epsilon(1e-10));
  CHECK(fya == doctest::Approx(0.0).epsilon(1e-10));
}
