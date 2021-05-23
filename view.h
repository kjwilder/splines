#ifndef VIEW_H
#define VIEW_H

#define NUMCOLORS 125

#define String XString
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef String

#include "includes.h"

//___________________________________________________________________________

class viewobj
{
 private:
  static int objcount;

 protected:

  // Window variables;
  static Display *display;
  static int screen;

  Window window;
  GC gc;
  int border;
  int height;
  int width;

  // Color data
  static Colormap cmap;
  static XColor grays[NUMCOLORS];
  static XColor redcol, bluecol, greencol;
  static XColor cyancol, magentacol, yellowcol;

  // Window Manager data
  static XClassHint xch;
  static XSizeHints xsh;
  static XWMHints xwmh;

  // Font data
  static XFontStruct *xfs;
  static int fontheight;
  static int fontwidth;

  // Window data
  int xcorner;
  int ycorner;
  int cbut;

  // Variables used to keep track of exposed regions.
  int xlowx, xlowy, xhighx, xhighy;

 public:
  viewobj() : border(0), height(0), width(0), xcorner(0), ycorner(0), cbut(-1),
  xlowx(1000000), xlowy(1000000), xhighx(0), xhighy(0) { ++objcount; }
  virtual ~viewobj() { assert(objcount >= 0); if (--objcount == 0) quitX(); }
  int initX();
  int quitX();
  virtual int view(const char *title);
  virtual void init_window() { }
  virtual void init_gc();
  virtual void draw() { }
  virtual void draw_point(float x, float y);
  virtual void draw_line(float x1, float y1, float x2, float y2, int middle);
  virtual void draw_string(const char *text, float i, float j, int middle = 0);
  virtual void proc_expose(XEvent& event, const char * title);
  virtual void proc_confignotify(XEvent&) { }
  virtual int proc_buttonpress(XEvent&) { return 0; }
  virtual int proc_buttonrelease(XEvent&) { return 0; }
  virtual int proc_buttonmotion(XEvent&) { return 0; }
  virtual char proc_keyrelease(XEvent&) { return 0; }
};

//___________________________________________________________________________
// view.h
#endif
