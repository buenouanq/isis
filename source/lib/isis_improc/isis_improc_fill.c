/* 
   
   Image processing library 
   
   by Stefan Agamanolis
   May 1995 
   
   fill_constant.c
   
   */

#include <stdlib.h>
#include "isis_improc_private.h"

/*----------------------------------------------------------------------*/
void fill_constant_byte(unsigned char val,
			int xsize,
			int ysize,
			unsigned char *ptr,
			int skip, 
			int stride)
{
  int x, y, diff; 

  diff = stride - (xsize * skip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *ptr = val;      
      ptr += skip; 
    }
    ptr += diff;
  }
}

/*----------------------------------------------------------------------*/
void fill_constant_int(int val,
		       int xsize,
		       int ysize,
		       int *ptr,
		       int skip, 
		       int stride)
{
  int x, y, diff; 

  diff = stride - (xsize * skip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *ptr = val;      
      ptr += skip; 
    }
    ptr += diff;
  }
}

/*----------------------------------------------------------------------*/
void fill_constant_long(long val,
			int xsize,
			int ysize,
			long *ptr,
			int skip, 
			int stride)
{
  int x, y, diff; 

  diff = stride - (xsize * skip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *ptr = val;      
      ptr += skip; 
    }
    ptr += diff;
  }
}

/*----------------------------------------------------------------------*/
void fill_constant_float(float val,
			 int xsize,
			 int ysize,
			 float *ptr,
			 int skip, 
			 int stride)
{
  int x, y, diff; 

  diff = stride - (xsize * skip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *ptr = val;      
      ptr += skip; 
    }
    ptr += diff;
  }
}

/*----------------------------------------------------------------------*/
void fill_constant_double(double val,
			  int xsize,
			  int ysize,
			  double *ptr,
			  int skip, 
			  int stride)
{
  int x, y, diff; 

  diff = stride - (xsize * skip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *ptr = val;      
      ptr += skip; 
    }
    ptr += diff;
  }
}

