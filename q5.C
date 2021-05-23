#define Q5_C

static char rcsid[] = "$Id: q5.C,v 1.1 1997/03/24 02:31:47 wilder Exp $";

//___________________________________________________________________________
// Includes

#include "includes.h"
#include "question.h"
#include "grid.h"
#include "landmark.h"
#include "digittree.h"
#include "globals.h"

//___________________________________________________________________________
// Function prototypes

void get_best_q5(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, igrid& vertlist, 
		 const vector<cgrid>& instances, lgrid& instdata, float& ent);
int check_q5(relation& rel, const cgrid& instances);

//___________________________________________________________________________

void get_best_q5(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, igrid& vertlist, 
		 const vector<cgrid>& instances, lgrid& instdata, float& ent)
{
  if (qlist.dimx() == 0)
    return;

  int index = 0;
  int counter = 0;
  relation rel;
  for (rel.ang = minang; rel.ang < minang + numang; ++rel.ang)
    for (rel.tol = mintol; rel.tol < mintol + numtol; ++rel.tol)
      for (rel.dis = mindis; rel.dis < mindis + numdis; ++rel.dis)
	for (rel.v1 = 0; rel.v1 < vertlist.dimx(); ++rel.v1)
	  for (rel.v2 = 0; rel.v2 < vertlist.dimx(); ++rel.v2)
	  {
	    if (rel.v1 == rel.v2)
	      continue;
	    if (index == qlist.dimx())
	      return;
	    else if (qlist(index) == counter++)
	    {
	      digithist yes, no;
	      for (int i = 0; i < dlist.dimx(); ++i)
	      {
		if (check_q5(rel, instances(instdata(i))))
		  ++yes[lmarks(dlist(i)).truth()];
		else
		  ++no[lmarks(dlist(i)).truth()];
	      }
	      float newent = ave_entropy(yes, no);
	      if (newent < ent)
	      {
		ent = newent;
		que.typ = 5;
		que.r1 = rel;
	      }
	      ++index;
	    }
	  }

} // get_best_q5

//___________________________________________________________________________

int check_q5(relation& rel, const cgrid& instances)
{
  int numinst = instances.dimy();
  for (int i = 0; i < numinst; ++i)
  {
    char l1x = instances(rel.v1 * 2, i);
    char l1y = instances(rel.v1 * 2 + 1, i);
    char l2x = instances(rel.v2 * 2, i);
    char l2y = instances(rel.v2 * 2 + 1, i);
    if (rel.check(l1x, l1y, l2x, l2y))
      return 1;
  }
  return 0;
 
} // check_q5

//___________________________________________________________________________

int upd_inst5(relation& rel, const cgrid& oldinst, cgrid& newinst)
{
  int foundinst = 0;
  char insts[maxinst];
  int numinst = oldinst.dimy(); 
  for (int i = 0; i < numinst; ++i)
  {
    char l1x = oldinst(rel.v1 * 2, i);
    char l1y = oldinst(rel.v1 * 2 + 1, i);
    char l2x = oldinst(rel.v2 * 2, i);
    char l2y = oldinst(rel.v2 * 2 + 1, i);
    if (rel.check(l1x, l1y, l2x, l2y))
    {
      insts[foundinst] = i;
      ++foundinst;
    }
  }

  if (foundinst)
  {
    newinst.init(oldinst.dimx(), foundinst);
    for (int i = 0; i < newinst.dimx(); ++i)
      for (int j = 0; j < newinst.dimy(); ++j)
	newinst(i, j) = oldinst(i, insts[j]);
  }
  else
    newinst = oldinst;
    
  return foundinst;

}

//___________________________________________________________________________
// q5.c
