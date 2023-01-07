#define MAIN_C

//___________________________________________________________________________
// Include files
 
#define MAIN_DEFINED

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cstring>

#include "includes.h"
#include "image.h"
#include "spline.h"
#include "globals.h"

//___________________________________________________________________________

int main(int argc, char** argv)
{
  if (!set_parameter_file_arg_globals(argc, argv)) {
    exit(1);
  }
  set_command_line_globals(argc, argv);
  // Dump the values of global variables to the dumpfile.
  // dump_globals(dumpfile);


  // Seed the random number generator.
  if (seed == 0)
    seed = time(0);
  srand48((time_t) seed);
  
  // Set the impurity function
  // impfunction impfn;
  // set_impurity_func(impmeasure, impfn, 0);

  // Perform the desired option.
  switch (opt)
  {
   case 0:
    cout << "No option specified.  Possible options are:\n";
    cout << "  1: View a test image.\n";
    cout << "  2: View a test spline.\n";
    exit(0);
    break;

   case 1:
  {
    int size = (dum1 == 0 ? 25 : dum1);
    image x(size, size);
    for (int i = 0; i < size; ++i)
      for (int j = 0; j < size; ++j)
	x(i, j) = (i + j) % 255;
    x.normalize(124);
    x.view("test image");
  }
    break;

   case 2:
  {
    spline s(0, 0, 1, 1, 2, 2, 3, 3);
    s.view("test spline");
  }
    break;
    

   default:
    cerr << "Invalid option [" << opt << "] chosen." << endl;
    break;
  }
  
  return 0;

} // main

//___________________________________________________________________________
// main.C
