#define Q1_C

#include "includes.h"
#include "question.h"
#include "grid.h"
#include "landmark.h"
#include "digittree.h"
#include "globals.h"

using namespace std;

//___________________________________________________________________________
// Function prototypes

void get_best_q1(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, float& ent); 
int check_q1(relation& rel, const landmarks& lmarks);

//___________________________________________________________________________

void get_best_q1(question& que, lgrid& qlist, lgrid& dlist, 
		 const lmarkslist& lmarks, float& ent)
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
	  for (rel.land2 = minland; rel.land2 < minland + numland; ++rel.land2)
	  {
	    if (index == qlist.dimx())
	      return;
	    else if (qlist(index) == counter++)
	    {
	      digithist yes, no;
	      for (int i = 0; i < dlist.dimx(); ++i)
	      {
		if (check_q1(rel, lmarks(dlist(i))))
		  ++yes[lmarks(dlist(i)).truth()];
		else
		  ++no[lmarks(dlist(i)).truth()];
	      }
	      float newent = ave_entropy(yes, no);
	      if (newent < ent)
	      {
		ent = newent;
		que.typ = 1;
		que.r1 = rel;
	      }
	      ++index;
	    }
	  }

} // get_best_q1

//___________________________________________________________________________

int check_q1(relation& rel, const landmarks& lmarks)
{
  int num1 = lmarks.type_count(rel.land1);
  int num2 = lmarks.type_count(rel.land2);
  for (int i = 0; i < num1; ++i)
    for (int j = 0; j < num2; ++j)
    {
      char l1x = lmarks.location_x(rel.land1, i);
      char l1y = lmarks.location_y(rel.land1, i);
      char l2x = lmarks.location_x(rel.land2, j);
      char l2y = lmarks.location_y(rel.land2, j);
      if (rel.check(l1x, l1y, l2x, l2y))
	return 1;
  }
  return 0;

} // check_q1

//___________________________________________________________________________

int upd_inst1(relation& rel, const landmarks& lmarks, cgrid& instances)
{
  int foundinst = 0;
  char insts[4][maxinst];
  int num1 = lmarks.type_count(rel.land1);
  int num2 = lmarks.type_count(rel.land2);
  for (int i = 0; i < num1; ++i)
    for (int j = 0; j < num2 && foundinst < maxinst; ++j)
    {
      char l1x = lmarks.location_x(rel.land1, i);
      char l1y = lmarks.location_y(rel.land1, i);
      char l2x = lmarks.location_x(rel.land2, j);
      char l2y = lmarks.location_y(rel.land2, j);
      if (rel.check(l1x, l1y, l2x, l2y))
      {
	int okay = 1;
	for (int k = 0; k < foundinst && okay; ++k)
	{
	  okay =
	    (distance(insts[0][k] - l1x + 40, insts[1][k] - l1y + 40) > 
	     instdist ||
	     distance(insts[2][k] - l2x + 40, insts[3][k] - l2y + 40) > 
	     instdist);
	}
	if (okay)
	{
	  insts[0][foundinst] = l1x;
	  insts[1][foundinst] = l1y;
	  insts[2][foundinst] = l2x;
	  insts[3][foundinst] = l2y;
	  ++foundinst;
	}
      }
    }

  if (foundinst)
  {
    instances.init(4, foundinst);
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < foundinst; ++j)
	instances(i, j) = insts[i][j];
  }

  return foundinst;

}

// q1.C
