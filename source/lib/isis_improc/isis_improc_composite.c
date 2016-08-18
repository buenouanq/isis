/* 
   
   Image compositing tool 
   
   by Stefan Agamanolis
   May 1995 
   
   composite.c
   
   */

#include <stdlib.h>
#include "isis_improc_private.h"

static unsigned char **comptable = NULL;

/*----------------------------------------------------------------------*/
void initialize_composite()
{
  int i, a;
  
  comptable = (unsigned char **) malloc(256 * sizeof(unsigned char *));
  for(i = 0; i < 256; i++) 
    comptable[i] = (unsigned char *) malloc(256 * sizeof(unsigned char));
  
  for(i = 0; i < 256; i++) for(a = 0; a < 256; a++) 
    comptable[i][a] = (unsigned char) ((((double) a) / 255.0) * ((double) i));

}


/*----------------------------------------------------------------------*/
void composite_1ch(int xsize, 
		   int ysize,
		   unsigned char *iptr0, 
		   int inskip,
		   int instride,
		   unsigned char *iptra, 
		   int askip,
		   int astride,
		   unsigned char *optr0,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, adiff, outdiff;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*iptra) {
	if(*iptra == 255) *optr0 = *iptr0;
	else *optr0 = comptable[*iptr0][*iptra] +
	       comptable[*optr0][255 - *iptra];
      }
      iptr0 += inskip;
      iptra += askip;
      optr0 += outskip;
    }
    iptr0 += indiff;
    iptra += adiff;
    optr0 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void composite_2ch(int xsize, 
		   int ysize,
		   unsigned char *iptr0, 
		   unsigned char *iptr1, 
		   int inskip,
		   int instride,
		   unsigned char *iptra, 
		   int askip,
		   int astride,
		   unsigned char *optr0,
		   unsigned char *optr1,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, adiff, outdiff;
  int alpha, opalpha;

  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(alpha = *iptra) {
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
	  *optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
	}
	else {
	  *optr0 = *iptr0;
	  *optr1 = *iptr1;
	}
      }
      iptr0 += inskip; iptr1 += inskip;
      iptra += askip;
      optr0 += outskip; optr1 += outskip;
    }
    
    iptr0 += indiff; iptr1 += indiff;
    iptra += adiff;
    optr0 += outdiff; optr1 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void composite_3ch(int xsize, 
		   int ysize,
		   unsigned char *iptr0, 
		   unsigned char *iptr1, 
		   unsigned char *iptr2, 
		   int inskip,
		   int instride,
		   unsigned char *iptra, 
		   int askip,
		   int astride,
		   unsigned char *optr0,
		   unsigned char *optr1,
		   unsigned char *optr2,
		   int outskip,
		   int outstride)
{
  int x, y, indiff, adiff, outdiff;
  int alpha, opalpha;

  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(alpha = *iptra) {
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
	  *optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
	  *optr2 = comptable[*iptr2][alpha] + comptable[*optr2][opalpha];
	}
	else {
	  *optr0 = *iptr0;
	  *optr1 = *iptr1;
	  *optr2 = *iptr2;
	}
      }
      iptr0 += inskip; iptr1 += inskip; iptr2 += inskip;
      iptra += askip;
      optr0 += outskip; optr1 += outskip; optr2 += outskip;
    }
    
    iptr0 += indiff; iptr1 += indiff; iptr2 += indiff;
    iptra += adiff;
    optr0 += outdiff; optr1 += outdiff; optr2 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_composite_byte_1ch(int inxsize,
			      int inysize,
			      unsigned char *iptr0,	     
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      int outxsize,
			      int outysize,
			      unsigned char *optr0,
			      int outskip,
			      int outstride, 
			      int *xremap,
			      int *yremap)
{
  int x, y, indiff, adiff, outdiff, xoff, yoff;
  unsigned char *iline0, *ilinea;
  int alpha, opalpha;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  adiff = astride - (inxsize * askip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    ilinea = iptra + (yremap[y] * astride);
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    for(x = 0; x < outxsize; x++) {
      if(alpha = *(ilinea + (xremap[x] * askip))) {
	xoff = xremap[x] * inskip;
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*(iline0 + xoff)][alpha] + 
	    comptable[*optr0][opalpha];
	}
	else {
	  *optr0 = *(iline0 + xoff);
	}
      }
      optr0 += outskip;
    }
    optr0 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_composite_byte_2ch(int inxsize,
			      int inysize,
			      unsigned char *iptr0,	     
			      unsigned char *iptr1,	     
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      int outxsize,
			      int outysize,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      int outskip,
			      int outstride, 
			      int *xremap,
			      int *yremap)
{
  int x, y, indiff, adiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, *ilinea;
  int alpha, opalpha;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  adiff = astride - (inxsize * askip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    ilinea = iptra + (yremap[y] * astride);
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    for(x = 0; x < outxsize; x++) {
      if(alpha = *(ilinea + (xremap[x] * askip))) {
	xoff = xremap[x] * inskip;
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*(iline0 + xoff)][alpha] + 
	    comptable[*optr0][opalpha];
	  *optr1 = comptable[*(iline1 + xoff)][alpha] + 
	    comptable[*optr1][opalpha];
	}
	else {
	  *optr0 = *(iline0 + xoff);
	  *optr1 = *(iline1 + xoff);
	}
      }
      optr0 += outskip;
      optr1 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_composite_byte_3ch(int inxsize,
			      int inysize,
			      unsigned char *iptr0,	     
			      unsigned char *iptr1,	     
			      unsigned char *iptr2,	     
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
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
  int x, y, indiff, adiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, *iline2, *ilinea;
  int alpha, opalpha;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  adiff = astride - (inxsize * askip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    ilinea = iptra + (yremap[y] * astride);
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    iline2 = iptr2 + yoff;
    for(x = 0; x < outxsize; x++) {
      if(alpha = *(ilinea + (xremap[x] * askip))) {
	xoff = xremap[x] * inskip;
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*(iline0 + xoff)][alpha] + 
	    comptable[*optr0][opalpha];
	  *optr1 = comptable[*(iline1 + xoff)][alpha] + 
	    comptable[*optr1][opalpha];
	  *optr2 = comptable[*(iline2 + xoff)][alpha] + 
	    comptable[*optr2][opalpha];
	}
	else {
	  *optr0 = *(iline0 + xoff);
	  *optr1 = *(iline1 + xoff);
	  *optr2 = *(iline2 + xoff);
	}
      }
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
void composite_remapalpha_1ch(unsigned char *remapalpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      unsigned char *optr0,
			      int outskip,
			      int outstride)
{
  int x, y, indiff, adiff, outdiff;
  int alpha, opalpha;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(alpha = remapalpha[*iptra]) {
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
	}
	else {
	  *optr0 = *iptr0;
	}
      }
      iptr0 += inskip;
      iptra += askip;
      optr0 += outskip;
    }
    iptr0 += indiff;
    iptra += adiff;
    optr0 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void composite_remapalpha_2ch(unsigned char *remapalpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      unsigned char *iptr1, 
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      int outskip,
			      int outstride)
{
  int x, y, indiff, adiff, outdiff;
  int alpha, opalpha;

  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(alpha = remapalpha[*iptra]) {
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
	  *optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
	}
	else {
	  *optr0 = *iptr0;
	  *optr1 = *iptr1;
	}
      }
      iptr0 += inskip; iptr1 += inskip;
      iptra += askip;
      optr0 += outskip; optr1 += outskip;
    }
    
    iptr0 += indiff; iptr1 += indiff;
    iptra += adiff;
    optr0 += outdiff; optr1 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void composite_remapalpha_3ch(unsigned char *remapalpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      unsigned char *iptr1, 
			      unsigned char *iptr2, 
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      unsigned char *optr2,
			      int outskip,
			      int outstride)
{
  int x, y, indiff, adiff, outdiff;
  int alpha, opalpha;

  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(alpha = remapalpha[*iptra]) {
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
	  *optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
	  *optr2 = comptable[*iptr2][alpha] + comptable[*optr2][opalpha];
	}
	else {
	  *optr0 = *iptr0;
	  *optr1 = *iptr1;
	  *optr2 = *iptr2;
	}
      }
      iptr0 += inskip; iptr1 += inskip; iptr2 += inskip;
      iptra += askip;
      optr0 += outskip; optr1 += outskip; optr2 += outskip;
    }
    
    iptr0 += indiff; iptr1 += indiff; iptr2 += indiff;
    iptra += adiff;
    optr0 += outdiff; optr1 += outdiff; optr2 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_composite_remapalpha_byte_1ch(unsigned char *remapalpha,
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 int inskip,
					 int instride,
					 unsigned char *iptra, 
					 int askip,
					 int astride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap)
{
  int x, y, indiff, adiff, outdiff, xoff, yoff;
  unsigned char *iline0, *ilinea;
  int alpha, opalpha;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  adiff = astride - (inxsize * askip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    ilinea = iptra + (yremap[y] * astride);
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    for(x = 0; x < outxsize; x++) {
      if(alpha = remapalpha[*(ilinea + (xremap[x] * askip))]) {
	xoff = xremap[x] * inskip;
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*(iline0 + xoff)][alpha] + 
	    comptable[*optr0][opalpha];
	}
	else {
	  *optr0 = *(iline0 + xoff);
	}
      }
      optr0 += outskip;
    }
    optr0 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_composite_remapalpha_byte_2ch(unsigned char *remapalpha,
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 unsigned char *iptr1,	     
					 int inskip,
					 int instride,
					 unsigned char *iptra, 
					 int askip,
					 int astride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 unsigned char *optr1,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap)
{
  int x, y, indiff, adiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, *ilinea;
  int alpha, opalpha;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  adiff = astride - (inxsize * askip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    ilinea = iptra + (yremap[y] * astride);
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    for(x = 0; x < outxsize; x++) {
      if(alpha = remapalpha[*(ilinea + (xremap[x] * askip))]) {
	xoff = xremap[x] * inskip;
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*(iline0 + xoff)][alpha] + 
	    comptable[*optr0][opalpha];
	  *optr1 = comptable[*(iline1 + xoff)][alpha] + 
	    comptable[*optr1][opalpha];
	}
	else {
	  *optr0 = *(iline0 + xoff);
	  *optr1 = *(iline1 + xoff);
	}
      }
      optr0 += outskip;
      optr1 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_composite_remapalpha_byte_3ch(unsigned char *remapalpha,
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 unsigned char *iptr1,	     
					 unsigned char *iptr2,	     
					 int inskip,
					 int instride,
					 unsigned char *iptra, 
					 int askip,
					 int astride,
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
  int x, y, indiff, adiff, outdiff, xoff, yoff;
  unsigned char *iline0, *iline1, *iline2, *ilinea;
  int alpha, opalpha;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  adiff = astride - (inxsize * askip);
  outdiff = outstride - (outxsize * outskip);
  
  for(y = 0; y < outysize; y++) {
    ilinea = iptra + (yremap[y] * astride);
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    iline2 = iptr2 + yoff;
    for(x = 0; x < outxsize; x++) {
      if(alpha = remapalpha[*(ilinea + (xremap[x] * askip))]) {
	xoff = xremap[x] * inskip;
	if(opalpha = 255 - alpha) {
	  *optr0 = comptable[*(iline0 + xoff)][alpha] + 
	    comptable[*optr0][opalpha];
	  *optr1 = comptable[*(iline1 + xoff)][alpha] +
	    comptable[*optr1][opalpha];
	  *optr2 = comptable[*(iline2 + xoff)][alpha] + 
	    comptable[*optr2][opalpha];
	}
	else {
	  *optr0 = *(iline0 + xoff);
	  *optr1 = *(iline1 + xoff);
	  *optr2 = *(iline2 + xoff);
	}
      }
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
void composite_constalpha_1ch(int alpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      int inskip,
			      int instride,
			      unsigned char *optr0,
			      int outskip,
			      int outstride)
{
  int x, y, indiff, outdiff;
  int opalpha;

  if(!alpha) return;
  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  opalpha = 255 - alpha;

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
      iptr0 += inskip;
      optr0 += outskip;
    }
    iptr0 += indiff;
    optr0 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void composite_constalpha_2ch(int alpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      unsigned char *iptr1, 
			      int inskip,
			      int instride,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      int outskip,
			      int outstride)
{
  int x, y, indiff, outdiff;
  int opalpha;

  if(!alpha) return;
  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  opalpha = 255 - alpha;            

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
      *optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
      
      iptr0 += inskip; iptr1 += inskip; 
      optr0 += outskip; optr1 += outskip;
    }
    
    iptr0 += indiff; iptr1 += indiff; 
    optr0 += outdiff; optr1 += outdiff; 
  }
}

/*----------------------------------------------------------------------*/
void composite_constalpha_3ch(int alpha,
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
  int opalpha;

  if(!alpha) return;
  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  opalpha = 255 - alpha;            

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
      *optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
      *optr2 = comptable[*iptr2][alpha] + comptable[*optr2][opalpha];
      
      iptr0 += inskip; iptr1 += inskip; iptr2 += inskip;
      optr0 += outskip; optr1 += outskip; optr2 += outskip;
    }
    
    iptr0 += indiff; iptr1 += indiff; iptr2 += indiff;
    optr0 += outdiff; optr1 += outdiff; optr2 += outdiff;
  }
}

/*----------------------------------------------------------------------*/
void scale_composite_constalpha_byte_1ch(int alpha, 
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
  int x, y, indiff, outdiff, xoff, yoff;
  unsigned char *iline0;
  int opalpha;
  
  if(!alpha) return;
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  opalpha = 255 - alpha;            
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      *optr0 = comptable[*(iline0 + xoff)][alpha] + comptable[*optr0][opalpha];
      optr0 += outskip;
    }
    optr0 += outdiff;
  }
}


/*----------------------------------------------------------------------*/
void scale_composite_constalpha_byte_2ch(int alpha, 
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
  unsigned char *iline0, *iline1;
  int opalpha;
  
  if(!alpha) return;
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  opalpha = 255 - alpha;            
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      *optr0 = comptable[*(iline0 + xoff)][alpha] + comptable[*optr0][opalpha];
      *optr1 = comptable[*(iline1 + xoff)][alpha] + comptable[*optr1][opalpha];
      optr0 += outskip;
      optr1 += outskip;
    }
    optr0 += outdiff;
    optr1 += outdiff;
  }
}


/*----------------------------------------------------------------------*/
void scale_composite_constalpha_byte_3ch(int alpha, 
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
  unsigned char *iline0, *iline1, *iline2;
  int opalpha;
  
  if(!alpha) return;
  if(comptable == NULL) initialize_composite();

  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  opalpha = 255 - alpha;            
  
  for(y = 0; y < outysize; y++) {
    yoff = yremap[y] * instride;
    iline0 = iptr0 + yoff;
    iline1 = iptr1 + yoff;
    iline2 = iptr2 + yoff;
    for(x = 0; x < outxsize; x++) {
      xoff = xremap[x] * inskip;
      *optr0 = comptable[*(iline0 + xoff)][alpha] + comptable[*optr0][opalpha];
      *optr1 = comptable[*(iline1 + xoff)][alpha] + comptable[*optr1][opalpha];
      *optr2 = comptable[*(iline2 + xoff)][alpha] + comptable[*optr2][opalpha];
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
void composite_layer_1ch(int indepth,
			 int alpha_cutoff,
			 int xsize, 
			 int ysize,
			 unsigned char *iptr0, 
			 int inskip,
			 int instride,
			 unsigned char *iptra, 
			 int askip,
			 int astride,
			 unsigned char *optr0,
			 int outskip,
			 int outstride,
			 int *optrz,
			 int zskip,
			 int zstride)
{
  int x, y, indiff, adiff, outdiff, zdiff;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);
  zdiff = zstride - (xsize * zskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= indepth) {
	if(*iptra >= alpha_cutoff) *optrz = indepth;
	*optr0 = comptable[*iptr0][*iptra] + comptable[*optr0][255 - *iptra];
      }
      iptr0 += inskip;
      iptra += askip;
      optr0 += outskip;
      optrz += zskip;
    }
    
    iptr0 += indiff;
    iptra += adiff;
    optr0 += outdiff;
    optrz += zdiff;
  }
}

/*----------------------------------------------------------------------*/
void composite_layer_3ch(int indepth,
			 int alpha_cutoff,
			 int xsize, 
			 int ysize,
			 unsigned char *iptr0, 
			 unsigned char *iptr1, 
			 unsigned char *iptr2, 
			 int inskip,
			 int instride,
			 unsigned char *iptra, 
			 int askip,
			 int astride,
			 unsigned char *optr0,
			 unsigned char *optr1,
			 unsigned char *optr2,
			 int outskip,
			 int outstride,
			 int *optrz,
			 int zskip,
			 int zstride)
{
  int x, y, indiff, adiff, outdiff, zdiff;
  int alpha, opalpha;

  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  outdiff = outstride - (xsize * outskip);
  zdiff = zstride - (xsize * zskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= indepth) {
	if(*iptra >= alpha_cutoff) *optrz = indepth;
	alpha = *iptra;
	opalpha = 255 - alpha;
	*optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
	*optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
	*optr2 = comptable[*iptr2][alpha] + comptable[*optr2][opalpha];
      }
      iptr0 += inskip; iptr1 += inskip; iptr2 += inskip;
      iptra += askip;
      optr0 += outskip; optr1 += outskip; optr2 += outskip;
      optrz += zskip;
    }
    
    iptr0 += indiff; iptr1 += indiff; iptr2 += indiff;
    iptra += adiff;
    optr0 += outdiff; optr1 += outdiff; optr2 += outdiff;
    optrz += zdiff;
  }
}


/*----------------------------------------------------------------------*/
void composite_zbuffer_1ch(int zoffset,
			   int alpha_cutoff,
			   int xsize, 
			   int ysize,
			   unsigned char *iptr0, 
			   int inskip,
			   int instride,
			   unsigned char *iptra, 
			   int askip,
			   int astride,
			   int *iptrz,
			   int inzskip,
			   int inzstride,
			   unsigned char *optr0,
			   int outskip,
			   int outstride,
			   int *optrz,
			   int outzskip,
			   int outzstride)
{
  int x, y, indiff, adiff, outdiff, inzdiff, outzdiff;
  
  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  inzdiff = inzstride - (xsize * inzskip);
  outdiff = outstride - (xsize * outskip);
  outzdiff = outzstride - (xsize * outzskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= (zoffset + *iptrz)) {
	if(*iptra >= alpha_cutoff) *optrz = (zoffset + *iptrz);
	*optr0 = comptable[*iptr0][*iptra] + comptable[*optr0][255 - *iptra];
      }
      iptr0 += inskip;
      iptra += askip; 
      iptrz += inzskip;
      optr0 += outskip;
      optrz += outzskip;
    }
    
    iptr0 += indiff;
    iptra += adiff;
    iptrz += inzdiff;
    optr0 += outdiff;
    optrz += outzdiff;
  }
}


/*----------------------------------------------------------------------*/
void composite_zbuffer_3ch(int zoffset,
			   int alpha_cutoff,
			   int xsize, 
			   int ysize,
			   unsigned char *iptr0, 
			   unsigned char *iptr1, 
			   unsigned char *iptr2, 
			   int inskip,
			   int instride,
			   unsigned char *iptra, 
			   int askip,
			   int astride,
			   int *iptrz,
			   int inzskip,
			   int inzstride,
			   unsigned char *optr0,
			   unsigned char *optr1,
			   unsigned char *optr2,
			   int outskip,
			   int outstride,
			   int *optrz,
			   int outzskip,
			   int outzstride)
{
  int x, y, indiff, adiff, outdiff, inzdiff, outzdiff;
  int alpha, opalpha;

  if(comptable == NULL) initialize_composite();

  indiff = instride - (xsize * inskip);
  adiff = astride - (xsize * askip);
  inzdiff = inzstride - (xsize * inzskip);
  outdiff = outstride - (xsize * outskip);
  outzdiff = outzstride - (xsize * outzskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= (zoffset + *iptrz)) {
	if(*iptra >= alpha_cutoff) *optrz = (zoffset + *iptrz);
	alpha = *iptra;
	opalpha = 255 - alpha;
	*optr0 = comptable[*iptr0][alpha] + comptable[*optr0][opalpha];
	*optr1 = comptable[*iptr1][alpha] + comptable[*optr1][opalpha];
	*optr2 = comptable[*iptr2][alpha] + comptable[*optr2][opalpha];
      }
      iptr0 += inskip; iptr1 += inskip; iptr2 += inskip;
      iptra += askip;
      iptrz += inzskip;
      optr0 += outskip; optr1 += outskip; optr2 += outskip;
      optrz += outzskip;
    }
    
    iptr0 += indiff; iptr1 += indiff; iptr2 += indiff;
    iptra += adiff;
    iptrz += inzdiff;
    optr0 += outdiff; optr1 += outdiff; optr2 += outdiff;
    optrz += outzdiff;
  }
}


/*----------------------------------------------------------------------*/
void zbuffer_1ch(int zoffset,
		 int xsize, 
		 int ysize,
		 unsigned char *iptr0, 
		 int inskip,
		 int instride,
		 int *iptrz,
		 int inzskip,
		 int inzstride,
		 unsigned char *optr0,
		 int outskip,
		 int outstride,
		 int *optrz,
		 int outzskip,
		 int outzstride)
{
  int x, y, indiff, outdiff, inzdiff, outzdiff;
  
  indiff = instride - (xsize * inskip);
  inzdiff = inzstride - (xsize * inzskip);
  outdiff = outstride - (xsize * outskip);
  outzdiff = outzstride - (xsize * outzskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= (zoffset + *iptrz)) {
	*optrz = (zoffset + *iptrz);
	*optr0 = *iptr0;
      }
      iptr0 += inskip;
      iptrz += inzskip;
      optr0 += outskip;
      optrz += outzskip;
    }
    
    iptr0 += indiff;
    iptrz += inzdiff;
    optr0 += outdiff;
    optrz += outzdiff;
  }
}

/*----------------------------------------------------------------------*/
void zbuffer_3ch(int zoffset,
		 int xsize, 
		 int ysize,
		 unsigned char *iptr0, 
		 unsigned char *iptr1, 
		 unsigned char *iptr2, 
		 int inskip,
		 int instride,
		 int *iptrz,
		 int inzskip,
		 int inzstride,
		 unsigned char *optr0,
		 unsigned char *optr1,
		 unsigned char *optr2,
		 int outskip,
		 int outstride,
		 int *optrz,
		 int outzskip,
		 int outzstride)
{
  int x, y, indiff, outdiff, inzdiff, outzdiff;

  indiff = instride - (xsize * inskip);
  inzdiff = inzstride - (xsize * inzskip);
  outdiff = outstride - (xsize * outskip);
  outzdiff = outzstride - (xsize * outzskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= (zoffset + *iptrz)) {
	*optrz = (zoffset + *iptrz);
	*optr0 = *iptr0;
	*optr1 = *iptr1;
	*optr2 = *iptr2;
      }
      iptr0 += inskip; iptr1 += inskip; iptr2 += inskip;
      iptrz += inzskip;
      optr0 += outskip; optr1 += outskip; optr2 += outskip;
      optrz += outzskip;
    }
    
    iptr0 += indiff; iptr1 += indiff; iptr2 += indiff;
    iptrz += inzdiff;
    optr0 += outdiff; optr1 += outdiff; optr2 += outdiff;
    optrz += outzdiff;
  }
}


/*----------------------------------------------------------------------*/
void layer_1ch(int indepth,
	       int xsize, 
	       int ysize,
	       unsigned char *iptr0, 
	       int inskip,
	       int instride,
	       unsigned char *optr0,
	       int outskip,
	       int outstride,
	       int *optrz,
	       int outzskip,
	       int outzstride)
{
  int x, y, indiff, outdiff, outzdiff;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  outzdiff = outzstride - (xsize * outzskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= indepth) {
	*optrz = indepth;
	*optr0 = *iptr0;
      }
      iptr0 += inskip;
      optr0 += outskip;
      optrz += outzskip;
    }
    
    iptr0 += indiff;
    optr0 += outdiff;
    optrz += outzdiff;
  }
}

/*----------------------------------------------------------------------*/
void layer_3ch(int indepth,
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
	       int outstride,
	       int *optrz,
	       int outzskip,
	       int outzstride)
{
  int x, y, indiff, outdiff, outzdiff;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  outzdiff = outzstride - (xsize * outzskip);

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*optrz <= indepth) {
	*optrz = indepth;
	*optr0 = *iptr0;
	*optr1 = *iptr1;
	*optr2 = *iptr2;
      }
      iptr0 += inskip; iptr1 += inskip; iptr2 += inskip;
      optr0 += outskip; optr1 += outskip; optr2 += outskip;
      optrz += outzskip;

    }
    
    iptr0 += indiff; iptr1 += indiff; iptr2 += indiff;
    optr0 += outdiff; optr1 += outdiff; optr2 += outdiff;
    optrz += outzdiff;
  }
}
