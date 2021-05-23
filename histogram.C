#define HISTOGRAM_C

#include <cmath>
#include "histogram.h"

//___________________________________________________________________________
// Inline function

inline double LOG(double x) { return ((x > 0.0) ? log(x) : 0.0); }

//___________________________________________________________________________

double histogram::entropy()
{
  double t = total();
  if (t == 0.0)
    return 0.0;

  double ent = 0.0;
  for (int i = 0; i < size; ++i)
  {
    double pcnt = counts[i] / t;
    ent -= pcnt * LOG(pcnt);
  }
  return ent;

} // histogram::entropy

//___________________________________________________________________________

double histogram::gini()
{
  double t = total();
  if (t == 0.0)
    return 0.0;

  double gini = 0.0;
  for (int i = 0; i < size; ++i)
  {
    double pcnt1 = counts[i] / t;
    for (int j = i + 1; j < size; ++j)
    {
      double pcnt2 = counts[j] / t;
      gini += 2 * pcnt1 * pcnt2;
    }
  }
  return gini;

} // histogram::gini

//___________________________________________________________________________

double histogram::kolsmir()
{
  double t = total();
  if (t == 0.0)
    return 0.0;

  double kolsmir = 0.0;
  for (int i = 0; i < size; ++i)
  {
    double pcnt1 = counts[i] / t;
    for (int j = i + 1; j < size; ++j)
    {
      double pcnt2 = counts[j] / t;
      kolsmir += 2 * pcnt1 * pcnt2;
    }
  }
  return kolsmir;

} // histogram::kolsmir

//___________________________________________________________________________

double ave_impurity(histogram& h1, histogram& h2, impfunction imf)
{
  double t1 = h1.total();
  double t2 = h2.total();
  double t = t1 + t2;
  return ((t == 0.0) ? 0.0 : (t1 * (h1.*imf)() + t2 * (h2.*imf)()) / t);

} // ave_impurity

//___________________________________________________________________________

double ave_entropy(histogram& h1, histogram& h2)
{
  double t1 = h1.total();
  double t2 = h2.total();
  double t = t1 + t2;
  return ((t == 0.0) ? 0.0 : (t1 * h1.entropy() + t2 * h2.entropy()) / t);

} // ave_impurity

//___________________________________________________________________________

void set_impurity_func(const String& impmeasure, impfunction& impfn,
                       int exitflag)
{
  if (impmeasure == "entropy")
    impfn = &histogram::entropy;
  else if (impmeasure == "gini")
    impfn = &histogram::gini;
  else if (impmeasure == "kolsmir")
    impfn = &histogram::kolsmir;
  else if (exitflag)
  {
    cerr << "Invalid impurity measure " << impmeasure << " specified.\n"
      "Exiting.\n";
    exit(1);
  }
  else
    impfn = &histogram::entropy;

} // set_impurity_func

//___________________________________________________________________________
// histogram.C
