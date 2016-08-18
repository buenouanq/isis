/********************************************************
  Color Transformation Routines
  
  These transform to and from normalized red and normalized blue.

  Y = 0.299 R + 0.587 G + 0.114 B
  r = R - Y
  b = B - Y

  J. Watlington, 12/8/96
  
*/

#include <stdio.h>
#include <isis.h>

#include "isis_color.h"
#include "isis_color_private.h"

/*  (image-color-raw-rgb-to-br  [xsize ysize] 
                                [red-buf grn-buf blu-buf]
				[inxstep inystep] 
				[b-buf r-buf] 
				[outxstep outystep] )
*/
Value *image_color_raw_rgb_to_br(Script *script, char *proc_name,
				 int call_data, int argc, Value **argv)
{
  int             xsize, ysize;
  byte            *redbuf, *grnbuf, *bluebuf;
  int             inxstep, inystep;
  signed short    *rbuf, *bbuf;
  int             outxstep, outystep;

  register byte   *redptr, *grnptr, *blueptr;
  register signed short *rptr, *bptr;
  register signed short red, blue;
  register signed short luma;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, "size, inbuf, instep, outbuf, outstep");
  checkIntList(0, "size");
  checkAddrList(1, "inbuf");
  checkIntList(2, "instep");
  checkAddrList(3, "outbuf")
  checkIntList(4, "outstep")

  /*  Now extract the arguments from their Isis representation  */

  xsize = getInt( Listitem(argv[0], 0) );
  ysize = getInt( Listitem(argv[0], 1) );

  redbuf = getAddr( Listitem(argv[1], 0) );
  grnbuf = getAddr( Listitem(argv[1], 1) );
  bluebuf = getAddr( Listitem(argv[1], 2) );

  inxstep = getInt( Listitem(argv[2], 0) );
  inystep = getInt( Listitem(argv[2], 1) );

  bbuf = getAddr( Listitem(argv[3], 0) );
  rbuf = getAddr( Listitem(argv[3], 1) );

  outxstep = getInt( Listitem(argv[4], 0) );
  outystep = getInt( Listitem(argv[4], 1) );

  /*  Now calculate the chroma values for each sample of input data  */

  for( y = 0; y < ysize; y++ )
    {
      redptr = redbuf;  redbuf += inystep;
      grnptr = grnbuf;  grnbuf += inystep;
      blueptr = bluebuf;  bluebuf += inystep;
      rptr = rbuf;      rbuf += outystep;
      bptr = bbuf;      bbuf += outystep;

      for( x = 0; x < xsize; x++ )
	{
	  red = (signed short)*redptr;
	  blue = (signed short)*blueptr;

	  /*   The standard equation for luminance (y) from red, green and
	       blue is      y = [0.299, 0.587, 0.114] * [r, g, b]'         */

	  luma = (38 * red + 75 * (signed short)*grnptr + 15 * blue) >> 7;
	  *rptr = red - luma;
	  *bptr = blue - luma;

	  redptr += inxstep;  grnptr += inxstep;  blueptr += inxstep;
	  rptr += outxstep;   bptr += outxstep;
	}
    }

  return NullValue;
}


/*  (image-color-raw-rgb-to-y  [xsize ysize] 
                                [red-buf grn-buf blu-buf]
				[inxstep inystep] 
				[y-buf] 
				[outxstep outystep] )
*/
Value *image_color_raw_rgb_to_y(Script *script, char *proc_name,
				int call_data, int argc, Value **argv)
{
  int             xsize, ysize;
  byte            *redbuf, *grnbuf, *bluebuf;
  int             inxstep, inystep;
  byte            *lumabuf;
  int             outxstep, outystep;

  register byte   *redptr, *grnptr, *blueptr;
  register byte   *lumaptr;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(5, "size, inbuf, instep, outbuf, outstep");
  checkIntList(0, "size");
  checkAddrList(1, "inbuf");
  checkIntList(2, "instep");
  checkAddrList(3, "outbuf")
  checkIntList(4, "outstep")

  /*  Now extract the arguments from their Isis representation  */

  xsize = getInt( Listitem(argv[0], 0) );
  ysize = getInt( Listitem(argv[0], 1) );

  redbuf = getAddr( Listitem(argv[1], 0) );
  grnbuf = getAddr( Listitem(argv[1], 1) );
  bluebuf = getAddr( Listitem(argv[1], 2) );

  inxstep = getInt( Listitem(argv[2], 0) );
  inystep = getInt( Listitem(argv[2], 1) );

  lumabuf = getAddr( Listitem(argv[3], 0) );

  outxstep = getInt( Listitem(argv[4], 0) );
  outystep = getInt( Listitem(argv[4], 1) );

  /*  Now calculate the luminance value for each sample of input data  */

  for( y = 0; y < ysize; y++ )
    {
      redptr = redbuf;  redbuf += inystep;
      grnptr = grnbuf;  grnbuf += inystep;
      blueptr = bluebuf;  bluebuf += inystep;
      lumaptr = lumabuf;  lumabuf += outystep;

      for( x = 0; x < xsize; x++ )
	{
	  /*   The standard equation for luminance (y) from red, green and
	       blue is      y = [0.299, 0.587, 0.114] * [r, g, b]'         */

	  *lumaptr = (byte)(( 38 * (signed int)*redptr
			   + 75 * (signed int)*grnptr
			   + 15 * (signed int)*blueptr) >> 7);

	  redptr += inxstep;  grnptr += inxstep;  blueptr += inxstep;
	  lumaptr += outxstep;
	}
    }

  return NullValue;
}


/*  (image-color-raw-ybr-to-rgb [xsize ysize] 
				[y-buf] 
				[yxstep yystep] 
				[b-buf r-buf] 
				[brxstep brystep] 
                                [red-buf grn-buf blu-buf]
				[outxstep outystep] )
*/
Value *image_color_raw_ybr_to_rgb(Script *script, char *proc_name,
				  int call_data, int argc, Value **argv)
{
  int             xsize, ysize;
  byte            *ybuf;
  int             yxstep, yystep;
  signed short    *rbuf, *bbuf;
  int             brxstep, brystep;
  byte            *redbuf, *grnbuf, *bluebuf;
  int             outxstep, outystep;

  register byte         *redptr, *grnptr, *blueptr, *yptr;
  register signed short red, blue;
  register signed short *rptr, *bptr;
  register signed short luma;

  register int    x;
  int             y;
  
  /*  First, check to see if the arguments are all there !   */

  checkCount(7, "size, ybuf, ystep, brbuf, brstep, outbuf, outstep");
  checkIntList(0, "size");
  checkAddrList(1, "ybuf");
  checkIntList(2, "ystep");
  checkAddrList(3, "brbuf")
  checkIntList(4, "brstep")
  checkAddrList(5, "outbuf")
  checkIntList(6, "outstep")

  /*  Now extract the arguments from their Isis representation  */

  xsize = getInt( Listitem(argv[0], 0) );
  ysize = getInt( Listitem(argv[0], 1) );

  ybuf = getAddr( Listitem(argv[1], 0) );

  yxstep = getInt( Listitem(argv[2], 0) );
  yystep = getInt( Listitem(argv[2], 1) );

  bbuf = getAddr( Listitem(argv[3], 0) );
  rbuf = getAddr( Listitem(argv[3], 1) );

  brxstep = getInt( Listitem(argv[4], 0) );
  brystep = getInt( Listitem(argv[4], 1) );

  redbuf = (byte *)getAddr( Listitem(argv[5], 0) );
  grnbuf = (byte *)getAddr( Listitem(argv[5], 1) );
  bluebuf = (byte *)getAddr( Listitem(argv[5], 2) );

  outxstep = getInt( Listitem(argv[6], 0) );
  outystep = getInt( Listitem(argv[6], 1) );

  /*  Now calculate the chroma values for each sample of input data  */

  for( y = 0; y < ysize; y++ )
    {
      rptr = rbuf;  rbuf += brystep;
      bptr = bbuf;  bbuf += brystep;
      yptr = ybuf;  ybuf += yystep;
      redptr = redbuf; redbuf += outystep;
      grnptr = grnbuf; grnbuf += outystep;
      blueptr = bluebuf; bluebuf += outystep;

      for( x = 0; x < xsize; x++ )
	{
	  luma = (signed short) *yptr;
	  
	  red = *rptr + luma;
	  if( red < 0 ) red = 0;
	  if( red > 255 ) red = 255;
	  *redptr = (byte)red;
	  
	  blue = *bptr + luma;
	  if( blue < 0 ) blue = 0;
	  if( blue > 255 ) blue = 255;
	  *blueptr = (byte)blue;
	  
	  red = (((luma << 7) - 65 * red) + ( 91 * luma - 25 * blue)) >> 7;
	  if( red < 0 ) red = 0;
	  if( red > 255 ) red = 255;
	  *grnptr = (byte)red;

	  redptr += outxstep; blueptr += outxstep; grnptr += outxstep;
	  yptr += yxstep;  rptr += brxstep;  bptr += brxstep;
	}
    }

  return NullValue;
}


