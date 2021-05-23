#ifndef VIEW_C
#define VIEW_C

//___________________________________________________________________________

extern char *ltos(const long x);

//___________________________________________________________________________

#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#define String XString
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#undef String

#include "includes.h"
#include "view.h"
#include "image.h"
#include "globals.h"

using namespace std;

//___________________________________________________________________________
// Define static viewobj members

int viewobj::objcount = 0;

Display *viewobj::display;
int viewobj::screen;

Colormap viewobj::cmap;
XColor viewobj::grays[NUMCOLORS];
XColor viewobj::redcol, viewobj::bluecol, viewobj::greencol;
XColor viewobj::cyancol, viewobj::magentacol, viewobj::yellowcol;

XClassHint viewobj::xch;
XSizeHints viewobj::xsh;
XWMHints viewobj::xwmh;

XFontStruct *viewobj::xfs;
int viewobj::fontheight;
int viewobj::fontwidth;

//___________________________________________________________________________
// Establish a connection to a display and set up defaults.

int viewobj::initX()
{
  // Return now if this routine was already called.
  static int called = 0;
  if (called > 0)
    return 1;

  // Open the default display.
  if ((display = XOpenDisplay(0)) == 0)
    return 0;
  
  // Set up the default screen.
  screen = DefaultScreen(display);
  
  // Allocate a font.
  if ((xfs = XLoadQueryFont(display, "9x15")) == 0)
    return 0;
  fontheight = xfs->max_bounds.ascent + xfs->max_bounds.descent;
  fontwidth = fontheight;

  // Set up the default colormap.
  cmap = DefaultColormap(display, screen);
    
  // Allocate some colorful colors.
  redcol.red = 50000; redcol.green = redcol.blue = 0;
  if (XAllocColor(display, cmap, &redcol) == 0)
    return 0;
  greencol.green = 50000; greencol.red = greencol.blue = 0;
  if (XAllocColor(display, cmap, &greencol) == 0)
    return 0;
  bluecol.blue = 50000; bluecol.red = bluecol.green = 0;
  if (XAllocColor(display, cmap, &bluecol) == 0)
    return 0;
  cyancol.red = 0; cyancol.green = cyancol.blue = 50000;
  if (XAllocColor(display, cmap, &cyancol) == 0)
    return 0;
  magentacol.green = 0; magentacol.red = magentacol.blue = 50000;
  if (XAllocColor(display, cmap, &magentacol) == 0)
    return 0;
  yellowcol.blue = 0; yellowcol.red = yellowcol.green = 50000;
  if (XAllocColor(display, cmap, &yellowcol) == 0)
    return 0;
  
  // Allocate a bunch of gray scale colors or some colorful colors.
  if (colors)
  {
    // For now assuming NUMCOLORS = 125
    for (int i = 0; i < 5; ++i)
      for (int j = 0; j < 5; ++j)
	for (int k = 0; k < 5; ++k)
	{
	  grays[i * 25 + j * 5 + k].red = 65535 - 65535 * i / 4;
	  grays[i * 25 + j * 5 + k].green = 65535 - 65535 * j / 4;
	  grays[i * 25 + j * 5 + k].blue = 65535 - 65535 * k / 4;
	  if (XAllocColor(display, cmap, &grays[i * 25 + j * 5 + k]) == 0)
	    return 0;
	}
  }
  else
  {
    for (int i = 0; i < NUMCOLORS; ++i)
    {
      grays[i].red = grays[i].green = grays[i].blue = 
	65535 - (65535 / (NUMCOLORS) * i);
      if (XAllocColor(display, cmap, &grays[i]) == 0)
	return 0;
    }
  }
  
  ++called;
  return 1;
  
} // initX

//___________________________________________________________________________
// Close a connection to a display.

int viewobj::quitX()
{
//  XCloseDisplay(display);
  
  return 1;
  
} // quitX

//___________________________________________________________________________
// Display a digit whose information is stored in 'viewgrid'.

int viewobj::view(const char* title)
{

  // Initialize the display.  Successive calls are ignored.
  if (!initX())
  {
    cerr << "\nCouldn't open display [" << XDisplayName(0)
      << "] to view images.\n\n";
    exit(1);
  }

  // Initialize a window.
  init_window();

  // Initialize the graphics context.
  init_gc();
  
  // Make sure we don't crash if quit is chosen from the olwm.
  Atom wmdw = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wmdw, 1);
  
  // Initialize the window manager properties.
  char *name = strdup("digits");
  XTextProperty window_name;
  XStringListToTextProperty(&name, 1, &window_name);
  xch.res_class = strdup("Drop");
  xch.res_name = strdup("drop");
  xsh.flags = PPosition | PSize;
  xwmh.flags = InputHint | StateHint;
  xwmh.input = True;
  xwmh.initial_state = NormalState;
  XSetWMProperties(display, window, &window_name, 0, 0, 0, 
                   &xsh, &xwmh, &xch);
  XFree(window_name.value);
  
  // Display the window.
  XMapRaised(display, window);
  
  // Enter event loop.
  XEvent event;
  while (True)
  {
    XNextEvent(display, &event);
    switch (event.type)
    {
     case ClientMessage :
      // Handle case WM kills the window.
      if (event.xclient.data.l[0] == int(wmdw))
      {
        XDestroyWindow(display, window);
        XFlush(display);
        return 1;
      }
      break;
      
     case Expose:
      proc_expose(event, title);
      break;
      
     case ConfigureNotify:
      proc_confignotify(event);
      break;
      
     case ButtonPress:
      proc_buttonpress(event);
      break;

     case ButtonRelease:
      proc_buttonrelease(event);
      break;

     case MotionNotify:
      proc_buttonmotion(event);
      break;

     case KeyRelease:
      if (proc_keyrelease(event) == 'q')
      {
	XDestroyWindow(display, window);
	XFlush(display);
	return 1;
      }
      break;
      
     default:
      break;
    }
  }
  
} // viewobj::view

//___________________________________________________________________________
// Process an Expose event.

void viewobj::proc_expose(XEvent &event, const char * title)
{
  // Update the exposed region bounds.
  xlowx = min(xlowx, event.xexpose.x);
  xlowy = min(xlowy, event.xexpose.y);
  xhighx = max(xhighx, event.xexpose.x + event.xexpose.width);
  xhighy = max(xhighy, event.xexpose.y + event.xexpose.height);

  // Leave now if there are more recent expose events.
  if (event.xexpose.count != 0)
    return;
  
  // Write the title of the dataset if we have enough space.
  if (imborder > int(fontheight))
  {
    XSetForeground(display, gc, magentacol.pixel);
    if (title == 0)
    {
      char tmp[STRLEN];
      strcpy(tmp, dataset.c_str());
      draw_string(basename(tmp), 0, 0);
    }
    else
      draw_string(title, 0, 0);
  }
  
  // Draw the object.
  draw();

  // Reset the exposed region bounds if no pending expose events.
  XEvent newevent;
  if (XCheckTypedWindowEvent(display, window, Expose, &newevent))
  {
    XPutBackEvent(display, &newevent);
    return;
  }
  else
  {
    xlowx = xlowy = 1000000;
    xhighx = xhighy = 0;
  }
  
} // viewobj::proc_expose

//___________________________________________________________________________
// Create a gc and initialize it's main properties.

void viewobj::init_gc()
{
  gc = XCreateGC(display, window, 0, 0);
  XSetFont(display, gc, xfs->fid);
  
} // viewobj::init_gc

//___________________________________________________________________________
// Draw a point at (x, y).

void viewobj::draw_point(float x, float y)
{
  int pointsize = min(5, pixsize);
  XFillRectangle(display, window, gc, 
		 int(imborder + x) - pointsize / 2, 
		 int(imborder + y) - pointsize / 2,
		 pointsize, pointsize);

} // viewobj::draw_point

//___________________________________________________________________________
// Draw a line from location (x1, y1) to location (x2, y2).  Lines begin
// and end in the middle of the location if middle != 0.

void viewobj::draw_line(float x1, float y1, float x2, float y2, int middle)
{
  if (middle == 0)
  {
    XDrawLine(display, window, gc, 
              int(imborder + x1 * pixsize), int(imborder + y1 * pixsize), 
              int(imborder + x2 * pixsize), int(imborder + y2 * pixsize));
  }
  else
  {
    XDrawLine(display, window, gc, 
	      int(imborder + (x1 + 0.5) * pixsize), 
              int(imborder + (y1 + 0.5) * pixsize),
	      int(imborder + (x2 + 0.5) * pixsize), 
              int(imborder + (y2 + 0.5) * pixsize));
  }

} // viewobj::draw_line

//___________________________________________________________________________
// Draw a string beginning on top of pixel (i, j).  If middle != 0, 
// draw it in the middle of the pixel.

void viewobj::draw_string(const char *text, float i, float j, int middle)
{
  if (middle == 0)
    XDrawString(display, window, gc, int(imborder + i * pixsize), 
                int(imborder + j * pixsize), text, strlen(text));
  else
    XDrawString(display, window, gc, 
                int(imborder + (i + 0.5) * pixsize - fontwidth / 2.0),
                int(imborder + (j + 0.5) * pixsize + fontheight / 2.0),
                text, strlen(text));


} // viewobj::draw_string

//___________________________________________________________________________
// Return the basename of a file.
 
const char *basename(const char *name)
{
  const char *tmpname;
  tmpname = strrchr(name, '/');
  return (tmpname == 0 ? name : tmpname + 1);

} // basename
 
//___________________________________________________________________________
// view.C

#endif // VIEW_C
