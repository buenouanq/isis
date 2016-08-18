/* Miscellaneous image statistics,
   Stefan Agamanolis 5/98 */

#include <stdio.h>                /* Must include before isis.h */
#include <isis.h>                 /* Holds all Isis interpreter info */

#include "isis_stats_private.h"

/* ------------------------------------------------------------ */
Value *image_stats_raw_centroid(Script *script, 
				char *proc_name, int call_data, 
				int argc, Value **argv)
{
  int i, chans, x, y;
  unsigned char low, high;
  int xsum, ysum, totcount;
  int xsize, ysize, xstep, ystep, diff;
  unsigned char *iptr;
  
  checkCount(5, NULL); 
  checkNumber(0, "lowest intensity");
  checkNumber(1, "highest intensity");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "steps");

  low = getIntfromNumber(argv[0]);
  high = getIntfromNumber(argv[1]);
  xsize = getInt(Listitem(argv[2], 0));
  ysize = getInt(Listitem(argv[2], 1));
  xstep = getInt(Listitem(argv[4], 0));
  ystep = getInt(Listitem(argv[4], 1));
  diff = ystep - (xsize * xstep);
  
  chans = Listsize(argv[3]);
  xsum = ysum = totcount = 0;
  
  for(i = 0; i < chans; i++) {
    iptr = getAddr(Listitem(argv[3], i));
    for(y = 0; y < ysize; y++, iptr += diff) {
      for(x = 0; x < xsize; x++, iptr += xstep) {
	if(*iptr >= low && *iptr <= high) {
	  totcount++;  xsum += x;  ysum += y;
	}
      }
    }
  }
  
  if(totcount) return newIntListva(2, (xsum / totcount), (ysum / totcount));
  return NullValue;
}
