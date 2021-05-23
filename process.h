#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>

#include "grid.h"

//___________________________________________________________________________
// Function prototypes

extern int data_dir_open(ifstream& ifs, const String& datafile, int abrt = 1);
extern int get_truth(int digit);
extern char *ltos(const long x);
extern void random_select(lgrid& randgrid, int max, int num);

//___________________________________________________________________________
// Template Function

template <class T>
void grid_select(grid<T>& oldgrid, grid<T>& newgrid, int number)
{
  // Select 'number' elements from the range 0..oldgrid.dimx
  random_select(newgrid, oldgrid.dimx(), number);

  // Set the i_th element of newgrid equal to the newgrid(i)_th element
  // of oldgrid
  for (int i = 0; i < newgrid.dimx(); ++i)
    newgrid(i) = oldgrid(newgrid(i));

} // grid_select

//___________________________________________________________________________
// process.h
#endif
