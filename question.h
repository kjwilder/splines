#ifndef QUESTION_H
#define QUESTION_H

#include <fstream>
#include <string>
#include <math.h>
#include "grid.h"
#include "globals.h"

extern fgrid angle, distance;

using namespace std;

//___________________________________________________________________________
// relation = angle + tolerance + distance parameter along with the landmarks,
// graph index, or vertex or edge indices which are related

class relation
{
 public:
  char graph;
  int land1, land2;
  char v1, v2, v3, v4;
  char ang;  // 0, PI_4, ..., 7*PI_4
  char tol;  // PI_2, PI_4, PI_8, PI_16, PI_32
  char dis;  // 0, 1, or 2

  relation() { memset(this, 0, sizeof(relation)); }
  void read(istream& is) { is.read(this, sizeof(relation)); }
  void write(ostream& os) { os.write(this, sizeof(relation)); }
  float relangle() { return ang * PI_4; }
  float reltolerance() { return PI_2 / pow(2.0, tol); }
  inline int check(char x1, char y1, char x2, char y2);
};


//___________________________________________________________________________

inline int relation::check(char x1, char y1, char x2, char y2)
{
  // Check the trivial case where both points are the same
  if (dis != 2 && x1 == x2 && y1 == y2)
    return 1;

  // Determine the angle between the two points
//  float ang = angle(x1, y1, x2, y2);
  float ang = angle(x2 - x1 + 40, y2 - y1 + 40);
  if (ang < 0.0) ang += PI2;

  // Return 0 now if the angle between the two points is not within 
  // 'reltolerance' of the angle 'relangle'
  if (fabs(ang - relangle()) > reltolerance() &&
      fabs(ang - relangle() < PI2 - reltolerance()))
    return 0;
  
  // If we got this far the angle is good, so check the distance
  // If the distance parameter is 0, we return 1 immediately
  if (dis == 0)
    return 1;

//  float dist = distance(x1, y2, x2, y2);
  float dist = distance(x2 - x1 + 40, y2 - y1 + 40);
  if (dis == 1 && dist < near)
    return 1;
  if (dis == 2 && dist > far)
    return 1;

  // If we got this far, the relation is not satisfied
  return 0;

} // relation::check

//___________________________________________________________________________
// question = type + 1 or 2 relations

class question
{
 public:
  int typ;
  relation r1, r2;
  question() : typ(0) { }
  void write(ostream& os);
  void read(istream& is);
};

//___________________________________________________________________________
// question.h

#endif // QUESTION_H

