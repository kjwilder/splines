#include "doctest.h"

#include <cstdio>
#include <cstring>
#include <fstream>

#include "../grid.h"

// =========================================================================
// Construction and dimensions
// =========================================================================

TEST_CASE("grid default constructor") {
  igrid g;
  CHECK(g.dimx() == 0);
  CHECK(g.dimy() == 0);
}

TEST_CASE("grid 1D constructor") {
  igrid g(5);
  CHECK(g.dimx() == 5);
  CHECK(g.dimy() == 1);
}

TEST_CASE("grid 2D constructor") {
  igrid g(3, 4);
  CHECK(g.dimx() == 3);
  CHECK(g.dimy() == 4);
}

// =========================================================================
// Element access
// =========================================================================

TEST_CASE("grid operator() read/write") {
  igrid g(3, 3);
  g.clear();
  g(0, 0) = 10;
  g(2, 2) = 99;
  CHECK(g(0, 0) == 10);
  CHECK(g(2, 2) == 99);
  CHECK(g(1, 1) == 0);
}

TEST_CASE("grid set and get") {
  igrid g(4, 4);
  g.clear();
  g.set(2, 3, 42);
  CHECK(g.get(2, 3) == 42);
  // Out-of-range get returns 0
  CHECK(g.get(-1, 0) == 0);
  CHECK(g.get(0, -1) == 0);
  CHECK(g.get(4, 0) == 0);
  CHECK(g.get(0, 4) == 0);
}

TEST_CASE("grid set with double coords truncates") {
  igrid g(4, 4);
  g.clear();
  g.set(1.9, 2.7, 55);
  CHECK(g.get(1, 2) == 55);
}

// =========================================================================
// Copy and assignment
// =========================================================================

TEST_CASE("grid copy constructor") {
  igrid g(3, 2);
  g.clear();
  g(0, 0) = 1;
  g(2, 1) = 2;

  igrid h(g);
  CHECK(h.dimx() == 3);
  CHECK(h.dimy() == 2);
  CHECK(h(0, 0) == 1);
  CHECK(h(2, 1) == 2);

  // Verify independence
  h(0, 0) = 99;
  CHECK(g(0, 0) == 1);
}

TEST_CASE("grid operator=") {
  igrid g(3, 2);
  g.clear();
  g(1, 1) = 7;

  igrid h;
  h = g;
  CHECK(h.dimx() == 3);
  CHECK(h.dimy() == 2);
  CHECK(h(1, 1) == 7);

  // Self-assignment
  h = h;
  CHECK(h(1, 1) == 7);
}

// =========================================================================
// Move operator<<
// =========================================================================

TEST_CASE("grid operator<< moves data") {
  igrid g(3, 2);
  g.clear();
  g(1, 0) = 42;

  igrid h;
  h << g;
  CHECK(h.dimx() == 3);
  CHECK(h.dimy() == 2);
  CHECK(h(1, 0) == 42);
  // Source should be empty
  CHECK(g.dimx() == 0);
  CHECK(g.dimy() == 0);
}

// =========================================================================
// Clear and init
// =========================================================================

TEST_CASE("grid clear zeros elements") {
  igrid g(3, 3);
  g(0, 0) = 1;
  g(1, 1) = 2;
  g(2, 2) = 3;
  g.clear();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      CHECK(g(i, j) == 0);
}

TEST_CASE("grid init resizes") {
  igrid g(10, 10);
  g.clear();
  g(0, 0) = 5;
  g.init(2, 3);
  CHECK(g.dimx() == 2);
  CHECK(g.dimy() == 3);
}

TEST_CASE("grid init to zero") {
  igrid g(5, 5);
  g.init(0, 0);
  CHECK(g.dimx() == 0);
  CHECK(g.dimy() == 0);
}

// =========================================================================
// Arithmetic
// =========================================================================

TEST_CASE("grid operator+=") {
  igrid a(2, 2);
  a.clear();
  a(0, 0) = 1; a(1, 0) = 2;
  a(0, 1) = 3; a(1, 1) = 4;

  igrid b(2, 2);
  b.clear();
  b(0, 0) = 10; b(1, 0) = 20;
  b(0, 1) = 30; b(1, 1) = 40;

  a += b;
  CHECK(a(0, 0) == 11);
  CHECK(a(1, 0) == 22);
  CHECK(a(0, 1) == 33);
  CHECK(a(1, 1) == 44);
}

TEST_CASE("grid operator+") {
  igrid a(2, 2);
  a.clear();
  a(0, 0) = 1; a(1, 0) = 2;
  a(0, 1) = 3; a(1, 1) = 4;

  igrid b(2, 2);
  b.clear();
  b(0, 0) = 5; b(1, 0) = 6;
  b(0, 1) = 7; b(1, 1) = 8;

  igrid c = a + b;
  CHECK(c(0, 0) == 6);
  CHECK(c(1, 0) == 8);
  CHECK(c(0, 1) == 10);
  CHECK(c(1, 1) == 12);
}

TEST_CASE("grid operator-") {
  igrid a(2, 2);
  a.clear();
  a(0, 0) = 10; a(1, 0) = 20;
  a(0, 1) = 30; a(1, 1) = 40;

  igrid b(2, 2);
  b.clear();
  b(0, 0) = 1; b(1, 0) = 2;
  b(0, 1) = 3; b(1, 1) = 4;

  igrid c = a - b;
  CHECK(c(0, 0) == 9);
  CHECK(c(1, 0) == 18);
  CHECK(c(0, 1) == 27);
  CHECK(c(1, 1) == 36);
}

// =========================================================================
// Matrix multiply
// =========================================================================

TEST_CASE("grid matrix multiply 2x2") {
  // A = [1 2; 3 4], B = [5 6; 7 8]
  // grid uses (x,y) where x=column, y=row, stored row-major as sto[y*nx+x]
  dgrid a(2, 2);
  a(0, 0) = 1; a(1, 0) = 2;
  a(0, 1) = 3; a(1, 1) = 4;

  dgrid b(2, 2);
  b(0, 0) = 5; b(1, 0) = 6;
  b(0, 1) = 7; b(1, 1) = 8;

  dgrid c = a * b;
  CHECK(c.dimx() == 2);
  CHECK(c.dimy() == 2);
  // C(i,j) = sum_k A(k,j) * B(i,k)
  // C(0,0) = A(0,0)*B(0,0) + A(1,0)*B(0,1) = 1*5 + 2*7 = 19
  CHECK(c(0, 0) == doctest::Approx(19.0));
  // C(1,0) = A(0,0)*B(1,0) + A(1,0)*B(1,1) = 1*6 + 2*8 = 22
  CHECK(c(1, 0) == doctest::Approx(22.0));
  // C(0,1) = A(0,1)*B(0,0) + A(1,1)*B(0,1) = 3*5 + 4*7 = 43
  CHECK(c(0, 1) == doctest::Approx(43.0));
  // C(1,1) = A(0,1)*B(1,0) + A(1,1)*B(1,1) = 3*6 + 4*8 = 50
  CHECK(c(1, 1) == doctest::Approx(50.0));
}

TEST_CASE("grid identity multiply") {
  dgrid a(2, 2);
  a(0, 0) = 3; a(1, 0) = 7;
  a(0, 1) = 2; a(1, 1) = 5;

  dgrid id(2, 2);
  id.clear();
  id(0, 0) = 1; id(1, 1) = 1;

  dgrid c = a * id;
  CHECK(c(0, 0) == doctest::Approx(3.0));
  CHECK(c(1, 0) == doctest::Approx(7.0));
  CHECK(c(0, 1) == doctest::Approx(2.0));
  CHECK(c(1, 1) == doctest::Approx(5.0));
}

// =========================================================================
// Normalize
// =========================================================================

TEST_CASE("grid normalize") {
  igrid g(3, 1);
  g(0) = 10;
  g(1) = 20;
  g(2) = 50;
  g.normalize(100);
  CHECK(g(0) == 20);
  CHECK(g(1) == 40);
  CHECK(g(2) == 100);
}

TEST_CASE("grid normalize all zeros") {
  igrid g(3, 1);
  g.clear();
  g.normalize(100);  // Should not crash
  CHECK(g(0) == 0);
  CHECK(g(1) == 0);
  CHECK(g(2) == 0);
}

// =========================================================================
// Transpose
// =========================================================================

TEST_CASE("grid transpose") {
  igrid g(2, 3);
  g.clear();
  g(0, 0) = 1; g(1, 0) = 2;
  g(0, 1) = 3; g(1, 1) = 4;
  g(0, 2) = 5; g(1, 2) = 6;

  igrid t = g.transpose();
  CHECK(t.dimx() == 3);
  CHECK(t.dimy() == 2);
  CHECK(t(0, 0) == 1);
  CHECK(t(1, 0) == 3);
  CHECK(t(2, 0) == 5);
  CHECK(t(0, 1) == 2);
  CHECK(t(1, 1) == 4);
  CHECK(t(2, 1) == 6);
}

TEST_CASE("grid transpose 1x1") {
  igrid g(1, 1);
  g(0, 0) = 42;
  igrid t = g.transpose();
  CHECK(t.dimx() == 1);
  CHECK(t.dimy() == 1);
  CHECK(t(0, 0) == 42);
}

// =========================================================================
// LU decomposition
// =========================================================================

TEST_CASE("grid LU decomposition 2x2") {
  // A = [4 3; 6 3], grid(x=col, y=row)
  dgrid a(2, 2);
  a(0, 0) = 4; a(1, 0) = 3;
  a(0, 1) = 6; a(1, 1) = 3;

  dgrid lu = a.LU();
  // U(0,0) = 4
  CHECK(lu(0, 0) == doctest::Approx(4.0));
  // L(1,0) stored at lu(1,0) = A(1,0)/A(0,0) = 3/4 = 0.75
  CHECK(lu(1, 0) == doctest::Approx(0.75));
  // U(0,1) = 3
  CHECK(lu(0, 1) == doctest::Approx(6.0));
  // U(1,1) = A(1,1) - L(1,0)*U(0,1) = 3 - 0.75*6 = -1.5
  CHECK(lu(1, 1) == doctest::Approx(-1.5));
}

// =========================================================================
// Inverse
// =========================================================================

TEST_CASE("grid inverse 2x2") {
  dgrid a(2, 2);
  a(0, 0) = 4; a(1, 0) = 7;
  a(0, 1) = 2; a(1, 1) = 6;

  dgrid inv = a.inverse();
  dgrid prod = a * inv;

  CHECK(prod(0, 0) == doctest::Approx(1.0).epsilon(1e-10));
  CHECK(prod(1, 0) == doctest::Approx(0.0).epsilon(1e-10));
  CHECK(prod(0, 1) == doctest::Approx(0.0).epsilon(1e-10));
  CHECK(prod(1, 1) == doctest::Approx(1.0).epsilon(1e-10));
}

TEST_CASE("grid inverse 3x3") {
  dgrid a(3, 3);
  a(0, 0) = 1; a(1, 0) = 2; a(2, 0) = 3;
  a(0, 1) = 0; a(1, 1) = 1; a(2, 1) = 4;
  a(0, 2) = 5; a(1, 2) = 6; a(2, 2) = 0;

  dgrid inv = a.inverse();
  dgrid prod = a * inv;

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      CHECK(prod(i, j) == doctest::Approx(i == j ? 1.0 : 0.0).epsilon(1e-10));
}

// =========================================================================
// Sort
// =========================================================================

TEST_CASE("grid sort") {
  igrid g(5);
  g(0) = 3; g(1) = 1; g(2) = 4; g(3) = 1; g(4) = 5;
  g.sort();
  CHECK(g(0) == 1);
  CHECK(g(1) == 1);
  CHECK(g(2) == 3);
  CHECK(g(3) == 4);
  CHECK(g(4) == 5);
}

TEST_CASE("grid sort already sorted") {
  igrid g(4);
  g(0) = 1; g(1) = 2; g(2) = 3; g(3) = 4;
  g.sort();
  CHECK(g(0) == 1);
  CHECK(g(1) == 2);
  CHECK(g(2) == 3);
  CHECK(g(3) == 4);
}

TEST_CASE("grid sort single element") {
  igrid g(1);
  g(0) = 42;
  g.sort();
  CHECK(g(0) == 42);
}

// =========================================================================
// On/off pixels
// =========================================================================

TEST_CASE("grid onpixels and offpixels") {
  igrid g(3, 3);
  g.clear();
  g(0, 0) = 1;
  g(1, 1) = 2;
  g(2, 2) = 3;
  CHECK(g.onpixels() == 3);
  CHECK(g.offpixels() == 6);
}

// =========================================================================
// Size
// =========================================================================

TEST_CASE("grid size") {
  igrid g(3, 4);
  int expected = 2 * sizeof(int) + 3 * 4 * sizeof(int);
  CHECK(g.size() == expected);
}

// =========================================================================
// Binary I/O
// =========================================================================

TEST_CASE("grid write and read file") {
  const char *tmpfile = "/tmp/test_grid_io.bin";

  igrid g(3, 2);
  g.clear();
  g(0, 0) = 10; g(1, 0) = 20; g(2, 0) = 30;
  g(0, 1) = 40; g(1, 1) = 50; g(2, 1) = 60;
  g.write(tmpfile);

  igrid h;
  CHECK(h.read(tmpfile) == 1);
  CHECK(h.dimx() == 3);
  CHECK(h.dimy() == 2);
  CHECK(h(0, 0) == 10);
  CHECK(h(1, 0) == 20);
  CHECK(h(2, 0) == 30);
  CHECK(h(0, 1) == 40);
  CHECK(h(1, 1) == 50);
  CHECK(h(2, 1) == 60);

  std::remove(tmpfile);
}

TEST_CASE("grid write and read stream") {
  const char *tmpfile = "/tmp/test_grid_stream_io.bin";

  dgrid g(2, 2);
  g(0, 0) = 1.5; g(1, 0) = 2.5;
  g(0, 1) = 3.5; g(1, 1) = 4.5;

  {
    std::ofstream ofs(tmpfile, std::ios::binary);
    CHECK(g.write(ofs) == 1);
  }

  dgrid h;
  {
    std::ifstream ifs(tmpfile, std::ios::binary);
    CHECK(h.read(ifs) == 1);
  }

  CHECK(h.dimx() == 2);
  CHECK(h.dimy() == 2);
  CHECK(h(0, 0) == doctest::Approx(1.5));
  CHECK(h(1, 0) == doctest::Approx(2.5));
  CHECK(h(0, 1) == doctest::Approx(3.5));
  CHECK(h(1, 1) == doctest::Approx(4.5));

  std::remove(tmpfile);
}

TEST_CASE("grid read nonexistent file") {
  igrid g;
  CHECK(g.read("/tmp/nonexistent_grid_file_xyz.bin") == 0);
}

// =========================================================================
// Type aliases
// =========================================================================

TEST_CASE("grid unsigned char (ucgrid)") {
  ucgrid g(2, 2);
  g.clear();
  g(0, 0) = 255;
  g(1, 1) = 128;
  CHECK(g(0, 0) == 255);
  CHECK(g(1, 1) == 128);
  CHECK(g(0, 1) == 0);
}

TEST_CASE("grid double (dgrid)") {
  dgrid g(2, 1);
  g(0) = 3.14;
  g(1) = 2.71;
  CHECK(g(0) == doctest::Approx(3.14));
  CHECK(g(1) == doctest::Approx(2.71));
}
