#ifndef IMAGE_H
#define IMAGE_H

#include "grid.h"
#include "view.h"

//___________________________________________________________________________
// data structure used to store and manipulate images

class image : public ucgrid, public viewobj
{
 private:
  int tr;

 public:
  image() : ucgrid(), tr(0) { }
  image(int x) : ucgrid(x), tr(0) { }
  image(int x, int y) : ucgrid(x, y), tr(0) { }
  image(const image &i) : ucgrid(i), tr(i.tr) { }
  image& operator=(const image &i) { 
    ucgrid::operator=(i); tr = i.tr; return *this;
  }
  ~image() { }

  int truth() const { return tr; }
  void init_window();
  void draw();
  void redraw();
  void proc_confignotify(XEvent &event);
  int proc_buttonrelease(XEvent &event);
  char proc_keyrelease(XEvent &event);


}; // class image

//___________________________________________________________________________
// image.h

#endif
