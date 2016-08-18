/* 
   
   Image processing library 
   
   by Stefan Agamanolis
   May 1995 
   
   arith.c
   
   */

#include <stdlib.h>
#include <math.h>
#include "isis_improc_private.h"

/*----------------------------------------------------------------------*/
void image_add_byte(int xsize,
		    int ysize,
		    unsigned char *i1ptr,
		    int i1skip,
		    int i1stride,
		    unsigned char *i2ptr,
		    int i2skip,
		    int i2stride,
		    unsigned char *optr,
		    int oskip,
		    int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = ((int) *i1ptr) + ((int) *i2ptr);
      *optr = (unsigned char) ((pix > 255) ? 255 : pix);
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_add_byte_int_byte(int xsize,
			     int ysize,
			     unsigned char *i1ptr,
			     int i1skip,
			     int i1stride,
			     int *i2ptr,
			     int i2skip,
			     int i2stride,
			     unsigned char *optr,
			     int oskip,
			     int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = ((int) *i1ptr) + *i2ptr;
      *optr = (unsigned char) ((pix > 255) ? 255 : ((pix < 0) ? 0 : pix));
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_add_byte_int_int(int xsize,
			    int ysize,
			    unsigned char *i1ptr,
			    int i1skip,
			    int i1stride,
			    int *i2ptr,
			    int i2skip,
			    int i2stride,
			    int *optr,
			    int oskip,
			    int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = ((int) *i1ptr) + *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_add_int(int xsize,
		   int ysize,
		   int *i1ptr,
		   int i1skip,
		   int i1stride,
		   int *i2ptr,
		   int i2skip,
		   int i2stride,
		   int *optr,
		   int oskip,
		   int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr + *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_add_double(int xsize,
		      int ysize,
		      double *i1ptr,
		      int i1skip,
		      int i1stride,
		      double *i2ptr,
		      int i2skip,
		      int i2stride,
		      double *optr,
		      int oskip,
		      int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr + *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}


/*----------------------------------------------------------------------*/
void image_subtract_byte(int xsize,
			 int ysize,
			 unsigned char *i1ptr,
			 int i1skip,
			 int i1stride,
			 unsigned char *i2ptr,
			 int i2skip,
			 int i2stride,
			 unsigned char *optr,
			 int oskip,
			 int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;
  
  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = ((int) *i1ptr) - ((int) *i2ptr);
      *optr = (unsigned char) ((pix < 0) ? 0 : pix);
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_subtract_byte_int_byte(int xsize,
				  int ysize,
				  unsigned char *i1ptr,
				  int i1skip,
				  int i1stride,
				  int *i2ptr,
				  int i2skip,
				  int i2stride,
				  unsigned char *optr,
				  int oskip,
				  int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;
      
  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);
      
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = ((int) *i1ptr) - *i2ptr;
      *optr = (unsigned char) ((pix > 255) ? 255 : ((pix < 0) ? 0 : pix));
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_subtract_int(int xsize,
			int ysize,
			int *i1ptr,
			int i1skip,
			int i1stride,
			int *i2ptr,
			int i2skip,
			int i2stride,
			int *optr,
			int oskip,
			int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr - *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_subtract_double(int xsize,
			   int ysize,
			   double *i1ptr,
			   int i1skip,
			   int i1stride,
			   double *i2ptr,
			   int i2skip,
			   int i2stride,
			   double *optr,
			   int oskip,
			   int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr - *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}


/*----------------------------------------------------------------------*/
void image_diff_byte(int xsize,
		     int ysize,
		     unsigned char *i1ptr,
		     int i1skip,
		     int i1stride,
		     unsigned char *i2ptr,
		     int i2skip,
		     int i2stride,
		     unsigned char *optr,
		     int oskip,
		     int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (unsigned char) abs(((int) *i1ptr) - ((int) *i2ptr));
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }    
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_diff_byte_int_byte(int xsize,
			      int ysize,
			      unsigned char *i1ptr,
			      int i1skip,
			      int i1stride,
			      int *i2ptr,
			      int i2skip,
			      int i2stride,
			      unsigned char *optr,
			      int oskip,
			      int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;
	  
  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);
	  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = abs(((int) *i1ptr) - *i2ptr);
      *optr = (unsigned char) ((pix > 255) ? 255 : pix);
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_diff_int(int xsize,
		    int ysize,
		    int *i1ptr,
		    int i1skip,
		    int i1stride,
		    int *i2ptr,
		    int i2skip,
		    int i2stride,
		    int *optr,
		    int oskip,
		    int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = abs(*i1ptr - *i2ptr);
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_diff_double(int xsize,
		       int ysize,
		       double *i1ptr,
		       int i1skip,
		       int i1stride,
		       double *i2ptr,
		       int i2skip,
		       int i2stride,
		       double *optr,
		       int oskip,
		       int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = fabs(*i1ptr - *i2ptr);
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_multiply_byte(int xsize,
			 int ysize,
			 unsigned char *i1ptr,
			 int i1skip,
			 int i1stride,
			 unsigned char *i2ptr,
			 int i2skip,
			 int i2stride,
			 unsigned char *optr,
			 int oskip,
			 int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;
	      
  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);
	      
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = ((int) *i1ptr) * ((int) *i2ptr);
      *optr = (unsigned char) ((pix > 255) ? 255 : pix);
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_multiply_byte_int_byte(int xsize,
				  int ysize,
				  unsigned char *i1ptr,
				  int i1skip,
				  int i1stride,
				  int *i2ptr,
				  int i2skip,
				  int i2stride,
				  unsigned char *optr,
				  int oskip,
				  int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;
		  
  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);
		  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = ((int) *i1ptr) * *i2ptr;
      *optr = (unsigned char) ((pix > 255) ? 255 : ((pix < 0) ? 0 : pix));
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_multiply_int(int xsize,
			int ysize,
			int *i1ptr,
			int i1skip,
			int i1stride,
			int *i2ptr,
			int i2skip,
			int i2stride,
			int *optr,
			int oskip,
			int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr * *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_multiply_double(int xsize,
			   int ysize,
			   double *i1ptr,
			   int i1skip,
			   int i1stride,
			   double *i2ptr,
			   int i2skip,
			   int i2stride,
			   double *optr,
			   int oskip,
			   int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr * *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_divide_byte(int xsize,
		       int ysize,
		       unsigned char *i1ptr,
		       int i1skip,
		       int i1stride,
		       unsigned char *i2ptr,
		       int i2skip,
		       int i2stride,
		       unsigned char *optr,
		       int oskip,
		       int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr / *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_divide_byte_int_byte(int xsize,
				int ysize,
				unsigned char *i1ptr,
				int i1skip,
				int i1stride,
				int *i2ptr,
				int i2skip,
				int i2stride,
				unsigned char *optr,
				int oskip,
				int ostride)
{
  int x, y, i1diff, i2diff, odiff, pix;
		      
  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);
		      
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      pix = ((int) *i1ptr) / *i2ptr;
      *optr = (unsigned char) ((pix > 255) ? 255 : ((pix < 0) ? 0 : pix));
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_divide_int(int xsize,
		      int ysize,
		      int *i1ptr,
		      int i1skip,
		      int i1stride,
		      int *i2ptr,
		      int i2skip,
		      int i2stride,
		      int *optr,
		      int oskip,
		      int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr / *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_divide_double(int xsize,
			 int ysize,
			 double *i1ptr,
			 int i1skip,
			 int i1stride,
			 double *i2ptr,
			 int i2skip,
			 int i2stride,
			 double *optr,
			 int oskip,
			 int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = *i1ptr / *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_maximum_byte(int xsize,
			int ysize,
			unsigned char *i1ptr,
			int i1skip,
			int i1stride,
			unsigned char *i2ptr,
			int i2skip,
			int i2stride,
			unsigned char *optr,
			int oskip,
			int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (*i1ptr > *i2ptr) ? *i1ptr : *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_maximum_int(int xsize,
		       int ysize,
		       int *i1ptr,
		       int i1skip,
		       int i1stride,
		       int *i2ptr,
		       int i2skip,
		       int i2stride,
		       int *optr,
		       int oskip,
		       int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (*i1ptr > *i2ptr) ? *i1ptr : *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_maximum_double(int xsize,
			  int ysize,
			  double *i1ptr,
			  int i1skip,
			  int i1stride,
			  double *i2ptr,
			  int i2skip,
			  int i2stride,
			  double *optr,
			  int oskip,
			  int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (*i1ptr > *i2ptr) ? *i1ptr : *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_minimum_byte(int xsize,
			int ysize,
			unsigned char *i1ptr,
			int i1skip,
			int i1stride,
			unsigned char *i2ptr,
			int i2skip,
			int i2stride,
			unsigned char *optr,
			int oskip,
			int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (*i1ptr < *i2ptr) ? *i1ptr : *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_minimum_int(int xsize,
		       int ysize,
		       int *i1ptr,
		       int i1skip,
		       int i1stride,
		       int *i2ptr,
		       int i2skip,
		       int i2stride,
		       int *optr,
		       int oskip,
		       int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (*i1ptr < *i2ptr) ? *i1ptr : *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}

/*----------------------------------------------------------------------*/
void image_minimum_double(int xsize,
			  int ysize,
			  double *i1ptr,
			  int i1skip,
			  int i1stride,
			  double *i2ptr,
			  int i2skip,
			  int i2stride,
			  double *optr,
			  int oskip,
			  int ostride)
{
  int x, y, i1diff, i2diff, odiff;

  i1diff = i1stride - (xsize * i1skip);
  i2diff = i2stride - (xsize * i2skip);
  odiff = ostride - (xsize * oskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = (*i1ptr < *i2ptr) ? *i1ptr : *i2ptr;
      i1ptr += i1skip;
      i2ptr += i2skip;
      optr += oskip;
    }
    i1ptr += i1diff;
    i2ptr += i2diff;
    optr += odiff;
  }
}
