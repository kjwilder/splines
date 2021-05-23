#ifndef IMAGE_C
#define IMAGE_C

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#define String XString
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#undef String

#include "includes.h"
#include "image.h"
#include "globals.h"

using namespace std;

//___________________________________________________________________________

void image::init_window()
{
  height = dimy() * pixsize + 2 * imborder;
  width = dimx() * pixsize + 2 * imborder;
  border = 4;
  XSetWindowAttributes xswa;
  xswa.event_mask = ExposureMask | StructureNotifyMask |
    ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | KeyReleaseMask;
  xswa.border_pixel = BlackPixel(display, screen);
  xswa.background_pixel = grays[NUMCOLORS * 1 / 6].pixel;
  xswa.colormap = DefaultColormap(display, screen);
  window = XCreateWindow(display, RootWindow(display, screen),
			 xcorner, ycorner, width, height, border, 
			 DefaultDepth(display, screen), 
			 InputOutput, DefaultVisual(display, screen), 
			 CWBackPixel | CWBorderPixel | 
			 CWColormap | CWEventMask, &xswa);

} // image::init_window

//___________________________________________________________________________
// Draw an image in a window.  Pay close attention to X events to avoid 
// unwanted redraws.

void image::draw()
{
  for (int j = 0; j < dimy() * pixsize; j += pixsize)
  {
    // Skip unexposed regions.
    if (j + imborder + pixsize < xlowy || j + imborder > xhighy)
      continue;

    // Stop drawing if an expose or button release event occurs.
    XEvent newevent;
    if (XCheckTypedWindowEvent(display, window, Expose, &newevent) || 
        (XCheckTypedWindowEvent(display, window, ButtonRelease, &newevent) &&
         (newevent.xbutton.button != Button1 || pixsize > 1)))
    {
      XPutBackEvent(display, &newevent);
      return;
    }

    for (int i = 0; i < dimx() * pixsize; i += pixsize)
    {
      // Skip unexposed regions.
      if (i + imborder + pixsize < xlowx || i + imborder > xhighx)
        continue;

      int numval = (*this)(i / pixsize, j / pixsize);
      XSetForeground(display, gc, revvideo ? 
		     grays[NUMCOLORS - numval - 1].pixel :
		     grays[numval].pixel);
      XFillRectangle(display, window, gc, i + imborder,
                     j + imborder, pixsize, pixsize);
    }
    if (showpixels)
    {
      XSetForeground(display, gc, magentacol.pixel);
      for (int i = 0; i < dimx() * pixsize; i += pixsize)
      {
        // Skip unexposed regions.
        if (i + imborder + pixsize < xlowx || i + imborder > xhighx)
          continue;

        if ((*this)(i / pixsize, j / pixsize) != 0)
          XDrawRectangle(display, window, gc, i + imborder,
                         j + imborder, pixsize, pixsize);
      }
    }
  }

} // image::draw

//___________________________________________________________________________
// Draw an image in a window.  Pay close attention to X events to avoid 
// unwanted redraws.

void image::redraw()
{
  xlowx = xlowy = 0;
  xhighx = xhighy = 1000000;
  draw();
}

//___________________________________________________________________________
// Process a ConfigureNotify event.

void image::proc_confignotify(XEvent &event)
{
  XWindowAttributes xwa;
  if (XGetWindowAttributes(display, window, &xwa) != 0)
  {
    pixsize = max(1, min((xwa.width - 2 * imborder) / dimx(),
                         (xwa.height - 2 * imborder) / dimy()));
    xcorner = event.xconfigure.x - xwa.x + border;
    ycorner = event.xconfigure.y - xwa.y + border;
  }
  
} // image::proc_configurenotify

//___________________________________________________________________________
// Process a ButtonRelease event.

int image::proc_buttonrelease(XEvent &event)
{
  switch (event.xbutton.button)
  {
   case Button1 :
   case Button2 :
    // Increase or decrease the size of the window.
    if (event.xbutton.button == Button1)
      pixsize -= (1 * (pixsize > 1));
    else
      pixsize += 1;
    height = dimy() * pixsize + 2 * imborder;
    width = dimx() * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
    break;
    
   default:
    break;
  }

  return int(event.xbutton.button);

} // image::proc_buttonrelease

//___________________________________________________________________________
// Process a KeyRelease event.

char image::proc_keyrelease(XEvent &event)
{
  char thechar = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
  if (thechar == 'r')
  {
    revvideo = 1 - revvideo;
    redraw();
  }
  else if (thechar == 's')
  {
    showpixels = 1 - showpixels;
    redraw();
  }
  else if (thechar >= '1' && thechar <= '9')
  {
    pixsize = thechar - '0';
    height = dimy() * pixsize + 2 * imborder;
    width = dimx() * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
  }
  else if (pixsize > 1 && (thechar == '<' || thechar == ','))
  {
    --pixsize;
    height = dimy() * pixsize + 2 * imborder;
    width = dimx() * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
  }
  else if (thechar == '>' || thechar == '.')
  {
    ++pixsize;
    height = dimy() * pixsize + 2 * imborder;
    width = dimx() * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
  }
  
  return thechar;

} // image::proc_keyrelease

//___________________________________________________________________________
// image.C

#endif // IMAGE_C
