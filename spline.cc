#ifndef SPLINE_C
#define SPLINE_C

//___________________________________________________________________________

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#define String XString
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#undef String
#include <math.h>

#include "includes.h"
#include "spline.h"
#include "globals.h"

//___________________________________________________________________________

void spline::init_window()
{
  height = width = viewsize * pixsize + 2 * imborder;
  border = 4;
  XSetWindowAttributes xswa;
  xswa.event_mask = ExposureMask | StructureNotifyMask |
    ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | KeyReleaseMask;
  xswa.border_pixel = BlackPixel(display, screen);
  xswa.background_pixel = grays[NUMCOLORS * 0 / 6].pixel;
  xswa.colormap = DefaultColormap(display, screen);
  window = XCreateWindow(display, RootWindow(display, screen),
			 xcorner, ycorner, width, height, border, 
			 DefaultDepth(display, screen), 
			 InputOutput, DefaultVisual(display, screen), 
			 CWBackPixel | CWBorderPixel | 
			 CWColormap | CWEventMask, &xswa);

} // spline::init_window

//___________________________________________________________________________
// Draw an spline in a window at the point ('sx', 'sy').  Pay close
// attention to X events to avoid unwanted redraws.

void spline::draw()
{
  const double p1 = 1.0 / 3.0;
  const double p2 = 2.0 / 3.0;
  const double p3 = 1.0;
  const auto center = viewsize / 2;

  XSetForeground(display, gc, redcol.pixel);
  for (double t = 0; t < 0.999999; t += 0.001)
  {
    double tt = t + 0.001;
    double fx1 = ctrl[0] * (p1 - t) * (p2 - t) * (p3 - t) * 9.0 / 2.0 +
      ctrl[2] * t * (p2 - t) * (p3 - t) * 27.0 / 2.0 +
      ctrl[4] * t * (t - p1) * (p3 - t) * 27.0 / 2.0 +
      ctrl[6] * t * (t - p1) * (t - p2) * 9.0 / 2.0;
    double fy1 = ctrl[1] * (p1 - t) * (p2 - t) * (p3 - t) * 9.0 / 2.0 +
      ctrl[3] * t * (p2 - t) * (p3 - t) * 27.0 / 2.0 +
      ctrl[5] * t * (t - p1) * (p3 - t) * 27.0 / 2.0 +
      ctrl[7] * t * (t - p1) * (t - p2) * 9.0 / 2.0;
    double fx2 = ctrl[0] * (p1 - tt) * (p2 - tt) * (p3 - tt) * 9.0 / 2.0 +
      ctrl[2] * tt * (p2 - tt) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[4] * tt * (tt - p1) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[6] * tt * (tt - p1) * (tt - p2) * 9.0 / 2.0;
    double fy2 = ctrl[1] * (p1 - tt) * (p2 - tt) * (p3 - tt) * 9.0 / 2.0 +
      ctrl[3] * tt * (p2 - tt) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[5] * tt * (tt - p1) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[7] * tt * (tt - p1) * (tt - p2) * 9.0 / 2.0;
    draw_line(fx1 + center, fy1 + center, fx2 + center, fy2 + center, 0);
  }
  XSetForeground(display, gc, bluecol.pixel);
  draw_point(pixsize * (ctrl[0] + center), pixsize * (ctrl[1] + center));
  draw_point(pixsize * (ctrl[2] + center), pixsize * (ctrl[3] + center));
  draw_point(pixsize * (ctrl[4] + center), pixsize * (ctrl[5] + center));
  draw_point(pixsize * (ctrl[6] + center), pixsize * (ctrl[7] + center));


  if (showpixels)
  {
    XSetForeground(display, gc, magentacol.pixel);
    for (int i = 0; i < viewsize; ++i)
      for (int j = 0; j < viewsize; ++j)
	XDrawRectangle(display, window, gc, i * pixsize + imborder,
		       j * pixsize + imborder, pixsize, pixsize);
  }
  
} // spline::draw

//___________________________________________________________________________
// Erase a spline in a window at the point ('sx', 'sy').

void spline::erase_spline(int sx, int sy)
{
  const double p1 = 1.0 / 3.0;
  const double p2 = 2.0 / 3.0;
  const double p3 = 1.0;

  XSetForeground(display, gc, WhitePixel(display, screen));
  for (double t = 0; t < 0.999999; t += 0.001)
  {
    double tt = t + 0.001;
    double fx1 = ctrl[0] * (p1 - t) * (p2 - t) * (p3 - t) * 9.0 / 2.0 +
      ctrl[2] * t * (p2 - t) * (p3 - t) * 27.0 / 2.0 +
      ctrl[4] * t * (t - p1) * (p3 - t) * 27.0 / 2.0 +
      ctrl[6] * t * (t - p1) * (t - p2) * 9.0 / 2.0;
    double fy1 = ctrl[1] * (p1 - t) * (p2 - t) * (p3 - t) * 9.0 / 2.0 +
      ctrl[3] * t * (p2 - t) * (p3 - t) * 27.0 / 2.0 +
      ctrl[5] * t * (t - p1) * (p3 - t) * 27.0 / 2.0 +
      ctrl[7] * t * (t - p1) * (t - p2) * 9.0 / 2.0;
    double fx2 = ctrl[0] * (p1 - tt) * (p2 - tt) * (p3 - tt) * 9.0 / 2.0 +
      ctrl[2] * tt * (p2 - tt) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[4] * tt * (tt - p1) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[6] * tt * (tt - p1) * (tt - p2) * 9.0 / 2.0;
    double fy2 = ctrl[1] * (p1 - tt) * (p2 - tt) * (p3 - tt) * 9.0 / 2.0 +
      ctrl[3] * tt * (p2 - tt) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[5] * tt * (tt - p1) * (p3 - tt) * 27.0 / 2.0 +
      ctrl[7] * tt * (tt - p1) * (tt - p2) * 9.0 / 2.0;
    draw_line(fx1 + sx, fy1 + sy, fx2 + sx, fy2 + sy, 0);
  }
  draw_point(pixsize * (ctrl[0] + sx), pixsize * (ctrl[1] + sy));
  draw_point(pixsize * (ctrl[2] + sx), pixsize * (ctrl[3] + sy));
  draw_point(pixsize * (ctrl[4] + sx), pixsize * (ctrl[5] + sy));
  draw_point(pixsize * (ctrl[6] + sx), pixsize * (ctrl[7] + sy));
  
} // spline::erase_spline

//___________________________________________________________________________
// Process a ConfigureNotify event.

void spline::proc_confignotify(XEvent &event)
{
  XWindowAttributes xwa;
  if (XGetWindowAttributes(display, window, &xwa) != 0)
  {
    pixsize = max(1, min(((xwa.width - 2 * imborder) / viewsize),
                         ((xwa.height - 2 * imborder) / viewsize)));
    xcorner = event.xconfigure.x - xwa.x + border;
    ycorner = event.xconfigure.y - xwa.y + border;
  }
  
} // spline::proc_configurenotify

//___________________________________________________________________________
// Process a ButtonPress event.

int spline::proc_buttonpress(XEvent &event)
{
  cbut = event.xbutton.button;
  if (cbut == 1)
  {
    double px = 1.0 * (event.xbutton.x - imborder) / pixsize - 20;
    double py = 1.0 * (event.xbutton.y - imborder) / pixsize - 20;
    double mindist = idistance(px, py, ctrl[0], ctrl[1]);
    currctrl = 0;
    double d = idistance(px, py, ctrl[2], ctrl[3]);
    if (d < mindist)
    {
      currctrl = 1;
      mindist = d;
    }
    d = idistance(px, py, ctrl[4], ctrl[5]);
    if (d < mindist)
    {
      currctrl = 2;
      mindist = d;
    }
    d = idistance(px, py, ctrl[6], ctrl[7]);
    if (d < mindist)
    {
      currctrl = 3;
      mindist = d;
    }
  }

  return cbut;

} // spline::proc_buttonpress

//___________________________________________________________________________
// Process a ButtonMotion event.

int spline::proc_buttonmotion(XEvent &event)
{
  XEvent newevent;
  if (XCheckTypedWindowEvent(display, window, MotionNotify, &newevent))
  {
    XPutBackEvent(display, &newevent);
    return 1;
  }

  if (currctrl != -1)
  {
    erase_spline(viewsize / 2, viewsize / 2);
    double px = 1.0 * (event.xmotion.x - imborder) / pixsize - 20;
    double py = 1.0 * (event.xmotion.y - imborder) / pixsize - 20;
    ctrl[currctrl * 2] = px;
    ctrl[currctrl * 2 + 1] = py;
    draw();
  }

  return 1;

} // spline::proc_buttonmotion

//___________________________________________________________________________
// Process a ButtonRelease event.

int spline::proc_buttonrelease(XEvent &event)
{
  double px, py;
  switch (event.xbutton.button)
  {
   case Button1 :
    erase_spline(viewsize / 2, viewsize / 2);
    px = 1.0 * (event.xbutton.x - imborder) / pixsize - 20;
    py = 1.0 * (event.xbutton.y - imborder) / pixsize - 20;
    if (currctrl != -1)
    {
      ctrl[currctrl * 2] = px;
      ctrl[currctrl * 2 + 1] = py;
      draw();
    }
    currctrl = -1;
    break;
    
   case Button2 :
    height = width = viewsize * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
    break;
    
   case Button3:
    height = width = viewsize * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
    break;

   default:
    break;
  }

  cbut = -1;

  return int(event.xbutton.button);

} // spline::proc_buttonrelease

//___________________________________________________________________________
// Process a KeyRelease event.

char spline::proc_keyrelease(XEvent &event)
{
  char thechar = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);

  if (thechar == 'r')
  {
    revvideo = 1 - revvideo;
  }
  else if (thechar == 's')
  {
    showpixels = 1 - showpixels;
    if (!showpixels)
    {
      XSetForeground(display, gc, grays[0].pixel);
      for (int i = 0; i < viewsize; ++i)
	for (int j = 0; j < viewsize; ++j)
	  XDrawRectangle(display, window, gc, i * pixsize + imborder,
			 j * pixsize + imborder, pixsize, pixsize);
    }
    draw();
  }
  else if (thechar >= '1' && thechar <= '9')
  {
    pixsize = thechar - '0';
    height = width = viewsize * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
  }
  else if (pixsize > 1 && (thechar == '<' || thechar == ','))
  {
    --pixsize;
    height = width = viewsize * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
  }
  else if (thechar == '>' || thechar == '.')
  {
    ++pixsize;
    height = width = viewsize * pixsize + 2 * imborder;
    XResizeWindow(display, window, width, height);
  }
  
  return thechar;

} // spline::proc_keyrelease

//___________________________________________________________________________
// spline.C

#endif // SPLINE_C
