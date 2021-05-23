#define DIGITTREE_C

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "includes.h"
#include "grid.h"
#include "digittree.h"
#include "process.h"
#include "landmark.h"
#include "globals.h"

//___________________________________________________________________________
// Typedefs, etc.

static impfunction impfn = &histogram::entropy;
void make_vertlist(igrid& newverts, igrid& oldverts, question& que);
void get_best_question(digitdata& nodedata, const lmarkslist& lmarks, 
		       lgrid& quedata, const vector<cgrid>& instances,
		       lgrid& instdata);
int get_answer(question& que, const landmarks& lmarks, 
	       const cgrid& oldinst, cgrid& newinst);
void get_best_q1(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, float& ent); 
void get_best_q2(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, igrid& vertlist, 
		 const vector<cgrid>& instances, lgrid& instdata, float& ent);
void get_best_q5(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, igrid& vertlist, 
		 const vector<cgrid>& instances, lgrid& instdata, float& ent);
int upd_inst1(relation& rel, const landmarks& lmark, cgrid& instances);
int upd_inst2(relation& rel, const landmarks& lmarks, 
	      const cgrid& oldinst, cgrid& newinst);
int upd_inst5(relation& rel, const cgrid& oldinst, cgrid& newinst);



//___________________________________________________________________________

void digittree::grow()
{

  // Initialize the tree and determine the data which will be used to
  // build the tree

  maketop();
  lgrid treedata;
  switch (initdataopt)
  {
   case 0:
    random_select(treedata, numdata, numtreedata);
    break;
   case 1:
    break;
  }

  // Get the truths of all of the digits
  igrid truths(numtreedata);
  for (int i = 0; i < numtreedata; ++i)
    truths(i) = get_truth(treedata(i));

    // Data tracking variables.  They don't need to be initialized
  vector<history> hists(numtreedata);  // Histories of individual digits
  vector<history> histories(10000);    // List of histories in tree
  igrid histcounts(10000);             // Number of digits with history i

  cgrid finished(numtreedata);
  finished.clear();
  int done = 0, numloops = 0;
  while (!done)
  {
    // Make sure we're not hopelessly lost in this loop
    if (++numloops > 10)
    {
      cerr << "grow_tree cycled thru more then ten times... oops!\n";
      write(treefile);
      exit(1);
    }

    // Determine if we are done.  
    done = 1;
    for (int i = 0; i < numtreedata && done; ++i)
      done = finished(i);
    if (done)
      continue;

    // Determine current history of each unfinished digit
    if (top().branch())
    {
      for (int i = 0; i < numtreedata; ++i)
      {
	if (!finished(i))
	{
	  landmarks lmarks;
	  lmarks.read(treedata(i));
	  digititer terminal;
	  dropdata(lmarks, terminal);
	  hists(i) = terminal->hist();
	  if (terminal->depth() == maxdepth)
	    finished(i) = 1;
	}
      }
    }

    // Determine the histories actually used and the histcounts
    int histindex = 0;
    for (int i = 0; i < numtreedata; ++i)
    {
      int found = 0;
      if (!finished(i))
      {
	int j = 0;
	for (j = 0; j < histindex && !found; ++j)
	  found = (hists(i) == histories(j));
	if (found)
	  ++histcounts(j - 1);
	else
	{
	  histcounts(histindex) = 1;
	  histories(histindex) = hists(i);
	  ++histindex;
	}
      }
    }
    if (histindex > 10000)
    {
      cerr << "Too many branches.  Oops!\n";
      exit(1);
    }

    // Determine the automatically finished data; those whose branch contains
    // a feasible amount of data 'tmptreedata'
    for (int i = 0; i < numtreedata; ++i)
    {
      if (!finished(i))
      {
	int j = 0;
	while (histories(j) != hists(i))
	  ++j;
	if (histcounts(j) < tmptreedata)
	  finished(i) = 1;
      }
    }

    // Now continue growing the tree, branch by branch, but several at
    // a time if possible
    int numused = 0;
    for (int i = 0; i < histindex; i += numused)
    {
      // Determine how many branches to drop this time.  Keep adding branches
      // while more remain and the total data is less then 'tmptreedata'
      numused = 1;
      int currtotal = histcounts(i);
      while (i + numused < histindex && 
	     currtotal + histcounts(i + numused) <= tmptreedata)
	currtotal += histcounts(i + numused++);
      
      // Use the data that went to branches i...(i + numused - 1)
      // to build the data list
      lgrid& dlist = top()->data.datalist;
      dlist.init(currtotal);
      int count = 0;
      for (int j = 0; j < numtreedata; ++j)
      {
	for (int k = 0; k < numused; ++k)
	{
	  if (hists(j) == histories(i + k))
	    dlist(count++) = j;
	}
      }

      // If the data came from a branch with a lot of data, the data list
      // may need to be abridged
      if (currtotal > tmptreedata)
      {
	lgrid tmp;
	grid_select(dlist, tmp, tmptreedata);
	dlist << tmp;
	currtotal = tmptreedata;
      }

      // Fill in the histogram information for the top node
      digithist& dhist = top()->data.hist;
      dhist.clear();
      for (int j = 0; j < currtotal; ++j)
	++dhist[truths(dlist(j))];

      // Handle the case of a branch we just learned will be finished
      // Need to make sure this check handles all the cases.  At this
      // point it's important that tmptreedata > numdata / numclass
      if (currtotal == tmptreedata && prestop(top(), intmindata))
      {
	currtotal = 0;
	dlist.init(histcounts(i));
	dhist.clear();
	for (int j = 0; j < numtreedata; ++j)
	  if (histories(i) == hists(j))
	  {
	    dlist(currtotal++) = j;
	    finished(j) = 1;
	    ++dhist[truths(j)];
	  }
	assert(currtotal == histcounts(i));
      }
      
      // Create the list of landmarks and set up all the other data required
      // for the deepen routine
      int final = finished(dlist(0));
      lmarkslist lmarks(currtotal);
      for (int j = 0; j < currtotal; ++j)
	lmarks(j).read(treedata(dlist(j)));
      dlist.init(currtotal);
      for (int j = 0; j < currtotal; ++j)
	dlist(j) = j;
      vector<cgrid> instances(currtotal);

      deepen(top(), lmarks, instances, final);
    }
  }

} // digittree::grow

//___________________________________________________________________________

int digittree::read(const String& treename)
{

  ifstream ifs(treename);
  if (!ifs)
    return 0;

  char buf[5];
  ifs.read(buf, 4);
  buf[4] = 0;
  if (strcmp(buf, "TR11") != 0)
  {
    cerr << treename << " is not a tree file\n";
    exit(1);
  }

  if (!ifs.eof())
  {
    maketop();
    readnodes(top(), ifs);
    updatehists(top());
  }
  
  return 1;

} // digittree::read

//___________________________________________________________________________

void digittree::readnodes(const digititer& curr, ifstream& ifs)
{
  
  curr->data.que.read(ifs);
  curr->data.vertlist.read(ifs);

  if (curr->data.que.typ == 0)
  {
    curr->data.hist.read(ifs);
  }
  else
  {
    makechildren(curr);
    readnodes(curr.no(), ifs);
    readnodes(curr.yes(), ifs);
  }

} // digittree::readnodes

//___________________________________________________________________________

void digittree::updatehists(const digititer& curr)
{
  if (curr.branch())
  {
    updatehists(curr.no());
    updatehists(curr.yes());
    for (int i = 0; i < 10; ++i)
      curr->data.hist[i] = curr.no()->data.hist[i] + curr.yes()->data.hist[i];
  }

} // digittree::updatehists

//___________________________________________________________________________

void digittree::write(const String& treename)
{

  ofstream ofs(treename);
  if (!ofs)
  {
    cerr << "Unable to write to treefile [" << treename << "]\n";
    exit(1);
  }

  ofs << "TR11";
  writenodes(top(), ofs);

} // digittree::write

//___________________________________________________________________________

void digittree::writenodes(const digititer& curr, ofstream& ofs)
{
  if (!curr)
    return;

  curr->data.que.write(ofs);
  curr->data.vertlist.write(ofs);

  if (curr.leaf())
  {
    curr->data.hist.write(ofs);
  }
  else
  {
    writenodes(curr.no(), ofs);
    writenodes(curr.yes(), ofs);
  }

} // digittree::writenodes
//___________________________________________________________________________

void digittree::dump(ostream& os)
{

  os.setf(ios::fixed, ios::floatfield);

  stats = new digittreestats;
  calc_stats(top());

  dumpnodes(top(), os);

  os << "\n\nClassification:\n";
  os << "Total classified correctly: " << stats->correct << endl;
  os << "Total amount of data:       " << stats->total << endl;
  if (stats->total > 0)
    os << "Percent classified correct: " << setprecision(2)
      << float(stats->correct) / stats->total * 100;

  os << "\n\nRelations:\n";
  for (int i = 0; i < stats->taguse.dimx(); ++i)
    if (stats->taguse(i) > 0)
      os << "Tag " << i << " : " << stats->taguse(i) << " times\n";
  int btags = 0, stags = 0;
  for (int i = 0; i < 8; ++i)
    btags += stats->taguse(i);
  for (int i = 0; i < 8; ++i)
    stags += stats->taguse(i + 8);
  os << "Big tag usage (tags 0-7): " << btags << endl;
  os << "Small tag usage (tags 8-15): " << stags << endl;

  for (int i = 0; i < stats->anguse.dimx(); ++i)
    if (stats->anguse(i) > 0)
      os << "Angle " << i << " : " << stats->anguse(i) << " times\n";
  for (int i = 0; i < stats->toluse.dimx(); ++i)
    if (stats->toluse(i) > 0)
      os << "Tolerance " << i << " : " << stats->toluse(i) << " times\n";
  for (int i = 0; i < stats->disuse.dimx(); ++i)
    if (stats->disuse(i) > 0)
      os << "Distance " << i << " : " << stats->disuse(i) << " times\n";

  os << endl;

} // digittree::dump

//___________________________________________________________________________

void digittree::calc_stats(const digititer& diter)
{
  if (diter.leaf())
  {
    stats->total += diter->data.hist.total();
    stats->correct += diter->data.hist[diter->data.hist.mode()];
    stats->wtdepth += diter->data.hist.total() * diter->depth();
    stats->wtent += diter->data.hist.entropy() * diter->data.hist.total();
  }
  else
  {
    ++stats->queuse(diter->data.que.typ);
    ++stats->anguse(diter->data.que.r1.ang);
    ++stats->toluse(diter->data.que.r1.tol);
    ++stats->disuse(diter->data.que.r1.dis);
    switch(diter->data.que.typ)
    {
     case 1:
      ++stats->taguse(diter->data.que.r1.land1);
      ++stats->taguse(diter->data.que.r1.land2);
      break;
     case 2:
      ++stats->taguse(diter->data.que.r1.land1);
      break;
     case 5:
      break;
     default:
      cerr << "Unknown question type [" << diter->data.que.typ 
	<< "] in calc_stats\n";
      exit(1);
    }
    calc_stats(diter.no());
    calc_stats(diter.yes());
  }


} // digittree::calc_stats
//___________________________________________________________________________

void digittree::dumpnodes(const digititer& d, ostream& os)
{
  for (int i = 0; i < 78; ++i) 
    os << "-";
  os << "\nDepth: [" << setw(2) << d->depth()
    << "], History: [" << d->hist() << "]\n";

  if (d.branch())
  {
    os << "Question = Type: [" << setw(2) << d->data.que.typ << "], Ang: [" 
      << setw(6) << setprecision(2) << d->data.que.r1.relangle() * 180 / PI
      << "], Tol: [" << setw(6) << setprecision(2) 
      << d->data.que.r1.reltolerance() * 180 / PI << "], Dis: [" << setw(1) 
      << int(d->data.que.r1.dis) << "]\n";
  }

  os << "Class:";
  for (int i = 0; i < 10; ++i)
    os << setw(6) << i;
  os << "\nCurr:  ";
  for (int i = 0; i < 10; ++i)
  {
    if (d->data.hist[i] > 0)
      os << setw(6) << d->data.hist[i];
    else
      os << "      ";
  }  

  if (d.leaf())
  {
    os << "\n";
  }
  else
  {
    os << "\nNo:    ";
    for (int i = 0; i < 10; ++i)
    {
      if (d.no()->data.hist[i] > 0)
	os << setw(6) << d.no()->data.hist[i];
      else
	os << "      ";
    }
    os << endl;
    
    os << "Yes:   ";
    for (int i = 0; i < 10; ++i)
    {
      if (d.yes()->data.hist[i] > 0)
	os << setw(6) << d.yes()->data.hist[i];
      else
	os << "      ";

    }
    os << endl;
    
    dumpnodes(d.no(), os);
    dumpnodes(d.yes(), os);
  }
  

} // digittree::dumpnodes

//___________________________________________________________________________

void digittree::dropdata(const landmarks& lmarks, digititer& terminal)
{
  terminal = top();
  cgrid oldinst, newinst;
  while (terminal.branch())
  {
    if (get_answer(terminal->data.que, lmarks, oldinst, newinst))
      terminal = terminal.yes();
    else 
      terminal = terminal.no();
    oldinst << newinst;
  }
  if (dum1 == 10)
    cerr << oldinst.dimy() << endl;

} // digittree::dropdata

//___________________________________________________________________________

int get_answer(question& que, const landmarks& lmarks, 
	       const cgrid& oldinst, cgrid& newinst)
{
  int answer;
  switch (que.typ)
  {
   case 1:
    answer = upd_inst1(que.r1, lmarks, newinst);
    break;
   case 2:
    answer = upd_inst2(que.r1, lmarks, oldinst, newinst);
    break;
   case 5:
    answer = upd_inst5(que.r1, oldinst, newinst);
    break;
   default:
    cerr << "Unknown question type [" << que.typ << "] in "
      << "get_answer routine\n";
    exit(1);
    break;
  }

  return answer;

} // get_answer

//___________________________________________________________________________

void digittree::deepen(const digititer& diter, const lmarkslist& lmarks,
		       const vector<cgrid>& instances, int final)
{
  // Set up an alias for the node data and find out how much data there is
  digitdata& nodedata = diter->data;
  int numnodedata = nodedata.datalist.dimx();
  
  // What we do depens on whether or not we're at a leaf node or not
  if (diter.leaf())
  {
    // Test the pre-stopping criterion
    if (prestop(diter, (final ? mindata : intmindata)))
      return;

    // Determine how many data actually here can actually be used to 
    // ask the next question
    int m1, m2;
    if (diter->depth() == 0)
    {
      m1 = 1; m2 = 0;
      for (int i = 0; i < extension; ++i)
      {
	if (m1 == m2 + 1)
	{
	  ++m1;
	  m2 = 0;
	}
	else
	  ++m2;
      }
    }
    else
    {
      m1 = nodedata.hist.mode();
      m2 = nodedata.hist.mode2();
    }
    int count = 0;
    for (int i = 0; i < numnodedata; ++i)
      count += (lmarks(nodedata.datalist(i)).truth() == m1 || 
		   lmarks(nodedata.datalist(i)).truth() == m2);
    
    // Set up the list of potential data
    lgrid currdata, quedata, instdata;
    currdata.init(count);
    count = 0;
    for (int i = 0; i < numnodedata; ++i)
    {
      if (lmarks(nodedata.datalist(i)).truth() == m1 || 
	  lmarks(nodedata.datalist(i)).truth() == m2)
	currdata(count++) = nodedata.datalist(i);
    }

    // Shorten the list as necessary
    grid_select(currdata, quedata, numquesdata);
    instdata.init(quedata.dimx());
    int ind = 0;
    for (int i = 0; ind < quedata.dimx(); ++i)
    {
      if (quedata(ind) == nodedata.datalist(i))
	instdata(ind++) = i;
    }

    // Get a new question for this node
    get_best_question(nodedata, lmarks, quedata, instances, instdata);

    // Prepare to ask the new question
    makechildren(diter);
    diter.no()->data.vertlist = nodedata.vertlist;
    make_vertlist(diter.yes()->data.vertlist, nodedata.vertlist, nodedata.que);
  }
  else
  {
    // Clear the histograms and digitlists of the children
    diter.yes()->data.datalist.init();
    diter.yes()->data.hist.clear();
    diter.no()->data.datalist.init();
    diter.no()->data.hist.clear();
  }

  // Ask the node question of all the data at the current node
  int numyes = 0;
  vector<cgrid> newinst(numnodedata);
  cgrid answers(numnodedata);
  for (int i = 0; i < numnodedata; ++i)
  {
    answers(i) = get_answer(nodedata.que, lmarks(nodedata.datalist(i)), 
			    instances(i), newinst(i));
    numyes += (answers(i) > 0);
  }

  // Set up the children
  vector<cgrid> yesinstances(numyes), noinstances(numnodedata - numyes);
  diter.no()->data.datalist.init(numnodedata - numyes);
  diter.yes()->data.datalist.init(numyes);
  
  int noind = 0, yesind = 0;
  for (int i = 0; i < numnodedata; ++i)
  {
    if (answers(i))
    {
      diter.yes()->data.datalist(yesind) = nodedata.datalist(i);
      ++diter.yes()->data.hist[lmarks(nodedata.datalist(i)).truth()];
      yesinstances(yesind) << newinst(i);
      ++yesind;
    }
    else
    {
      diter.no()->data.datalist(noind) = nodedata.datalist(i);
      ++diter.no()->data.hist[lmarks(nodedata.datalist(i)).truth()];
      noinstances(noind) << newinst(i);
      ++noind;
    }
  }

  assert((yesind == numyes) && (yesind + noind == numnodedata));

  deepen(diter.no(), lmarks, noinstances, final);
  deepen(diter.yes(), lmarks, yesinstances, final);

}  // digittree::deepen

//___________________________________________________________________________

void get_best_question(digitdata& nodedata, const lmarkslist& lmarks, 
		       lgrid& quedata, const vector<cgrid>& instances,
		       lgrid& instdata)
{
  lgrid maxq(20), numq(20), doneques(20), qlist;
  maxq.clear(); numq.clear(); doneques.clear();
  int numverts = nodedata.vertlist.dimx();

  maxq(1) = numang / 2 * numdis * numtol * numland * numland;
  maxq(2) = numang * numdis * numtol * numland * numverts;
  maxq(5) = numang * numdis * numtol * numverts * (numverts - 1);

  float bestent = 1000;
  if (numverts == 0)
  {
    numq(1) =  numquesask;
    random_select(qlist, maxq(1), numq(1));
    get_best_q1(nodedata.que, qlist, quedata, lmarks, bestent);
  }
  else
  {
    int numused = 0;
    int totalques = 0;
    for (int i = 0; i < 20; ++i)
    {
      totalques += useques[i] * maxq(i);
      numused += (useques[i] > 0);
    }
    
    int numquesper = numquesask / numused;
    int extraques, surplus;
    int done = 0;
    while (!done)
    {
      extraques = 0;
      surplus = 0;
      for (int i = 0; i < 20; ++i)
      {
	if (!useques[i])
	  continue;
	if (maxq(i) > numquesper)
	{
	  numq(i) = numquesper;
	  ++surplus;
	}
	else if (!doneques(i))
	{
	  ++doneques(i);
	  numq(i) = maxq(i);
	  extraques += numquesper - maxq(i);
	}
      }
      if (!(done = (extraques == 0 || !surplus)))
	numquesper = numquesper + extraques / surplus;
    }
    if (useques[2])
    {
      random_select(qlist, maxq(2), numq(2));
      get_best_q2(nodedata.que, qlist, quedata, lmarks, 
		  nodedata.vertlist, instances, instdata, bestent);
    }
    if (useques[5])
    {
      random_select(qlist, maxq(5), numq(5));
      get_best_q5(nodedata.que, qlist, quedata, lmarks, 
		  nodedata.vertlist, instances, instdata, bestent);
    }
  }

} // get_best_question

//___________________________________________________________________________

void make_vertlist(igrid& newverts, igrid& oldverts, question& que)
{
  switch(que.typ)
  {
   case 1:
    newverts.init(oldverts.dimx() + 2);
    for (int i = 0; i < oldverts.dimx(); ++i)
      newverts(i) = oldverts(i);
    newverts(oldverts.dimx()) = que.r1.land1;
    newverts(oldverts.dimx() + 1) = que.r1.land2;
    break;
    
   case 2:
    newverts.init(oldverts.dimx() + 1);
    for (int i = 0; i < oldverts.dimx(); ++i)
      newverts(i) = oldverts(i);
    newverts(oldverts.dimx()) = que.r1.land1;
    break;

   case 5:
    newverts = oldverts;
    break;

   default:
    break;
  }

} // make_vertlist

//___________________________________________________________________________

void digittree::update()
{
  clearhists(top());

  for (int i = first; i < first + numdata; ++i)
  {
    landmarks currland;
    currland.read(i);
    digititer terminal = top();
    cgrid oldinst, newinst;
    while (terminal.branch())
    {
      if (get_answer(terminal->data.que, currland, oldinst, newinst))
	terminal = terminal.yes();
      else 
	terminal = terminal.no();
      oldinst << newinst;
    }
    ++terminal->data.hist[currland.truth()];
  }

} // digittree::update

//___________________________________________________________________________

void digittree::clearhists(const digititer& t)
{
  if (t)
  {
    if (t.leaf())
    {
      t->data.hist.clear();
    }
    else
    {
      clearhists(t.no());
      clearhists(t.yes());
    }
  }

} // digittree::clearhists

//___________________________________________________________________________

void digittreestats::dump(int n) const
{
  cout.setf(ios::fixed, ios::floatfield);
  cout << "\nAverage classification rate: " << setprecision(2) 
    << 100.0 * correct / total << endl;
  cout << "Average depth: " << 1.0 * wtdepth / total << endl;
  cout << "Average entropy: " << wtent / total << endl;
  cout << "\nQuestion usage:\n";
  int quetotal = 0;
  for (int i = 0; i < 20; ++i)
    quetotal += queuse(i);
  for (int i = 0; i < 20; ++i)
    if (queuse(i) > 0)
    {
      cout << "Question " << i << ": " << setw(7) << 1.0 * queuse(i) / n 
	<< " (" << setw(6) << 100.0 * queuse(i) / quetotal << " %)\n";
    }
} // digittreestats::dump

//___________________________________________________________________________

// Return 1 if we can not continue growing the tree from node 't'.

int prestop(const digititer& t, int mind)
{
  // Stop if tree depth is too large
  if (t->depth() > maxdepth)
    return 1;

  // Stop if the node is pure
  if (t->data.hist.pure(purepar))
    return 1;

  // Stop if there is not enough data to make a split
  if (t->data.hist.total() < mind)
    return 1;

  // Stop if the second mode doesn't have a reasonable amount of data
  if (t->data.hist[t->data.hist.mode2()] < modetwo)
    return 1;
 
  return 0;
 
} // prestop
 
//___________________________________________________________________________
// Return 1 if we can not continue growing the tree from node 't'.

int poststop(const digititer& w, double bestent)
{
  double oldent = (w->data.hist.*impfn)();
  if ((oldent - bestent) < minentdrop)
    // return 1;
    return 0;

  return 0;

} // poststop

//___________________________________________________________________________
// digittree.C
