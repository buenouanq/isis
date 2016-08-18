/* Bounding box routines */
/* by Stefan Agamanolis and Ann Bui */

#include <stdio.h>
#include <stdlib.h>
#include "isis_improc_private.h"


/*----------------------------------------------------------------------*/
int bounding_box(unsigned char low,
		 unsigned char high,
		 int xsize,
		 int ysize,
		 unsigned char *buf,
		 int skip,
		 int stride,
		 int *topleftx,
		 int *toplefty,
		 int *botrightx,
		 int *botrighty)
{
  int x, y, diff, found;
  int boxleft, boxright, boxtop, boxbottom;
  
  boxleft = xsize;
  boxtop = ysize;
  boxright = boxbottom = -1;

  diff = stride - (xsize * skip);

  for(y = 0; y < ysize; y++) {

    found = 0;

    for(x = 0; x < xsize; x++) {
      if (*buf <= high && *buf >= low) {
	if(x < boxleft) boxleft = x;
	if(x > boxright) boxright = x;
	found = 1;   /* there is something on this line */
      }
      buf += skip;
    }
    
    if(found) {
      if(y < boxtop) boxtop = y;
      if(y > boxbottom) boxbottom = y;
    }
    
    buf += diff;
  }
    
  if(boxright >= 0) {
    *topleftx = boxleft;
    *toplefty = boxtop;
    *botrightx = boxright;
    *botrighty = boxbottom;
    return 1;
  }    
  else return 0;
}



/*----------------------------------------------------------------------*/
int bounding_box_rgb(unsigned char rlow,
		     unsigned char rhigh,
		     unsigned char glow,
		     unsigned char ghigh,
		     unsigned char blow,
		     unsigned char bhigh,
		     int xsize,
		     int ysize,
		     unsigned char *rbuf,
		     unsigned char *gbuf,
		     unsigned char *bbuf,
		     int skip,
		     int stride,
		     int *topleftx,
		     int *toplefty,
		     int *botrightx,
		     int *botrighty)
{
  int x, y, diff, found;
  int boxleft, boxright, boxtop, boxbottom;
  
  boxleft = xsize;
  boxtop = ysize;
  boxright = boxbottom = -1;

  diff = stride - (xsize * skip);

  for(y = 0; y < ysize; y++) {

    found = 0;

    for(x = 0; x < xsize; x++) {
      if (*rbuf <= rhigh && *rbuf >= rlow && 
	  *gbuf <= ghigh && *gbuf >= glow && 
	  *bbuf <= bhigh && *bbuf >= blow ) {
	if(x < boxleft) boxleft = x;
	if(x > boxright) boxright = x;
	found = 1;   /* there is something on this line */
      }
      rbuf += skip;
      gbuf += skip;
      bbuf += skip;
    }
    
    if(found) {
      if(y < boxtop) boxtop = y;
      if(y > boxbottom) boxbottom = y;
    }
    
    rbuf += diff;
    gbuf += diff;
    bbuf += diff;
  }
    
  if(boxright >= 0) {
    *topleftx = boxleft;
    *toplefty = boxtop;
    *botrightx = boxright;
    *botrighty = boxbottom;
    return 1;
  }    
  else return 0;
}



/*----------------------------------------------------------------------*/
int fuzzy_bounding_box(int thresh, 
		       int pixcount,
		       int xdim, 
		       int ydim,
		       unsigned char *pbuf, 
		       int skip,
		       int stride,
		       int *box)
{
  int x, y;
  int something;
  int total;
  int ymin, ymax, xmin, xmax;
  int exxmin, exxmax, exymax, exymin; 
  unsigned char *abuf, *temp;

  exxmin = exxmax = exymax = exymin = 0;
  ymin = ydim;
  xmin = xdim;
  xmax = ymax = 0;
  
  for(y = 0; y < ydim; y++) {
    abuf = temp = pbuf;
    something = total = 0;

    for(x = 0; x < pixcount; x++) { 
      total += *temp;
      temp += skip;
    }

    for(x = pixcount; x < xdim; x++) {
      if(total >= thresh) {
	if(x < xmin) {
	  exxmin = y; xmin = x;
	}
	if(x > xmax) {
	  exxmax = y; xmax = x;
	}
	something = 1;  /* something found on this line */
      }
      total += *temp;
      total -= *abuf;
      temp += skip;
      abuf += skip;
    }
    if(something) {
      if(y < ymin) {
	exymin = x; ymin = y;
      }
      if(y > ymax) {
	exymax = x; ymax = y;
      }
    }
    pbuf += stride;
  }
  
  box[0] = xmin - pixcount;
  box[1] = ymin;
  box[2] = xmax;
  box[3] = ymax;
  box[4] = exxmin;
  box[5] = exymin;
  box[6] = exxmax;
  box[7] = exymax;
  
  return (ymin < ymax);    /* if something was found, this will be true */
}
  

