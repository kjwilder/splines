#define PROCESS_C

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

#include "includes.h"
#include "image.h"
#include "process.h"
#include "globals.h"

//___________________________________________________________________________
// Read in the data for a digit.  'index' specified which digit to read in.
// Return before actually reading in the image if 'flag' is set.

int image::read(int index, int flag)
{
  // Simple error check
  if (index < 0)
    return 0;

  // Initialize some parameters.
  int digit_index = index % imagesperfile;
  int file_index = index / imagesperfile;
  String data_file = dataset + "/Images_" + ltos(file_index);

  // Open the data_file.
  ifstream ifs;
  if (!data_dir_open(ifs, data_file, 0))
    return 0;

  // Make sure we're getting the data for a digit that really exists.
  int entries;
  ifs.read(&entries, Sint);
  if (digit_index >= entries)
    return 0;

  // First skip to where the offset to the image data is, read in the offset,
  // and move to the offset.
  ifs.seekg((digit_index + 1) * Sint); 
  int offset;
  ifs.read(&offset, Sint);
  ifs.seekg(offset);

  // Read in the truth of the image.
  int truth;
  ifs.read(&truth, Sint);
  tr = truth - '0';

  // Read in the x-dimension.
  int xdim;
  ifs.read(&xdim, Sint);
  
  // Read in the y-dimension.
  int ydim;
  ifs.read(&ydim, Sint);

  // Read in the image data to set up the readgrid.
  int imsize = (xdim * ydim - 1) / 8 + 1;
  char imdata[imsize];
  ifs.read(&imdata, imsize);

  init(xdim, ydim);
  if (flag)
    return 1;

  for (int j = 0; j < ydim; ++j)
  {
    for (int i = 0; i < xdim; ++i)
    {
      int thebyte = (j * xdim + i) / 8;
      int thebit = 7 - (j * xdim + i) % 8;
      (*this)(i, j) = (imdata[thebyte] >> thebit) & 1;
    } 
  }

  return 1;

} // image::read
//___________________________________________________________________________
// Read in the data for a digit.  'index' specified which digit to read in.
// Return before actually reading in the image if 'flag' is set.

int image::readgrey(int index, int flag)
{
  // Simple error check
  if (index < 0)
    return 0;

  // Initialize some parameters.
  int digit_index = index % imagesperfile;
  int file_index = index / imagesperfile;
  String data_file = dataset + "/Images_" + ltos(file_index);

  // Open the data_file.
  ifstream ifs;
  if (!data_dir_open(ifs, data_file, 0))
    return 0;

  // Make sure we're getting the data for a digit that really exists.
  int entries;
  ifs.read(&entries, Sint);
  if (digit_index >= entries)
    return 0;

  // First skip to where the offset to the image data is, read in the offset,
  // and move to the offset.
  ifs.seekg((digit_index + 1) * Sint); 
  int offset;
  ifs.read(&offset, Sint);
  ifs.seekg(offset);

  // Read in the truth of the image.
  int truth;
  ifs.read(&truth, Sint);
  tr = truth - '0';

  // Read in the x-dimension.
  int xdim;
  ifs.read(&xdim, Sint);
  
  // Read in the y-dimension.
  int ydim;
  ifs.read(&ydim, Sint);

  init(xdim, ydim);
  if (flag)
    return 1;

  for (int j = 0; j < ydim; ++j)
  {
    for (int i = 0; i < xdim; ++i)
    {
      ifs.read(&((*this)(i, j)), 1);
      (*this)(i, j) = 255 - (*this)(i, j);
    } 
  }

  return 1;

} // image::readgrey

//___________________________________________________________________________
// Given a digit name and index, returns the true value of the digit.
// Despite it's size, this routine should execute quite fast.

int get_truth(int digit)
{
  // Determine the name of the data_file and file index.
  int digit_index = digit % imagesperfile;
  int file_index = digit / imagesperfile;
  String data_file = dataset + "/Images_" + ltos(file_index);
 
  // Open the data_file.
  ifstream ifs;
  if (!data_dir_open(ifs, data_file))
    return -1;

  // Make sure we're getting the truth for a digit that really exists.
  int entries;
  ifs.read(&entries, Sint);
  if (!(digit_index < entries))
    return -1;

  // First skip to where the offset to the image data is, read in the offset,
  // and move to the offset.
  ifs.seekg((digit_index + 1) * Sint);
  int offset;
  ifs.read(&offset, Sint);
  ifs.seekg(offset);
 
  // Read in the truth of the image.
  int truth;
  ifs.read(&truth, Sint);

  return truth - '0';

} // get_truth

//___________________________________________________________________________
// Try to open a file, first from the current directory, and then from the 
// directory 'dataname'.  Exit upon failure if 'abrt' flag is set
 
int data_dir_open(ifstream& ifs, const String& dataname, int abrt)
{
  ifs.open(dataname);
  if (ifs)
    return 1;
 
  ifs.open(datadir + "/" + dataname);
  if (!ifs && abrt)
  {
    cerr << "Unable to open [" << dataname << "] from data_dir_open" << endl;
    exit(1);
  }
  else if (ifs)
    return 1;
  else
    return 0;

 
} // data_dir_open
 
//___________________________________________________________________________
// Convert a long to a string, return a pointer to the string.  This string
// is a global object and should be used with care.

char *ltos(const long x)
{
  static char tmpbuf[STRLEN];

  sprintf(tmpbuf, "%li", x);
  return tmpbuf;

} // ltos

//___________________________________________________________________________
// Randomly select 'num' integers from 0..'max'.  Put the values in 'l'
// If max > 250000 replication can occur.

void random_select(lgrid& randgrid, int max, int num)
{
  if (num <= 0 || max <= 0)
  {
    randgrid.init();
    return;
  }

  if (num > max)
  {
    randgrid.init(max);
    for (int i = 0; i < max; ++i)
      randgrid(i) = i;
    return;
  }

  randgrid.init(num);

  if (max > 250000)
  {
    for (int i = 0; i < num; ++i)
      randgrid(i) = lrand48() % max;
    randgrid.sort();
  }
  else
  {
    cgrid tmp(max);

    if (num > max / 2)
    {
      for (int i = 0; i < max; ++i)
	tmp(i) = 1;
      for (int i = 0; i < max - num; ++i)
      {
	long r = lrand48() % max;
	while (!tmp(r)) 
	  r = lrand48() % max;
	tmp(r) = 0;
      }
    }
    else
    {
      tmp.clear();
      for (int i = 0; i < num; ++i)
      {
	long r = lrand48() % max;
	while (tmp(r))
	  r = lrand48() % max;
	tmp(r) = 1;
      }
    }
    int index = 0;
    for (int i = 0; i < max; ++i)
      if (tmp(i))
	randgrid(index++) = i;
  }

} // random_select

//___________________________________________________________________________
// process.c
