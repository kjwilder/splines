#ifndef INCLUDES_H
#define INCLUDES_H

//___________________________________________________________________________
// This file contains stuff which ought to be included in every header 
// and source file.

#ifndef USE_ASSERTS
#define NDEBUG
#endif

#include <cassert>

#define STRLEN 255
#define FUZZ 0.001

const int Sint = sizeof(int);
const int Slong = sizeof(long);
const int Sfloat = sizeof(float);
const int Sdouble = sizeof(double);

const double PI_2  = 1.570796326;
const double PI_4  = 0.785398163;
const double PI_8  = 0.392699081;
const double PI_16 = 0.196349540;
const double PI_32 = 0.098174770;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

//___________________________________________________________________________

template <class T>
inline double idistance(T x1, T y1, T x2, T y2)
{
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

//___________________________________________________________________________

template <class T>
inline double idistance_squared(T x1, T y1, T x2, T y2)
{
  return double((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

//___________________________________________________________________________
// This apparently works even if (x1, y1) == (x2, y2)

template <class T>
inline double iangle(T x1, T y1, T x2, T y2)
{
  return atan2(y1 - y2, x2 - x1);
}

//___________________________________________________________________________

// Function prototypes.
extern const char *basename(const char *name);

//___________________________________________________________________________

#endif // INCLUDES_H
