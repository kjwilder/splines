#ifndef SPLINE_H
#define SPLINE_H

#include "grid.h"
#include "landmark.h"
#include "view.h"

//___________________________________________________________________________
// data structure used to store and manipulate splines

class spline : public viewobj
{
  enum {viewsize = 40};

 private:
  double ctrl[8];
  int currctrl;

 public:
  spline() : currctrl(-1) { memset(ctrl, 0, 8 * sizeof(double)); }
  spline(const spline &s) : currctrl(-1) { 
    memcpy(ctrl, s.ctrl, 8 * sizeof(double)); 
  }
  spline(double x1, double y1, double x2, double y2,
	 double x3, double y3, double x4, double y4) : currctrl(-1) {
    ctrl[0] = x1; ctrl[1] = y1; ctrl[2] = x2; ctrl[3] = y2;
    ctrl[4] = x3; ctrl[5] = y3; ctrl[6] = x4; ctrl[7] = y4;
  }
  spline& operator=(const spline &s) { 
    memcpy(ctrl, s.ctrl, 8 * sizeof(double)); return *this;
  }
  ~spline() { }

  void init_window();
  void draw();
  void erase_spline(int sx, int sy);
  void proc_confignotify(XEvent &event);
  int proc_buttonpress(XEvent &event);
  int proc_buttonrelease(XEvent &event);
  int proc_buttonmotion(XEvent &event);
  char proc_keyrelease(XEvent &event);

}; // class spline

//___________________________________________________________________________
// spline.h

#endif
