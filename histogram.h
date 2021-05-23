#ifndef HISTOGRAM_H
#define HISTOGRAM_H

//___________________________________________________________________________
// Include files

#include <iostream>
#include <string>
#include "includes.h"

//___________________________________________________________________________
// Local declarations and typedefs

class histogram;
typedef double (histogram::*impfunction)();

//___________________________________________________________________________

class histogram
{
 private:
  int size;
  int *counts;

 public:

  histogram(int s);
  ~histogram();
  void operator=(const histogram& m);
  int& operator[](int i);
  int total();
  int mode();
  int mode2();
  int pure(double p = 1.0);
  double entropy();
  double gini();
  double kolsmir();
  void write(ostream& os);
  void read(istream& is);
  void clear();
};

//___________________________________________________________________________

inline histogram::histogram(int s)
{
  assert(s > 0);
  size = s;
  counts = new int[size];
  for (int i = 0; i < s; ++i)
    counts[i] = 0;
}

//___________________________________________________________________________

inline histogram::~histogram()
{
  delete [] counts;
}

//___________________________________________________________________________

inline void histogram::operator=(const histogram& m)
{
  assert(size == m.size);

  if (size != m.size)
    return;

  for (int i = 0; i < size; ++i)
    counts[i] = m.counts[i];
}

//___________________________________________________________________________

inline int& histogram::operator[](int i)
{
  assert(i >= 0 && i < size);
  return counts[i];
}

//___________________________________________________________________________

inline int histogram::total()
{
  int t = 0;
  for (int i = 0; i < size; ++i)
    t += counts[i];
  return t;
}

//___________________________________________________________________________

inline int histogram::mode()
{
  int m = 0;
  int count = -1;
  for (int i = 0; i < size; ++i)
    if (counts[i] > count)
    {
      count = counts[i];
      m = i;
    }
  return m;
}
//___________________________________________________________________________

inline int histogram::mode2()
{
  int m1 = mode();
  int m2 = 0;
  int count2 = -1;
  for (int i = 0; i < size; ++i)
    if (counts[i] > count2 && i != m1)
    {
      count2 = counts[i];
      m2 = i;
    }
  return m2;
}

//___________________________________________________________________________

inline int histogram::pure(double p)
{
  double t = total();
  for (int i = 0; i < size; ++i)
    if (counts[i] / t >= p)
      return 1;
  return 0;
}

//___________________________________________________________________________

inline void histogram::write(ostream& os)
{
  os.write(&size, Sint);
  os.write(counts, Sint * size);
}

//___________________________________________________________________________

inline void histogram::read(istream& is)
{
  is.read(&size, Sint);
  is.read(counts, Sint * size);
}

//___________________________________________________________________________

inline void histogram::clear()
{
  for (int i = 0; i < size; ++i)
    counts[i] = 0;
}

//___________________________________________________________________________
// in histogram.C

extern double ave_impurity(histogram& h1, histogram& h2, impfunction imf);
extern double ave_entropy(histogram& h1, histogram& h2);
extern void set_impurity_func(const String& impmeasure, impfunction& impfn,
                              int exitflag = 1);

//___________________________________________________________________________
// histogram.h
#endif
