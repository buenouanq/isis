/* 
   
   Image processing library 
   
   by Stefan Agamanolis
   other code fragments by Shawn Becker (from datscale.c)
   July 1995 
   
   scale.c
   
*/

#include <stdlib.h>
#include "isis_improc_private.h"

#define roundcast(x) ((int) ((x > 0.0) ? (x + 0.5) : (x - 0.5)))

/*----------------------------------------------------------------------*/
void scale_byte_1ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap)
{
  int x, y, indiff, outdiff;
  unsigned char *iline0;

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    iline0 = iptr0 + (yremap[y] * instride);
    for(x = 0; x < outxsize; x++) {
      *optr0 = *(iline0 + (xremap[x] * inskip));
      optr0 += outskip;
    }
    optr0 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_byte_2ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    unsigned char *iptr1,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap)
{
  int x, y, indiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1;

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      *optr0 = *(iline0 + xoff);
      *optr1 = *(iline1 + xoff);
      optr0 += outskip;
      optr1 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_byte_3ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    unsigned char *iptr1,	     
		    unsigned char *iptr2,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    unsigned char *optr2,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap)
{
  int x, y, indiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, *iline2;

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    iline2 = iptr2 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      *optr0 = *(iline0 + xoff);
      *optr1 = *(iline1 + xoff);
      *optr2 = *(iline2 + xoff);
      optr0 += outskip;
      optr1 += outskip;
      optr2 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_byte_4ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    unsigned char *iptr1,	     
		    unsigned char *iptr2,	     
		    unsigned char *iptr3,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    unsigned char *optr2,
		    unsigned char *optr3,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap)
{
  int x, y, indiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, *iline2, *iline3;

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    iline2 = iptr2 + yoff;
    iline3 = iptr3 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      *optr0 = *(iline0 + xoff);
      *optr1 = *(iline1 + xoff);
      *optr2 = *(iline2 + xoff);
      *optr3 = *(iline3 + xoff);
      optr0 += outskip;
      optr1 += outskip;
      optr2 += outskip;
      optr3 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
    optr3 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_range_byte_1ch(unsigned char low,
			  unsigned char high,
			  int inxsize,
			  int inysize,
			  unsigned char *iptr0,	     
			  int inskip,
			  int instride,
			  int outxsize,
			  int outysize,
			  unsigned char *optr0,
			  int outskip,
			  int outstride, 
			  int *xremap,
			  int *yremap)
{
  int x, y, indiff, outdiff;
  unsigned char *iline0, pix;

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    iline0 = iptr0 + (yremap[y] * instride);
    for(x = 0; x < outxsize; x++) {
      pix = *(iline0 + (xremap[x] * inskip));
      if(pix >= low && pix <= high) *optr0 = pix;
      optr0 += outskip;
    }
    optr0 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_range_byte_2ch(unsigned char low,
			  unsigned char high,
			  int inxsize,
			  int inysize,
			  unsigned char *iptr0,	     
			  unsigned char *iptr1,	     
			  int inskip,
			  int instride,
			  int outxsize,
			  int outysize,
			  unsigned char *optr0,
			  unsigned char *optr1,
			  int outskip,
			  int outstride, 
			  int *xremap,
			  int *yremap)
{
  int x, y, indiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, pix;

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      pix = *(iline0 + xoff);
      if(pix >= low && pix <= high) *optr0 = pix;
      pix = *(iline1 + xoff);
      if(pix >= low && pix <= high) *optr1 = pix;
      optr0 += outskip;
      optr1 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_range_byte_3ch(unsigned char low,
			  unsigned char high,
			  int inxsize,
			  int inysize,
			  unsigned char *iptr0,	     
			  unsigned char *iptr1,	     
			  unsigned char *iptr2,	     
			  int inskip,
			  int instride,
			  int outxsize,
			  int outysize,
			  unsigned char *optr0,
			  unsigned char *optr1,
			  unsigned char *optr2,
			  int outskip,
			  int outstride, 
			  int *xremap,
			  int *yremap)
{
  int x, y, indiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, *iline2, pix;

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    iline2 = iptr2 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      pix = *(iline0 + xoff);
      if(pix >= low && pix <= high) *optr0 = pix;
      pix = *(iline1 + xoff);
      if(pix >= low && pix <= high) *optr1 = pix;
      pix = *(iline2 + xoff);
      if(pix >= low && pix <= high) *optr2 = pix;
      optr0 += outskip;
      optr1 += outskip;
      optr2 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
    optr2 += outdiff;
  }
}

