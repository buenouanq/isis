/*  filters.c

    Written by ? (Shawn Becker and/or Stefan Agamanolis, most likely)

    Modified:
    12/17/96  wad   Added filter_1d_short.
*/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>  /* for SHRT_MAX, SHRT_MIN, UCHAR_MAX, etc. */
#include "isis_improc_private.h"

/* the real type for these functions is double */

/* Applies a filter in 1d 

   To change from a horizontal to vertical filter, switch around
   xsize/ysize, inskip/instride, outskip/outstride.

   Multiplies every series on a line by the values in the taps
   and places the result in the center.

   The positioning is done by the start of the taps in order to
   simplify addressing. Relative to the start of the image, it
   can be divided into three parts:

         (-centertap to 0) : left edge 
	 (0 to end-numberoftaps): main body
	 (end-numberoftaps to end): right edge

   The code will check for under/over-address, and if so, either pass a 0 
   or a mirrored value depending on the mirror parameter. */

/* really should be called filter_1d_byte */
int filter_1d(int numberoftaps,
	      double * taps,
	      int centertap,
	      int mirror,
	      int xsize,
	      int ysize,
	      unsigned char * inbuf,
	      int inskip,
	      int instride,
	      unsigned char * outbuf,
	      int outskip,
	      int outstride)
{

  int line;
  unsigned char *istart, *ostart, *iptr, *optr, *iend, *lineiend;
  double *endtap, *ctap, tot;
  

  istart = inbuf;
  ostart = outbuf;
  endtap = taps + numberoftaps;
  
  /******* Line-looping start *******/

/*  printf("Going now xsize=%d ysize=%d.\n",xsize,ysize); */

  /* loop for each line in the image */
  for (line = 0; line < ysize; line++) {
    
    /* iend is the end of this line's buffer space */
    iend = istart + xsize*inskip;
     
    /* first do the portion at the right edge */
    iptr = istart - centertap*inskip;
    optr = ostart - centertap*outskip;

    lineiend = iend-(inskip*(numberoftaps-centertap)); 

    while (iptr < lineiend )  {
      
      /* set the total to zero, and the current tap to the first tap */
      tot = 0;    ctap = taps;
      
      /* add up points multiplied, depending on mirror/non-mirror */
      if (mirror)
	/* mirrored out version */
	while (ctap < endtap) {
	  tot += ( *ctap++ * (double)
		  *( (iptr<istart) ? ((istart-iptr)+istart) : 
		    ((iptr>=iend)  ? ((iend-(inskip*2)-iptr)+iend) : iptr )));
	  iptr+=inskip;
      } else
	/* zeroed version */
	while (ctap < endtap) {
	  tot += ( *ctap++ * (double)
		  ( ((iptr<istart)||(iptr>=iend)) ? 0 :  *iptr ));
	  iptr+=inskip;
	}
      
      /*  the center of the output pointer is the new value */	 
      *(optr + centertap*outskip) = (unsigned char)(tot>255?255:(tot<0?0:tot));
      optr+=outskip;
      /* set the new input pointer one more than it was before */
      iptr -= (numberoftaps-1)*inskip;
      
    }
    

    /* next line */
    istart += instride;
    ostart += outstride;
  }
  
  return(1);			/* we're done */
}


int filter_1d_short(int numberoftaps,
		    double * taps,
		    int centertap,
		    int mirror,
		    int xsize,
		    int ysize,
		    short * inbuf,
		    int inskip,
		    int instride,
		    short * outbuf,
		    int outskip,
		    int outstride)
{

  int line;
  short *istart, *ostart, *iptr, *optr, *iend, *lineiend;
  double *endtap, *ctap, tot;

  istart = inbuf;
  ostart = outbuf;
  endtap = taps + numberoftaps;
  
  /******* Line-looping start *******/

  /* loop for each line in the image */
  for (line = 0; line < ysize; line++) {
    
    /* iend is the end of this line's buffer space */
    iend = istart + xsize*inskip;
     
    /* first do the portion at the right edge */
    iptr = istart - centertap*inskip;
    optr = ostart - centertap*outskip;

    lineiend = iend-(inskip*(numberoftaps-centertap)); 

    while (iptr < lineiend )  {
      
      /* set the total to zero, and the current tap to the first tap */
      tot = 0;    ctap = taps;
      
      /* add up points multiplied, depending on mirror/non-mirror */
      if (mirror)
	/* mirrored out version */
	while (ctap < endtap) {
	  tot += ( *ctap++ * (double)
		  *( (iptr<istart) ? ((istart-iptr)+istart) : 
		    ((iptr>=iend)  ? ((iend-(inskip*2)-iptr)+iend) : iptr )));
	  iptr+=inskip;
      } else
	/* zeroed version */
	while (ctap < endtap) {
	  tot += ( *ctap++ * (double)
		  ( ((iptr<istart)||(iptr>=iend)) ? 0 :  *iptr ));
	  iptr+=inskip;
	}
      
      /*  the center of the output pointer is the new value */	 

      if( tot > SHRT_MAX )
	tot = SHRT_MAX;
      else if( tot < SHRT_MIN )
	tot = SHRT_MIN;
      *(optr + centertap*outskip) = (short)tot;
      optr+=outskip;
      /* set the new input pointer one more than it was before */
      iptr -= (numberoftaps-1)*inskip;
      
    }
    

    /* next line */
    istart += instride;
    ostart += outstride;
  }
  
  return(1);			/* we're done */
}



/* Applies a 2-d filter in 2-d

   The filter is placed where each output it needed and the values
   are multiplied from the inbuffer by the filter values.

   When a value referenced doesn't exist on the image (if it's less
   than 0 or >xsize or ysize), it is either referred a 0 or the
   mirrored corresponding value, depending on the value of mirror.

 */

int filter_2d(int numxtaps,
	      int numytaps,
	      double * taps,
	      int centerx,
	      int centery,
	      int mirror,
	      int xsize,
	      int ysize,
	      unsigned char * inbuf,
	      int inskip,
	      int instride,
	      unsigned char * outbuf,
	      int outskip,
	      int outstride)
{

  int xpos, xi, yi, xend, line;

  unsigned char *ostart, *iptr, *optr;
  double *ctap, tot;
  

  ostart = outbuf;

  
  /******* Line-looping start *******/

  /* loop for each line in the image */
  for (line = -centery; line < (ysize-centery); line++) {    
    
    optr=ostart;

    xend = xsize-centerx;
    
    /* loop for each pixel on the line */
    for (xpos = -centerx; xpos < xsize-centerx; xpos++) {
        
      /* set pixel total to 0 */
      tot = 0;

      /* loop through each line of the filter */
      for (yi=line; yi<numytaps+line; yi++) {

	/* ctap is set to the first tap of the line to refer to */
	ctap = taps+((yi-line)*numxtaps);

	/* branch once based on the value of mirror*/
	if (mirror) {
	  /* calculate the starting address for the line */
	  if (yi<0) iptr=inbuf-yi*instride; else
	    if (yi>=ysize) 
	      iptr=inbuf+(ysize-2-(yi-ysize))*instride; else
	      iptr=inbuf+yi*instride;
	  
	  /* loop through the line */
	  for (xi=xpos; xi<xpos+numxtaps; xi++)
	    tot += ( *ctap++ *(double)
		    ((xi<0)?*(iptr-xi*inskip):
		     ((xi>=xsize)?*(iptr+(inskip*(xsize-2-(xi-xsize)))):
		      *(iptr+xi*inskip))));

	} else {
	  /* not mirror, just zero here */
	  /* if it is a line that exists, calculate address and loop,
	     otherwise, nothing needs to be added */
	  if ((yi>=0)&&(yi<ysize)) {
	    iptr=inbuf+yi*instride;
	    
	    /* loop through the line */
	    for (xi=xpos; xi<xpos+numxtaps; xi++)
	      tot += ( *ctap++ *(double)
		      ((xi<0)?0:((xi>=xsize)?0:*(iptr+xi*inskip)))); }

	} /* done with line of the filter */

      } /* done with filter for the pixel */
	 
      /*  the center of the output pointer is the new value */	 
      *optr = (unsigned char) (tot>255?255:(tot<0?0:tot));
      optr+=outskip;
      

    } /* loop for each x */
    
    
    /* next line */
    ostart += outstride;

  } /* done with each line */
    
  return(1);			/* we're done */
}



/* Does an affine transformation between two image buffers.

   It multiplies each pixel coordinate by the coefficients
   to map in into the new out window:

                x' = ax + by + c
		y' = dx + ey + f

   It can also work in reverse mode, which goes through the output
   buffer coordinates and looks to what pixel would have been
   mapped to it for an affine transformation. It takes the a-f
   cofficients but uses them as follows:

         x= e(x' - c) - b(y' - f)    y= e(x' - c) - b(y' - f)
            ---------------------       ---------------------
      	          ae - bd                   ae - bd

   (Where x' and y' are the values from the output window, as
   in the previous set of equations, and x and y are where to
   retrieve the value from.

   The computation requirements are the same for a reverse or a forward
   transformation, because both are reduced to a simple add and multiply
   equation for each line.

   So that points aren't plotted in bad locations, xsize and ysize
   are the values to use for the inbuffer and xoutmax and youtmax
   are the maximum translation values that it will plot for (or, if
   in reverse mode, it's the set of values mapped for).

*/

int affine_trans(double a, double b, double c,
		 double d, double e, double f,
		 int reverse,
		 int inxsize,
		 int inysize,
		 unsigned char * inbuf,
		 int inskip,
		 int instride,
		 int outxsize,
		 int outysize,
		 unsigned char * outbuf,
		 int outskip,
		 int outstride)
{
  int indiff, outdiff, xpos, ypos, newx, newy;
  double newxadd, newyadd, newxmult, newymult, ae_bd, ae_bdrec;
  
  indiff = instride - (inxsize * inskip);
  outdiff = outstride - (outxsize * outskip);
  
  if (!reverse) {     /* regular affine mapping */
    
    newxmult = a;
    newymult = d;
    
    /* loop for each line in the image */
    for (ypos = 0; ypos < inysize; ypos++) {
      
      /* cache floating point information that stays the same per line */
      newxadd = b * (double)ypos + c;
      newyadd = e * (double)ypos + f;

      for (xpos = 0; xpos < inxsize; xpos++) {
	
	newx = (int) ((newxmult * xpos) + newxadd);
	newy = (int) ((newymult * xpos) + newyadd);
	
	if ((newx >= 0) && (newx < outxsize) &&
	    (newy >= 0) && (newy < outysize))
	  *(outbuf + newx * outskip + newy * outstride) = *inbuf;

	inbuf += inskip;
      }
      inbuf += indiff;
    } 
  }
  else { 
    /* reversed-point mapping */
    
    ae_bd = a * e - b * d;
    if (ae_bd == 0.0) return(0);

    ae_bdrec = 1.0 / ae_bd;
    newxmult = e * ae_bdrec;
    newymult = (-d * ae_bdrec);

    /* loop for each line in the image */
    for (ypos = 0; ypos < outysize; ypos++) {
      
      /* cache floating point information that stays the same per line */
      newxadd = (-(b * ypos) - (e * c) + (f * b)) * ae_bdrec;
      newyadd = ((a * ypos) - (a * f) + (d * c)) * ae_bdrec;
      
      for (xpos = 0; xpos < outxsize; xpos++) {
	
	newx = (int) ((newxmult * xpos) + newxadd);
	newy = (int) ((newymult * xpos) + newyadd);
	
	if ((newx >= 0) && (newx < inxsize) &&
	    (newy >= 0) && (newy < inysize))
	  *outbuf = *(inbuf + newx * inskip + newy * instride);

	outbuf += outskip;
      }
      outbuf += outdiff;
    }
  }
  
  return(1);			/* we're done */
}


/* Warp-mapped affine transformation 

   For the following given equations:

                x' = ax + by + c
		y' = dx + ey + f

   It produces a warp map to do this affine transformation. 
   The reverse-mapping formulas are the following:

         x= e(x' - c) - b(y' - f)    y= e(x' - c) - b(y' - f)
            ---------------------       ---------------------
      	          ae - bd                   ae - bd

   (Where x' and y' are the values from the output window, as
   in the previous set of equations, and x and y are where to
   retrieve the value from.

*/

int affine_warp(double a, double b, double c,
		double d, double e, double f,
		int xsize,
		int ysize,
		int * xbuf,
		int * ybuf,
		int skip,
		int stride)
{
  int diff, xpos, ypos; 
  double newx, newy, newxadd, newyadd, newxmult, newymult, ae_bd, ae_bdrec;
  
  diff = stride - (xsize * skip);

  ae_bd = a * e - b * d;
  if (ae_bd == 0.0) return(0);
  
  ae_bdrec = 1.0 / ae_bd;
  newxmult = e * ae_bdrec;
  newymult = (-d * ae_bdrec);

  /* loop for each line in the image */
  for (ypos = 0; ypos < ysize; ypos++) {
    
    /* cache floating point information that stays the same per line */
    newxadd = (-(b * ypos) - (e * c) + (f * b)) * ae_bdrec;
    newyadd = ((a * ypos) - (a * f) + (d * c)) * ae_bdrec;
     
    for (xpos = 0; xpos < xsize; xpos++) {
      
      newx = (int) ((newxmult * xpos) + newxadd);
      newy = (int) ((newymult * xpos) + newyadd);
     
      *xbuf=((((int)newx) << 8) |
	     ((int)((newx - ((double)((int)newx))) * 256)));
      *ybuf=((((int)newy) << 8) | 
	     ((int)((newy - ((double)((int)newy))) * 256))); 

      xbuf += skip;
      ybuf += skip;
      
    }
    xbuf += diff;
    ybuf += diff;

  }
  
  return(1);
}

int affine_warp_safe(double a, double b, double c,
		     double d, double e, double f,
		     int xsize,
		     int ysize,
		     int * xbuf,
		     int * ybuf,
		     int skip,
		     int stride,
		     int inxsize,
		     int inysize)
{
  int diff, xpos, ypos; 
  double newx, newy;
  double newxadd, newyadd, newxmult, newymult, ae_bd, ae_bdrec;
  
  diff = stride - (xsize * skip);

  ae_bd = a * e - b * d;
  if (ae_bd == 0.0) return(0);
  
  ae_bdrec = 1.0 / ae_bd;
  newxmult = e * ae_bdrec;
  newymult = (-d * ae_bdrec);

  /* loop for each line in the image */
  for (ypos = 0; ypos < ysize; ypos++) {
    
    /* cache floating point information that stays the same per line */
    newxadd = (-(b * ypos) - (e * c) + (f * b)) * ae_bdrec;
    newyadd = ((a * ypos) - (a * f) + (d * c)) * ae_bdrec;
     
    for (xpos = 0; xpos < xsize; xpos++) {
      
      newx = (int) ((newxmult * xpos) + newxadd);
      newy = (int) ((newymult * xpos) + newyadd);

      if(newx < 0.0 || newx >= inxsize) *xbuf = 0;
      else *xbuf=((((int)newx) << 8) |
		  ((int)((newx - ((double)((int)newx))) * 256)));
      
      if(newy < 0.0 || newy >= inysize) *ybuf = 0;
      else *ybuf=((((int)newy) << 8) | 
		  ((int)((newy - ((double)((int)newy))) * 256))); 
      
      xbuf += skip;
      ybuf += skip;
      
    }
    xbuf += diff;
    ybuf += diff;
  }
    
  return(1);
}

