#ifndef GLOBLIST_H
#define GLOBLIST_H
#endif

//___________________________________________________________________________
// List of global variables along with initial values

// Temporary dummy variables useful for testing code;  Anything can be
// done with them at any time
GLOBAL(double, alpha1)
GLOBAL(double, alpha2)
GLOBAL(int, dum1)
GLOBAL(int, dum2)

// Options controlling the current run of the program
GLOBAL(int, opt)                // Main option for running the program
GLOBAL(int, viewopt)            // Options used when viewing objects
GLOBAL(int, extension)          // Used to index repeated runs of the program
GLOBAL(long, seed)              // The random number generator seed

// Input/Output file names
GLOBALI(string, datadir, "/usr/data")  // Directory containing data
GLOBALI(string, dataset, "nisttrain")  // A particular data set
GLOBAL(string, dumpfile)       // Output file containing all global values
GLOBAL(string, parfile)        // Input file containing global initializers
GLOBAL(string, listfile)       // File with a list of digits indices
GLOBAL(string, treefile)       // Name of binary tree file
GLOBAL(string, aggfile)        // Binary file stores aggregated results


GLOBAL(int, revvideo)
GLOBAL(int, colors)

GLOBALI(double, alpha, 1.0 / 3.0)
GLOBALI(int, cluster, 3)
GLOBAL(int, codesize)
GLOBAL(string, codesfile)
GLOBAL(string, codetreefile)
GLOBAL(int, far)
GLOBAL(int, thin)
GLOBALI(int, filter, 1)
GLOBAL(int, first)
GLOBALI(int, imagesperfile, 100)
GLOBALI(int, imborder, 20)
GLOBALI(string, impmeasure, "entropy")
GLOBAL(int, initdataopt)
GLOBALI(int, instdist, 6)
GLOBAL(int, intmindata)
GLOBALI(string, landdir, "/usr/data/nisttrainL")
GLOBAL(int, last)
GLOBAL(int, maxcodelength)
GLOBALI(int, maxdepth, 30)
GLOBAL(int, minang)
GLOBAL(int, minland)
GLOBAL(int, mindis)
GLOBAL(int, mintol)
GLOBAL(int, numang)
GLOBAL(int, numland)
GLOBAL(int, numdis)
GLOBAL(int, numtol)
GLOBALI(int, mindata, 1)
GLOBAL(double, minentdrop)
GLOBAL(int, minpernode)
GLOBAL(int, modetwo)
GLOBAL(int, near)
GLOBAL(int, numdata)
GLOBALI(int, numdigits, 1)
GLOBAL(int, numtreedata)
GLOBAL(int, numquesask)
GLOBAL(int, numquesdata)
GLOBALI(int, pixsize, 4)
GLOBALI(double, purepar, 1.0)
GLOBAL(int, randomize)
GLOBAL(double, imrotate)
GLOBALI(int, scale, 32)
GLOBAL(int, showorig)
GLOBAL(int, showpixels)
GLOBAL(double, skew)
GLOBALI(int, slant, 1)
GLOBAL(int, tmptreedata)
GLOBALB(useques, 21)

// Extern double minentdrop;      // Minimum entropy drop to accept a question
// Extern double purepar;         // Stop if mode % is greater then purepar
// Extern char useques[20];       // List of questions to use
// 
// // Question parameters
// Extern int minland;
// Extern int numland;
// Extern int minang;
// Extern int numang;
// Extern int mintol;
// Extern int numtol;
// Extern int mindis;
// Extern int numdis;
// Extern int near, far;
// Extern int numquesask;
// Extern int instdist;
// 
// Extern string impmeasure;      // Impurity function name (entropy, gini, ...)
// Extern int imagesperfile;      // Number of images stored in a data file
// Extern int first;              // Index of first digit to use
// Extern int last;               // Index of last digit to use
// Extern int numdigits;          // Number of digits to use
// 
// // Codes options
// Extern int codesize;           // Size of codes
// Extern string codesfile;       // Name of file containing extracted codes
// Extern int maxcodelength;      // Depth of the code tree
// Extern string codetreefile;    // Name of file containing codetree data
// Extern string landdir;         // Directory containing binary landmark files
// Extern int cluster;            // Box size to use for clustering
// 
// // Image processing options
// Extern int randomize;          // Whether to randomize certain processes
// Extern double imrotate;          // Amount to rotate an image
// Extern int scale;              // Dimensions of final downscaled image
// Extern int slant;              // Whether to slant-correct an image
// Extern double skew;            // Amount to skew an image
// Extern int filter;             // How many times to median filter an image
// Extern double alpha;           // Parameter used in downscaling an image
// 
// // image viewing options
// Extern int pixsize;            // How many screen pixels per image pixel
// Extern int showpixels;         // Set to draw boxes around screen pixels
// Extern int imborder;           // Blank pixels to display around an image
// Extern int showorig;           // Show the original image

//___________________________________________________________________________
// globlist.h

