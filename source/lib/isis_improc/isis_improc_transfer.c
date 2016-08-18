/* 
   
   Image processing library 
   
   by Stefan Agamanolis
   May 1995 
   
   transfer.c
   
   12/9/96  wad  Added transfer_short()

   */

#include <stdlib.h>
#include <memory.h>
#include "isis_improc_private.h"

/*----------------------------------------------------------------------*/
void transfer_byte(int xsize,
		   int ysize,
		   unsigned char *iptr,	     
		   int inskip,
		   int instride,
		   unsigned char *optr,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, outdiff;
  
  if(inskip == 1 && outskip == 1 && 
     instride == xsize && outstride == xsize && 
     iptr != optr) {
    memcpy(optr, iptr, xsize * ysize * sizeof(unsigned char));
    return;
  }

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
void transfer_int(int xsize,
		  int ysize,
		  int *iptr,
		  int inskip,
		  int instride,
		  int *optr,
		  int outskip,
		  int outstride)
{
  int x, y, indiff, outdiff;
  
  if(inskip == 1 && outskip == 1 && 
     instride == xsize && outstride == xsize && 
     iptr != optr) {
    memcpy(optr, iptr, xsize * ysize * sizeof(int));
    return;
  }

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
void transfer_short(int xsize,
		    int ysize,
		    short *iptr,	     
		    int inskip,
		    int instride,
		    short *optr,
		    int outskip,
		    int outstride)
{
  int x, y, indiff, outdiff;
  
  if(inskip == 1 && outskip == 1 && 
     instride == xsize && outstride == xsize && 
     iptr != optr) {
    memcpy(optr, iptr, xsize * ysize * sizeof(short));
    return;
  }

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
void transfer_long(int xsize,
		   int ysize,
		   long *iptr,	     
		   int inskip,
		   int instride,
		   long *optr,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, outdiff;
  
  if(inskip == 1 && outskip == 1 && 
     instride == xsize && outstride == xsize && 
     iptr != optr) {
    memcpy(optr, iptr, xsize * ysize * sizeof(long));
    return;
  }

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
void transfer_float(int xsize,
		    int ysize,
		    float *iptr,	     
		    int inskip,
		    int instride,
		    float *optr,
		    int outskip,
		    int outstride)
{
  int x, y, indiff, outdiff;
  
  if(inskip == 1 && outskip == 1 && 
     instride == xsize && outstride == xsize && 
     iptr != optr) {
    memcpy(optr, iptr, xsize * ysize * sizeof(float));
    return;
  }

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
void transfer_double(int xsize,
		     int ysize,
		     double *iptr,	     
		     int inskip,
		     int instride,
		     double *optr,
		     int outskip,
		     int outstride)
{
  int x, y, indiff, outdiff;
  
  if(inskip == 1 && outskip == 1 && 
     instride == xsize && outstride == xsize && 
     iptr != optr) {
    memcpy(optr, iptr, xsize * ysize * sizeof(double));
    return;
  }

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
void remap_intensity(int xsize,
		     int ysize,
		     unsigned char *iptr,	     
		     int inskip,
		     int instride,
		     unsigned char *optr,
		     int outskip,
		     int outstride,
		     unsigned char *remap)
{
  int x, y, indiff, outdiff;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = remap[*iptr];
      iptr += inskip;
      optr += outskip;
    }
    iptr += indiff;
    optr += outdiff;
  }
}


