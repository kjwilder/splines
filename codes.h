#ifndef CODES_H
#define CODES_H

//___________________________________________________________________________

class codesnode
{
 public:
  int pixel;
  int depth;
  int index;
  codesnode* left;
  codesnode* right;

  codesnode() : pixel(0), depth(0), index(0), left(0), right(0) { }
  ~codesnode() { }
};

class codestree
{
 public:
  int size;
  codesnode* nodes;

  codestree(int s = 0) { init(s); }
  ~codestree() { delete [] nodes; }
  codestree(const codestree& c) { c.noop(); }
  void operator=(const codestree& c) { c.noop(); }

  void init(int s = 0);
  void read(string& codetreefile, int numnodes, int& numcodes);
  void noop() const { exit(1); }
  codesnode& operator()(int x) { return nodes[x]; }
};

//___________________________________________________________________________
// Fuction prototypes

int write_nbhds(int codesize, string& listfile, string& codesfile);
int show_codes(int codesize, string& codesfile, int quantity);
void write_codelms(int codesize, int maxcodelength, 
		  string& codetreefile, string& new_land_file);


//___________________________________________________________________________
// codes.h

#endif // CODES_H

