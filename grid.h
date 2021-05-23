#ifndef GRID_H
#define GRID_H

//__________________________________________________________________________
// Includes

#include <cassert>
#include <fstream>
#include <cstring>

#include "includes.h"

using namespace std;

//___________________________________________________________________________
// grid of elements of type T

template <class T>
class grid
{
friend class image;

 private:
  int nx, ny;
  T *sto;

  // Initialization Member Functions
  void initnew(int x, int y) { 
    assert(x >= 0 && y >= 0); 
    nx = x; ny = y; sto = (x == 0 || y == 0) ? 0 : new T[x * y];
    assert(invariant()); 
  }
  void freegrid() { assert(invariant()); nx = ny = 0; delete [] sto; sto = 0; }
  
  // Consistency Checking Functions
  int inrange(int x, int y) const { 
    return (x >= 0 && y >= 0 && x < nx && y < ny && sto != 0); 
  }
  int invariant() const { 
    return (((nx == 0 || ny == 0) && sto == 0 && nx >= 0 && ny >= 0) || 
	    ((nx > 0 && ny > 0) && sto != 0));
  }

 public:
  // Constructors, Destructor, and Operator=
  grid() { initnew(0, 0); }
  grid(int x) { initnew(x, 1); }
  grid(int x, int y) { initnew(x, y); }
  grid(const grid &m) : nx(0), ny(0), sto(0) { *this = m; }
  grid<T>& operator=(const grid &m);
  ~grid() { assert (invariant()); delete [] sto; }

  // Basic Member Access Functions
  int dimx() const { return nx; }
  int dimy() const { return ny; }
  T& operator()(int x, int y = 0) const { 
    assert(inrange(x, y)); return sto[y * nx + x]; 
  }
  void set(int x, int y, T val) { if (inrange(x, y)) (*this)(x, y) = val; }
  void set(double x, double y, T val) { set(int(x), int(y), val); }
  void set(int x, double y, T val) { set(x, int(y), val); }
  void set(double x, int y, T val) { set(int(x), y, val); }
  T get(int x, int y) const { return (inrange(x, y) ? (*this)(x, y) : 0); }
  T get(double x, double y) const { return get(int(x), int(y)); }
  T get(int x, double y) const { return get(x, int(y)); }
  T get(double x, int y) const { return get(int(x), y); }

  // Initialization Member Functions
  void init() { init(0, 0); }
  void init(int x) { init(x, 1); }
  void init(int x, int y) { 
    assert(invariant());
    if (x > 0 && y > 0 && x * y <= nx * ny && x * y * 2 >= nx * ny) 
      { nx = x; ny = y; }
    else 
      { freegrid() ; initnew(x, y); } 
    assert(invariant()); 
  }
  void clear() { assert(invariant()); memset(sto, 0, nx * ny * sizeof(T)); }

  // I/O Functions
  void write(const char *file);
  int write(ofstream& os);
  int read(const char *file);
  int read(ifstream& is);
  int size();
  void dump() const;

  // Useful Utility Functions
  grid<T>& operator<<(grid &m);
  grid<T>& operator+=(const grid &m);
  grid<T> operator*(const grid &m) const;
  void normalize(T val);
  grid<T> transpose() const;
  grid<T> LU() const;
  grid<T> inverse() const;
  int offpixels() { 
    int c = 0; for (int i = 0; i < nx * ny; ++i) c += (sto[i] == 0); return c;
  }
  int onpixels() { return(nx * ny - offpixels()); }
  void sort(int left = -1, int right = -1);

}; // class grid

//___________________________________________________________________________
// Dump a grid; Will only work for classes with ostream<<(const T&).

template <class T>
void grid<T>::dump() const
{
  for (int j = 0; j < ny; ++j)
  {
    for (int i = 0; i < nx; ++i)
      std::cout << ((*this)(i, j)) << " ";
    std::cout << "\n";
  }

} // grid::dump

//___________________________________________________________________________
// Write out a grid to a file

template<class T>
void grid<T>::write(const char *file)
{
  assert(invariant());
  ofstream ofs(file);
  if (!ofs)
  {
    std::cerr << "Unable to write a grid to file [" << file << "]." << endl;
    return;
  }

  ofs.write("GR11", 4);
  ofs.write(&nx, sizeof(nx));
  ofs.write(&ny, sizeof(ny));
  if (nx > 0 && ny > 0)
    ofs.write(sto, nx * ny * sizeof(T));

} // grid::write

//___________________________________________________________________________
// Write out a grid to an open output stream

template<class T>
int grid<T>::write(ofstream& os)
{
  assert(invariant());

  if (!os.is_open())
    return 0;

  os.write(&nx, sizeof(nx));
  os.write(&ny, sizeof(ny));
  if (nx > 0 && ny > 0)
    os.write(sto, nx * ny * sizeof(T));

  return 1;

} // grid::write

//___________________________________________________________________________
// Calculate the size of a grid which is to be written out

template<class T>
int grid<T>::size()
{
  assert(invariant());
  return (sizeof(nx) + sizeof(ny) + nx * ny * sizeof(T));

} // grid::size

//___________________________________________________________________________
// Read in a grid which was previously written to a file

template<class T>
int grid<T>::read(const char *file)
{
  assert(invariant());
  ifstream ifs(file);
  if (!ifs)
    return 0;

  char version[4];
  ifs.read(version, 4);
  if (memcmp(version, "GR11", 4) != 0 && memcmp(version, "GR12", 4) != 0 &&
      memcmp(version, "MA11", 4) != 0)
  {
    std::cerr << "The file [" << file << "] is not a grid file" << endl;
    return 0;
  }
 
  freegrid();
  if (!memcmp(version, "GR11", 4))
  {
    ifs.read(&nx, sizeof(nx));
    ifs.read(&ny, sizeof(ny));
    assert(nx >= 0 && ny >= 0);
    if (nx > 0 && ny > 0)
    {
      sto = new T[nx * ny];
      ifs.read(sto, nx * ny * sizeof(T));
    }
  }
  else if (!memcmp(version, "MA11", 4))
  {
    ifs.read(&ny, sizeof(ny));
    ifs.read(&ny, sizeof(ny));
    ifs.read(&nx, sizeof(nx));
    ifs.read(&nx, sizeof(nx));
    nx += 1;
    ny += 1;
    assert(nx >= 0 && ny >= 0);
    if (nx > 0 && ny > 0)
    {
      sto = new T[nx * ny];
      ifs.read(sto, nx * ny * sizeof(T));
    }
  }
  else
  {
    ifs >> nx;
    ifs >> ny;
    assert(nx >= 0 && ny >= 0);
    if (nx > 0 && ny > 0)
    {
      sto = new T[nx * ny];
      for (int i = 0; i < nx; ++i)
        for (int j = 0; j < ny; ++j)
          ifs >> (*this)(i, j);
    }
  }
  assert(invariant());

  return 1;

} // grid::read

//___________________________________________________________________________
// Read in a grid from an open istream

template<class T>
int grid<T>::read(ifstream& is)
{
  assert(invariant());

  if (!is.is_open())
    return 0;

  freegrid();
  is.read(&nx, sizeof(nx));
  is.read(&ny, sizeof(ny));
  assert(nx >= 0 && ny >= 0);
  if (nx > 0 && ny > 0)
  {
    sto = new T[nx * ny];
    is.read(sto, nx * ny * sizeof(T));
  }

  assert(invariant());

  return 1;

} // grid::read

//___________________________________________________________________________
// Set a grid equal to a grid 'm'; leave 'm' unchanged.

template<class T>
grid<T>& grid<T>::operator=(const grid& m)
{
  assert(invariant() && m.invariant());
  if (this != &m)
  {
    init(m.nx, m.ny);
    if (nx > 0 && ny > 0) 
      memcpy(sto, m.sto, nx * ny * sizeof(T));
    
  }
  assert(invariant() && m.invariant());

  return *this;

} // grid::operator=

//___________________________________________________________________________
// Set a grid equal to 'm'; obliterate 'm'.

template<class T>
grid<T>& grid<T>::operator<<(grid &m)
{
  assert(invariant() && m.invariant());
  if (this != &m)
  {
    freegrid(); 
    nx = m.nx; ny = m.ny; sto = m.sto;
    m.initnew(0, 0);
  }
  assert(invariant() && m.invariant());

  return *this;

} // grid::operator<<
  
//___________________________________________________________________________
// Add another grid to the current grid.

template<class T>
grid<T>& grid<T>::operator+=(const grid& m)
{
  assert(invariant() && m.invariant());
  assert(nx == m.nx && ny == m.ny);

  for (int i = 0; i < nx * ny; ++i)
    sto[i] += m.sto[i];

  assert(invariant() && m.invariant());

  return *this;

} // grid::operator+=

//___________________________________________________________________________

template<class T>
void grid<T>::normalize(T val)
{
  T gridmax = 0;

  assert(invariant());
  for (int i = 0; i < nx; ++i)
    for (int j = 0; j < ny; ++j)
      if ((*this)(i, j) > gridmax)
        gridmax = (*this)(i, j);
  if (gridmax > 0)
  {
    for (int i = 0; i < nx; ++i)
      for (int j = 0; j < ny; ++j)
        (*this)(i, j) = (*this)(i, j) * val / gridmax;
  }
  assert(invariant());

} // grid::normalize

//___________________________________________________________________________

template<class T>
grid<T> grid<T>::operator*(const grid &m) const
{
  assert(invariant() && m.invariant());
  assert(nx == m.ny);

  grid<T> tmp(m.nx, ny);
  tmp.clear();
  for (int i = 0; i < m.nx; ++i)
    for (int j = 0; j < ny; ++j)
      for (int k = 0; k < nx; ++k)
	tmp(i, j) += (*this)(k, j) * m(i, k);

  assert(invariant() && m.invariant() && tmp.invariant());
  return tmp;

} // grid<T>::operator*

//___________________________________________________________________________

template<class T>
grid<T> grid<T>::LU() const
{
  assert(invariant());
  assert(nx == ny);

  grid<T> tmp = *this;
  for (int i = 0; i < nx - 1; ++i)
  {
    for (int j = i + 1; j < nx; ++j)
      tmp(j, i) /= tmp(i, i);
    for (int j = i + 1; j < nx; ++j)
      for (int k = i + 1; k < nx; ++k)
	tmp(j, k) -= tmp(j, i) * tmp(i, k);
  }

  assert(invariant() && tmp.invariant());
  return tmp;

} // grid::LU

//___________________________________________________________________________

template<class T>
grid<T> grid<T>::inverse() const
{
  assert(invariant());
  assert(nx == ny);
  
  grid<T> tmp = *this;

  grid<int> p(nx);
  for (int j = 0; j < nx; ++j)
    p(j) = j;
  grid<double> hv(nx);
  hv.clear();

  for (int j = 0; j < nx; ++j)
  {
    T max = fabs(tmp(j, j));
    int r = j;
    for (int i = j + 1; i < nx; ++i)
    {
      if (fabs(tmp(i, j)) > max)
      {
	max = fabs(tmp(i, j));
	r = i;
      }
    }
    if (max == 0.0)
    {
      std::cerr << "Unable to invert a matrix" << endl;
      exit(1);
    }
    if (r > j)
    {
      for (int k = 0; k < nx; ++k)
	swap(tmp(j, k), tmp(r, k));
      swap(p(j), p(r));
    }
    T hr = 1 / tmp(j, j);
    for (int i = 0; i < nx; ++i)
      tmp(i, j) *= hr;
    tmp(j, j) = hr;
    for (int k = 0; k < nx; ++k)
    {
      if (k == j)
	continue;
      for (int i = 0; i < nx; ++i)
      {
	if (i == j)
	  continue;
	tmp(i, k) -= tmp(i, j) * tmp(j, k);
      }
      tmp(j, k) *= (-hr);
    }
  }
  for (int i = 0; i < nx; ++i)
  {
    for (int k = 0; k < nx; ++k)
      hv(p(k)) = tmp(i, k);
    for (int k = 0; k < nx; ++k)
      tmp(i, k) = hv(k);
  }
  
  assert(invariant() && tmp.invariant());
  return tmp;

} // grid::inverse

//___________________________________________________________________________

template<class T>
grid<T> grid<T>::transpose() const
{
  assert(invariant());
  grid<T> tmp(ny, nx);
  for (int i = 0; i < ny; ++i)
    for (int j = 0; j < nx; ++j)
      tmp(i, j) = (*this)(j, i);
  assert(invariant() && tmp.invariant());

  return tmp;

} // grid::transpose

//___________________________________________________________________________

template<class T>
void grid<T>::sort(int left, int right)
{
  assert(ny == 1);

  if (left == -1 && right == -1)
  {
    left = 0;
    right = nx - 1;
  }

  int i = left;
  int j = right;

  T midval = (*this)((left + right) / 2);

  do 
  {
    while ((*this)(i) < midval && i < right)
      ++i;
    while (midval < (*this)(j) && j > left)
      --j;

    if (i <= j)
    {
      T tmpval = (*this)(i);
      (*this)(i) = (*this)(j);
      (*this)(j) = tmpval;
      ++i;
      --j;
    }

  } 
  while (i <= j);

  if (left < j)
    sort(left, j);
  if (i < right)
    sort(i, right);

} // grid::sort

//___________________________________________________________________________
// Local defines

#define cgrid grid<char>
#define ucgrid grid<uchar>
#define igrid grid<int>
#define uigrid grid<uint>
#define lgrid grid<long>
#define fgrid grid<float>
#define dgrid grid<double>

//___________________________________________________________________________
// grid.h

#endif
