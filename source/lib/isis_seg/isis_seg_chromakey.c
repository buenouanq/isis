/* Chromakey */
/* by Stefan Agamanolis, May 1995 */

#include <stdio.h>
#include <isis.h>
#include "isis_seg_private.h"

/* local data */

static unsigned char bg_min_ch0 = 255, bg_max_ch0 = 0; 
static unsigned char bg_min_ch1 = 255, bg_max_ch1 = 0; 
static unsigned char bg_min_chk = 255, bg_max_chk = 0;

/* ------------------------------------------------------------ */
Value *isis_init_chromakey(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  checkCount(4, NULL); 
  checkIntList(1, "size");
  checkAddrList(2, "bufs");
  checkIntList(3, "steps");
  
  init_chromakey(
		 trueValue(argv[0]),
		 getInt(Listitem(argv[1], 0)),
		 getInt(Listitem(argv[1], 1)),
		 getAddr(Listitem(argv[2], 0)),
		 getAddr(Listitem(argv[2], 1)),
		 getAddr(Listitem(argv[2], 2)),
		 getInt(Listitem(argv[3], 0)),
		 getInt(Listitem(argv[3], 1))
		 );
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *isis_chromakey(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  checkCount(9, NULL); 
  checkNumber(0, "maxbg");
  checkNumber(1, "minfg");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "insteps");
  checkAddrList(5, "outkeybuf");
  checkIntList(6, "outkeysteps");
  checkAddrList(7, "outalphabuf");
  checkIntList(8, "outalphasteps");
  
  chromakey(
	    getIntfromNumber(argv[0]),
	    getIntfromNumber(argv[1]),
	    getInt(Listitem(argv[2], 0)),
	    getInt(Listitem(argv[2], 1)),
	    getAddr(Listitem(argv[3], 0)),
	    getAddr(Listitem(argv[3], 1)),
	    getAddr(Listitem(argv[3], 2)),
	    getInt(Listitem(argv[4], 0)),
	    getInt(Listitem(argv[4], 1)),
	    getAddr(Listitem(argv[5], 0)),
	    getInt(Listitem(argv[6], 0)),
	    getInt(Listitem(argv[6], 1)),
	    getAddr(Listitem(argv[7], 0)),
	    getInt(Listitem(argv[8], 0)),
	    getInt(Listitem(argv[8], 1))
	    );
  
  return NullValue;
}

/*----------------------------------------------------------------------*/
void init_chromakey(int reset,
		    int xsize,
		    int ysize,
		    unsigned char *chan0, 
		    unsigned char *chan1, 
		    unsigned char *chank,
		    int skip,
		    int stride)
{
  int x, y, diff;

  diff = stride - (xsize * skip);

  if(reset) {
    bg_min_ch0 = bg_min_ch1 = bg_min_chk = 255;
    bg_max_ch0 = bg_max_ch1 = bg_max_chk = 0;
  }
  
  /* Find max and min of r g and b in all the blue */
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*chan0 > bg_max_ch0) bg_max_ch0 = *chan0;
      if(*chan1 > bg_max_ch1) bg_max_ch1 = *chan1;
      if(*chank > bg_max_chk) bg_max_chk = *chank;
      if(*chan0 < bg_min_ch0) bg_min_ch0 = *chan0;
      if(*chan1 < bg_min_ch1) bg_min_ch1 = *chan1;
      if(*chank < bg_min_chk) bg_min_chk = *chank;
      chan0 += skip;
      chan1 += skip;
      chank += skip;
    }
    chan0 += diff;
    chan1 += diff;
    chank += diff;
  }
  
}

/*----------------------------------------------------------------------*/
void chromakey(int maxbg,
	       int minfg,
	       int xsize,
	       int ysize,
	       unsigned char *inchan0, 
	       unsigned char *inchan1, 
	       unsigned char *inchank,
	       int inskip,
	       int instride,
	       unsigned char *outchank, 
	       int keyskip,
	       int keystride,
	       unsigned char *outalpha, 
	       int outskip,
	       int outstride)
{
  unsigned char *r, *g, *b, *a, *bp, n;
  int x, y, indiff, keydiff, outdiff;
  unsigned char bdiff, maxdiff, mindiff, range;
  
  indiff = instride - (xsize * inskip);
  keydiff = keystride - (xsize * keyskip);
  outdiff = outstride - (xsize * outskip);

  /* First calculate a binary alpha buffer.  It is foreground if it is
     within the maxes and mins of r g and b calculated in
     init_chromakey() */
  
  r = inchan0;
  g = inchan1;
  b = inchank;
  a = outalpha;

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*r <= bg_max_ch0 && *r >= bg_min_ch0 && *g <= bg_max_ch1 && 
	 *g >= bg_min_ch1 && *b <= bg_max_chk && *b >= bg_min_chk) *a = 0;
      else *a = 255;
      r += inskip;
      g += inskip;
      b += inskip;
      a += outskip;
    }
    r += indiff;
    g += indiff;
    b += indiff;
    a += outdiff;
  }

  /* Now cut blue down to max of r and g. */

  r = inchan0;
  g = inchan1;
  b = inchank;
  bp = outchank;

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      n = ((*r > *g) ? *r : *g);
      *bp = ((n < *b) ? n : *b);
      r += inskip;
      g += inskip;
      b += inskip;
      bp += keyskip;
    }
    r += indiff;
    g += indiff;
    b += indiff;
    bp += keydiff;
  }
  
  /* Now find the max and and min difference between the old and new
     blue.  The min difference will usually be 0.  The range of these
     differences will determine how the variable alpha channel is made */
  
  maxdiff = 0;
  mindiff = 255;

  b = inchank;
  bp = outchank;
  a = outalpha;

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*a) {
	bdiff = *b - *bp;
	if(bdiff > maxdiff) maxdiff = bdiff;
	if(bdiff < mindiff) mindiff = bdiff;
      }
      b += inskip;
      bp += keyskip;
      a += outskip;
    }
    b += indiff;
    bp += keydiff;
    a += outdiff;
  }
  range = maxdiff - mindiff;
  
  /* Now calculate the variable alpha buffer */
  
  b = inchank;
  bp = outchank;
  a = outalpha;

  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      if(*a) {
	*a = 255 - ((255 * (*b - *bp)) / range);
	if(*a >= minfg) *a = 255;
	if(*a <= maxbg) *a = 0;
      }
      b += inskip;
      bp += keyskip;
      a += outskip;
    }
    b += indiff;
    bp += keydiff;
    a += outdiff;
  }
  
}
