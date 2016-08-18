/*   stats.c

     Statistics Processing Isis interface routines

     J. Watlington, 12/96

     from template provided by Stefan Agamanolis.

     This file contains a separate function for handling each type of input
     data.  Each function performs all of the statistical computations.
     Messy, but fast.  Adding support for computing multiple computations
     in one pass is an idea left on the table...

*/

#include <stdio.h>                /* Must include before isis.h */
#include <float.h>
#include <isis.h>                 /* Holds all Isis interpreter info */

#include "isis_stats.h"           /* Holds a declaration bind_my_funcs() */
#include "isis_stats_private.h"

enum {
  STATS_MEAN, STATS_MAX, STATS_MIN,
  STATS_ENERGY, STATS_VARIANCE, STATS_SUM,
};

#define MY_MAX_UINT  4294967295U
#define MY_MAX_INT   2147483647
#define MY_MIN_INT  -2147483647  /* some compilers can't handle -2147483648 */

/*  Local Function Prototypes  */

Value *image_stats_byte(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *image_stats_short(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *image_stats_ushort(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *image_stats_long(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *image_stats_ulong(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *image_stats_float(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *image_stats_double(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);

/* ------------------------------------------------------------ */
void bind_stats_library(Script *script)
{
  bindBool(script, "isis_stats", 1);

  bindCfunc( script, "image-stats-raw-mean-byte",
	    image_stats_byte, STATS_MEAN );
  bindCfunc( script, "image-stats-raw-max-byte",
	    image_stats_byte, STATS_MAX );
  bindCfunc( script, "image-stats-raw-min-byte",
	    image_stats_byte, STATS_MIN );
  bindCfunc( script, "image-stats-raw-energy-byte",
	    image_stats_byte, STATS_ENERGY );
  bindCfunc( script, "image-stats-raw-variance-byte",
	    image_stats_byte, STATS_VARIANCE );
  bindCfunc( script, "image-stats-raw-sum-byte",
	    image_stats_byte, STATS_SUM );

  bindCfunc( script, "image-stats-raw-mean-short",
	    image_stats_short, STATS_MEAN );
  bindCfunc( script, "image-stats-raw-max-short",
	    image_stats_short, STATS_MAX );
  bindCfunc( script, "image-stats-raw-min-short",
	    image_stats_short, STATS_MIN );
  bindCfunc( script, "image-stats-raw-energy-short",
	    image_stats_short, STATS_ENERGY );
  bindCfunc( script, "image-stats-raw-variance-short",
	    image_stats_short, STATS_VARIANCE );
  bindCfunc( script, "image-stats-raw-sum-short",
	    image_stats_short, STATS_SUM );

  bindCfunc( script, "image-stats-raw-mean-ushort",
	    image_stats_ushort, STATS_MEAN );
  bindCfunc( script, "image-stats-raw-max-ushort",
	    image_stats_ushort, STATS_MAX );
  bindCfunc( script, "image-stats-raw-min-ushort",
	    image_stats_ushort, STATS_MIN );
  bindCfunc( script, "image-stats-raw-energy-ushort",
	    image_stats_ushort, STATS_ENERGY );
  bindCfunc( script, "image-stats-raw-variance-ushort",
	    image_stats_ushort, STATS_VARIANCE );
  bindCfunc( script, "image-stats-raw-sum-ushort",
	    image_stats_ushort, STATS_SUM );

  bindCfunc( script, "image-stats-raw-mean-long",
	    image_stats_long, STATS_MEAN );
  bindCfunc( script, "image-stats-raw-max-long",
	    image_stats_long, STATS_MAX );
  bindCfunc( script, "image-stats-raw-min-long",
	    image_stats_long, STATS_MIN );
  bindCfunc( script, "image-stats-raw-energy-long",
	    image_stats_long, STATS_ENERGY );
  bindCfunc( script, "image-stats-raw-variance-long",
	    image_stats_long, STATS_VARIANCE );
  bindCfunc( script, "image-stats-raw-sum-long",
	    image_stats_long, STATS_SUM );

  bindCfunc( script, "image-stats-raw-mean-ulong",
	    image_stats_ulong, STATS_MEAN );
  bindCfunc( script, "image-stats-raw-max-ulong",
	    image_stats_ulong, STATS_MAX );
  bindCfunc( script, "image-stats-raw-min-ulong",
	    image_stats_ulong, STATS_MIN );
  bindCfunc( script, "image-stats-raw-energy-ulong",
	    image_stats_ulong, STATS_ENERGY );
  bindCfunc( script, "image-stats-raw-variance-ulong",
	    image_stats_ulong, STATS_VARIANCE );
  bindCfunc( script, "image-stats-raw-sum-ulong",
	    image_stats_ulong, STATS_SUM );

  bindCfunc( script, "image-stats-raw-mean-float",
	    image_stats_float, STATS_MEAN );
  bindCfunc( script, "image-stats-raw-max-float",
	    image_stats_float, STATS_MAX );
  bindCfunc( script, "image-stats-raw-min-float",
	    image_stats_float, STATS_MIN );
  bindCfunc( script, "image-stats-raw-energy-float",
	    image_stats_float, STATS_ENERGY );
  bindCfunc( script, "image-stats-raw-variance-float",
	    image_stats_float, STATS_VARIANCE );
  bindCfunc( script, "image-stats-raw-sum-float",
	    image_stats_float, STATS_SUM );


  bindCfunc( script, "image-stats-raw-mean-double",
	    image_stats_double, STATS_MEAN );
  bindCfunc( script, "image-stats-raw-max-double",
	    image_stats_double, STATS_MAX );
  bindCfunc( script, "image-stats-raw-min-double",
	    image_stats_double, STATS_MIN );
  bindCfunc( script, "image-stats-raw-energy-double",
	    image_stats_double, STATS_ENERGY );
  bindCfunc( script, "image-stats-raw-variance-double",
	    image_stats_double, STATS_VARIANCE );
  bindCfunc( script, "image-stats-raw-sum-double",
	    image_stats_double, STATS_SUM );


  bindCfunc( script, "image-stats-raw-2D-hist-byte",
	    image_stats_raw_2D_hist_byte, 0 );
  bindCfunc( script, "image-stats-raw-2D-hist-short",
	    image_stats_raw_2D_hist_short, 0 );
  bindCfunc( script, "image-stats-raw-2D-hist-ushort",
	    image_stats_raw_2D_hist_ushort, 0 );
  bindCfunc( script, "image-stats-raw-2D-hist-long",
	    image_stats_raw_2D_hist_long, 0 );
  bindCfunc( script, "image-stats-raw-2D-hist-ulong",
	    image_stats_raw_2D_hist_ulong, 0 );
  bindCfunc( script, "image-stats-raw-2D-hist-float",
	    image_stats_raw_2D_hist_float, 0 );
  bindCfunc( script, "image-stats-raw-2D-hist-double",
	    image_stats_raw_2D_hist_double, 0 );

  bindCfunc( script, "image-stats-raw-1D-hist-byte-int",
	    image_stats_raw_1D_hist_byte, STATS_HIST_INTEGER );
  bindCfunc( script, "image-stats-raw-1D-hist-byte-real",
	    image_stats_raw_1D_hist_byte, STATS_HIST_REAL );

  bindCfunc( script, "image-stats-raw-fast-1D-hist-byte",
	     image_stats_raw_fast_1D_hist_byte, 0);

  bindCfunc( script, "image-stats-raw-centroid",
	     image_stats_raw_centroid, 0);

}



/*  (image-stats-raw-op-byte  in-buf xsize ysize xstep ystep )
*/
Value *image_stats_byte(Script *script, char *proc_name,
			int call_data, int argc, Value **argv)
{
  byte            *inbuf;
  int             ystep, ysize;
  register int    xstep, xsize;
  byte            *lineptr;

  register byte   *inptr;
  register double   value;
  register double   tmp;
  register double   mean;
  register byte   b_value;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, NULL);
  checkAddr(0, "buffer address");
  checkInt(1, "xsize");
  checkInt(2, "ysize");
  checkInt(3, "xstep");
  checkInt(4, "ystep");

  /*  Now extract the arguments from their Isis representation  */

  inbuf = (byte *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  /*  Now calculate the desired statistics   */

  switch( call_data ) {
  case STATS_MEAN:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_MAX:
    b_value = 0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr > b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_MIN:
    b_value = 255;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr < b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_ENERGY:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = (double)*inptr;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }
    break;

  case STATS_VARIANCE:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (mean = value / (double)( xsize * ysize )) == 0.0 )
      {
	value = 0.0;
	break;
      }

    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = ((double)*inptr) - mean;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_SUM:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  default:
    fprintf(stderr, "** %s: Unknown stat function requested\n",
	    proc_name);
    return NullValue;
  }

  return( newReal( (real)value ) );
}



/*  (image-stats-raw-op-short  in-buf xsize ysize xstep ystep )
*/
Value *image_stats_short(Script *script, char *proc_name,
			int call_data, int argc, Value **argv)
{
  short           *inbuf;
  short           *lineptr;
  int             ystep, ysize;
  register int    xstep, xsize;

  register short  *inptr;
  register double   value;
  register double   tmp;
  register double   mean;
  register short  b_value;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, NULL);
  checkAddr(0, "buffer address");
  checkInt(1, "xsize");
  checkInt(2, "ysize");
  checkInt(3, "xstep");
  checkInt(4, "ystep");

  /*  Now extract the arguments from their Isis representation  */

  inbuf = (short *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  /*  Now calculate the desired statistics   */

  switch( call_data ) {
  case STATS_MEAN:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_MAX:
    b_value = -32768;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr > b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_MIN:
    b_value = 32767;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr < b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_ENERGY:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr * (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  case STATS_VARIANCE:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (mean = value / (double)( xsize * ysize )) == 0.0 )
      {
	value = 0.0;
	break;
      }

    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = ((double)*inptr) - mean;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_SUM:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  default:
    fprintf(stderr, "** %s: Unknown stat function requested\n",
	    proc_name);
    return NullValue;
  }

  return( newReal( (real)value ) );
}


/*  (image-stats-raw-op-ushort  in-buf xsize ysize xstep ystep )
*/
Value *image_stats_ushort(Script *script, char *proc_name,
			int call_data, int argc, Value **argv)
{
  unsigned short  *inbuf;
  unsigned short  *lineptr;
  int             ystep, ysize;
  register int    xstep, xsize;

  register unsigned short  *inptr;
  register double   value;
  register double   tmp;
  register double   mean;
  register unsigned short  b_value;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, NULL);
  checkAddr(0, "buffer address");
  checkInt(1, "xsize");
  checkInt(2, "ysize");
  checkInt(3, "xstep");
  checkInt(4, "ystep");

  /*  Now extract the arguments from their Isis representation  */

  inbuf = (unsigned short *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  /*  Now calculate the desired statistics   */

  switch( call_data ) {
  case STATS_MEAN:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_MAX:
    b_value = 0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr > b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_MIN:
    b_value = 65535;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr < b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_ENERGY:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr * (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  case STATS_VARIANCE:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (mean = value / (double)( xsize * ysize )) == 0.0 )
      {
	value = 0.0;
	break;
      }

    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = ((double)*inptr) - mean;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_SUM:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  default:
    fprintf(stderr, "** %s: Unknown stat function requested\n",
	    proc_name);
    return NullValue;
  }

  return( newReal( (real)value ) );
}

/*  (image-stats-raw-op-long  in-buf xsize ysize xstep ystep )
*/
Value *image_stats_long(Script *script, char *proc_name,
			int call_data, int argc, Value **argv)
{
  int            *inbuf;
  int            *lineptr;

  int             ystep, ysize;
  register int    xstep, xsize;

  register int   *inptr;
  register double   value;
  register double   tmp;
  register double   mean;
  register int   b_value;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, NULL);
  checkAddr(0, "buffer address");
  checkInt(1, "xsize");
  checkInt(2, "ysize");
  checkInt(3, "xstep");
  checkInt(4, "ystep");

  /*  Now extract the arguments from their Isis representation  */

  inbuf = (int *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  /*  Now calculate the desired statistics   */

  switch( call_data ) {
  case STATS_MEAN:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_MAX:
    b_value = MY_MIN_INT - 1;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr > b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }
    value = (double)b_value;
    break;

  case STATS_MIN:
    b_value = MY_MAX_INT;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr < b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_ENERGY:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr * (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  case STATS_VARIANCE:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (mean = value / (double)( xsize * ysize )) == 0.0 )
      {
	value = 0.0;
	break;
      }

    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = ((double)*inptr) - mean;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_SUM:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  default:
    fprintf(stderr, "** %s: Unknown stat function requested\n",
	    proc_name);
    return NullValue;
  }

  return( newReal( (real)value ) );
}



/*  (image-stats-raw-op-ulong  in-buf xsize ysize xstep ystep )
*/
Value *image_stats_ulong(Script *script, char *proc_name,
			int call_data, int argc, Value **argv)
{
  unsigned int   *inbuf;
  unsigned int   *lineptr;
  int             ystep, ysize;
  register int    xstep, xsize;

  register unsigned int  *inptr;
  register double   value;
  register double   tmp;
  register double   mean;
  register unsigned int  b_value;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, NULL);
  checkAddr(0, "buffer address");
  checkInt(1, "xsize");
  checkInt(2, "ysize");
  checkInt(3, "xstep");
  checkInt(4, "ystep");

  /*  Now extract the arguments from their Isis representation  */

  inbuf = (unsigned int *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  /*  Now calculate the desired statistics   */

  switch( call_data ) {
  case STATS_MEAN:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_MAX:
    b_value = 0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr > b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_MIN:
    b_value = MY_MAX_UINT;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr < b_value )
	      b_value = *inptr;

	    inptr += xstep;
	  }
      }

    value = (double)b_value;
    break;

  case STATS_ENERGY:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr * (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  case STATS_VARIANCE:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (mean = value / (double)( xsize * ysize )) == 0.0 )
      {
	value = 0.0;
	break;
      }

    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = ((double)*inptr) - mean;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_SUM:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  default:
    fprintf(stderr, "** %s: Unknown stat function requested\n",
	    proc_name);
    return NullValue;
  }

  return( newReal( (real)value ) );
}




/*  (image-stats-raw-op-float  in-buf xsize ysize xstep ystep )
*/
Value *image_stats_float(Script *script, char *proc_name,
			int call_data, int argc, Value **argv)
{
  float             *inbuf;
  float             *lineptr;
  int               ystep, ysize;
  register int      xstep, xsize;

  register float    *inptr;
  register double   value;
  register double   tmp;
  register double   mean;

  register int      x;
  int               y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, NULL);
  checkAddr(0, "buffer address");
  checkInt(1, "xsize");
  checkInt(2, "ysize");
  checkInt(3, "xstep");
  checkInt(4, "ystep");

  /*  Now extract the arguments from their Isis representation  */

  inbuf = (float *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  /*  Now calculate the desired statistics   */

  switch( call_data ) {
  case STATS_MEAN:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_MAX:
    value = FLT_MIN;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr > value )
	      value = (double)*inptr;

	    inptr += xstep;
	  }
      }
    break;

  case STATS_MIN:
    value = FLT_MAX;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr < value )
	      value = (double)*inptr;

	    inptr += xstep;
	  }
      }
    break;

  case STATS_ENERGY:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr * (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  case STATS_VARIANCE:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (mean = value / (double)( xsize * ysize )) == 0.0 )
      {
	value = 0.0;
	break;
      }

    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = ((double)*inptr) - mean;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_SUM:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }
    break;

  default:
    fprintf(stderr, "** %s: Unknown stat function requested\n",
	    proc_name);
    return NullValue;
  }

  return( newReal( (real)value ) );
}



/*  (image-stats-raw-op-double  in-buf xsize ysize xstep ystep )
*/
Value *image_stats_double(Script *script, char *proc_name,
			int call_data, int argc, Value **argv)
{
  double            *inbuf;
  double            *lineptr;
  int             ystep, ysize;
  register int    xstep, xsize;

  register double   *inptr;
  register double   value;
  register double   tmp;
  register double   mean;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, NULL);
  checkAddr(0, "buffer address");
  checkInt(1, "xsize");
  checkInt(2, "ysize");
  checkInt(3, "xstep");
  checkInt(4, "ystep");

  /*  Now extract the arguments from their Isis representation  */

  inbuf = (double *)getAddr( argv[0] );

  xsize = getInt( argv[1] );
  ysize = getInt( argv[2] );
  xstep = getInt( argv[3] );
  ystep = getInt( argv[4] );

  /*  Now calculate the desired statistics   */

  switch( call_data ) {
  case STATS_MEAN:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += (double)*inptr;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_MAX:
    value = DBL_MIN;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr > value )
	      value = *inptr;

	    inptr += xstep;
	  }
      }
    break;

  case STATS_MIN:
    value = DBL_MAX;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    if( *inptr < value )
	      value = *inptr;

	    inptr += xstep;
	  }
      }
    break;

  case STATS_ENERGY:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = *inptr;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }
    break;

  case STATS_VARIANCE:
    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += *inptr;
	    inptr += xstep;
	  }
      }

    if( (mean = value / (double)( xsize * ysize )) == 0.0 )
      {
	value = 0.0;
	break;
      }

    value = 0.0;
    lineptr = inbuf;
    for( y = 0; y < ysize; y++ )
      {
	inptr = lineptr;
	lineptr += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    tmp = *inptr - mean;
	    value += tmp * tmp;
	    inptr += xstep;
	  }
      }

    if( (xsize * ysize) > 0 )
      value = value / (double)( xsize * ysize );
    else
      value = 0.0;
    break;

  case STATS_SUM:
    value = 0.0;
    for( y = 0; y < ysize; y++ )
      {
	inptr = inbuf;
	inbuf += ystep;

	for( x = 0; x < xsize; x++ )
	  {
	    value += *inptr;
	    inptr += xstep;
	  }
      }
    break;

  default:
    fprintf(stderr, "** %s: Unknown stat function requested\n",
	    proc_name);
    return NullValue;
  }

  return( newReal( (real)value ) );
}
