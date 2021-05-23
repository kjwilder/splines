#ifndef TREE_H
#define TREE_H

#include <iostream>

//___________________________________________________________________________

class history
{
 private:
  int h;  // 4 Bytes used to store a history of yes and no answers in a tree

 public:
  history(int x = 1) : h(x) { }
  void operator=(int x) { h = x; }
  void  operator=(const history& x) { h = x.h; }
  operator int() const { return h; }
  friend ostream& operator<<(ostream& os, const history& m);
};

//___________________________________________________________________________

inline ostream& operator<<(ostream& os, const history& m)
{
  int foundstart = 0;
  for (int i = 31; i >= 0; --i)
  {
    int currbit = ((m.h >> i) & 1);
    if (foundstart)
      os << currbit;
    foundstart |= currbit;
  }
  return os;

} // operator<<

//___________________________________________________________________________

template <class T>
class treenode
{
 private:
  history h;
  treenode *parent, *no, *yes;

 public:
  T data;

  treenode(int h = 1, treenode* p = 0, treenode* n = 0, treenode* y = 0) : 
    h(h), parent(p), no(n), yes(y) { }
  int depth() const { int k = 32; while (!(h >> --k)); return k; }
  const history& hist() const { return h; }
  friend class treeiterator<T>;
  friend class tree<T>;
};

//___________________________________________________________________________

template <class T>
class treeiterator
{
 private:
  treenode<T> *p;

 public:
  treeiterator(treenode<T> *x = 0) : p(x) { }
  treeiterator(const treeiterator<T>& t) : p(t.p) { }
  void operator=(const treeiterator<T>& t) { p = t.p; }
  operator int() const { return (p ? 1 : 0); }
  treenode<T>* operator->() const { assert(p != 0); return p; }
  treeiterator<T> parent() const { return treeiterator<T>(p ? p->parent : 0); }
  treeiterator<T> no() const { return treeiterator<T>(p ? p->no : 0); }
  treeiterator<T> yes() const { return treeiterator<T>(p ? p->yes : 0); }
  int branch() const { return (p != 0 && p->yes != 0 && p->no != 0); }
  int leaf() const { return (p != 0 && p->yes == 0 && p->no == 0); }
  friend class tree<T>;
};

//___________________________________________________________________________

template <class T>
class tree
{
 private:
  treeiterator<T> treetop;

 public:
  tree() { }
  ~tree() { freenodes(treetop); }
  void freenodes(const treeiterator<T>& t);
  const treeiterator<T>& top() { return treetop; }
  void maketop() { treetop.p = new treenode<T>; }
  void makechildren(const treeiterator<T>& t);
};

template <class T>
inline void tree<T>::makechildren(const treeiterator<T>& t)
{
  assert(t);
  t.p->no = new treenode<T>(t.p->h * 2, t.p, 0, 0);
  t.p->yes = new treenode<T>(t.p->h * 2 + 1, t.p, 0, 0);

} // tree:makechildren

template <class T>
inline void tree<T>::freenodes(const treeiterator<T>& t)
{
  if (!t)
    return;
  
  if (t.branch()) {
    freenodes(t.yes());
    freenodes(t.no());
  }

  delete t.p;

} // tree::freenodes

//___________________________________________________________________________
// tree.h
#endif
