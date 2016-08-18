/* 
   
   Image processing library 
   
   Gain Bias routines
   
   by Stefan Agamanolis
   march 1996 
   
   gainbias.c
   
   12/9/96  wad  Added gainbias_short()
   12/19/96 wad  Added support for type conversion !
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "isis_improc_private.h"

/*----------------------------------------------------------------------*/
/*   BYTE -> ANY TYPE   */

/*  This function should be called gainbias_byte_byte, but isn't for
    historical reasons...  */

void gainbias_byte(double gain,
		   double bias,
		   int xsize,
		   int ysize,
		   unsigned char *iptr,
		   int inskip,
		   int instride,
		   unsigned char *optr,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, outdiff, pix; 
  unsigned char gbmap[256];
  
  if(gain == 1.0 && bias == 0.0 && iptr != optr) {
    transfer_byte(xsize, ysize, 
		  iptr, inskip, instride, 
		  optr, outskip, outstride);
    return;
  }

  /* create table for gain bias values */

  for(x = 0; x < 256; x++) {
    pix = (int) ((x * gain) + bias);
    gbmap[x] = (unsigned char) ((pix > 255) ? 255 : ((pix < 0) ? 0 : pix));
  }
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = gbmap[*iptr];
      iptr += inskip;
      optr += outskip; 
    }  
    iptr += indiff;
    optr += outdiff;
  }
}


void gainbias_byte_short(double gain,
			 double bias,
			 int xsize,
			 int ysize,
			 unsigned char *iptr,
			 int inskip,
			 int instride,
			 signed short *optr,
			 int outskip,
			 int outstride)
{
  int x, y, indiff, outdiff, pix; 
  signed short   gbmap[256];
  
  if(gain == 1.0 && bias == 0.0) {
    convert_byte_short(xsize, ysize, 
		       iptr, inskip, instride, 
		       optr, outskip, outstride);
    return;
  }

  /* create table for gain bias values */

  for(x = 0; x < 256; x++) {
    pix = (int)(((double)x * gain) + bias);
    gbmap[x] = (signed short)((pix > SHRT_MAX) ? SHRT_MAX :
			      ((pix < SHRT_MIN) ? SHRT_MIN : pix));
  }
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = gbmap[*iptr];
      iptr += inskip;
      optr += outskip; 
    }  
    iptr += indiff;
    optr += outdiff;
  }
}


void gainbias_byte_int( double gain,
			  double bias,
			  int xsize,
			  int ysize,
			  unsigned char *iptr,
			  int inskip,
			  int instride,
			  int *optr,
			  int outskip,
			  int outstride)
{
  int x, y, indiff, outdiff; 
  int gbmap[256];
  
  if(gain == 1.0 && bias == 0.0) {
    convert_byte_int( xsize, ysize, 
			 iptr, inskip, instride, 
			 optr, outskip, outstride);
    return;
  }

  /* create table for gain bias values */

  for(x = 0; x < 256; x++) {
    gbmap[x] = (int)(((double)x * gain) + bias);
  }
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = gbmap[*iptr];
      iptr += inskip;
      optr += outskip; 
    }  
    iptr += indiff;
    optr += outdiff;
  }
}


void gainbias_byte_long( double gain,
			  double bias,
			  int xsize,
			  int ysize,
			  unsigned char *iptr,
			  int inskip,
			  int instride,
			  long *optr,
			  int outskip,
			  int outstride)
{
  int x, y, indiff, outdiff; 
  long gbmap[256];
  
  if(gain == 1.0 && bias == 0.0) {
    convert_byte_long( xsize, ysize, 
		      iptr, inskip, instride, 
		      optr, outskip, outstride);
    return;
  }

  /* create table for gain bias values */

  for(x = 0; x < 256; x++) {
    gbmap[x] = (long)(((double)x * gain) + bias);
  }
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = gbmap[*iptr];
      iptr += inskip;
      optr += outskip; 
    }  
    iptr += indiff;
    optr += outdiff;
  }
}


void gainbias_byte_float(double gain,
			 double bias,
			 int xsize,
			 int ysize,
			 unsigned char *iptr,
			 int inskip,
			 int instride,
			 float *optr,
			 int outskip,
			 int outstride)
{
  int x, y, indiff, outdiff; 
  float gbmap[256];
  
  if(gain == 1.0 && bias == 0.0) {
    convert_byte_float(xsize, ysize, 
			iptr, inskip, instride, 
			optr, outskip, outstride);
    return;
  }

  /* create table for gain bias values */

  for(x = 0; x < 256; x++) {
    gbmap[x] = (float)(((double)x * gain) + bias);
  }
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = gbmap[*iptr];
      iptr += inskip;
      optr += outskip; 
    }  
    iptr += indiff;
    optr += outdiff;
  }
}


void gainbias_byte_double(double gain,
			  double bias,
			  int xsize,
			  int ysize,
			  unsigned char *iptr,
			  int inskip,
			  int instride,
			  double *optr,
			  int outskip,
			  int outstride)
{
  int x, y, indiff, outdiff; 
  double gbmap[256];
  
  if(gain == 1.0 && bias == 0.0) {
    convert_byte_double(xsize, ysize, 
			iptr, inskip, instride, 
			optr, outskip, outstride);
    return;
  }

  /* create table for gain bias values */

  for(x = 0; x < 256; x++) {
    gbmap[x] = (((double)x * gain) + bias);
  }
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = gbmap[*iptr];
      iptr += inskip;
      optr += outskip; 
    }  
    iptr += indiff;
    optr += outdiff;
  }
}


/*----------------------------------------------------------------------*/
/*   SHORT -> ANY TYPE   */

void gainbias_short_byte(double gain,
			 double bias,
			 int xsize,
			 int ysize,
			 short *iptr,
			 int inskip,
			 int instride,
			 unsigned char *optr,
			 int outskip,
			 int outstride)
{
  int x, y, indiff, outdiff; 
  int inttmp, intbias;
  double  tmp;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = ((double)*iptr * gain) + bias;
	  if( tmp > (double)UCHAR_MAX )
	    tmp = (double)UCHAR_MAX;
	  else if( tmp < 0.0 )
	    tmp = 0.0;
	  *optr = (unsigned char)tmp;
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = (double)*iptr * gain;
	  if( tmp > (double)UCHAR_MAX )
	    tmp = (double)UCHAR_MAX;
	  else if( tmp < 0.0 )
	    tmp = 0.0;
	  *optr = (unsigned char)tmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to a long for
         increased speed (will use integer addition).  */
      intbias = (int)bias;
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  inttmp = (int)*iptr + intbias;
	  if( inttmp > UCHAR_MAX )
	    inttmp = UCHAR_MAX;
	  else if( inttmp < 0 )
	    inttmp = 0;
	  *optr = (unsigned char)inttmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  inttmp = (int)*iptr;
	  if( inttmp > UCHAR_MAX )
	    inttmp = UCHAR_MAX;
	  else if( inttmp < 0 )
	    inttmp = 0;
	  *optr = (unsigned char)inttmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
}


/*  This function should be named gainbias_short_short,
    but hey, it was here first !       */

void gainbias_short(double gain,
		    double bias,
		    int xsize,
		    int ysize,
		    short *iptr,
		    int inskip,
		    int instride,
		    short *optr,
		    int outskip,
		    int outstride)
{
  int x, y, indiff, outdiff; 
  int intbias, inttmp;
  double tmp;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = ((double)*iptr * gain) + bias;
	  if( tmp > (double)SHRT_MAX )
	    tmp = (double)SHRT_MAX;
	  else if( tmp < (double)SHRT_MIN )
	    tmp = (double)SHRT_MIN;
	  *optr = (short)tmp;
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = (double)*iptr * gain;
	  if( tmp > (double)SHRT_MAX )
	    tmp = (double)SHRT_MAX;
	  else if( tmp < (double)SHRT_MIN )
	    tmp = (double)SHRT_MIN;
	  *optr = (short)tmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to a long for
         increased speed (will use integer addition).  */
      intbias = (short) bias;
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  inttmp = (int)*iptr + intbias;
	  if( inttmp > SHRT_MAX )
	    inttmp = SHRT_MAX;
	  else if( inttmp < SHRT_MIN )
	    inttmp = SHRT_MIN;
	  *optr = (short)inttmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      if(iptr != optr) transfer_short(xsize, ysize, 
				      iptr, inskip, instride, 
				      optr, outskip, outstride);
      else
	fprintf( stderr, "Can't gainbias into same buffer !! \n" );
    }
  }
}


void gainbias_short_int(double gain,
			double bias,
			int xsize,
			int ysize,
			short *iptr,
			int inskip,
			int instride,
			int *optr,
			int outskip,
			int outstride)
{
  int x, y, indiff, outdiff; 
  int intbias, inttmp;
  double tmp;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = ((double)*iptr * gain) + bias;
	  if( tmp > (double)INT_MAX )
	    tmp = (double)INT_MAX;
	  else if( tmp < (double)INT_MIN )
	    tmp = (double)INT_MIN;
	  *optr = (int)tmp;
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = (double)*iptr * gain;
	  if( tmp > (double)INT_MAX )
	    tmp = (double)INT_MAX;
	  else if( tmp < (double)INT_MIN )
	    tmp = (double)INT_MIN;
	  *optr = (int)tmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to a long for
         increased speed (will use integer addition).  */
      intbias = (short) bias;
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  inttmp = (int)*iptr + intbias;
	  if( inttmp > INT_MAX )
	    inttmp = INT_MAX;
	  else if( inttmp < INT_MIN )
	    inttmp = INT_MIN;
	  *optr = inttmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      convert_short_int(xsize, ysize, 
			iptr, inskip, instride, 
			optr, outskip, outstride);
    }
  }
}


void gainbias_short_long(double gain,
			double bias,
			int xsize,
			int ysize,
			short *iptr,
			int inskip,
			int instride,
			long *optr,
			int outskip,
			int outstride)
{
  int x, y, indiff, outdiff; 
  long intbias, inttmp;
  double tmp;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = ((double)*iptr * gain) + bias;
	  if( tmp > (double)LONG_MAX )
	    tmp = (double)LONG_MAX;
	  else if( tmp < (double)LONG_MIN )
	    tmp = (double)LONG_MIN;
	  *optr = (long)tmp;
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  tmp = (double)*iptr * gain;
	  if( tmp > (double)LONG_MAX )
	    tmp = (double)LONG_MAX;
	  else if( tmp < (double)LONG_MIN )
	    tmp = (double)LONG_MIN;
	  *optr = (long)tmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to a long for
         increased speed (will use integer addition).  */
      intbias = (long)bias;
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  inttmp = (int)*iptr + intbias;
	  if( inttmp > LONG_MAX )
	    inttmp = LONG_MAX;
	  else if( inttmp < LONG_MIN )
	    inttmp = LONG_MIN;
	  *optr = inttmp;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      convert_short_long(xsize, ysize, 
			 iptr, inskip, instride, 
			 optr, outskip, outstride);
    }
  }
}

void gainbias_short_float(double gain,
			  double bias,
			  int xsize,
			  int ysize,
			  short *iptr,
			  int inskip,
			  int instride,
			  float *optr,
			  int outskip,
			  int outstride)
{
  int x, y, indiff, outdiff; 
  float fbias;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (float)(((double)*iptr * gain) + bias);
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (float)((double)*iptr * gain);
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to a long for
         increased speed (will use integer addition).  */
      fbias = (float)bias;
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (float)*iptr + fbias;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      convert_short_float(xsize, ysize, 
			  iptr, inskip, instride, 
			  optr, outskip, outstride);
    }
  }
}


void gainbias_short_double(double gain,
			  double bias,
			  int xsize,
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

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = ((double)*iptr * gain) + bias;
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (double)*iptr * gain;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to a long for
         increased speed (will use integer addition).  */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (double)*iptr + bias;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      convert_short_double(xsize, ysize, 
			  iptr, inskip, instride, 
			  optr, outskip, outstride);
    }
  }
}


/*----------------------------------------------------------------------*/
void gainbias_int(double gain,
		  double bias,
		  int xsize,
		  int ysize,
		  int *iptr,
		  int inskip,
		  int instride,
		  int *optr,
		  int outskip,
		  int outstride)
{
  int x, y, indiff, outdiff, intbias; 

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (int) ((*iptr * gain) + bias);
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (int) (*iptr * gain);
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to an integer for
         increased speed (will use integer addition).  */
      intbias = (int) bias;
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = *iptr + intbias;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      if(iptr != optr) transfer_int(xsize, ysize, 
				    iptr, inskip, instride, 
				    optr, outskip, outstride);
    }
  }
}



/*----------------------------------------------------------------------*/
void gainbias_long(double gain,
		   double bias,
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
  long intbias;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (long) ((*iptr * gain) + bias);
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (long) (*iptr * gain);
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias, so we can convert the bias to a long for
         increased speed (will use integer addition).  */
      intbias = (long) bias;
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = *iptr + intbias;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      if(iptr != optr) transfer_long(xsize, ysize, 
				     iptr, inskip, instride, 
				     optr, outskip, outstride);
    }
  }
}



/*----------------------------------------------------------------------*/
void gainbias_float(float gain,
		    float bias,
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

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (*iptr * gain) + bias;
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = *iptr * gain;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = *iptr + bias;
	  iptr += inskip;
	  optr += outskip;
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      if(iptr != optr) transfer_float(xsize, ysize, 
				      iptr, inskip, instride, 
				      optr, outskip, outstride);      
    }    
  }
}


/*----------------------------------------------------------------------*/
void gainbias_double(double gain,
		     double bias,
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

  if(gain != 1.0) {
    if(bias != 0.0) {
      /* do both gain and bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = (*iptr * gain) + bias;
	  iptr += inskip;
	  optr += outskip; 
	}
	iptr += indiff;
	optr += outdiff;
      }      
    }
    else {
      /* only do gain */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = *iptr * gain;
	  iptr += inskip;
	  optr += outskip; 	
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
  }
  else {			/* don't have to do gain (it's 1.0)  */
    if(bias != 0.0) {
      /* only do bias */
      for(y = 0; y < ysize; y++) {
	for(x = 0; x < xsize; x++) {
	  *optr = *iptr + bias;
	  iptr += inskip;
	  optr += outskip;
	}
	iptr += indiff;
	optr += outdiff;
      }
    }
    else {
      /* do nothing--just transfer */
      if(iptr != optr) transfer_double(xsize, ysize, 
				       iptr, inskip, instride, 
				       optr, outskip, outstride);      
    }    
  }
}
