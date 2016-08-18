/* 
   
   Image processing library 
   
   by Stefan Agamanolis
   May 1995 
   
   convert.c
   
   */

#include <stdlib.h>
#include "isis_improc_private.h"

/* BYTE to whatever: */

/*----------------------------------------------------------------------*/
void convert_byte_short(int xsize,
			int ysize,
			unsigned char *iptr,	     
			int inskip,
			int instride,
			short *optr,
			int outskip,
			int outstride)
{
  int x, y, indiff, outdiff;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_byte_int(int xsize,
		      int ysize,
		      unsigned char *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_byte_long(int xsize,
		       int ysize,
		       unsigned char *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_byte_float(int xsize,
			int ysize,
			unsigned char *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_byte_double(int xsize,
			 int ysize,
			 unsigned char *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/* SHORT to whatever: */

/*----------------------------------------------------------------------*/
void convert_short_byte(int xsize,
			int ysize,
			short *iptr,	     
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
      *optr = (unsigned char) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_short_int(int xsize,
		       int ysize,
		       short *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_short_long(int xsize,
			int ysize,
			short *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_short_float(int xsize,
			 int ysize,
			 short *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_short_double(int xsize,
			  int ysize,
			  short *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/* INT to whatever: */

/*----------------------------------------------------------------------*/
void convert_int_byte(int xsize,
		      int ysize,
		      int *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_int_short(int xsize,
		       int ysize,
		       int *iptr,	     
		       int inskip,
		       int instride,
		       short *optr,
		       int outskip,
		       int outstride)
{
  int x, y, indiff, outdiff;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_int_long(int xsize,
		      int ysize,
		      int *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_int_float(int xsize,
		       int ysize,
		       int *iptr,	     
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
      *optr = (float) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_int_double(int xsize,
			int ysize,
			int *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/* LONG to whatever: */

/*----------------------------------------------------------------------*/
void convert_long_byte(int xsize,
		       int ysize,
		       long *iptr,	     
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
      *optr = (unsigned char) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_long_short(int xsize,
			int ysize,
			long *iptr,	     
			int inskip,
			int instride,
			short *optr,
			int outskip,
			int outstride)
{
  int x, y, indiff, outdiff;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (short) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_long_int(int xsize,
		      int ysize,
		      long *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_long_float(int xsize,
			int ysize,
			long *iptr,	     
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
      *optr = (float) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_long_double(int xsize,
			 int ysize,
			 long *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/* FLOAT to whatever: */

/*----------------------------------------------------------------------*/
void convert_float_byte(int xsize,
			int ysize,
			float *iptr,	     
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
      *optr = (unsigned char) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_float_short(int xsize,
			 int ysize,
			 float *iptr,	     
			 int inskip,
			 int instride,
			 short *optr,
			 int outskip,
			 int outstride)
{
  int x, y, indiff, outdiff;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (short) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_float_int(int xsize,
		       int ysize,
		       float *iptr,	     
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
      *optr = (int) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_float_long(int xsize,
			int ysize,
			float *iptr,	     
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
      *optr = (long) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_float_double(int xsize,
			  int ysize,
			  float *iptr,	     
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
      *optr = *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/* DOUBLE to whatever: */

/*----------------------------------------------------------------------*/
void convert_double_byte(int xsize,
			 int ysize,
			 double *iptr,	     
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
      *optr = (unsigned char) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_double_short(int xsize,
			  int ysize,
			  double *iptr,	     
			  int inskip,
			  int instride,
			  short *optr,
			  int outskip,
			  int outstride)
{
  int x, y, indiff, outdiff;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (short) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_double_int(int xsize,
			int ysize,
			double *iptr,	     
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
      *optr = (int) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_double_long(int xsize,
			 int ysize,
			 double *iptr,	     
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
      *optr = (long) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void convert_double_float(int xsize,
			  int ysize,
			  double *iptr,	     
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
      *optr = (float) *iptr;
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

