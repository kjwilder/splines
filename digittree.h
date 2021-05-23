#ifndef DIGITTREE_H
#define DIGITTREE_H

//___________________________________________________________________________
// Include files

#include <iomanip.h>
#include "question.h"
#include "histogram.h"
#include "landmark.h"
#include "tree.h"
#include "globals.h"

//___________________________________________________________________________
// Function prototypes


//___________________________________________________________________________

class digithist : public histogram
{
 private:

 public:
  digithist() : histogram(10) { }

}; // class digithist

//___________________________________________________________________________

class digitdata
{ 
 private:

 public:
  question que;
  igrid vertlist;
  lgrid datalist;
  digithist hist;

  digitdata() { }
};

class digittree;

typedef treeiterator<digitdata> digititer;

class digittreestats
{
 private:
  const int maxtags = 200;
  int total;
  int correct;
  int wtdepth;
  float wtent;
  igrid queuse;
  igrid taguse;
  igrid anguse;
  igrid toluse;
  igrid disuse;

 public:
  digittreestats() : total(0), correct(0), wtdepth(0), wtent(0), queuse(20), 
  taguse(maxtags), anguse(8), toluse(5), disuse(3) { 
    queuse.clear(); taguse.clear(); anguse.clear(); 
    toluse.clear(); disuse.clear(); 
  }
  void operator+=(const digittreestats& d) { 
    total += d.total; correct += d.correct; wtdepth += d.wtdepth; 
    wtent += d.wtent; queuse += d.queuse; taguse += d.taguse; 
    anguse += d.anguse; toluse += d.toluse; disuse += d.disuse; 
  }
  void dump(int n) const;

  friend class digittree;
};

class digittree : public tree<digitdata>
{
 private:

 public:
  digittreestats *stats;

  digittree() : stats(0) { }
  ~digittree() { delete stats; }

  void calc_stats(const digititer& diter);

  void dump(ostream& os = cout);
  void dumpnodes(const digititer& t, ostream& os);
  void dumpnode(const digititer& t, ostream& os);

  void write(ostream& ofs);
  void write(const String& treename);
  void writenodes(const digititer& t, ofstream& ofs);

  int read(const String& treename);
  void readnodes(const digititer& t, ifstream& ifs);
  void updatehists(const digititer& t);

  void deepen(const digititer& diter, const lmarkslist& lmarks, 
	      const vector<cgrid>& instances, int final);
  void dropdata(const landmarks& lmarks, digititer& diter);
  void grow();
  void update();
  void calcsummary(const digititer& t);
  void dumpsummary(ostream& os);

  void drop(dgrid& traingrid, dgrid& targetgrid,
	    const digititer& t, int numdates, int& yescount, int& nocount);
  void clearhists(const digititer& t);
};

//___________________________________________________________________________
// Typedefs, Function prototypes and inline functions

extern int prestop(const digititer& qiter, int mind = mindata);
extern int poststop(const digititer& qiter, double bestent);


//___________________________________________________________________________
// digittree.h
#endif
