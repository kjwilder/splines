GLOBAL(int, dum1, 0)

// Options controlling the current run of the program
GLOBAL(int, opt, 0)                // Main option for running the program
GLOBAL(int, viewopt, 0)            // Options used when viewing objects
GLOBAL(int, extension, 0)          // Used to index repeated runs of the program
GLOBAL(long, seed, 0)              // The random number generator seed

// Input/Output file names
GLOBAL(string, datadir, "/usr/data")  // Directory containing data
GLOBAL(string, dataset, "nisttrain")  // A particular data set
GLOBAL(string, dumpfile, "")       // Output file containing all global values
GLOBAL(string, parfile, "")        // Input file containing global initializers
GLOBAL(string, listfile, "")       // File with a list of digits indices
GLOBAL(string, treefile, "")       // Name of binary tree file
GLOBAL(string, aggfile, "")        // Binary file stores aggregated results


GLOBAL(int, revvideo, 0)
GLOBAL(int, colors, 0)

GLOBAL(double, alpha, 1.0 / 3.0)
GLOBAL(int, cluster, 3)
GLOBAL(int, codesize, 0)
GLOBAL(string, codesfile, "")
GLOBAL(string, codetreefile, "")
GLOBAL(int, far, 0)
GLOBAL(int, thin, 0)
GLOBAL(int, filter, 1)
GLOBAL(int, first, 0)
GLOBAL(int, imagesperfile, 100)
GLOBAL(int, imborder, 20)
GLOBAL(string, impmeasure, "entropy")
GLOBAL(int, initdataopt, 0)
GLOBAL(int, instdist, 6)
GLOBAL(int, intmindata, 0)
GLOBAL(string, landdir, "/usr/data/nisttrainL")
GLOBAL(int, last, 0)
GLOBAL(int, maxcodelength, 0)
GLOBAL(int, maxdepth, 30)
GLOBAL(int, minang, 0)
GLOBAL(int, minland, 0)
GLOBAL(int, mindis, 0)
GLOBAL(int, mintol, 0)
GLOBAL(int, numang, 0)
GLOBAL(int, numland, 0)
GLOBAL(int, numdis, 0)
GLOBAL(int, numtol, 0)
GLOBAL(int, mindata, 1)
GLOBAL(double, minentdrop, 0)
GLOBAL(int, minpernode, 0)
GLOBAL(int, modetwo, 0)
GLOBAL(int, near, 0)
GLOBAL(int, numdata, 0)
GLOBAL(int, numdigits, 1)
GLOBAL(int, numtreedata, 0)
GLOBAL(int, numquesask, 0)
GLOBAL(int, numquesdata, 0)
GLOBAL(int, pixsize, 4)
GLOBAL(double, purepar, 1.0)
GLOBAL(int, randomize, 0)
GLOBAL(double, imrotate, 0)
GLOBAL(int, scale, 32)
GLOBAL(int, showorig, 0)
GLOBAL(int, showpixels, 0)
GLOBAL(double, skew, 0)
GLOBAL(int, slant, 1)
GLOBAL(int, tmptreedata, 0)
// GLOBALB(useques, 21)
