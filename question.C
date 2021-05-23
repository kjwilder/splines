#define QUESTION_C

static char rcsid[] = "$Id: question.C,v 1.1 1997/03/24 02:31:47 wilder Exp $";

#include "includes.h"
#include "question.h"

//___________________________________________________________________________

void question::write(ostream& os)
{
  os.write(&typ, Sint);
  switch (typ)
  {
   case 0:
    break;

   case 1:
    os.write(&r1.land1, Sint);
    os.write(&r1.land2, Sint);
    os.write(&r1.ang, 1);
    os.write(&r1.tol, 1);
    os.write(&r1.dis, 1);
    break;

   case 2:
    os.write(&r1.v1, 1);
    os.write(&r1.land1, Sint);
    os.write(&r1.ang, 1);
    os.write(&r1.tol, 1);
    os.write(&r1.dis, 1);
    break;

   case 5:
    os.write(&r1.v1, 1);
    os.write(&r1.v2, 1);
    os.write(&r1.ang, 1);
    os.write(&r1.tol, 1);
    os.write(&r1.dis, 1);
    break;

   default:
    cerr << "Unable to read question type [" << typ << "]\n";
    exit(1);
    break;

  }

} // question::write

//___________________________________________________________________________

void question::read(istream& is)
{

  is.read(&typ, 4); 
  switch (typ)
  {
   case 0:
    break;

   case 1:
    is.read(&r1.land1, Sint);
    is.read(&r1.land2, Sint);
    is.read(&r1.ang, 1);
    is.read(&r1.tol, 1);
    is.read(&r1.dis, 1);
    break;

   case 2:
    is.read(&r1.v1, 1);
    is.read(&r1.land1, Sint);
    is.read(&r1.ang, 1);
    is.read(&r1.tol, 1);
    is.read(&r1.dis, 1);
    break;

   case 5:
    is.read(&r1.v1, 1);
    is.read(&r1.v2, 1);
    is.read(&r1.ang, 1);
    is.read(&r1.tol, 1);
    is.read(&r1.dis, 1);
    break;

   default:
    cerr << "Unable to read question type [" << typ << "]\n";
    exit(1);
    break;

  }

} // question::read

//___________________________________________________________________________
// question.c
