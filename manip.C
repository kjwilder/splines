#define MANIP_C

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

#include "includes.h"
#include "image.h"
#include "globals.h"

#define GP(i, x, y) (tnx[i]*(y)+(x))
int ia[8][2] =
{ {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1} };
int tnx[2], tny[2];
extern int cross_num(uchar *b, int type, int x, int y);
extern int nbhd_sum(uchar *b, int x, int y);
extern uchar offpix(uchar *b, int i, int x, int y);

//___________________________________________________________________________
// Thin an image

image& image::thin(int masksize)
{
  // Handle the degenerate case.
  if (masksize == 0 || dimx() == 0 || dimy() == 0)
    return *this;

  image newimage = *this;

  tnx[1] = tnx[0] = newimage.dimx();
  tny[1] = tny[0] = newimage.dimy();
 
  uchar id[tnx[0] * tny[0]];
  uchar f_id[tnx[0] * tny[0]];
 
  for (int x = 0; x < tnx[0]; ++x)
    for (int y = 0; y < tny[0]; ++y)
      id[tnx[0] * y + x] = newimage(x, y);

  
  int k = 1;
  while (k)
  {
    for (int i = 0; i < tnx[0] * tny[0]; i++)
      f_id[i] = 0;
    for (int y = 0; y < tny[0]; y++)
    {
      for (int x = 0; x < tnx[0]; x++)
      {
	if (id[GP(0, x, y)] && (nbhd_sum(id, x, y) != 1))
	{
	  if (cross_num(id, 0, x, y) == 1)
	  {
	    f_id[GP(0, x, y)] = 1;
	    if (offpix(f_id, 2, x, y) && (cross_num(id, 2, x, y) != 1))
	      f_id[GP(0, x, y)] = 0;
	    if (offpix(f_id, 4, x, y) && (cross_num(id, 4, x, y) != 1))
	      f_id[GP(0, x, y)] = 0;
	  }
	}
      }
    }
    
    k = 0;
    for (int i = 0; i < tnx[0] * tny[0]; i++)
    {
      if (f_id[i])
	id[i] = 0;
      k += f_id[i];
    }
  }
 
  for (int x = 0; x < tnx[0]; ++x)
    for (int y = 0; y < tny[0]; ++y)
      newimage(x, y) = id[y * tnx[0] + x];

  image realnewimage = newimage;
  if (masksize > 1)
  {
    for (int x = 0; x < newimage.dimx(); ++x)
      for (int y = 0; y < newimage.dimy(); ++y)
	if ((*this)(x, y) > 0)
	{
	  for (int i = -masksize / 2; i <= masksize / 2; ++i)
	    for (int j = -masksize / 2; j <= masksize / 2; ++j)
              if (i * i + j * j < masksize * masksize / 4 &&
	          newimage.get(x + i, y + j) > 0)
	        realnewimage(x, y) = 1;
	}
  }

  *this << realnewimage;

  return *this;
  
} // image::thin

//___________________________________________________________________________
// crossing number.
 
int cross_num(uchar *b, int type, int x, int y)
{
  int k = 0;
  switch (type)
  {
   case 0:
    if ((!offpix(b, 0, x, y)) && (offpix(b, 1, x, y) || offpix(b, 2, x, y)))
      k++;
    if ((!offpix(b, 2, x, y)) && (offpix(b, 3, x, y) || offpix(b, 4, x, y)))
      k++;
    if ((!offpix(b, 4, x, y)) && (offpix(b, 5, x, y) || offpix(b, 6, x, y)))
      k++;
    if ((!offpix(b, 6, x, y)) && (offpix(b, 7, x, y) || offpix(b, 0, x, y)))
      k++;
    break;
   case 2:
    if ((!offpix(b, 0, x, y)) && offpix(b, 1, x, y))
      k++;
    if (offpix(b, 3, x, y) || offpix(b, 4, x, y))
      k++;
    if ((!offpix(b, 4, x, y)) && (offpix(b, 5, x, y) || offpix(b, 6, x, y)))
      k++;
    if ((!offpix(b, 6, x, y)) && (offpix(b, 7, x, y) || offpix(b, 0, x, y)))
      k++;
    break;
   case 4:
    if ((!offpix(b, 0, x, y)) && (offpix(b, 1, x, y) || offpix(b, 2, x, y)))
      k++;
    if ((!offpix(b, 2, x, y)) && offpix(b, 3, x, y))
      k++;
    if (offpix(b, 5, x, y) || offpix(b, 6, x, y))
      k++;
    if ((!offpix(b, 6, x, y)) && (offpix(b, 7, x, y) || offpix(b, 0, x, y)))
      k++;
    break;
  }
  return k;
 
} // cross_num
 
//___________________________________________________________________________
// sum of neighborhood values.
 
int nbhd_sum(uchar *b, int x, int y)
{
  int k = 0;
  for (int i = 0; i < 8; i++)
    k += offpix(b, i, x, y);
 
  return k;
 
} // nbhd_sum
 
//___________________________________________________________________________
// get offset pixel value based on index array.
 
uchar offpix(uchar *b, int i, int x, int y)
{
 
  x += ia[i][0];
  y += ia[i][1];
 
  if ((x >= 0) && (x < tnx[0]) && (y >= 0) && (y < tny[0]))
    return b[GP(0, x, y)];
  else
    return 0;
 
} // offpix
 
//___________________________________________________________________________
// Perform a median filter operation on an image 'times' times

image& image::filter(int times)
{
  for (int count = 0; count < times; ++count)
  {
    image newimage(dimx(), dimy());
    for (int i = 0; i < dimx(); ++i)
      for (int j = 0; j < dimy(); ++j)
      {
        int numstroke = 0;
        for (int k = -1; k <= 1; ++k)
          for (int l = -1; l <= 1; ++l)
            numstroke += (get(i + k, j + l) != 0);
        newimage(i, j) = (numstroke >= 5);
      }
    *this << newimage;
  }

  return *this;

} // image::filter

//___________________________________________________________________________
// Rotate an image by an angle 'rtt'

image& image::rotate(float rtt)
{
  if (rtt == 0.0)
    return (*this);

  float cos_rotate = cos(rtt);
  float sin_rotate = sin(rtt);

  // Initialize the dimension of the newgrid sufficiently large.
  int rot_dim = 2 * (dimx() >? dimy());
  image newimage(rot_dim, rot_dim);
  float oldcenx = dimx() / 2;
  float oldceny = dimy() / 2;
  float newcenx = rot_dim / 2;
  float newceny = rot_dim / 2;
  for (int i = 0; i < rot_dim; ++i)
    for (int j = 0; j < rot_dim; ++j)
    {
      // Weird transform as images and sin/cos use different
      // coordinate systems.
      float dx = i - newcenx;
      float dy = newceny - j;
      int transx = int(oldcenx + cos_rotate * dx - sin_rotate * dy);
      int transy = int(oldceny - sin_rotate * dx - cos_rotate * dy);
      newimage(i, j) = get(transx, transy);
    }

  (*this) << newimage.trim();

  return (*this);

} // image::rotate

//___________________________________________________________________________
// If 'amount' >= 0, put a border of 'amount' pixels on all edges of 'oldgrid'.
// If 'amount' < 0, center 'oldgrid' in an amount by amount grid.

image& image::trim(int amount)
{
  int leftmar = 0, rightmar = 0, topmar = 0, bottommar = 0;

  int found = 0;
  for (int i = 0; i < dimx() && !found; ++i)
    for (int j = 0; j < dimy() && !found; ++j)
    {
      found = ((*this)(i, j) > 0);
      if (found)
        leftmar = i;
    }

  // Return now if the image is blank
  if (!found)
    return *this;

  found = 0;
  for (int i = dimx() - 1; !found; --i)
    for (int j = 0; j < dimy() && !found; ++j)
    {
      found = ((*this)(i, j) > 0);
      if (found)
        rightmar = i;
    }

  found = 0;
  for (int j = 0; !found; ++j)
    for (int i = 0; i < dimx() && !found; ++i)
    {
      found = ((*this)(i, j) > 0);
      if (found)
        topmar = j;
    }

  found = 0;
  for (int j = dimy() - 1; !found; --j)
    for (int i = 0; i < dimx() && !found; ++i)
    {
      found = ((*this)(i, j) > 0);
      if (found)
        bottommar = j;
    }

  image newimage;
  int xlength = rightmar - leftmar + 1;
  int ylength = bottommar - topmar + 1;
  if (amount >= 0)
  {
    newimage.init(xlength + 2 * amount, ylength + 2 * amount);
    newimage.clear();
    for (int i = amount; i < newimage.dimx() - amount; ++i)
      for (int j = amount; j < newimage.dimy() - amount; ++j)
	newimage(i, j) = (*this)(i + leftmar - amount, j + topmar - amount);
  }
  else
  {
    amount *= -1;
    newimage.init(amount, amount);
    newimage.clear();
    int startx = ((amount > xlength) ? ((amount - xlength) / 2) : 0);
    int starty = ((amount > ylength) ? ((amount - ylength) / 2) : 0);
    for (int i = 0; i < amount && i < xlength; ++i)
      for (int j = 0; j < amount && j < ylength; ++j)
	newimage(i + startx, j + starty) = (*this)(i + leftmar, j + topmar);
  }

  (*this) << newimage;

  return (*this);

} // image::trim

//___________________________________________________________________________

image& image::make_spots(int num_spots, int spot_size)
{
  static int mask_made = 0;
  static igrid mask;

  // Make the mask if necessary.
  if (!mask_made)
  {
    ++mask_made;
    mask.init(spot_size, spot_size);
    for (int i = 0; i < spot_size; ++i)
      for (int j = 0; j < spot_size; ++j)
      {
	  mask(i, j) = (int) (1.0 / (pow(2.0, fabs(i - (spot_size - 1) / 2)) +
				     pow(2.0, fabs(j - (spot_size - 1) / 2))) * 
			      10000.0);
      }
  }

  image newimage = *this;
  for (int i = 0; i < num_spots; ++i)
  {
    if (!randomize || lrand48() % 2 )  // 50% chance of a spot if randomizing.
    {
      int x = lrand48() % (dimx() - spot_size);
      int y = lrand48() % (dimy() - spot_size);
      for (int j = 0; j < spot_size; ++j)
	for (int k = 0; k < spot_size; ++k)
	  newimage(x + j, y + k) = abs(newimage(x + j, y + k) - 
				       (lrand48() % 10000 < mask(j, k)));
    }
  }

  (*this) << newimage.trim();

  return (*this);

} // image::make_spots

//___________________________________________________________________________
// upscale a grid to 'size' by 'size'

image& image::upscale(int size)
{
  if (size <= 0 || dimx() == 0 || dimy() == 0)
    return *this;

  image newimage(size, size);
  newimage.clear();

  double scale = double(size) / (dimx() >? dimy());
  int startx = int(size - scale * dimx() + FUZZ) / 2;
  int starty = int(size - scale * dimy() + FUZZ) / 2;

  for (int i = 0; i < newimage.dimx() - 2 * startx; ++i)
    for (int j = 0; j < newimage.dimy() - 2 * starty; ++j)
      newimage(i + startx, j + starty) = get(i / scale, j / scale);

  *this << newimage;

  return (*this);

} // image::upscale

//___________________________________________________________________________
// upscale a grid to 'size' by 'size'

image& image::downsample(float scl)
{
  if (scl == 0.0)
    return *this;

  int boxsize = int(1 / scl + FUZZ);
  int newx = int(dimx() * scl + FUZZ);
  int newy = int(dimy() * scl + FUZZ);
  image newimage(newx, newy);
  newimage.clear();

  for (int i = 0; i < newx; ++i)
    for (int j = 0; j < newy; ++j)
    {
      int val = 0;
      for (int k = 0; k < boxsize; ++k)
	for (int l = 0; l < boxsize; ++l)
	  val += get(i / scl + k, j / scl + l);
      newimage(i, j) = val / (boxsize * boxsize);
    }

  *this << newimage;

  return (*this);

} // image::downsample

//___________________________________________________________________________
// subtrace a best fit linear function from a greyscale image.

image& image::linearfit()
{

  if (nx != 20 || nx != ny)
  {
    cerr << "Unable to linearfit the image\n";
    exit(1);
  }

  dgrid x, xtrans, y, beta;
  x.init(3, 360);
  y.init(1, 360);
  
  int counter = 0;
  for (int i = 0; i < 20; ++i)
    for (int j = 0; j < 20; ++j)
    {
      if (i + j <= 3 || i + (19 - j) <= 3 || 
	  (19 - i) + j <= 3 || (19 - i) + (19 - j) <= 3)
	continue;
      x(0, counter) = 1;
      x(1, counter) = i;
      x(2, counter) = j;
      y(0, counter) = (*this)(i, j);
      ++counter;
    }

  xtrans = x.transpose();

  beta = (xtrans * x).inverse() * xtrans * y;
//  cout << "betadump\n";
//  beta.dump();
//  cout << endl;

  dgrid bf(20, 20);
  for (int i = 0; i < 20; ++i)
    for (int j = 0; j < 20; ++j)
      bf(i, j) = (*this)(i, j) - beta(0, 0) - beta(0, 1) * i - beta(0, 2) * j;

  double min = 1000.0;
  for (int i = 0; i < 20; ++i)
    for (int j = 0; j < 20; ++j)
      if (bf(i, j) < min)
	min = bf(i, j);
  
  for (int i = 0; i < 20; ++i)
    for (int j = 0; j < 20; ++j)
      (*this)(i, j) = uchar(bf(i, j) - min + FUZZ);
  
//  beta = (xtrans * x).inverse() * xtrans * y;
//  cout << "betadump\n";
//  beta.dump();
//  cout << endl;

  return *this;


} // linearfit

//___________________________________________________________________________
// histogram equalize a greyscale image

image& image::equalize()
{

  igrid values(255);
  values.clear();
  for (int i = 0; i < 20; ++i)
    for (int j = 0; j < 20; ++j)
    {
      if (i + j <= 3 || i + (19 - j) <= 3 || 
	  (19 - i) + j <= 3 || (19 - i) + (19 - j) <= 3)
	continue;
      ++values((*this)(i, j));
    }

  cgrid map(255);
  int count = 0;
  for (int i = 0; i < 255; ++i)
  {
    map(i) = (count + values(i) / 2) * 255 / 360;
    count += values(i);
  }

  for (int i = 0; i < 20; ++i)
    for (int j = 0; j < 20; ++j)
    {
//      if (i + j <= 3 || i + (19 - j) <= 3 || 
//	  (19 - i) + j <= 3 || (19 - i) + (19 - j) <= 3)
//	continue;
      (*this)(i, j) = map((*this)(i, j));
    }

  return *this;

} // image::equalize

//___________________________________________________________________________
// downscale a grid to 'size' by 'size'

image& image::downscale(int size)
{
  if (size <= 0 || dimx == 0 || dimy == 0)
    return *this;

  image newimage(size, size);
  float scale = (dimx() >? dimy()) / float(size);

  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      int spots = 0;
      for (int k = 0; k < scale; ++k)
	for (int l = 0; l < scale; ++l)
	  spots += get(i * scale + k, j * scale + l);
      newimage(i, j) = (spots > scale * scale * alpha);
    }
  }

  (*this) << newimage.trim(-size);

  return (*this);

} // downscale

//___________________________________________________________________________

image& image::slant(int slnt)
{
  // Check to see we really want to slant.
  if (slnt <= 0)
    return *this;


  // Determine the average x and y indices of the stroke pixels.
  float yavg = 0.0, xavg = 0.0;
  int numstroke = 0;
  for (int i = 0; i < dimx(); ++i)
    for (int j = 0; j < dimy(); ++j)
      if ((*this)(i, j) != 0)
      {
        ++numstroke;
        xavg += i;
        yavg += j;
      }

  // Return now if the image is blank
  if (numstroke == 0)
    return *this;

  xavg /= numstroke;
  yavg /= numstroke;
  
  // Calculate the slope and intercept of the least squares line.
  float slopenum = 0.0, slopeden = 0.0;
  for (int i = 0; i < dimx(); ++i)
    for (int j = 0; j < dimy(); ++j)
      if ((*this)(i, j) != 0)
      {
        slopenum += j * (i - xavg);
        slopeden += j * (j - yavg);
      }
  float slope = (slopeden == 0.0 ? 0.0 : slopenum / slopeden);
  slope *= (fabs(slope) < 1.2);
//  float intercept = xavg - slope * yavg;
//
//   // Determine the MSE of the least squares line.
//   float mse = 0.0;
//   if (numstroke > 0)
//   {
//     for (int i = 0; i < dimx(); ++i)
//       for (int j = 0; j < dimy(); ++j)
//         if ((*this)(i, j) != 0)
//           mse += (slope * j + intercept - i) * (slope * j + intercept - i);
//     mse /= numstroke;
//   }

  // Correct the slant according to the slope calculated above.
  image newimage(dimx() * 2, dimy());
  newimage.clear();
  for (int i = 0; i < dimx() * 2; ++i)
    for (int j = 0; j < dimy(); ++j)
      newimage(i, j) = get(i - dimx() / 2 - (dimy() - j) * slope, j);

  *this << newimage.trim();

  return (*this);

} // image::slant

//___________________________________________________________________________

image& image::skew(float skw)
{
  if (skw == 0)
    return (*this);

  if (randomize)
  {
    int randskew = lrand48() % 1001;
    skw *= float(randskew - 500) / 500;
  }

  // Correct the skew according to the slope.
  // Initially make the skew corrected grid very large.
  image newimage(dimx() * 2, dimy());
  newimage.clear();
  for (int i = 0; i < dimx() * 2; ++i)
    for (int j = 0; j < dimy(); ++j)
      newimage(i, j) = get( i / (1.0 + skw), j);

  *this << newimage.trim();

  return (*this);

} // image::skew

//___________________________________________________________________________
// Regularly changing procedure...

image& image::std_process(int fltr, int scl, int slnt, 
			  float skw, float rtt, int thn)
{

  filter(fltr);
  slant(slnt);
  skew(skw);
  rotate(rtt);
  if (scl) {
    upscale(128);
    downscale(scl);
  }
  thin(thn);

  return (*this);

} // image::std_process

//___________________________________________________________________________
// image.c
