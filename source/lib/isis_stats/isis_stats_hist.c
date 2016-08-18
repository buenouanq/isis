/*   stats_hist.c

     Histogramming routines for Isis

     J. Watlington, 12/96

     Modified:
     12/27/96  wad   cleaned up the limit checking while generating the
                     histogram, and added the 1D functions.
*/

#include <stdlib.h>
#include <stdio.h>                /* Must include before isis.h */
#include <isis.h>                 /* Holds all Isis interpreter info */

#include "isis_stats_private.h"

#define MY_MAX_UINT  4294967295
#define MY_MAX_INT   2147483647
#define MY_MIN_INT  -2147483648

/*  (image-stats-raw-2D-hist-byte a-buf b-buf
                                   xsize ysize xstep ystep 
				   hist-buf
				   a-bins a-min a-max
				   b-bins b-min b-max
				   hist-scale )
*/
Value *image_stats_raw_2D_hist_byte(Script *script, char *proc_name,
				int call_data, int argc, Value **argv)
{
  byte            *abuf, *bbuf;
  int             xsize, ysize;
  int             xstep, ystep;
  int             hist_scale;
  byte            *outbuf;

  register long   offset;
  unsigned long   *hist;
  unsigned long   *histbuf;
  register byte   *aptr, *bptr;

  int             temp;
  register int    x;
  int             y;

  int             a_bins, b_bins;
  double          a_min, a_max;
  double          b_min, b_max;
  double          a_scale, b_scale;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(14, NULL);
  checkAddr(0, NULL);
  checkAddr(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkAddr(6, NULL);
  checkInt(7, NULL);
  checkReal(8, NULL);
  checkReal(9, NULL);
  checkInt(10, NULL);
  checkReal(11, NULL);
  checkReal(12, NULL);
  checkInt(13, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (byte *)getAddr( argv[0] );
  bbuf = (byte *)getAddr( argv[1] );

  xsize = getInt( argv[2] );
  ysize = getInt( argv[3] );
  xstep = getInt( argv[4] );
  ystep = getInt( argv[5] );

  outbuf = (byte *)getAddr( argv[6] );

  a_bins = getInt( argv[7] );
  a_min = (double)getReal( argv[8] );
  a_max = (double)getReal( argv[9] );

  b_bins = getInt( argv[10] );
  b_min = (double)getReal( argv[11] );
  b_max = (double)getReal( argv[12] );

  hist_scale = getInt( argv[13] );

  a_scale = (double)(a_bins - 1) / (a_max - a_min);
  b_scale = (double)(b_bins - 1) / (b_max - b_min);

  /*  Allocate temporary memory for the histogram  */

  if( (histbuf = (unsigned long *)malloc( a_bins * b_bins *
					 sizeof(unsigned long))) == 0 )
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * b_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < (a_bins * b_bins); x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;
      bptr = bbuf;  bbuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  hist = histbuf + offset;

	  offset = (long)(((double)*bptr - b_min) * b_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= b_bins ) offset = b_bins - 1;
	  hist = hist + ((b_bins - offset) * a_bins);

	  (*hist)++;

	  aptr += xstep;
	  bptr += xstep;
	}
    }

  /*  And generate an output image from the histogram  */

  for( x = 0; x < (a_bins * b_bins); x++ )
    {
      temp = histbuf[x] / hist_scale;
      if( temp > 255 ) temp = 255;
      outbuf[x] = (byte)temp;
    }

  free( histbuf );
  return NullValue;
}


/*  (image-stats-raw-2D-hist-short a-buf b-buf
                                   xsize ysize xstep ystep 
				   hist-buf
				   a-bins a-min a-max
				   b-bins b-min b-max
				   hist-scale )
*/
Value *image_stats_raw_2D_hist_short(Script *script, char *proc_name,
				int call_data, int argc, Value **argv)
{
  signed short    *abuf, *bbuf;
  int             xsize, ysize;
  int             xstep, ystep;
  int             hist_scale;
  byte            *outbuf;

  long            offset;
  unsigned long   *hist;
  unsigned long   *histbuf;
  register signed short  *aptr, *bptr;

  long            temp;
  register int    x;
  int             y;

  int             a_bins, b_bins;
  double          a_min, a_max;
  double          b_min, b_max;
  double          a_scale, b_scale;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(14, NULL);
  checkAddr(0, NULL);
  checkAddr(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkAddr(6, NULL);
  checkInt(7, NULL);
  checkReal(8, NULL);
  checkReal(9, NULL);
  checkInt(10, NULL);
  checkReal(11, NULL);
  checkReal(12, NULL);
  checkInt(13, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (signed short *)getAddr( argv[0] );
  bbuf = (signed short *)getAddr( argv[1] );

  xsize = getInt( argv[2] );
  ysize = getInt( argv[3] );
  xstep = getInt( argv[4] );
  ystep = getInt( argv[5] );

  outbuf = (byte *)getAddr( argv[6] );

  a_bins = getInt( argv[7] );
  a_min = (double)getReal( argv[8] );
  a_max = (double)getReal( argv[9] );

  b_bins = getInt( argv[10] );
  b_min = (double)getReal( argv[11] );
  b_max = (double)getReal( argv[12] );

  hist_scale = getInt( argv[13] );

  a_scale = (double)(a_bins - 1) / (a_max - a_min);
  b_scale = (double)(b_bins - 1) / (b_max - b_min);

  /*  Allocate temporary memory for the histogram  */

  if( (histbuf = (unsigned long *)malloc( a_bins * b_bins *
					 sizeof(unsigned long))) == 0 )
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * b_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < (a_bins * b_bins); x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;
      bptr = bbuf;  bbuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  hist = histbuf + offset;

	  offset = (long)(((double)*bptr - b_min) * b_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= b_bins ) offset = b_bins - 1;
	  hist = hist + ((b_bins - offset) * a_bins);

	  (*hist)++;
	  aptr += xstep;
	  bptr += xstep;
	}
    }

  /*  And generate an output image from the histogram  */

  for( x = 0; x < (a_bins * b_bins); x++ )
    {
      temp = histbuf[x] / hist_scale;
      if( temp > 255 ) temp = 255;
      outbuf[x] = (byte)temp;
    }

  free( histbuf );
  return NullValue;
}



/*  (image-stats-raw-2D-hist-ushort a-buf b-buf
                                   xsize ysize xstep ystep 
				   hist-buf
				   a-bins a-min a-max
				   b-bins b-min b-max
				   hist-scale )
*/
Value *image_stats_raw_2D_hist_ushort(Script *script, char *proc_name,
				int call_data, int argc, Value **argv )
{
  unsigned short  *abuf, *bbuf;
  int             xsize, ysize;
  int             xstep, ystep;
  int             hist_scale;
  byte            *outbuf;

  long            offset;
  unsigned long   *hist;
  unsigned long   *histbuf;
  register unsigned short  *aptr, *bptr;

  long            temp;
  register int    x;
  int             y;

  int             a_bins, b_bins;
  double          a_min, a_max;
  double          b_min, b_max;
  double          a_scale, b_scale;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(14, NULL);
  checkAddr(0, NULL);
  checkAddr(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkAddr(6, NULL);
  checkInt(7, NULL);
  checkReal(8, NULL);
  checkReal(9, NULL);
  checkInt(10, NULL);
  checkReal(11, NULL);
  checkReal(12, NULL);
  checkInt(13, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (unsigned short *)getAddr( argv[0] );
  bbuf = (unsigned short *)getAddr( argv[1] );

  xsize = getInt( argv[2] );
  ysize = getInt( argv[3] );
  xstep = getInt( argv[4] );
  ystep = getInt( argv[5] );

  outbuf = (byte *)getAddr( argv[6] );

  a_bins = getInt( argv[7] );
  a_min = (double)getReal( argv[8] );
  a_max = (double)getReal( argv[9] );

  b_bins = getInt( argv[10] );
  b_min = (double)getReal( argv[11] );
  b_max = (double)getReal( argv[12] );

  hist_scale = getInt( argv[13] );

  a_scale = (double)(a_bins - 1) / (a_max - a_min);
  b_scale = (double)(b_bins - 1) / (b_max - b_min);

  /*  Allocate temporary memory for the histogram  */

  if( (histbuf = (unsigned long *)malloc( a_bins * b_bins *
					 sizeof(unsigned long))) == 0 )
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * b_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < (a_bins * b_bins); x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;
      bptr = bbuf;  bbuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  hist = histbuf + offset;

	  offset = (long)(((double)*bptr - b_min) * b_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= b_bins ) offset = b_bins - 1;
	  hist = hist + ((b_bins - offset) * a_bins);

	  (*hist)++;
	  aptr += xstep;
	  bptr += xstep;
	}
    }

  /*  And generate an output image from the histogram  */

  for( x = 0; x < (a_bins * b_bins); x++ )
    {
      temp = histbuf[x] / hist_scale;
      if( temp > 255 ) temp = 255;
      outbuf[x] = (byte)temp;
    }

  free( histbuf );
  return NullValue;
}


/*  (image-stats-raw-2D-hist-long a-buf b-buf
                                   xsize ysize xstep ystep 
				   hist-buf
				   a-bins a-min a-max
				   b-bins b-min b-max
				   hist-scale )
*/

Value *image_stats_raw_2D_hist_long(Script *script, char *proc_name,
				int call_data, int argc, Value **argv)
{
  signed long     *abuf, *bbuf;
  int             xsize, ysize;
  int             xstep, ystep;
  int             hist_scale;
  byte            *outbuf;

  long            offset;
  unsigned long   *hist;
  unsigned long   *histbuf;
  register signed long  *aptr, *bptr;

  long            temp;
  register int    x;
  int             y;

  int             a_bins, b_bins;
  double          a_min, a_max;
  double          b_min, b_max;
  double          a_scale, b_scale;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(14, NULL);
  checkAddr(0, NULL);
  checkAddr(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkAddr(6, NULL);
  checkInt(7, NULL);
  checkReal(8, NULL);
  checkReal(9, NULL);
  checkInt(10, NULL);
  checkReal(11, NULL);
  checkReal(12, NULL);
  checkInt(13, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (signed long *)getAddr( argv[0] );
  bbuf = (signed long *)getAddr( argv[1] );

  xsize = getInt( argv[2] );
  ysize = getInt( argv[3] );
  xstep = getInt( argv[4] );
  ystep = getInt( argv[5] );

  outbuf = (byte *)getAddr( argv[6] );

  a_bins = getInt( argv[7] );
  a_min = (double)getReal( argv[8] );
  a_max = (double)getReal( argv[9] );

  b_bins = getInt( argv[10] );
  b_min = (double)getReal( argv[11] );
  b_max = (double)getReal( argv[12] );

  hist_scale = getInt( argv[13] );

  a_scale = (double)(a_bins - 1) / (a_max - a_min);
  b_scale = (double)(b_bins - 1) / (b_max - b_min);

  /*  Allocate temporary memory for the histogram  */

  if( (histbuf = (unsigned long *)malloc( a_bins * b_bins *
					 sizeof(unsigned long))) == 0 )
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * b_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < (a_bins * b_bins); x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;
      bptr = bbuf;  bbuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  hist = histbuf + offset;

	  offset = (long)(((double)*bptr - b_min) * b_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= b_bins ) offset = b_bins - 1;
	  hist = hist + ((b_bins - offset) * a_bins);

	  (*hist)++;
	  aptr += xstep;
	  bptr += xstep;
	}
    }

  /*  And generate an output image from the histogram  */

  for( x = 0; x < (a_bins * b_bins); x++ )
    {
      temp = histbuf[x] / hist_scale;
      if( temp > 255 ) temp = 255;
      outbuf[x] = (byte)temp;
    }

  free( histbuf );
  return NullValue;
}



/*  (image-stats-raw-2D-hist-ulong a-buf b-buf
                                   xsize ysize xstep ystep 
				   hist-buf
				   a-bins a-min a-max
				   b-bins b-min b-max
				   hist-scale )
*/
Value *image_stats_raw_2D_hist_ulong(Script *script, char *proc_name,
				    int call_data, int argc, Value **argv)
{
  unsigned long    *abuf, *bbuf;
  int             xsize, ysize;
  int             xstep, ystep;
  int             hist_scale;
  byte            *outbuf;

  long            offset;
  unsigned long   *hist;
  unsigned long   *histbuf;
  register unsigned long  *aptr, *bptr;

  long            temp;
  register int    x;
  int             y;

  int             a_bins, b_bins;
  double          a_min, a_max;
  double          b_min, b_max;
  double          a_scale, b_scale;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(14, NULL);
  checkAddr(0, NULL);
  checkAddr(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkAddr(6, NULL);
  checkInt(7, NULL);
  checkReal(8, NULL);
  checkReal(9, NULL);
  checkInt(10, NULL);
  checkReal(11, NULL);
  checkReal(12, NULL);
  checkInt(13, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (unsigned long *)getAddr( argv[0] );
  bbuf = (unsigned long *)getAddr( argv[1] );

  xsize = getInt( argv[2] );
  ysize = getInt( argv[3] );
  xstep = getInt( argv[4] );
  ystep = getInt( argv[5] );

  outbuf = (byte *)getAddr( argv[6] );

  a_bins = getInt( argv[7] );
  a_min = (double)getReal( argv[8] );
  a_max = (double)getReal( argv[9] );

  b_bins = getInt( argv[10] );
  b_min = (double)getReal( argv[11] );
  b_max = (double)getReal( argv[12] );

  hist_scale = getInt( argv[13] );

  a_scale = (double)(a_bins - 1) / (a_max - a_min);
  b_scale = (double)(b_bins - 1) / (b_max - b_min);

  /*  Allocate temporary memory for the histogram  */

  if( (histbuf = (unsigned long *)malloc( a_bins * b_bins *
					 sizeof(unsigned long))) == 0 )
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * b_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < (a_bins * b_bins); x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;
      bptr = bbuf;  bbuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  hist = histbuf + offset;

	  offset = (long)(((double)*bptr - b_min) * b_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= b_bins ) offset = b_bins - 1;
	  hist = hist + ((b_bins - offset) * a_bins);

	  (*hist)++;
	  aptr += xstep;
	  bptr += xstep;
	}
    }

  /*  And generate an output image from the histogram  */

  for( x = 0; x < (a_bins * b_bins); x++ )
    {
      temp = histbuf[x] / hist_scale;
      if( temp > 255 ) temp = 255;
      outbuf[x] = (byte)temp;
    }

  free( histbuf );
  return NullValue;
}


/*  (image-stats-raw-2D-hist-float a-buf b-buf
                                   xsize ysize xstep ystep 
				   hist-buf
				   a-bins a-min a-max
				   b-bins b-min b-max
				   hist-scale )
*/
Value *image_stats_raw_2D_hist_float(Script *script, char *proc_name,
				int call_data, int argc, Value **argv)
{
  float          *abuf, *bbuf;
  int             xsize, ysize;
  int             xstep, ystep;
  int             hist_scale;
  byte            *outbuf;

  long            offset;
  unsigned long   *hist;
  unsigned long   *histbuf;
  register float  *aptr, *bptr;

  long            temp;
  register int    x;
  int             y;

  int             a_bins, b_bins;
  double          a_min, a_max;
  double          b_min, b_max;
  double          a_scale, b_scale;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(14, NULL);
  checkAddr(0, NULL);
  checkAddr(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkAddr(6, NULL);
  checkInt(7, NULL);
  checkReal(8, NULL);
  checkReal(9, NULL);
  checkInt(10, NULL);
  checkReal(11, NULL);
  checkReal(12, NULL);
  checkInt(13, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (float *)getAddr( argv[0] );
  bbuf = (float *)getAddr( argv[1] );

  xsize = getInt( argv[2] );
  ysize = getInt( argv[3] );
  xstep = getInt( argv[4] );
  ystep = getInt( argv[5] );

  outbuf = (byte *)getAddr( argv[6] );

  a_bins = getInt( argv[7] );
  a_min = (double)getReal( argv[8] );
  a_max = (double)getReal( argv[9] );

  b_bins = getInt( argv[10] );
  b_min = (double)getReal( argv[11] );
  b_max = (double)getReal( argv[12] );

  hist_scale = getInt( argv[13] );

  a_scale = (double)(a_bins - 1)/ (a_max - a_min);
  b_scale = (double)(b_bins - 1)/ (b_max - b_min);

  /*  Allocate temporary memory for the histogram  */

  if( (histbuf = (unsigned long *)malloc( a_bins * b_bins *
					 sizeof(unsigned long))) == 0 )
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * b_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < (a_bins * b_bins); x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;
      bptr = bbuf;  bbuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  hist = histbuf + offset;

	  offset = (long)(((double)*bptr - b_min) * b_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= b_bins ) offset = b_bins - 1;
	  hist = hist + ((b_bins - offset) * a_bins);

	  (*hist)++;
	  aptr += xstep;
	  bptr += xstep;
	}
    }

  /*  And generate an output image from the histogram  */

  for( x = 0; x < (a_bins * b_bins); x++ )
    {
      temp = histbuf[x] / hist_scale;
      if( temp > 255 ) temp = 255;
      outbuf[x] = (byte)temp;
    }

  free( histbuf );
  return NullValue;
}



/*  (image-stats-raw-2D-hist-double a-buf b-buf
                                   xsize ysize xstep ystep 
				   hist-buf
				   a-bins a-min a-max
				   b-bins b-min b-max
				   hist-scale )
*/
Value *image_stats_raw_2D_hist_double(Script *script, char *proc_name,
				int call_data, int argc, Value **argv)
{
  double          *abuf, *bbuf;
  int             xsize, ysize;
  int             xstep, ystep;
  int             hist_scale;
  byte            *outbuf;

  long            offset;
  unsigned long   *hist;
  unsigned long   *histbuf;
  register double *aptr, *bptr;

  long            temp;
  register int    x;
  int             y;

  int             a_bins, b_bins;
  double          a_min, a_max;
  double          b_min, b_max;
  double          a_scale, b_scale;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(14, NULL);
  checkAddr(0, NULL);
  checkAddr(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkAddr(6, NULL);
  checkInt(7, NULL);
  checkReal(8, NULL);
  checkReal(9, NULL);
  checkInt(10, NULL);
  checkReal(11, NULL);
  checkReal(12, NULL);
  checkInt(13, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (double *)getAddr( argv[0] );
  bbuf = (double *)getAddr( argv[1] );

  xsize = getInt( argv[2] );
  ysize = getInt( argv[3] );
  xstep = getInt( argv[4] );
  ystep = getInt( argv[5] );

  outbuf = (byte *)getAddr( argv[6] );

  a_bins = getInt( argv[7] );
  a_min = (double)getReal( argv[8] );
  a_max = (double)getReal( argv[9] );

  b_bins = getInt( argv[10] );
  b_min = (double)getReal( argv[11] );
  b_max = (double)getReal( argv[12] );

  hist_scale = getInt( argv[13] );

  a_scale = (double)(a_bins - 1) / (a_max - a_min);
  b_scale = (double)(b_bins - 1) / (b_max - b_min);

  /*  Allocate temporary memory for the histogram  */

  if( (histbuf = (unsigned long *)malloc( a_bins * b_bins *
					 sizeof(unsigned long))) == 0 )
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * b_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < (a_bins * b_bins); x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;
      bptr = bbuf;  bbuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  hist = histbuf + offset;

	  offset = (long)(((double)*bptr - b_min) * b_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= b_bins ) offset = b_bins - 1;
	  hist = hist + ((b_bins - offset) * a_bins);

	  (*hist)++;
	  aptr += xstep;
	  bptr += xstep;
	}
    }

  /*  And generate an output image from the histogram  */

  for( x = 0; x < (a_bins * b_bins); x++ )
    {
      temp = histbuf[x] / hist_scale;
      if( temp > 255 ) temp = 255;
      outbuf[x] = (byte)temp;
    }

  free( histbuf );
  return NullValue;
}


/*  (image-stats-raw-1D-hist-byte a-buf xsize ysize xstep ystep 
				   a-bins a-min a-max )
*/
Value *image_stats_raw_1D_hist_byte(Script *script, char *proc_name,
				int call_data, int argc, Value **argv)
{
  byte            *abuf;
  int             xsize, ysize;
  int             xstep, ystep;

  register long   offset;
  unsigned long   *histbuf;
  register byte   *aptr;

  register int    x;
  int             y;

  int             a_bins;
  double          a_min, a_max;
  double          a_scale;

  int             *ioutbuf;
  double          *foutbuf;

  /*  First, check to see if the arguments are all there !   */

  checkCount(8, NULL);
  checkAddr(0, NULL);
  checkInt(1, NULL);
  checkInt(2, NULL);
  checkInt(3, NULL);
  checkInt(4, NULL);
  checkInt(5, NULL);
  checkReal(6, NULL);
  checkReal(7, NULL);

  /*  Now extract the arguments from their Isis representation  */

  abuf = (byte *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  a_bins = getInt( argv[5] );
  a_min = (double)getReal( argv[6] );
  a_max = (double)getReal( argv[7] );

  a_scale = (double)( a_bins - 1 ) / (a_max - a_min);

  /*  Allocate temporary memory for the histogram,
      and memory for the returned list.                  */

  if((histbuf = (unsigned long *)malloc( a_bins * sizeof(unsigned long))) == 0)
    {
      fprintf(stderr,
	      "** %s: Unable to allocate %d bytes of temporary memory\n",
	      proc_name, a_bins * sizeof(unsigned long));
      return NullValue;
    }

  for( x = 0; x < a_bins; x++ )
    histbuf[ x ] = 0;

  /*  Now calculate the histogram from the input data  */

  for( y = 0; y < ysize; y++ )
    {
      aptr = abuf;  abuf += ystep;

      for( x = 0; x < xsize; x++ )
	{
	  offset = (long)(((double)*aptr - a_min) * a_scale);
	  if( offset < 0 ) offset = 0;
	  if( offset >= a_bins ) offset = a_bins - 1;
	  (*(histbuf + offset))++;
	  aptr += xstep;
	}
    }

  /*  And generate an output list from the histogram  */

  switch( call_data ){
  case STATS_HIST_INTEGER:
    if( (ioutbuf = (int *)malloc( a_bins * sizeof(int))) == 0 )
      {
	fprintf(stderr,
		"** %s: Unable to allocate %d bytes of temporary memory\n",
		proc_name, a_bins * sizeof(int));
	return NullValue;
      }
    
    for( x = 0; x < a_bins; x++ )
      if( histbuf[x] > MY_MAX_INT )
	ioutbuf[x] = (int)MY_MAX_INT;
      else
	ioutbuf[x] = (int)histbuf[x];

    free( histbuf );
    return( newIntList( a_bins, ioutbuf ) );

  case STATS_HIST_REAL:
    if( (foutbuf = (double *)malloc( a_bins * sizeof(double))) == 0 )
      {
	fprintf(stderr,
		"** %s: Unable to allocate %d bytes of temporary memory\n",
		proc_name, a_bins * sizeof(double));
	return NullValue;
      }
    
    for( x = 0; x < a_bins; x++ )
      foutbuf[x] = (double)histbuf[x];
    
    free( histbuf );
    return( newRealList( a_bins, foutbuf ) );

  default:
    fprintf( stderr,
	    "** %s: Unrecognized output list type\n", proc_name);
    break;
  }
  return NullValue;
}



Value *image_stats_raw_fast_1D_hist_byte(Script *script, char *proc_name,
					 int call_data, int argc, Value **argv)
{
  /* Added by Stefan, January 1998 */

  byte            *abuf;
  int             xsize, ysize;
  int             xstep, ystep;
  
  int             histbuf[256];
  register byte   *aptr;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(3, NULL);
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "steps");

  /*  Now extract the arguments from their Isis representation  */

  abuf = (byte *)getAddr( Listitem(argv[1], 0) );
  
  xsize = getInt(Listitem(argv[0], 0));
  ysize = getInt(Listitem(argv[0], 1)); 
  xstep = getInt(Listitem(argv[2], 0));
  ystep = getInt(Listitem(argv[2], 1));

  for( x = 0; x < 256; x++ ) histbuf[ x ] = 0;
  
  /*  Now calculate the histogram from the input data  */
  
  for(y = 0; y < ysize; y++) {
    aptr = abuf;  abuf += ystep;
    for(x = 0; x < xsize; x++) {
      histbuf[*aptr]++;
      aptr += xstep;
    }
  }

  /*  Return histogram in list of 256 integers  */

  return( newIntList( 256, histbuf ) );
}
