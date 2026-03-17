#include "doctest.h"
#include "../includes.h"

TEST_CASE("idistance") {
  CHECK(idistance(0, 0, 3, 4) == doctest::Approx(5.0));
  CHECK(idistance(0, 0, 0, 0) == doctest::Approx(0.0));
  CHECK(idistance(1, 1, 4, 5) == doctest::Approx(5.0));
  CHECK(idistance(-3, -4, 0, 0) == doctest::Approx(5.0));
  CHECK(idistance(0.0, 0.0, 1.0, 1.0) == doctest::Approx(sqrt(2.0)));
}

TEST_CASE("idistance_squared") {
  CHECK(idistance_squared(0, 0, 3, 4) == doctest::Approx(25.0));
  CHECK(idistance_squared(0, 0, 0, 0) == doctest::Approx(0.0));
  CHECK(idistance_squared(1, 1, 4, 5) == doctest::Approx(25.0));
  // Consistent with idistance
  double d = idistance(2, 3, 7, 11);
  CHECK(idistance_squared(2, 3, 7, 11) == doctest::Approx(d * d));
}

TEST_CASE("iangle") {
  // Pointing right: angle should be 0
  CHECK(iangle(0, 0, 1, 0) == doctest::Approx(0.0));
  // Pointing left: angle should be pi
  CHECK(iangle(1, 0, 0, 0) == doctest::Approx(M_PI).epsilon(0.0001));
  // Pointing up (screen coords: y1 > y2 means up in atan2 convention)
  CHECK(iangle(0, 1, 0, 0) == doctest::Approx(M_PI / 2.0).epsilon(0.0001));
  // Same point: atan2(0,0) returns 0
  CHECK(iangle(0, 0, 0, 0) == doctest::Approx(0.0));
}
