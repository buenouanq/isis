/********************************************************
  Color Transformation Routines: RGB->YIQ, YIQ->RGB
 ********************************************************/
  
/********************************************************
  image-color-raw-rgb-to-yiq:

  Transforms RGB to luminance (Y), red-cyan (I) and green-magenta (Q):

  Y = 0.299 R + 0.587 G + 0.114 B
  I = 0.595 R - 0.274 G - 0.322 B
  Q = 0.211 R - 0.522 G + 0.311 B

  Note: To keep them in byte format (and in compliance with standard
  CCIR-601, the I and Q channels are mean-centered around 128, not 0.

  Jon Dakss and John Watlington, 11/23/98 
 ********************************************************/

#include <stdio.h>
#include <isis.h>

#include "isis_color.h"
#include "isis_color_private.h"

/*  (image-color-raw-rgb-to-yiq [xsize ysize] 
                                [red-buf grn-buf blu-buf]
				[inxstep inystep] 
                                [y-buf i-buf q-buf] 
				[outxstep outystep] )
*/

Value *image_color_raw_rgb_to_yiq(Script *script, char *proc_name,
				 int call_data, int argc, Value **argv)
  {
  int             xsize, ysize;
  byte            *redbuf, *grnbuf, *bluebuf;
  int             inxstep, inystep;
  byte            *ybuf, *ibuf, *qbuf;
  int             outxstep, outystep;

  register byte   *redptr, *grnptr, *blueptr;
  register byte   *yptr, *iptr, *qptr;

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

  ybuf = getAddr( Listitem(argv[3], 0) );
  ibuf = getAddr( Listitem(argv[3], 1) );
  qbuf = getAddr( Listitem(argv[3], 2) );

  outxstep = getInt( Listitem(argv[4], 0) );
  outystep = getInt( Listitem(argv[4], 1) );

  /*  Now calculate the chroma values for each sample of input data  */

  for( y = 0; y < ysize; y++ )
    {
    redptr = redbuf;   redbuf += inystep;
    grnptr = grnbuf;   grnbuf += inystep;
    blueptr = bluebuf; bluebuf += inystep;

    yptr = ybuf;      ybuf += outystep;
    iptr = ibuf;      ibuf += outystep;
    qptr = qbuf;      qbuf += outystep;

    for( x = 0; x < xsize; x++ )
      {
      /*   The standard equation for luminance (y) from red, green and
           blue is      y = [0.299, 0.587, 0.114] * [r, g, b]'         */

      *yptr = (byte) ((38 * (signed int)*redptr 
                     + 75 * (signed int)*grnptr 
                     + 15 * (signed int)*blueptr) >> 7);

      /*   The standard equation for I from red, green and blue is      
           i = [0.595, -0.274, -0.322] * [r, g, b]'         */

      *iptr = (byte) (((76 * (signed int)*redptr 
                      - 35 * (signed int)*grnptr 
                      - 41 * (signed int)*blueptr) >> 7) + 128);

      /*   The standard equation for Q from red, green and blue is      
           q = [0.211, -0.522, 0.311] * [r, g, b]'         */

      *qptr = (byte) (((27 * (signed int)*redptr 
                      - 67 * (signed int)*grnptr 
                      + 40 * (signed int)*blueptr) >> 7) + 128);

      redptr += inxstep;  grnptr += inxstep;  blueptr += inxstep;
      yptr += outxstep;   iptr += outxstep;   qptr += outxstep; 
      }
    }

  return NullValue;
  }

/*********************************************************
  image_color_raw_yiq_to_rgb:

  These transform from YIQ back to RGB.

  R = 1.000 Y + 0.958 I + 0.623 Q
  G = 1.000 Y - 0.272 I - 0.648 Q
  B = 1.000 Y - 1.105 I + 1.705 Q

  Note: This implicitly assumes the I and Q channels
  are centered around 128, not 0,

  Jon Dakss and John Watlington, 12/1/98
 *********************************************************/

/*  (image-color-raw-yiq-to-rgb [xsize ysize] 
                                [y-buf i-buf q-buf]
				[inxstep inystep] 
                                [red-buf grn-buf blue-buf] 
				[outxstep outystep] )
*/

Value *image_color_raw_yiq_to_rgb(Script *script, char *proc_name,
				 int call_data, int argc, Value **argv)
  {
  int             xsize, ysize;
  byte            *ybuf, *ibuf, *qbuf;
  int             inxstep, inystep;
  byte            *redbuf, *grnbuf, *bluebuf;
  int             outxstep, outystep;

  register byte   *yptr, *iptr, *qptr;
  register byte   *redptr, *grnptr, *blueptr;
  signed short    red, green, blue;

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

  ybuf = getAddr( Listitem(argv[1], 0) );
  ibuf = getAddr( Listitem(argv[1], 1) );
  qbuf = getAddr( Listitem(argv[1], 2) );

  inxstep = getInt( Listitem(argv[2], 0) );
  inystep = getInt( Listitem(argv[2], 1) );

  redbuf = getAddr( Listitem(argv[3], 0) );
  grnbuf = getAddr( Listitem(argv[3], 1) );
  bluebuf = getAddr( Listitem(argv[3], 2) );

  outxstep = getInt( Listitem(argv[4], 0) );
  outystep = getInt( Listitem(argv[4], 1) );

  /*  Now calculate the chroma values for each sample of input data  */

  for( y = 0; y < ysize; y++ )
    {
    yptr = ybuf;      ybuf += inystep;
    iptr = ibuf;      ibuf += inystep;
    qptr = qbuf;      qbuf += inystep;

    redptr = redbuf;    redbuf += outystep;
    grnptr = grnbuf;    grnbuf += outystep;
    blueptr = bluebuf;  bluebuf += outystep;

    for( x = 0; x < xsize; x++ )
      {
      /*   The standard equation for red from Y, I, Q is:
           R = [1.000, 0.958, 0.623] * [y, i, q]'         */

      red = (signed short)
            ((128 * (signed int)*yptr 
            + 123 * ((signed int)*iptr - 128) 
            + 80  * ((signed int)*qptr - 128)) >> 7);

      if(red < 0)        *redptr = 0;
      else if(red > 255) *redptr = 255; 
      else               *redptr = (byte) red;

      /*   The standard equation for green from Y, I, Q is:      
           G = [1.000, -0.272, -0.648] * [y, i, q]'         */ 

      green = (signed short)
              ((128 * (signed int)*yptr 
               - 35 * ((signed int)*iptr - 128) 
               - 83 * ((signed int)*qptr - 128)) >> 7);

      if(green < 0)        *grnptr = 0;
      else if(green > 255) *grnptr = 255; 
      else                 *grnptr = (byte) green;

      /*   The standard equation for blue from Y, I, Q is:      
           B = [1.000, -1.105, 1.705] * [y, i, q]'         */ 

      blue = (signed short)
             ((128 * (signed int)*yptr 
             - 141 * ((signed int)*iptr - 128)
             + 218 * ((signed int)*qptr - 128)) >> 7);

      if(blue < 0)        *blueptr = 0;
      else if(blue > 255) *blueptr = 255; 
      else                *blueptr = (byte) blue;

      yptr += inxstep;   iptr += inxstep;   qptr += inxstep; 
      redptr += outxstep;  grnptr += outxstep;  blueptr += outxstep;
      }
    }

  return NullValue;
  }
