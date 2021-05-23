#define LANDMARK_C

#include <ctype.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "includes.h"
#include "process.h"
#include "landmark.h"
#include "globals.h"

using namespace std;

//_________________________________________________________________________*/
// Finds the 'index' landmark in the file 'lmfilename' and puts it's
// data in 'currlm'.  'index' starts at 0.

int landmarks::read(int index)
{
  // Simple error check
  if (index < 0)
    return 0;

  // Initialize some parameters.
  int land_index = index % 100;
  int file_index = index / 100;
  string data_file = landdir + "/Lands_" + ltos(file_index);

  // Open the landmark file.
  ifstream ifs;
  if (!data_dir_open(ifs, data_file, 0))
    return 0;

  // Make sure we're getting the data for a landmark that really exists.
  int entries;
  ifs.read(&entries, Sint);
  if (land_index >= entries)
    return 0;
 
  // First skip to where the offset to the landmarks are, read in the offset,
  // and move to the offset.
  ifs.seekg((land_index + 1) * Sint);
  int offset;
  ifs.read(&offset, Sint);
  ifs.seekg(offset);
 
  // Determine the true value of the data with these landmarks.
  ifs.read(&value, Sint);
  if (ifs.gcount() != Sint)
    return 0;
 
  // Read in the typecounts
  if (!typecounts.read(ifs))
    return 0;
 
  // Determine the typeoffsets
  offset = 0;
  typeoffsets.init(typecounts.dimx());
  for (int i = 0; i < typecounts.dimx(); ++i)
  {
    typeoffsets(i) = offset;
    offset += typecounts(i);
  }
 
  // Read in the locations
  if (!locations.read(ifs))
    return 0;
 
  return 1;

} // read

//___________________________________________________________________________

int landmarks::size()
{
  return (Sint + typecounts.size() + locations.size());

} // landmarks::size

//___________________________________________________________________________

int landmarks::write(ofstream& ofs)
{
  ofs.write(&value, Sint);
  if (typecounts.write(ofs) && locations.write(ofs))
    return 1;
  else
    return 0;

} // landmarks::write

//___________________________________________________________________________
// Prints out the contents of a landmark

void landmarks::dump()
{
  // Print out the landmark data.
  cout << "Truth is [" << truth();
  if (isprint(truth())) 
    cout << " (" << char(truth()) << ")";
  cout << "]\n";
  cout << "Number of entries is [" << total_types() << "]\n";
  if (total_types() > 0)
  {
    for (int i = 0; i < total_types(); ++i)
    {
      cout << "  (" << i << ") - Number = [" << type_count(i)
	<< "], Offset = [" << type_offset(i) << "]\n";
    }
    cout << "There are " << total_instances() << " total instances.\n";
    if (total_instances > 0)
    {
      cout << "The locations are the following:\n";
      for (int i = 0; i < total_instances(); i += 7)
      {
	for (int j = 0; j < 7 && i + j < total_instances(); ++j)
	  cout << "  [" << setw(2) << int(location_x(i + j)) << ", " 
            << setw(2) << int(location_y(i + j)) << "] ";
	cout << endl;
      }
    }
  }

} // dump_lm

#ifdef 0

//___________________________________________________________________________
// Given a list of landmark locations 'locs', combine all the instances
// of landmark 'lmind' in the 'clustsize' sized box at (startx, starty)
// into a single instance.

void cluster_lm(instances *locs, int lmind, int clustsize, 
               int x1, int y1, int x2, int y2)
{
  // Use clustsize = 1 to get rid of duplicate instances.
  if (clustsize < 1)
    return;
  
  typedef instances *instancesptr;
  typedef grid<instancesptr> locptrgrid;

  int i, j;
  int sizex = (x2 - x1) / clustsize + 1;
  int sizey = (y2 - y1) / clustsize + 1;
  locptrgrid firstlocs(sizex, sizey);
  igrid numfound(sizex, sizey), homex(sizex, sizey), homey(sizex, sizey);
  fgrid cenx(sizex, sizey), ceny(sizex, sizey);
  for (i = 0; i < sizex; ++i)
    for (j = 0; j < sizey; ++j)
    {
      firstlocs(i, j) = 0;
      homex(i, j) = homey(i, j) = numfound(i, j) = 0;
      cenx(i, j) = ceny(i, j) = 0.0;
    }
  
  instances *tmplocs = locs;
  instances **tmpadd = &locs;
  while (tmplocs != 0)
  {
    if (tmplocs->mask == lmind)
    {
      i = (tmplocs->x - x1) / clustsize;
      if (i < 0)
        i = 0;
      if (i > sizex - 1)
        i = sizex - 1;
      j = (tmplocs->y - y1) / clustsize;
      if (j < 0)
        j = 0;
      if (j > sizey - 1)
        j = sizey - 1;
      ++numfound(i, j);
      homex(i, j) += tmplocs->x;
      homey(i, j) += tmplocs->y;
      cenx(i, j) += tmplocs->fx;
      ceny(i, j) += tmplocs->fy;
      
      // Keep trace of the first instance, delete all others.
      if (numfound(i, j) == 1)
      {
        firstlocs(i, j) = tmplocs;
        tmpadd = &tmplocs->next;
        tmplocs = tmplocs->next;
      }
      else
      {
        instances *oldadd = tmplocs;
        tmplocs = tmplocs->next;
        *tmpadd = tmplocs;
        delete oldadd;
      }
    }
    else
    {
      tmpadd = &tmplocs->next;
      tmplocs = tmplocs->next;
    }
  }
  
  // Initialize the upper left corner and center of mass of the 
  // clustered instance.

  for (i = 0; i < sizex; ++i)
    for (j = 0; j < sizey; ++j)
      if (numfound(i, j) > 1)
      {
        firstlocs(i, j)->x = homex(i, j) / numfound(i, j);
        firstlocs(i, j)->y = homey(i, j) / numfound(i, j);
        firstlocs(i, j)->fx = cenx(i, j) / numfound(i, j);
        firstlocs(i, j)->fy = ceny(i, j) / numfound(i, j);
      }
  
} // cluster_lm

//___________________________________________________________________________

void make_images()
{
  for (int i = 0; i < NUMCLASS; ++i)
  {
    number num;
    number procnum[100];
    read_charac(i, num);
    for (int j = 0; j < 100; ++j)
      std_process(num, procnum[j], 0, 0, 0, 0, imrotate, 0);
    char datafile[80];
    sprintf(datafile, "Images_%d", i);
    ofstream ofs(datafile);
    
    int num_images = 100;
    ofs.write(&num_images, 4);
    
    int offset = 4 * 101;
    ofs.write(&offset, 4);
    
    for (int j = 1; j < 100; ++j)
    {
      offset += (12 + (procnum[j - 1].dimx() * 
                       procnum[j - 1].dimy() - 1) / 8 + 1);
      ofs.write(&offset, 4);
    }
    
    for (int j = 0; j < 100; ++j)
    {
      char currbit = 7;
      char currbyte = 0;
      int counter = 0;
      
      int truth = i;
      ofs.write(&truth, 4);
      
      int xdim = procnum[j].dimx();
      ofs.write(&xdim, 4);
      
      int ydim = procnum[j].dimy();
      ofs.write(&ydim, 4);
      
      for (int n = 0; n < ydim; ++n)
        for (int m = 0; m < xdim; ++m)
        {
          currbyte |= ((procnum[j](m, n) > 0) << currbit);
          if (currbit == 0)
          {
            ++counter;
            ofs.write(&currbyte, 1);
            currbit = 7; currbyte = 0;
          }
          else
            --currbit;
        }
      if (currbit != 7)
      {
        ++counter; ofs.write(&currbyte, 1);
      }
    }
  }
  
} // make_images

#endif
//___________________________________________________________________________
// landmark.c
