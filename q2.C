#define Q2_C

#include "includes.h"
#include "question.h"
#include "grid.h"
#include "landmark.h"
#include "digittree.h"
#include "globals.h"

using namespace std;

//___________________________________________________________________________
// Function prototypes

void get_best_q2(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, igrid& vertlist, 
		 const vector<cgrid>& instances, lgrid& instdata, float& ent);
int check_q2(relation& rel, const landmarks& lmarks, const cgrid& instances);

//___________________________________________________________________________

void get_best_q2(question& que, lgrid& qlist, lgrid& dlist, 
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
	for (rel.land1 = minland; rel.land1 < minland + numland; ++rel.land1)
	  for (rel.v1 = 0; rel.v1 < vertlist.dimx(); ++rel.v1)
	  {
	    if (index == qlist.dimx())
	      return;
	    else if (qlist(index) == counter++)
	    {
	      digithist yes, no;
	      for (int i = 0; i < dlist.dimx(); ++i)
	      {
		if (check_q2(rel, lmarks(dlist(i)), instances(instdata(i))))
		  ++yes[lmarks(dlist(i)).truth()];
		else
		  ++no[lmarks(dlist(i)).truth()];
	      }
	      float newent = ave_entropy(yes, no);
	      if (newent < ent)
	      {
		ent = newent;
		que.typ = 2;
		que.r1 = rel;
	      }
	      ++index;
	    }
	  }

} // get_best_q2

//___________________________________________________________________________


int check_q2(relation& rel, const landmarks& lmarks, const cgrid& instances)
{
  int numinst = instances.dimy(); 
  int numland = lmarks.type_count(rel.land1);
  for (int i = 0; i < numinst; ++i)
    for (int k = 0; k < numland; ++k)
    {
      char l1x = instances(rel.v1 * 2, i);
      char l1y = instances(rel.v1 * 2 + 1, i);
      char l2x = lmarks.location_x(rel.land1, k);
      char l2y = lmarks.location_y(rel.land1, k);
      if (rel.check(l1x, l1y, l2x, l2y))
	return 1;
  }
  return 0;

} // check_q2

//___________________________________________________________________________

int upd_inst2(relation& rel, const landmarks& lmarks, 
	      const cgrid& oldinst, cgrid& newinst)
{
  int foundinst = 0;
  char insts[3][maxinst];
  int numinst = oldinst.dimy(); 
  int numland = lmarks.type_count(rel.land1);
  for (int i = 0; i < numinst; ++i)
    for (int k = 0; k < numland && foundinst < maxinst; ++k)
    {
      char l1x = oldinst(rel.v1 * 2, i);
      char l1y = oldinst(rel.v1 * 2 + 1, i);
      char l2x = lmarks.location_x(rel.land1, k);
      char l2y = lmarks.location_y(rel.land1, k);
      if (rel.check(l1x, l1y, l2x, l2y))
      {
	int okay = 1;
	for (int l = 0; l < foundinst && okay; ++l)
	{
	  okay =
	    (insts[0][l] != i || 
	     distance(insts[1][l] - l2x + 40, insts[2][l] - l2y + 40) > 
	     instdist);
	}
	if (okay)
	{
	  insts[0][foundinst] = i;
	  insts[1][foundinst] = l2x;
	  insts[2][foundinst] = l2y;
	  ++foundinst;
	}
      }
    }

  if (foundinst)
  {
    newinst.init(oldinst.dimx() + 2, foundinst);
    for (int j = 0; j < foundinst; ++j)
    {
      for (int i = 0; i < oldinst.dimx(); ++i)
	newinst(i, j) = oldinst(i, insts[0][j]);
      for (int i = 0; i < 2; ++i)
	newinst(oldinst.dimx() + i, j) = insts[i + 1][j];
    }
  }
  else
    newinst = oldinst;

  return foundinst;
}
//___________________________________________________________________________
// q2.c
