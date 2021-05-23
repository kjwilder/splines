#define AGGREGATE_C

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <string>

#include "includes.h"
#include "grid.h"
#include "globals.h"


//___________________________________________________________________________

void aggregate_nntest()
{
  // Read in the relevant grids
  std::cerr << "Reading in the grids\n";
  fgrid aggtrain, aggtest;
  igrid badtest;
  if (!aggtrain.read("aggtrain") || !aggtest.read("aggtest") || 
      !badtest.read("badtest"))
  {
    std::cerr << "Couldn't open aggtrain or aggtest or badtest\n";
    exit(1);
  }

  // Normalize the relevant grids
  std::cerr << "Normalizing the grids\n";
  float total = 0;
  for (int i = 0; i < aggtrain.dimx() - 1; ++i)
    total += aggtrain(i, 0);
  for (int i = 0; i < aggtrain.dimx() - 1; ++i)
    for (int j = 0; j < aggtrain.dimy(); ++j)
      aggtrain(i, j) /= total;
  total = 0;
  for (int i = 0; i < aggtest.dimx() - 1; ++i)
    total += aggtest(i, 0);
  for (int i = 0; i < aggtest.dimx() - 1; ++i)
    for (int j = 0; j < aggtest.dimy(); ++j)
      aggtest(i, j) /= total;

  dum1 = 25;
  std::cerr << "Calculating " << dum1 << " nearest neighbors\n";
  float distance;
  igrid bestind(dum1);
  fgrid bestdist(dum1);
  for (int i = 0; i < badtest.dimx(); ++i)
  {
    for (int j = 0; j < dum1; ++j)
    {
      bestind(j) = -1;
      bestdist(j) = 1000.0;
    }
    const int bti = badtest(i);
    for (int j = 0; j < aggtrain.dimy(); ++j)
    {
      int traintruth = int(aggtrain(10, j) + FUZZ);
      if (fabs(aggtrain(traintruth, j) - aggtest(traintruth, bti)) > bestdist(0))
	continue;
      distance = 0;
      for (int k = 0; k < 10; ++k)
	distance += fabs(aggtrain(k, j) - aggtest(k, bti));
      if (distance < bestdist(0))
      {
	bestdist(0) = distance;
	bestind(0) = j;
	for (int k = 0; k < dum1 - 1 && bestdist(k) < bestdist(k + 1); ++k)
	{
	  swap(bestdist(k), bestdist(k + 1));
	  swap(bestind(k), bestind(k + 1));
	}
      }
    }
    std::cerr << "Test digit [" << badtest(i) << "], Best train matches";
      for (int j = 0; j < dum1; ++j)
	std::cerr << " [" << bestind(j) << "]";
    std::cerr << "\nTest truth [" << aggtest(10, badtest(i)) << "], Train truths ";
      for (int j = 0; j < dum1; ++j)
	std::cerr << " [" << aggtrain(10, bestind(j)) << "]";
    std::cerr << std::endl;
  }


};

//___________________________________________________________________________

void aggstats()
{
  ofstream lfs(listfile);
  ostream& os = (lfs ? (ostream&) lfs : (ostream&) std::cout);
  os.setf(ios::fixed, ios::floatfield);
  fgrid agg;
  if (!agg.read(aggfile))
  {
    std::cerr << "Unable to open aggfile " << aggfile << std::endl;
    exit(1);
  }
  int good1 = 0;
  int bad1 = 0;
  int good2 = 0;
  int bad2 = 0;
  for (int i = 0; i < agg.dimy(); ++i)
  {
    int mode = 0;
    float modecount = -1;
    float total = 0;
    for (int j = 0; j < 10; ++j)
    {
      total += agg(j, i);
      if (agg(j, i) > modecount)
      {
	mode = j;
	modecount = agg(j, i);
      }
    }
    
    if (modecount / total > alpha1 && modecount / total < alpha2)
    {
      if (mode == int(agg(10, i) + FUZZ))
      {
	if (i < agg.dimy() / 2)
	  ++good1;
	else
	  ++good2;
      }
      else
      {
	if (i < agg.dimy() / 2)
	  ++bad1;
	else
	  ++bad2;
      }
      
      os << "[" << setw(5) <<i << "] (T " 
	<< int(agg(10, i)) << ") :";
      for (int j = 0; j < 10; ++j)
	os << setprecision(2) << setw(6) << agg(j, i);
      os << std::endl;
    }
    
  }
  int good = good1 + good2;
  int bad = bad1 + bad2;
  os << good + bad << " total data\n";
  os << good << " correct\n";
  os << bad << " incorrect\n";
  os << "classification rate " << setprecision(2) 
    << float(good) / (good + bad) * 100 << std::endl;
  
} // aggstats

//___________________________________________________________________________

void rejectagg()
{
  fgrid agg, aggnew;
  if (!agg.read(aggfile))
  {
    std::cerr << "Unable to open aggfile " << aggfile << std::endl;
    exit(1);
  }
  int count = 0;
  for (int i = 0; i < agg.dimy(); ++i)
  {
    int mode = 0;
    float modecount = -1;
    float total = 0;
    for (int j = 0; j < 10; ++j)
    {
      total += agg(j, i);
      if (agg(j, i) > modecount)
      {
	mode = j;
	modecount = agg(j, i);
      }
    }
    
    if (modecount / total > alpha1 && modecount / total < alpha2)
      ++count;
  }
  aggnew.init(agg.dimx(), count);
  count = 0;
  for (int i = 0; i < agg.dimy(); ++i)
  {
    int mode = 0;
    float modecount = -1;
    float total = 0;
    for (int j = 0; j < 10; ++j)
    {
      total += agg(j, i);
      if (agg(j, i) > modecount)
      {
	mode = j;
	modecount = agg(j, i);
      }
    }
    
    if (modecount / total > alpha1 && modecount / total < alpha2)
    {
      for (int j = 0; j < agg.dimx(); ++j)
	aggnew(j, count) = agg(j, i);
      count++;
    }
  }
  aggnew.write(aggfile + "new");
  
} // rejectagg

//___________________________________________________________________________
// aggregate.C

