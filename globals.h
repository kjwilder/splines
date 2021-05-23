#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef MAIN_DEFINED
 #define Extern
#else 
 #define Extern extern
#endif // MAIN_DEFINED

#include <string>

using namespace std;

const int maxinst = 25;

#define GLOBAL(x, y) Extern x y;
#define GLOBALI(x, y, z) Extern x y;
#define GLOBALB(x, y) Extern char x[y];
#include "globlist.h"  
#undef GLOBAL
#undef GLOBALI
#undef GLOBALB

//___________________________________________________________________________
// Functions in globals.h

extern void init_globals();
extern void get_parfile(int argc, char **argv, string& parfile);
extern void parse_argv(int argc, char **argv);
extern int parse_parfile(string& parfile);
extern void dump_globals(string& dumpfile);

//___________________________________________________________________________

#endif // GLOBALS_H
