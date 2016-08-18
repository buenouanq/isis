/* Background difference image segmentation
   J. Watlington, 3/10/97

   cleaned up by stefan, 3/25/98 
*/

#include <stdio.h>
#include <isis.h>
#include "isis_seg_private.h"

#undef DEBUG_SEG

#define LUMA_MIN_TOL  2   /*  This difference will always be tolerated... */

/*----------------------------------------------------------------------*/
Value *isis_init_bkseg_yuv(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  checkCount(11, "byaddrs bysize bysteps buvaddrs buvsteps \n\t luma-minus luma-plus chroma-u chroma-v chroma-core segmaps\n");
  checkAddrList(0, "byaddrs");
  checkIntList(1, "bysize");
  checkIntList(2, "bysteps");
  checkAddrList(3, "buvaddrs");
  checkIntList(4, "buvsteps");
  checkInt(5, "luma-minus");
  checkInt(6, "luma-plus");
  checkInt(7, "chroma-u");
  checkInt(8, "chroma-v");
  checkInt(9, "chroma-core");
  checkAddr(10, "segmaps");

  init_bkseg_yuv( 
		 getAddr(Listitem(argv[0],0)),
		 getInt(Listitem(argv[1],0)),
		 getInt(Listitem(argv[1],1)),
		 getInt(Listitem(argv[2],0)),
		 getInt(Listitem(argv[2],1)),
		 getAddr(Listitem(argv[3],0)),
		 getAddr(Listitem(argv[3],1)),
		 getInt(Listitem(argv[4],0)),
		 getInt(Listitem(argv[4],1)),
		 getInt(argv[5]),
		 getInt(argv[6]),
		 getInt(argv[7]),
		 getInt(argv[8]),
		 getInt(argv[9]),
		 getAddr(argv[10]) 
		 );

  return NullValue;
}


/*----------------------------------------------------------------------*/
Value *isis_bkseg_yuv(Script *script, 
		      char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  checkCount(12, "fyaddrs fbufsize fysteps fuvaddrs \n\t fuvsteps byaddrs bysteps buvaddrs buvsteps \n\t segmaps abufaddrs abufsteps\n");
  checkAddrList(0, "fyaddrs");
  checkIntList(1, "fbufsize");
  checkIntList(2, "fysteps");
  checkAddrList(3, "fuvaddrs");
  checkIntList(4, "fuvsteps");
  checkAddrList(5, "byaddrs");
  checkIntList(6, "bysteps");
  checkAddrList(7, "buvaddrs");
  checkIntList(8, "buvsteps");
  checkAddr(9, "segmaps");
  checkAddrList(10, "abufaddrs");
  checkIntList(11, "abufsteps");

  bkseg_yuv( 
	    getAddr(Listitem(argv[0],0)),
	    getInt(Listitem(argv[1],0)),
	    getInt(Listitem(argv[1],1)),
	    getInt(Listitem(argv[2],0)),
	    getInt(Listitem(argv[2],1)),
	    getAddr(Listitem(argv[3],0)),
	    getAddr(Listitem(argv[3],1)),
	    getInt(Listitem(argv[4],0)),
	    getInt(Listitem(argv[4],1)),
	    getAddr(Listitem(argv[5],0)),
	    getInt(Listitem(argv[6],0)),
	    getInt(Listitem(argv[6],1)),
	    getAddr(Listitem(argv[7],0)),
	    getAddr(Listitem(argv[7],1)),
	    getInt(Listitem(argv[8],0)),
	    getInt(Listitem(argv[8],1)),
	    getAddr(argv[9]),
	    getAddr(Listitem(argv[10],0)),
	    getInt(Listitem(argv[11],0)),
	    getInt(Listitem(argv[11],1))
	    );

  return NullValue;
}

/*----------------------------------------------------------------------*/
void init_bkseg_yuv(unsigned char *ybuf,
		    int bufsize_x,
		    int bufsize_y,
		    int ystep_x,
		    int ystep_y,
		    unsigned char *ubuf,
		    unsigned char *vbuf,
		    int uvstep_x,
		    int uvstep_y,
		    int luma_plus,
		    int luma_minus,
		    int chroma_u,
		    int chroma_v,
		    int chroma_core,
		    unsigned char *segmaps)
{
  int  i, j;
  unsigned char *yptr;
  unsigned char *uptr;
  unsigned char *vptr;
  unsigned char *params = segmaps;

  float    temp;
  float    chroma_border;
  float    chroma_magnitude;
  float    luma_plus_frac = ((float)(luma_plus - LUMA_MIN_TOL)) / 255.0F;
  float    luma_minus_frac = ((float)(luma_minus - LUMA_MIN_TOL)) / 255.0F;
  
#ifdef DEBUG_SEG
  fprintf( stderr, "init_bkseg_yuv: 0x%x %dx%d %d,%d 0x%x 0x%x %d,%d\nl+ %d  l- %d  u %d  v %d  cc %d 0x%x\n",
	  ybuf, bufsize_x, bufsize_y, ystep_x, ystep_y, ubuf, vbuf,
	  uvstep_x, uvstep_y, luma_plus, luma_minus, chroma_u, chroma_v,
	  chroma_core, segmaps);
#endif

  chroma_border = (float)(chroma_core * chroma_core);

  for( j = 0; j < bufsize_y; j++ ) {

    yptr = ybuf;
    uptr = ubuf;
    vptr = vbuf;

    ybuf += ystep_y;
    ubuf += uvstep_y;
    vbuf += uvstep_y;

    for( i = 0; i < bufsize_x; i += 2 ) {

      /*  Compute a chroma magnitude to use for next two pels. */

      temp = (float)(*uptr);
      temp = 128.0F - temp;
      chroma_magnitude = temp * temp;
      temp = (float)(*vptr);
      temp = 128.0F - temp;
      chroma_magnitude += temp * temp;

      /*  We are going through the background image, writing
	  four parameters out for each pixel.   */

      temp = (float)*yptr;
      *(params++) = (unsigned char)( (255.0 - temp) * luma_plus_frac )
	              + LUMA_MIN_TOL;
      *(params++) = (unsigned char)( temp * luma_minus_frac )
	              + LUMA_MIN_TOL;

      /*  If the chroma levels are low, we are less sensitive to
	  chroma difference....   */

      if( chroma_magnitude < chroma_border) {
	*(params++) = (unsigned char)(chroma_u * 2);
	*(params++) = (unsigned char)(chroma_v * 2);
      } else {
	*(params++) = (unsigned char)chroma_u;
	*(params++) = (unsigned char)chroma_v;
      }

      yptr += ystep_x;

      /*  process the next luma sample...   */

      temp = (float)*yptr;
      *(params++) = (unsigned char)( (255.0 - temp) * luma_plus_frac )
	              + LUMA_MIN_TOL;
      *(params++) = (unsigned char)( temp * luma_minus_frac )
	              + LUMA_MIN_TOL;

      yptr += ystep_x;
      uptr += uvstep_x;
      vptr += uvstep_x;
    }
  }
}

#define NUM_SEGMENT_ALPHA_STEPS  6
static int segment_alpha_table[ NUM_SEGMENT_ALPHA_STEPS ] =
{ 226, 190, 156, 120, 80, 40 };

/*----------------------------------------------------------------------*/
void bkseg_yuv(unsigned char *fybuf, 
	       int dim_x, int dim_y,
	       int fstep_x, int fstep_y,
	       unsigned char *fubuf, 
	       unsigned char *fvbuf, 
	       int fuvstep_x, int fuvstep_y,
	       unsigned char *bybuf, 
	       int bstep_x, int bstep_y,
	       unsigned char *bubuf, 
	       unsigned char *bvbuf,
	       int buvstep_x, int buvstep_y,
	       unsigned char *segmaps,
	       unsigned char *abuf, 
	       int astep_x, int astep_y)
{
  int i, j;
  unsigned char *fyptr, *fuptr, *fvptr, *byptr, *buptr, *bvptr, *aptr;
  unsigned char *param_ptr = segmaps;
  int luma_minus, luma_plus, chroma_u, chroma_v;
  register signed int diff, alpha, temp, nalpha;
  
  for( j = 0; j < dim_y; j++ ){

    fyptr=fybuf; fuptr=fubuf; fvptr=fvbuf;
    byptr=bybuf; buptr=bubuf; bvptr=bvbuf;
    aptr=abuf;

    fybuf += fstep_y;
    fubuf += fuvstep_y;
    fvbuf += fuvstep_y;
    bybuf += bstep_y;
    bubuf += buvstep_y;
    bvbuf += buvstep_y;
    abuf += astep_y;

    for( i = 0; i < dim_x; i += 2 ){

      /*  Read in precalculated segmentation parameters  */

      luma_plus = (int)*(param_ptr++);
      luma_minus = (int)*(param_ptr++);
      chroma_u = (int)*(param_ptr++);
      chroma_v = (int)*(param_ptr++);

      /*  The basic idea is to take a distance measurement along each axis,
	  and scale the result using "per sample" parameters.    */

      /*  Calculate chroma diff  */

      diff = (int)(*fuptr) - (int)(*buptr);
      if(diff < 0)
	temp = - ( diff + chroma_u );
      else
	temp = diff - chroma_u;

      if( temp < 0)
	alpha = 255;
      else if( temp >= NUM_SEGMENT_ALPHA_STEPS )
	alpha = 0;
      else
	alpha = segment_alpha_table[ temp ];
      

      diff = (int)(*fvptr) - (int)(*bvptr);
      if(diff < 0)
	temp = - ( diff + chroma_v );
      else
	temp = diff - chroma_v;

      if( temp < 0)
	alpha *= 256;
      else if( temp >= NUM_SEGMENT_ALPHA_STEPS )
	alpha = 0;
      else
	alpha *= segment_alpha_table[ temp ];
      
      if( alpha == 0 ) {

	*aptr = 255;
	param_ptr += 2;
	aptr += astep_x; fyptr += fstep_x; byptr += bstep_x;
	*aptr = 255;

      } else {

	/*  Calculate the luma difference, for
	    first of two luminance pels...  */
	
	diff = (int)(*fyptr) - (int)(*byptr);
	if(diff < 0) {
	  temp = - ( diff + luma_minus );
	} else {
	  temp = diff - luma_plus;
	}
	
	if( temp < 0)
	  nalpha = alpha >> 8;
	else if( temp >= NUM_SEGMENT_ALPHA_STEPS )
	  nalpha = 0;
	else
	  nalpha = (segment_alpha_table[ temp ] * alpha) >> 16;
	
	*aptr = (unsigned char)(255 - nalpha);
	
	fyptr += fstep_x; byptr += bstep_x; aptr += astep_x;
	
	/*  The second luminance pel uses the same chroma as the first */
	/*  Read in another set of precalculated segmentation parameters  */
	
	luma_plus = (int)*(param_ptr++);
	luma_minus = (int)*(param_ptr++);
	
	diff = (int)(*fyptr) - (int)(*byptr);
	if(diff < 0) {
	  temp = - ( diff + luma_minus );
	} else {
	  temp = diff - luma_plus;
	}
	
	if( temp < 0)
	  nalpha = alpha >> 8;
	else if( temp >= NUM_SEGMENT_ALPHA_STEPS )
	  nalpha = 0;
	else
	  nalpha = (segment_alpha_table[ temp ] * alpha) >> 16;
	
	*aptr = (unsigned char)(255 - nalpha);
      }      

      fyptr += fstep_x; byptr += bstep_x; aptr += astep_x;
      fuptr += fuvstep_x; fvptr += fuvstep_x;
      buptr += buvstep_x; bvptr += buvstep_x;
    }
  }
}

