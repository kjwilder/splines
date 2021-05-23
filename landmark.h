#ifndef LANDMARK_H
#define LANDMARK_H

#include <iostream>
#include <fstream>
#include "grid.h"
#include "codes.h"

using namespace std;

//___________________________________________________________________________
// A single landmark (code)

class landmark
{
 public:
  int index;    // Which landmark is it?
  int cx, cy;   // The center of mass of the landmark

  landmark() : index(0), cx(0), cy(0) { }
};

//___________________________________________________________________________
// Arrays of landmarks and instances

class landmarks
{
 private:
  int value;
  igrid typecounts;
  igrid typeoffsets;
  ucgrid locations;

  landmarks(const landmarks&) { exit(1); }
  void operator=(const landmarks&) const { exit(1); }

 public:
  landmarks() : value(0) { }
  void init_counts(int num) { typecounts.init(num); }
  void init_offsets(int num) { typeoffsets.init(num); }
  void init_locations(int num) { locations.init(num, 2); }

  const int& truth() const { return value; }
  void set_truth(int val) { value = val; }

  int total_types() const { return typecounts.dimx(); }
  int& type_count(int t) const { return typecounts(t); }
  int& type_offset(int t) const { return typeoffsets(t); }

  int total_instances() const { return locations.dimx(); }
  uchar& location_x(int t) const { return locations(t, 0); }
  uchar& location_y(int t) const { return locations(t, 1); }
  uchar& location_x(int t, int ind) const
  { return location_x(typeoffsets(t) + ind); }
  uchar& location_y(int t, int ind) const
  { return location_y(typeoffsets(t) + ind); }

  int read(int index);
  int write(ofstream& ofs);
  int size();
  void dump();

  void extract(ucgrid& im, codestree& ct, int codesize);

};

//___________________________________________________________________________
// Function prototypes

typedef vector<landmark> lmarklist;
typedef vector<landmarks> lmarkslist;
void cluster_landmarks(lmarklist& lands, int clustsize, 
		       int numtypes, int& total);

//___________________________________________________________________________
// landmark.h

#endif // LANDMARK_H

