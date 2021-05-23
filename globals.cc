#define GLOBALS_C

//___________________________________________________________________________
// Include files
 
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>

#include "includes.h"
#include "globals.h"

using namespace std;

// Function prototypes

extern void parse_equation(char* eqn, char*& var, char*& val);
extern int set_global(char* var, char* val);
 
//___________________________________________________________________________
// Types and lists needed to keep track of global variables.

enum {boollistvar, intvar, longvar, floatvar, doublevar, stringvar};

struct typedesc { char *v; void *p; int t; };


typedesc varlist[] = 
{ 
#define GLOBAL(x, y) { strdup(#y), &y, x##var },
#define GLOBALI(x, y, z) { strdup(#y), &y, x##var },
#define GLOBALB(x, y) { strdup(#x), x, boollistvar },
#include "globlist.h"  
#undef GLOBAL
#undef GLOBALI
#undef GLOBALB
  { strdup(""), 0, 0 }
};
    
//___________________________________________________________________________
// Initialize the non-zero, non-null global variables.

void init_globals()
{
#define GLOBAL(x, y)
#define GLOBALI(x, y, z) y = z;
#define GLOBALB(x, y) { for (int i=0; i < y - 1; ++i) x[i] = 0; x[y - 1] = 'b'; }
#include "globlist.h"  
#undef GLOBAL
#undef GLOBALI
#undef GLOBALB

} // init_globals

//___________________________________________________________________________
// If argv[1] is not an assigment, it is the name of a parameter file. 

void get_parfile(int argc, char** argv, string& parfile)
{
  if (argc > 1 && strchr(argv[1], '=') == 0)
    parfile = argv[1];

} // get_parfile

//___________________________________________________________________________
// argv may contain statements of the form var=value.  Use these statements
// to set global variables.

void parse_argv(int argc, char** argv)
{
  // Skip argv[1] if it was the name of a parameter file
  int count = (argc > 1 && strchr(argv[1], '=') == 0) ? 2 : 1;

  // Parse the remaining vectors of argv
  while (count < argc)
  {
    char *var, *val;
    parse_equation(argv[count++], var, val);
    if (var != 0 && !set_global(var, val))
      cerr << "Invalid variable [" << var << "] on command line." << endl;
  }
 
} // parse_argv

//___________________________________________________________________________

int parse_parfile(string& parfile)
{
  ifstream ifs(parfile);
  if (!ifs)
    return 0;
 
  char buf[STRLEN];
  while (ifs.getline(buf, STRLEN))
  {
    char *var, *val;
    parse_equation(buf, var, val);
    if (var != 0 && !set_global(var, val))
      cerr << "Invalid variable [" << var << "] in parameter file." << endl;
  }
  return 1;

} // parse_parfile

//___________________________________________________________________________
// For an 'eqn' of the form ' A B C = D E F ', make 'var' equal to "C" and
// 'val' equal to "D E F".  'var' and 'val' end up pointing in to the 'eqn'
// buffer, which is mutilated by this routine.

void parse_equation(char* eqn, char*& var, char*& val)
{
  // Initialize var to something reasonable.
  var = 0;

  // Exit if eqn isn't something reasonable, or starts with a #.
  if (eqn == 0 || eqn[0] == '#')
    return;
 
  // If we have a newline character in 'eqn', make it a NUL character,
  // then set 'val' pointing to the end of the input string.
  if ((val = strchr(eqn, (int) '\n')) != 0)
    *val-- = '\0';
  else
    val = strchr(eqn, (int) '\0') - 1;
 
  // Get rid of trailing spaces and tabs in the val - Only
  // important for string variables.
  while (val != (eqn - 1) && (*val == ' ' || *val == '\t'))
    *val-- = '\0';
 
  // Set 'val' pointing to the character past the '=' character
  // and make the '=' character a NUL character.  Give up if there
  // is no '='.
  if ((val = strchr(eqn, (int) '=')) == 0)
    return;
  *val++ = '\0';
 
  // Get rid of leading spaces in the val  - Again, this
  // is only important for string variables.
  while (*val == ' ' || *val == '\t')
    val++;
 
  // Get a pointer to the name of the variable.  This has to be
  // the last word before the equal sign.
  char *nexttok;
  if ((var = strtok(eqn, " \t")) == 0)
    return;
  else
    while ((nexttok = strtok(0, " \t")) != 0)
      var = nexttok;

} // parse_equation

//___________________________________________________________________________
// Set the global variable 'var' equal to 'val' after translating the 
// string 'val' to an appropriate type for 'var'.

int set_global(char* var, char* val)
{
  int ind = 0;
  int done = 0;
  while (!done)
  {
    if (strcmp(var, varlist[ind].v) == 0)
    {
      switch (varlist[ind].t)
      {
      case (boollistvar):
	{
	  char *newp = (char *) varlist[ind].p;
	  int i = 0;
	  while (i < (int) strlen(val))
	  {
	    newp[i] = val[i] - '0';
	    ++i;
	  }
	  newp[i] = 'b';
	}
	break;
      case (intvar):
	*((int *)varlist[ind].p) = atoi(val);
	break;
      case (longvar):
	*((long *)varlist[ind].p) = atol(val);
	break;
      case (floatvar):
	*((float *)varlist[ind].p) = atof(val);
	break;
      case (doublevar):
	*((double *)varlist[ind].p) = atof(val);
	break;
      case (stringvar):
	*((string*)varlist[ind].p) = val;
	break;
      default:
	cerr << "Unable to determine type of variable [" << var << "].\n";
	break;
      }
      return 1;
    }
    done = (varlist[++ind].p == 0);
  }
  return 0;

} // set_global

//___________________________________________________________________________
// Print out all the global variables.  They can be dumped to either cout
// or a file.

void dump_globals(string& dumpfile)
{
  if (dumpfile == "")
    return;

  ofstream ofs;
  if (dumpfile != "cout")
  {
    ofs.open(dumpfile);
    if (!ofs.is_open())
    {
      cerr << "Unable to open dumpfile [" << dumpfile << "]\n";
      return;
    }
  }

  ostream& os = (dumpfile == "cout") ? (ostream&)cout : (ostream&)ofs;

  int ind = 0;
  int done = 0;
  while (!done)
  {
    os << varlist[ind].v << " = ";
    switch (varlist[ind].t)
    {
    case (boollistvar):
      {
	char *newp = (char *) varlist[ind].p;
	while (*newp != 'b')
	  os << int(*newp++);
      }
      break;
    case (intvar):
      os << *((int *)varlist[ind].p);
      break;
    case (longvar):
      os << *((long *)varlist[ind].p);
      break;
    case (floatvar):
      os << *((float *)varlist[ind].p);
      break;
    case (doublevar):
      os << *((double *)varlist[ind].p);
      break;
    case (stringvar):
      os << *((string *)varlist[ind].p);
      break;
    default:
      os << "[Unrecognized variable type]";
      break;
    }
    os << endl;
    done = (varlist[++ind].p == 0);
  }

} // dump_globals

//___________________________________________________________________________
// globals.c
