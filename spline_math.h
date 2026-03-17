#ifndef SPLINE_MATH_H
#define SPLINE_MATH_H

// Evaluate a cubic Lagrange interpolation spline at parameter t (in [0,1])
// with equally-spaced knots at 0, 1/3, 2/3, 1.
// Control points stored as ctrl[8] = {x0,y0, x1,y1, x2,y2, x3,y3}.
// Results returned via fx and fy.

inline void evaluate_spline(const double ctrl[8], double t,
                            double &fx, double &fy)
{
  const double p1 = 1.0 / 3.0;
  const double p2 = 2.0 / 3.0;

  // Lagrange basis functions at equally-spaced knots {0, 1/3, 2/3, 1}.
  double b0 = (p1 - t) * (p2 - t) * (1.0 - t) *  9.0 / 2.0;
  double b1 = t        * (p2 - t) * (1.0 - t) * 27.0 / 2.0;
  double b2 = t        * (t - p1) * (1.0 - t) * 27.0 / 2.0;
  double b3 = t        * (t - p1) * (t - p2)  *  9.0 / 2.0;

  fx = ctrl[0] * b0 + ctrl[2] * b1 + ctrl[4] * b2 + ctrl[6] * b3;
  fy = ctrl[1] * b0 + ctrl[3] * b1 + ctrl[5] * b2 + ctrl[7] * b3;
}

#endif // SPLINE_MATH_H
