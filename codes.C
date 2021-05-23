#ifndef CODES_C
#define CODES_C

#include <cmath>

#include "includes.h"
#include "grid.h"
#include "codes.h"
#include "image.h"
#include "process.h"
#include "globals.h"

//___________________________________________________________________________

void codestree::init(int size)
{
  if (size <= 0)
    return;

  nodes = (size == 0 ? 0 : new codesnode[size]);
  for (int i = 0; i < size / 2; ++i)
  {
    (*this)(i).left = &(*this)(i * 2 + 1);
    (*this)(i).right = &(*this)(i * 2 + 2);
  }

}
// __________________________________________________________________________

int write_nbhds(int codesize, String& listfile, String& codesfile)
{
  // Check to make sure we can proceed.
  if (codesize == 0 || listfile == "" || codesfile == "")
  {
    cerr << "Need to specify:\n";
    cerr << "codesize - the height and width of a code\n";
    cerr << "listfile - a file containing a list of images\n";
    cerr << "codesfile - a file to write out the extracted codes to\n";
    exit(1);
  }

  // Open the listfile and the output file to contain the codes.
  ifstream ifs(listfile);
  if (!ifs)
  {
    cerr << "Unable to open listfile " << listfile << "\n";
    exit(1);
  }
  ofstream ofs(codesfile);
  if (!ofs)
  {
    cerr << "Unable to write to codesfile " << codesfile << "\n";
    exit(1);
  }

  // Find the codes from the digits in the listfile and write
  // them out to the codesfile.
  int digit, digitcount = 0;
  while (ifs >> digit)
  {
    // Read in the next digit and process it.
    image im;
    im.read(digit);
    im.std_process(filter, scale, slant, skew, rotate, thin);

    // Find the codes.
    cout << "Processing digit # " << ++digitcount << "\n";
    for (int j = -codesize + 1; j < im.dimx(); ++j)
    {
      for (int k = -codesize + 1; k < im.dimy(); ++k)
      {
        // Determine the number of stroke pixels in the middle of this code.
        int midstroke = 0;
        for (int l = 1; l < codesize - 1; ++l)
        {
          for (int m = 1; m < codesize - 1; ++m)
          {
            midstroke += (im.get(j + l, k + m) > 0);
          }
        }

        // Make sure we have on and off pixels in the center.
        if (midstroke == 0 || midstroke == (codesize - 2) * (codesize - 2))
          continue;
	
	// Now associate a bit string to the code, output it as an integer.
	uint region = 0;
        for (int l = 0; l < codesize; ++l)
        {
          for (int m = 0; m < codesize; ++m)
          {
            region |= ((im.get(j + l, k + m) > 0) << (m * codesize + l));
          }
        }
        ofs << region << "\n";
      }
    }
  }
  return 1;
  
} // write_nbhds

// __________________________________________________________________________

int show_codes(int codesize, String& codesfile, int quantity)
{
  // Check to make sure we can proceed.
  if (codesize == 0 || codesfile == "")
  {
    cerr << "Need to specify:\n";
    cerr << "codesize - the height and width of a code\n";
    cerr << "codesfile - a file containing previously extracted codes\n";
    exit(1);
  }

  // Display the regions corresponding to the codes.
  int count = 0;
  uint code;
  ifstream ifs(codesfile);
  while (ifs >> code && count < quantity)
  {
    ++count;
    image im(codesize, codesize);
    for (int j = 0; j < codesize; ++j)
      for (int k = 0; k < codesize; ++k)
        im(j, k) = (code & (1 << k * codesize + j)) >> (k * codesize + j);
    im.view(ltos(count));
  }

  return 1;

} // show_codes

#ifdef 0

// __________________________________________________________________________

int make_codes_tree(int codesize, int maxcodelength, 
		    String& codesfile, String& codetreefile)
{
  // Check to make sure we can proceed.
  if (codesize == 0 || maxcodelength == 0 || 
      codesfile == "" || codetreefile == "")
  {
    cerr << "Need to specify:\n";
    cerr << "codesize - the height and width of a code\n";
    cerr << "maxcodelength - by default 5, this is the depth of the codetree\n";
    cerr << "codesfile - a file containing previously extracted codes\n";
    cerr << "codetreefile - the file the codetree will be written to\n";
    exit(1);
  }

  // Open the codesfile.
  ifstream ifs(codesfile);
  if (!ifs)
  {
    cerr << "Unable to open codesfile " << codesfile << "\n";
    exit(1);
  }

  // Determine how many codes there are.
  int code, numcodes = 0;
  while (ifs >> code)
    ++numcodes;

  // Initialize relevant variables, then calculate the codes tree.
  ifs.close();
  ifs.open(codesfile);
  uigrid histories(numcodes);
  igrid patterns(numcodes);
  for (int i = 0; i < numcodes; ++i)
  {
    histories(i) = 1;
    ifs >> patterns(i);
  }

  // Calculate the code_tree.
  float tree_impurity = 0.0;
  int qlistsize = int(pow(2, maxcodelength) - 1);
  int qlist[qlistsize];
  for (int i = 0; i < qlistsize; ++i)
    qlist[i] = -1;
  calc_code_tree(0, 1, histories, patterns, numcodes, qlist, tree_impurity);
  cout << "Tree impurity = [" << tree_impurity << "]\n";

  // Print out the patterns and the histories.
  ofstream ofsp("PATTERNS");
  for (int i = 0; i < numcodes; ++i)
  {
    for (int j = maxcodelength; j > 0; --j)
      ofsp << ((histories[i] >> (j - 1)) & 1);
    ofsp << " " << patterns[i] << endl;
  }

  // Output the qlistsize codes to the codetreefile.
  ofstream ofs(codetreefile);
  if (!ofs)
  {
    cerr << "Unable to open codetreefile " << codetreefile << "\n";
    exit(1);
  }
  for (int i = 0; i < qlistsize; ++i)
    ofs << qlist[i] << "\n";

  delete [] histories;
  delete [] patterns;

  return 1;

} // make_codes_tree

// __________________________________________________________________________

void calc_code_tree(int currdepth, uint hist, 
                   uint *histories, int *patterns, int numcodes, 
                   int *qlist, float &tree_impurity)
{
  // Initialize the answers.
  int answers[codesize * codesize][3];
  for (int i = 0; i < codesize * codesize; ++i)
    for (int j = 0; j < 3; ++j)
      answers[i][j] = 0;

  // Determine the number of positive and negative answers to each 
  // question for all the data with the current history.
  for (int i = 0; i < numcodes; ++i)
  {
    if (histories[i] == hist)
    {
      for (int j = 0; j < codesize * codesize; ++j)
      {
        ++answers[j][(patterns[i] >> j) & 1];
        ++answers[j][2];
      }
    }
  }
 
  // New stopping criterion.  See if any answer is good enough,
  // and determine for each question whether it is good enough.
  int good = (opt == 22);
  int good_ans[codesize * codesize];
  for (int i = 0; i < codesize * codesize; ++i)
  {
    good += (good_ans[i] = (opt == 22 || (answers[i][0] > alpha * numcodes && 
			    answers[i][1] > alpha * numcodes)));
  }
 
  // Quit the recursive routine if we are too deep and print out results.
  if (currdepth == maxcodelength || !good)
  {
    // Print out the depth and history
    cout << "Terminal Node: Depth = " << currdepth << ", History = ";
    for (int i = currdepth; i > 0; --i)
      cout << ((hist >> (i - 1)) & 1);
    cout << "\n";

    // Print out the terminal count
    int count = 0;
    for (int i = 0; i < numcodes; ++i)
      count += (hist == histories[i]);
    cout << count << " patterns were sent to this node.\n";

    // Print out the impurity and update the average tree impurity.
    float impurity = 0.0;
    if (answers[0][2] > 0)
      for (int i = 0; i < codesize * codesize; ++i)
        impurity += 1.0 * answers[i][0] * answers[i][1] / 
          answers[i][2] / answers[i][2];
    cout << "Impurity of this terminal is [" << impurity << "]\n\n";
    tree_impurity += impurity * count / numcodes;

    return;
  }
 
  // Determine the best question 50-50-wise.
  int bestsplitquest = 0;
  float bestsplit = 1.0;
  for (int i = 0; i < codesize * codesize; ++i)
  {
    if (answers[i][2] > 0 &&
	fabs(0.5 - (1.0 * answers[i][1] / answers[i][2])) < bestsplit)
    {
      bestsplitquest = i;
      bestsplit = fabs(0.5 - (1.0 * answers[i][1] / answers[i][2]));
    }  
  }
    
  // Determine the best question purity-wise.
  int bestimpquest = 0;
  float bestimp = 1000000.0;
  int counts_l[codesize * codesize][codesize * codesize][3];
  int counts_r[codesize * codesize][codesize * codesize][3];
  float aveimp[codesize * codesize];
  for (int i = 0; i < codesize * codesize; ++i)
  {
    if (!good_ans[i])
      continue;
    for (int j = 0; j < codesize * codesize; ++j)
    {
      counts_l[i][j][0] = counts_l[i][j][1] = counts_l[i][j][2] = 0;
      counts_r[i][j][0] = counts_r[i][j][1] = counts_r[i][j][2] = 0;
    }
    for (int k = 0; k < numcodes; ++k)
    {
      if (histories[k] == hist)
      {
        if ((patterns[k] >> i) & 1)
        {
          for (int j = 0; j < codesize * codesize; ++j)
	  {
	    ++counts_r[i][j][(patterns[k] >> j) & 1];
	    ++counts_r[i][j][2];
	  }
        }
	else
        {
          for (int j = 0; j < codesize * codesize; ++j)
	  {
	    ++counts_l[i][j][(patterns[k] >> j) & 1];
	    ++counts_l[i][j][2];
	  }
        }
      }
    }
    float imp_l = 0.0, imp_r = 0.0;
    for (int j = 0; j < codesize * codesize; ++j)
    {
      if (counts_l[i][j][2] > 0)
        imp_l += 1.0 * counts_l[i][j][0] * counts_l[i][j][1] / 
	  counts_l[i][j][2] / counts_l[i][j][2];
      if (counts_r[i][j][2] > 0)
        imp_r += 1.0 * counts_r[i][j][0] * counts_r[i][j][1] / 
	  counts_r[i][j][2] / counts_r[i][j][2];
    }
    float p_l = 1.0 * answers[i][0] / answers[i][2];
    float p_r = 1.0 * answers[i][1] / answers[i][2];
    aveimp[i] = p_l * imp_l + p_r * imp_r;
    if (aveimp[i] < bestimp && good_ans[i])
    {
      bestimp = aveimp[i];
      bestimpquest = i;
    }
  }
  int bestquest = (opt == 22 ? bestsplitquest : bestimpquest);
  qlist[hist - 1] = bestquest;
 
  // Print out the statistics for each question.
  cout << "Depth = " << currdepth << ", History = ";
  for (int i = currdepth; i > 0; --i)
    cout << ((hist >> (i - 1)) & 1);
  cout << "\nBest new question is : " << bestquest << "\n";
  for (int i = 0; i < codesize * codesize; ++i)
  {
    cout << setw(2) << i << " : " << setprecision(2) << setw(6) 
      << 100 * float(answers[i][1]) / answers[i][2] << " ("
      << setw(5) << answers[i][1] << " of " << setw(5) << answers[i][2]
      << ")\n";
  }
  float node_impurity = 0.0;
  for (int i = 0; i < codesize * codesize; ++i)
    node_impurity += 1.0 * answers[i][0] * answers[i][1] / 
      answers[i][2] / answers[i][2];
  cout << "Node impurity is : " << node_impurity << "\n";
  cout << "\n";
 
  // Update the histories of each pattern in the current node.
  for (int i = 0; i < numcodes; ++i)
  {
    if (histories[i] == hist)
      histories[i] = (histories[i] << 1) + ((patterns[i] >> bestquest) & 1);
  }
 
  // Recursively call this routine for the yes and no answers.
  calc_code_tree(currdepth + 1, (hist << 1), histories,
            patterns, numcodes, qlist, tree_impurity);
  calc_code_tree(currdepth + 1, (hist << 1) + 1, histories,
            patterns, numcodes, qlist, tree_impurity);
 
} // calc_code_tree

#endif

// __________________________________________________________________________

void write_codelms(int codesize, int maxcodelength, 
		  String& codetreefile, String& landdir)
{

  // Check to make sure we can proceed.
  if (codesize == 0 || maxcodelength == 0 || 
      codetreefile == "" || landdir == "")
  {
    cerr << "Need to specify:\n";
    cerr << "codesize - the height and width of a code\n";
    cerr << "maxcodelength - by default 5, this is the depth of the codetree\n";
    cerr << "codetreefile - the file the codetree will be written to\n";
    cerr << "landdir - a directory to store the landmark files\n";
    exit(1);
  }

  // Initialize and read in the code_tree.
  int numcodes, numnodes = int(pow(2, maxcodelength) - 1);
  codestree ct;
  ct.read(codetreefile, numnodes, numcodes);
    
  int count = 0;
  int namecount = 0;
  lmarkslist lands(100);
  // Loop throught the relevant digits.
  for (int digit = first; digit < first + numdigits; ++digit)
  {
    // Read in the digit and process it.
    image im;
    if (im.read(digit))
    {
      cout << "Processing digit " << digit << endl;
      im.std_process(filter, scale, slant, skew, rotate, thin);
      
      // Calculate the code locations, clustering as indicated.
      lands(count).set_truth(im.truth());
      lands(count).init_counts(numcodes);
      lands(count).init_offsets(numcodes);
      lands(count).extract(im, ct, codesize);

      // Write out the landmark information.
      if (++count == 100)
      {
	String landfile = landdir + "/Lands_" + ltos(namecount++);
	ofstream ofs(landfile);
	if (!ofs)
	{
	  cerr << "Unable to create landmark files\n";
	  exit(1);
	}

	ofs.write(&count, Sint);
	int offset = (1 + count) * Sint;
	ofs.write(&offset, Sint);
	for (int i = 1; i < 100; ++i)
	{
	  offset += lands(i - 1).size();
	  ofs.write(&offset, Sint);
	}
	for (int i = 0; i < count; ++i)
	  lands(i).write(ofs);
	count = 0;
      }
    }
  }

  // Write out the remaining landmark information.
  if (count != 0)
  {
    ofstream ofs(landdir + "/Lands_" + ltos(namecount++));
    if (!ofs)
    {
      cerr << "Unable to create landmark files\n";
      exit(1);
    }
    
    ofs.write(&count, Sint);
    int offset = (1 + count) * Sint;
    ofs.write(&offset, Sint);
    for (int i = 1; i < count; ++i)
    {
      offset += lands(i - 1).size();
      ofs.write(&offset, Sint);
    }
    
    for (int i = 0; i < count; ++i)
      lands(i).write(ofs);
  }

} // write_codelms

// __________________________________________________________________________

void codestree::read(String& codetreefile, int numnodes, int& numcodes)
{
  // Open the codetreefile
  ifstream ifs(codetreefile);
  if (!ifs)
  {
    cerr << "Unable to open codetreefile " << codetreefile << "\n";
    exit(1);
  }

  // Initialize the codestree
  init(numnodes);

  // Fill in the values for the codestree
  numcodes = 0;
  for (int i = 0; i < numnodes; ++i)
  {
    if (!(ifs >> (*this)(i).pixel))
    {
      cerr << "Problem parsing codetreefile " << codetreefile << "\n";
      exit(1);
    }
    if ((*this)(i).pixel != -1)
    {
      (*this)(i).index = numcodes;
      (*this)(i).depth = int(log(i + 1) / log(2) + .001);
      numcodes += 2;
    }
  }

} // codestree::read

// __________________________________________________________________________
// Finds the codes in a given 'num'.  Assumes that codesize is set and
// that the codes_tree has already been read in.

void landmarks::extract(ucgrid& u, codestree& ct, int codesize)
{

  // Use an exceptionally large array to store all instances
  // of all unclustered landmarks
  lmarklist land(10000);
  int landcount = 0;

  for (int j = -codesize + 1; j < u.dimx(); ++j)
  {
    for (int k = -codesize + 1; k < u.dimy(); ++k)
    {
      // Determine the number of stroke pixels in the middle of this code.
      int midstroke = 0;
      for (int l = 1; l < codesize - 1; ++l)
      {
        for (int m = 1; m < codesize - 1; ++m)
        {
          midstroke += (u.get(j + l, k + m) != 0);
        }
      }
      // Make sure we have on and off pixels in the center.
      if (midstroke == 0 || midstroke == (codesize - 2) * (codesize - 2))
        continue;

      // Determine the appropriate locations.
      codesnode *currcodenode = &ct(0);
      while (currcodenode != 0 && currcodenode->pixel != -1)
      {
        int answer = (u.get(j + currcodenode->pixel % codesize,
                              k + currcodenode->pixel / codesize) != 0);
	
	land(landcount).cx = j + codesize / 2;
	land(landcount).cy = k + codesize / 2;
        if (answer)
        {
	  land(landcount).index = currcodenode->index + 1;
          currcodenode = currcodenode->right;
        }
        else
        {
	  land(landcount).index = currcodenode->index;
          currcodenode = currcodenode->left;
        }
	++landcount;
      }
    }
  }

  // Check landcount size just in case something is weird
  if (landcount >= 10000)
  {
    cerr << "landcount too big in landmark::extract\n";
    exit(1);
  }

  // Cluster the landmarks as indicated.
  cluster_landmarks(land, cluster, total_types(), landcount);

  // Convert the array of locations into a landmarks structure

  // First set up the type counts;
  typecounts.clear();
  for (int i = 0; i < landcount; ++i)
    ++typecounts(land(i).index);

  // Next set up the type offsets;
  typeoffsets(0) = 0;
  for (int i = 1; i < total_types(); ++i)
    typeoffsets(i) = typeoffsets(i - 1) + typecounts(i - 1);

  // Initialize and fill in the locations
  init_locations(landcount);
  igrid offsets(total_types());
  offsets.clear();
  for (int i = 0; i < landcount; ++i)
  {
    int ind = land(i).index;
    location_x(ind, offsets(ind)) = land(i).cx;
    location_y(ind, offsets(ind)) = land(i).cy;
    ++offsets(ind);
  }

} // landmarks::extract

//___________________________________________________________________________

void cluster_landmarks(lmarklist& lands, int clustsize, 
		       int numtypes, int& total)
{
  // Return now if there is no clustering
  if (clustsize < 1)
    return;

  // Determine extents
  int minx = 1000;
  int miny = 1000;
  int maxx = 0;
  int maxy = 0;
  for (int i = 0; i < total; ++i)
  {
    if (lands(i).cx < minx) minx = lands(i).cx;
    if (lands(i).cx > maxx) maxx = lands(i).cx;
    if (lands(i).cy < miny) miny = lands(i).cy;
    if (lands(i).cy > maxy) maxy = lands(i).cy;
  }

  int xclusters = (maxx - minx + 1) / clustsize + 1;
  int yclusters = (maxy - miny + 1) / clustsize + 1;
  igrid numfound(xclusters, yclusters);
  igrid orig(xclusters, yclusters);
  for (int currtype = 0; currtype < numtypes; ++currtype)
  {
    numfound.clear();
    for (int i = 0; i < total; ++i)
    {
      if (lands(i).index != currtype)
	continue;

      // Determine which cluster the instance belongs to
      int x = (lands(i).cx - minx) / clustsize;
      int y = (lands(i).cy - miny) / clustsize;
      if (++numfound(x, y) == 1)
	orig(x, y) = i;
      else
      {
	lands(orig(x, y)).cx += lands(i).cx;
	lands(orig(x, y)).cy += lands(i).cy;
	swap(lands(i--), lands(--total));
      }
    }
    for (int i = 0; i < xclusters; ++i)
    {
      for (int j = 0; j < yclusters; ++j)
      {
	if (numfound(i, j) > 0)
	{
	  lands(orig(i, j)).cx /= numfound(i, j);
	  lands(orig(i, j)).cy /= numfound(i, j);
	}
      }
    }
  }

} // cluster_landmarks

// __________________________________________________________________________
// codes.c

#endif // CODES_C
