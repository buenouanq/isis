/* 
   
   Image processing library 
   
   Dithering routines

   by Shawn Becker

   converted for this library by Stefan Agamanolis
   
   this file has functions that support:   
   grey buffer variable bit display dithering
   color buffer variable bit display dithering

*/

#include <math.h>
#include <stdio.h>

#include "isis_xwin_private.h"

/*----------------------------------------------------------------------*/
/* local defines */

#define POWER2(exp) (1L << (exp))
#define REGION_2_PIXEL(r,cd)   (r * (cd).data_div_reg)
#define PIXEL_2_REGION(p,cd)   (p * (cd).reg_div_data)
#define TRUE 1
#define FALSE 0


/*----------------------------------------------------------------------*/
/* prototypes for local functions */

static void InitDitherChannel(int dith, 
			      int data, 
			      DitherChannelStruct *dc);

static unsigned char DitherPixel(int x,
				 int y, 
				 unsigned char val,
				 DitherChannelStruct *dc);

static unsigned char DiffuseError(unsigned char *addr,
				  int skip, int stride,
				  int shift, int error_help,
				  int i, int j, int xsize, int ysize);

static void add_int(unsigned char *p, int val);


/* global functions */

/*----------------------------------------------------------------------*/
/* Set up to display 1 or 3 channels of 8 bits per channel data.
   dither_from_color = 0 if original image is greyscale,
   dither_to_color = 0 if you are dithering to greyscale,
   dither_bits = however many bits are available to dither to,
   lut will be filled with the colormap values that should be 
   installed on your screen*/

void InitDither(Dither *dith,              /* the structure to initialize */
		int dither_from_color,     /* going from color image? */
		int dither_to_color,       /* going to color image? */
		int dither_bits,         /* number of bits to dither to */
		unsigned char lut[256][3]) /* lut for the output buffer */
{
  register int i, j, r, g, b;
  
  /* set globals */
  dith->dither_from_color = dither_from_color;
  dith->dither_to_color = dither_to_color;
  dith->dither_bits = dither_bits;
  
  /* default lut to black */
  for(j = 0; j < 256; j++) for(i = 0; i < 3; i++) lut[j][i] = 0;
  
  if( dither_from_color && dither_to_color ) {

    /* We have 24 bit color data, we want to see color and want to
       dither over dither_bits.  */

    switch( dith->dither_bits ) {

    case 3:
      InitDitherChannel(1,8,&dith->dr);
      InitDitherChannel(1,8,&dith->dg);
      InitDitherChannel(1,8,&dith->db);
      break;

    case 4:
      InitDitherChannel(1,8,&dith->dr);
      InitDitherChannel(2,8,&dith->dg);
      InitDitherChannel(1,8,&dith->db);
      break;

    case 5:
      InitDitherChannel(2,8,&dith->dr);
      InitDitherChannel(2,8,&dith->dg);
      InitDitherChannel(1,8,&dith->db);
      break;

    case 6:
      InitDitherChannel(2,8,&dith->dr);
      InitDitherChannel(2,8,&dith->dg);
      InitDitherChannel(2,8,&dith->db);
      break;

    case 7:
      InitDitherChannel(2,8,&dith->dr);
      InitDitherChannel(3,8,&dith->dg);
      InitDitherChannel(2,8,&dith->db);
      break;

    case 8:
      InitDitherChannel(3,8,&dith->dr);
      InitDitherChannel(3,8,&dith->dg);
      InitDitherChannel(2,8,&dith->db);
      break;
    }
    
    /* initialize values used to index the color lut */
    dith->green_row_size = POWER2(dith->db.dither_bits);
    dith->red_plane_size = POWER2(dith->dg.dither_bits) * dith->green_row_size;
    
    /* loop thru all possible rgb regions */
    i = 0;
    for( r=0; r < POWER2(dith->dr.dither_bits); r++ ) {
      for( g=0; g < POWER2(dith->dg.dither_bits); g++ ) {
	for( b=0; b < POWER2(dith->db.dither_bits); b++ ) {
	  
	  lut[i][0] = r * 255 / (POWER2(dith->dr.dither_bits)-1);
	  lut[i][1] = g * 255 / (POWER2(dith->dg.dither_bits)-1);
	  lut[i][2] = b * 255 / (POWER2(dith->db.dither_bits)-1);
	  i++;
	}
      }
    }
  }
  else {
  
    /* We have 8 bit grey scale data and want to dither into dither_bits */
    
    InitDitherChannel( dith->dither_bits,8,&dith->dr);
    
    /* loop over all grey regions */
    for( i=0; i < POWER2(dith->dr.dither_bits); i++ ) {
      
      /* convert max region to max display value */
      lut[i][0] = lut[i][1] = lut[i][2] = 
	i * 255 / (POWER2(dith->dr.dither_bits)-1);
    }
  }
}


/*----------------------------------------------------------------------*/
/* This function is called after InitDither to dither the 1 or 3
   channels of data into the output buffer. The values left in the
   output buffer correspond to lut values returned by InitDither.  if
   dithering 1 channel greyscale images, gbuf and bbuf should be
   NULL.  */

void DitherData(Dither *dith,
		int dither_quality,
		int xsize,              /* image dimensions */
		int ysize,
		unsigned char *rbuf,    /* 8 bits/channel data */
		unsigned char *gbuf,
		unsigned char *bbuf,
		int inskip,
		int instride,
		unsigned char *out,     /* the output buffer */
		int outskip,    
		int outstride)
{
  register int i, j, r, g, b, indiff, outdiff;
  int sr, sg, sb;
  
  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);

  /* color dithering */
  
  if( dith->dither_from_color && dith->dither_to_color ) {
    
    sr = (dith->dr.data_bits - dith->dr.dither_bits);
    sg = (dith->dg.data_bits - dith->dg.dither_bits);
    sb = (dith->db.data_bits - dith->db.dither_bits);
    
    switch( dither_quality ) {
      
    case Decimate:
      
      for( j=0; j < ysize; j++ ) {
	for( i=0; i < xsize; i++ ) {
	  
	  r = *rbuf >> sr;
	  g = *gbuf >> sg;
	  b = *bbuf >> sb;
	  
	  *out = r * dith->red_plane_size + g * dith->green_row_size + b;

	  rbuf += inskip;
	  gbuf += inskip;
	  bbuf += inskip;
	  out += outskip;
	}
	rbuf += indiff;
	gbuf += indiff;
	bbuf += indiff;
	out += outdiff;
      }
      break;
      
    case OrderedDither:
      
      for( j=0; j < ysize; j++ ) {
	for( i=0; i < xsize; i++ ) {
	  
	  r = DitherPixel(i, j, *rbuf, &dith->dr);
	  g = DitherPixel(i, j, *gbuf, &dith->dg);
	  b = DitherPixel(i, j, *bbuf, &dith->db);
	  
	  *out = r * dith->red_plane_size + g * dith->green_row_size + b;
	  
	  rbuf += inskip;
	  gbuf += inskip;
	  bbuf += inskip;
	  out += outskip;
	}
	rbuf += indiff;
	gbuf += indiff;
	bbuf += indiff;
	out += outdiff;
      }
      break;

    case FloydSteinbergErrorDiffusion:
      
      for( j = 0; j < ysize; j++ ) {
	for( i = 0; i < xsize; i++ ) {
	  
	  r = DiffuseError(rbuf, inskip, instride, sr,
			   dith->dr.error_help, i, j, xsize, ysize);
	  g = DiffuseError(gbuf, inskip, instride, sg,
			   dith->dg.error_help, i, j, xsize, ysize);
	  b = DiffuseError(bbuf, inskip, instride, sb,
			   dith->db.error_help, i, j, xsize, ysize);
	  
	  *out = r * dith->red_plane_size + g * dith->green_row_size + b;
	  
	  rbuf += inskip;
	  gbuf += inskip;
	  bbuf += inskip;
	  out += outskip;
	}
	rbuf += indiff;
	gbuf += indiff;
	bbuf += indiff;
	out += outdiff;
      }
      break;

    default:
      
      fprintf(stderr,"DitherData - invalid dither_quality:%d\n", 
	      dither_quality);
      break;

    } /* switch */
  }

  /* grey dithering */

  else {
    sr = (dith->dr.data_bits - dith->dr.dither_bits);
     
    switch( dither_quality ) {

    case Decimate:

      for( j=0; j<ysize; j++ ) {
	for( i=0; i<xsize; i++ ) {
	  *out = *rbuf >> sr;
	  rbuf += inskip;
	  out += outskip;
	}
	rbuf += indiff;
	out += outdiff;
      }
      break;

    case OrderedDither:
    
      for( j=0; j<ysize; j++ ) {
	for( i=0; i<xsize; i++ ) {
	  *out = DitherPixel(i, j, *rbuf, &dith->dr);
	  rbuf += inskip;
	  out += outskip;
	}
	rbuf += indiff;
	out += outdiff;
      }
      break;

    case FloydSteinbergErrorDiffusion:

      for( j = 0; j < ysize; j++ ) {
	for( i = 0; i < xsize; i++ ) {
	  *out = DiffuseError(rbuf, inskip, instride, sr,
			      dith->dr.error_help, i, j, xsize, ysize);
	  rbuf += inskip;
	  out += outskip;
	}
	rbuf += indiff;
	out += outdiff;
      }
      break;

    default:

      fprintf(stderr,"DitherData - invalid dither_quality:%d\n", 
	      dither_quality);
      break;

    } /* switch */
  }
}


/*----------------------------------------------------------------------*/
/* This function can be used if you have a 24 bit display.  The r g b
   data for each pixel is interleaved in 4-byte units in the form
   0x00RRGGBB.  FOR THIS REASON, THE OUTPUT BUFFER MUST BE 4 TIMES THE
   SIZE OF AN INDIVIDUAL COLOR CHANNEL BUFFER.  If using only 1 color
   channel gbuf and bbuf should be NULL.  Use msb_flag for altering
   byte order.  Also, use 'red_in_lowest_byte = 1' you want the RED
   channel to be in the lowest byte of the 32-bit word, then GREEN
   then BLUE then 0---otherwise BLUE will be in the lowest byte, then
   GREEN then RED.  This function added by Stefan Agamanolis */

void InterleaveData(int xsize,               /* image dimensions */
		    int ysize,
		    unsigned char *r,        /* R */
		    unsigned char *g,        /* G */
		    unsigned char *b,        /* B */
		    int inskip,
		    int instride,
		    unsigned char *op,       /* usually 4X ORIG SIZE */
		    int outskip,             /* usually 4 */
		    int outstride,           /* usually 4 * xsize */
		    int msb_data,            /* flag for MSB ordering */
		    int red_in_lowest_byte)  /* flag for red in lowest byte */
{
  register int x, y, indiff, outdiff, outpixdiff;

  indiff = instride - (xsize * inskip);
  outdiff = outstride - (xsize * outskip);
  outpixdiff = outskip - 4;

  if(g == NULL || b == NULL) {  /* 1 color channel */
    for(y = 0; y < ysize; y++) {
      for(x = 0; x < xsize; x++) {
	*op++ = *r;
	*op++ = *r;
	*op++ = *r;
	*op++ = *r;
	r += inskip;
	op += outpixdiff;
      }
      r += indiff;
      op += outdiff;
    }
  }
  else {   /* interleave 3 color channels */
    if(msb_data) {
      if(red_in_lowest_byte) {
	for(y = 0; y < ysize; y++) {
	  for(x = 0; x < xsize; x++) {
	    *op++;
	    *op++ = *b;
	    b += inskip;
	    *op++ = *g;
	    g += inskip;
	    *op++ = *r;
	    r += inskip;
	    op += outpixdiff;
	  }
	  r += indiff;
	  g += indiff;
	  b += indiff;
	  op += outdiff;
	}
      }
      else {
	for(y = 0; y < ysize; y++) {
	  for(x = 0; x < xsize; x++) {
	    *op++;
	    *op++ = *r;
	    r += inskip;
	    *op++ = *g;
	    g += inskip;
	    *op++ = *b;
	    b += inskip;
	    op += outpixdiff;
	  }
	  r += indiff;
	  g += indiff;
	  b += indiff;
	  op += outdiff;
	}
      }
    }
    else {
      if(red_in_lowest_byte) {
	for(y = 0; y < ysize; y++) {
	  for(x = 0; x < xsize; x++) {
	    *op++ = *r;
	    r += inskip;
	    *op++ = *g;
	    g += inskip;
	    *op++ = *b;
	    b += inskip;
	    *op++;
	    op += outpixdiff;
	  }
	  r += indiff;
	  g += indiff;
	  b += indiff;
	  op += outdiff;
	}
      }
      else  {
	for(y = 0; y < ysize; y++) {
	  for(x = 0; x < xsize; x++) {
	    *op++ = *b;
	    b += inskip;
	    *op++ = *g;
	    g += inskip;
	    *op++ = *r;
	    r += inskip;
	    *op++;
	    op += outpixdiff;
	  }
	  r += indiff;
	  g += indiff;
	  b += indiff;
	  op += outdiff;
	}
      }
    }
  }
  
}    
  


/*----------------------------------------------------------------------*/
/* local functions
 */

/* given a desired maximum bits of display and bits 
 * of data to process, create the dither matrix that 
 * will be used for a channel 
 */

static void InitDitherChannel(int dither_bits,
			      int data_bits,
			      DitherChannelStruct *dc)
{
  int matrix_shades;  /* #shades possible with given matrix & dither bits */
  int x,y;            /* matrix position */
  int i,div,rem;      /* convert 0..matrix_shades-1 to base 4 */
  int pow,two_to_the_pow,four_to_the_pow;
  int sub[4][2];
  int norm;           /* the normalized matrix_shade */

  if( data_bits != 8 ) {
    fprintf(stderr, 
	    "InitDitherChannel ERROR: non 8 bit data not currently handled\n");
    exit(0);
  }
  
  /* init to zero in case of error */
  dc->dim = dc->size = dc->data_bits = dc->dither_bits = 0;
  dc->num_regions = dc->max_data = 0;

  /* have more display bits than we need */
  if( dither_bits >= data_bits ) {
    dc->dim = 0;
    dc->size = 1;
    dc->data_bits = dc->dither_bits = dither_bits;
    dc->num_regions = POWER2(dc->dither_bits);
    dc->num_data = POWER2(dc->data_bits);
    dc->max_data = dc->num_data - 1;
    dc->dither_matrix[0] = 0;
    dc->reg_div_data = dc->num_regions / dc->num_data;
    dc->data_div_reg = dc->num_data / dc->num_regions;
    dc->error_help = 255 / (POWER2(dc->dither_bits) - 1);
    return;
  }

  /* the root 2x2 dither submatrix */
  /* 0 3
     2 1
  */
  sub[0][0] = 0;  sub[0][1] = 0;
  sub[1][0] = 1;  sub[1][1] = 1;
  sub[2][0] = 0;  sub[2][1] = 1;
  sub[3][0] = 1;  sub[3][1] = 0;

  dc->dither_bits = dither_bits;    /* bits of display buffer */
  dc->data_bits = data_bits;    /* bits of data we desire to display */

  dc->num_data = POWER2(data_bits);
  dc->max_data = dc->num_data - 1;

  dc->num_regions = POWER2(dither_bits)-1;
  dc->dim = (int)
    floor(log((float)dc->max_data/(float)dc->num_regions)/log(4.0));
  dc->size = POWER2(dc->dim);
  matrix_shades = dc->size * dc->size;
  if( matrix_shades < 2 )
    matrix_shades = 2;

  /* calculate shades of the matrix */
  for( i=0; i<matrix_shades; i++ ) {
    x = y = 0;
    rem = i;
    for( pow=dc->dim-1; pow>=0; pow-- ) {
      two_to_the_pow = POWER2(dc->dim-1-pow);
      four_to_the_pow = POWER2(2*pow);
      div = rem/four_to_the_pow;
      rem = rem - div * four_to_the_pow;
      x += sub[div][0] * two_to_the_pow;
      y += sub[div][1] * two_to_the_pow;
    }

    /* normalize the matrix so that the largest value 
     * is num_data/num_regions rather than matrix_shades-1
     */
    norm = (i * dc->num_data / dc->num_regions) / (matrix_shades-1);
    dc->dither_matrix[y * dc->size + x] = norm;
  }

  /* to help later */
  dc->reg_div_data = dc->num_regions / dc->num_data;
  dc->data_div_reg = dc->num_data / dc->num_regions;
  dc->error_help = 255 / (POWER2(dc->dither_bits) - 1);
  /* make sure last normalized value of matrix is 1 less than max data 
   */
  if( norm > dc->max_data-1 )
    norm = dc->max_data-1;
  dc->dither_matrix[y * dc->size + x] = norm;
}


/* given a position, value and the dither channel returned a dithered value 
 * that is 0 <= v < POWER2(dc->dith) 
 */

static unsigned char DitherPixel(int x,int y, unsigned char val,
				 DitherChannelStruct *dc)
{
  unsigned char dith,base;
  
  /* convert pixel to a dither position */
  x = x % dc->size;
  y = y % dc->size;
  
  /* convert value to a region number */
  base = PIXEL_2_REGION(val, *dc);
  
  /* convert position and base region to a pixel value */
  dith = dc->dither_matrix[y * dc->size + x] + REGION_2_PIXEL(base, *dc);
  
  /* compare data value to the dither compare value */
  if( val > dith )
    base++;

  return( base);
}


static unsigned char DiffuseError(unsigned char *addr,
				  int skip, int stride,
				  int shift, int error_help,
				  int i, int j, int xsize, int ysize)
{
  unsigned char out, *p;
  int error;

  /* convert to color region */
  out = *addr >> shift;
  
  /* find error */
  error = (int)*addr - ((int)out * error_help);
  
  /* diffuse error to untouched neighbors */
  if(i < (xsize - 1)) 
    add_int(addr + skip, error * 0.4375); /* right neighbor */
  if(j < (ysize - 1)) {
    p = addr + stride;
    add_int(p - skip, error * 0.1875);	  /* left bottom neighbor */
    add_int(p,        error * 0.3125);	  /* bottom neighbor */
    add_int(p + skip, error * 0.0625);	  /* right bottom neighbor */
  }
  return(out);
}

static void add_int(unsigned char *p,int val)
{
  int new;
  
  new = (int)*p + val;
  if( new < 0 )   new = 0;
  if( new > 255 ) new = 255;
  *p = (unsigned char) new;
}


