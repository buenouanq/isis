/* 
   
   Image processing library 
   
   by Stefan Agamanolis
   October 1996
   
   range.c
   
   */

#include <stdlib.h>
#include "isis_improc_private.h"

/*----------------------------------------------------------------------*/
void range_1ch_byte(unsigned char low,
		    unsigned char high,
		    int xsize,
		    int ysize,
		    unsigned char *iptr,
		    int inskip,
		    int instride,
		    unsigned char *optr,
		    int outskip,
		    int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr >= low && *iptr <= high) *optr = *iptr;
      iptr += inskip;
      optr += outskip; 
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void range_1ch_int(int low,
		   int high,
		   int xsize,
		   int ysize,
		   int *iptr,
		   int inskip,
		   int instride,
		   int *optr,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr >= low && *iptr <= high) *optr = *iptr;
      iptr += inskip;
      optr += outskip; 
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void range_1ch_long(long low,
		    long high,
		    int xsize,
		    int ysize,
		    long *iptr,
		    int inskip,
		    int instride,
		    long *optr,
		    int outskip,
		    int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr >= low && *iptr <= high) *optr = *iptr;
      iptr += inskip;
      optr += outskip; 
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void range_1ch_float(float low,
		     float high,
		     int xsize,
		     int ysize,
		     float *iptr,
		     int inskip,
		     int instride,
		     float *optr,
		     int outskip,
		     int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr >= low && *iptr <= high) *optr = *iptr;
      iptr += inskip;
      optr += outskip; 
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void range_1ch_double(double low,
		      double high,
		      int xsize,
		      int ysize,
		      double *iptr,
		      int inskip,
		      int instride,
		      double *optr,
		      int outskip,
		      int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr >= low && *iptr <= high) *optr = *iptr;
      iptr += inskip;
      optr += outskip; 
    }
    iptr += indiff;
    optr += outdiff;
  }
}


/*----------------------------------------------------------------------*/
void range_3ch_byte(unsigned char low0,
		    unsigned char high0,
		    unsigned char low1,
		    unsigned char high1,
		    unsigned char low2,
		    unsigned char high2,
		    int xsize,
		    int ysize,
		    unsigned char *iptr0,
		    unsigned char *iptr1,
		    unsigned char *iptr2,
		    int inskip,
		    int instride,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    unsigned char *optr2,
		    int outskip,
		    int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr0 >= low0 && *iptr0 <= high0 &&
	 *iptr1 >= low1 && *iptr1 <= high1 &&
	 *iptr2 >= low2 && *iptr2 <= high2) {
	*optr0 = *iptr0;
	*optr1 = *iptr1;
	*optr2 = *iptr2;
      }
      iptr0 += inskip;
      iptr1 += inskip;
      iptr2 += inskip;
      optr0 += outskip; 
      optr1 += outskip; 
      optr2 += outskip; 
    }
    iptr0 += indiff;
    iptr1 += indiff;
    iptr2 += indiff;
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
  }
}


/*----------------------------------------------------------------------*/
void range_3ch_int(int low0,
		   int high0,
		   int low1,
		   int high1,
		   int low2,
		   int high2,
		   int xsize,
		   int ysize,
		   int *iptr0,
		   int *iptr1,
		   int *iptr2,
		   int inskip,
		   int instride,
		   int *optr0,
		   int *optr1,
		   int *optr2,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr0 >= low0 && *iptr0 <= high0 &&
	 *iptr1 >= low1 && *iptr1 <= high1 &&
	 *iptr2 >= low2 && *iptr2 <= high2) {
	*optr0 = *iptr0;
	*optr1 = *iptr1;
	*optr2 = *iptr2;
      }
      iptr0 += inskip;
      iptr1 += inskip;
      iptr2 += inskip;
      optr0 += outskip; 
      optr1 += outskip; 
      optr2 += outskip; 
    }
    iptr0 += indiff;
    iptr1 += indiff;
    iptr2 += indiff;
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
  }
}


/*----------------------------------------------------------------------*/
void range_3ch_long(long low0,
		    long high0,
		    long low1,
		    long high1,
		    long low2,
		    long high2,
		    int xsize,
		    int ysize,
		    long *iptr0,
		    long *iptr1,
		    long *iptr2,
		    int inskip,
		    int instride,
		    long *optr0,
		    long *optr1,
		    long *optr2,
		    int outskip,
		    int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr0 >= low0 && *iptr0 <= high0 &&
	 *iptr1 >= low1 && *iptr1 <= high1 &&
	 *iptr2 >= low2 && *iptr2 <= high2) {
	*optr0 = *iptr0;
	*optr1 = *iptr1;
	*optr2 = *iptr2;
      }
      iptr0 += inskip;
      iptr1 += inskip;
      iptr2 += inskip;
      optr0 += outskip; 
      optr1 += outskip; 
      optr2 += outskip; 
    }
    iptr0 += indiff;
    iptr1 += indiff;
    iptr2 += indiff;
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
  }
}


/*----------------------------------------------------------------------*/
void range_3ch_float(float low0,
		     float high0,
		     float low1,
		     float high1,
		     float low2,
		     float high2,
		     int xsize,
		     int ysize,
		     float *iptr0,
		     float *iptr1,
		     float *iptr2,
		     int inskip,
		     int instride,
		     float *optr0,
		     float *optr1,
		     float *optr2,
		     int outskip,
		     int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr0 >= low0 && *iptr0 <= high0 &&
	 *iptr1 >= low1 && *iptr1 <= high1 &&
	 *iptr2 >= low2 && *iptr2 <= high2) {
	*optr0 = *iptr0;
	*optr1 = *iptr1;
	*optr2 = *iptr2;
      }
      iptr0 += inskip;
      iptr1 += inskip;
      iptr2 += inskip;
      optr0 += outskip; 
      optr1 += outskip; 
      optr2 += outskip; 
    }
    iptr0 += indiff;
    iptr1 += indiff;
    iptr2 += indiff;
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
  }
}


/*----------------------------------------------------------------------*/
void range_3ch_double(double low0,
		      double high0,
		      double low1,
		      double high1,
		      double low2,
		      double high2,
		      int xsize,
		      int ysize,
		      double *iptr0,
		      double *iptr1,
		      double *iptr2,
		      int inskip,
		      int instride,
		      double *optr0,
		      double *optr1,
		      double *optr2,
		      int outskip,
		      int outstride)
{
  int x, y, indiff, outdiff; 
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptr0 >= low0 && *iptr0 <= high0 &&
	 *iptr1 >= low1 && *iptr1 <= high1 &&
	 *iptr2 >= low2 && *iptr2 <= high2) {
	*optr0 = *iptr0;
	*optr1 = *iptr1;
	*optr2 = *iptr2;
      }
      iptr0 += inskip;
      iptr1 += inskip;
      iptr2 += inskip;
      optr0 += outskip; 
      optr1 += outskip; 
      optr2 += outskip; 
    }
    iptr0 += indiff;
    iptr1 += indiff;
    iptr2 += indiff;
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
  }
}

