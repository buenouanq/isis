/* Background segmentation using variance
   Stefan Agamanolis, September 1998  */

#include <stdio.h>
#include <stdlib.h>
#include <isis.h>
#include "isis_seg_private.h"

/*----------------------------------------------------------------------*/
Value *isis_init_var_bkseg_table(Script *script, 
				 char *proc_name, int call_data, 
				 int argc, Value **argv)
{
  checkCount(4, "segtable, gain, bias, vbias");
  checkAddr(0, "segtable");
  checkReal(1, "gain");
  checkReal(2, "bias");
  checkReal(3, "vbias");
  
  init_var_bkseg_table( 
		       getAddr(argv[0]),
		       getReal(argv[1]),
		       getReal(argv[2]),
		       getReal(argv[3])
		       );
  
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *isis_var_bkseg_yuv(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  checkCount(18, "segtable framesize \n\t fyaddrs fysteps fuvaddrs fuvsteps \n\t byaddrs bysteps buvaddrs buvsteps \n\t vyaddrs vysteps vuvaddrs vuvsteps \n\t abufaddrs abufsteps hbufaddrs hbufsteps\n");
  checkAddr(0, "segtable");
  checkIntList(1, "framesize");
  checkAddrList(2, "fyaddrs");
  checkIntList(3, "fysteps");
  checkAddrList(4, "fuvaddrs");
  checkIntList(5, "fuvsteps");
  checkAddrList(6, "byaddrs");
  checkIntList(7, "bysteps");
  checkAddrList(8, "buvaddrs");
  checkIntList(9, "buvsteps");
  checkAddrList(10, "vyaddrs");
  checkIntList(11, "vysteps");
  checkAddrList(12, "vuvaddrs");
  checkIntList(13, "vuvsteps");
  checkAddrList(14, "abufaddrs");
  checkIntList(15, "abufsteps");
  checkAddrList(16, "hbufaddrs");
  checkIntList(17, "hbufsteps");

  var_bkseg_yuv( 
		getAddr(argv[0]),
		getInt(Listitem(argv[1],0)),
		getInt(Listitem(argv[1],1)),
		getAddr(Listitem(argv[2],0)),
		getInt(Listitem(argv[3],0)),
		getInt(Listitem(argv[3],1)),
		getAddr(Listitem(argv[4],0)),
		getAddr(Listitem(argv[4],1)),
		getInt(Listitem(argv[5],0)),
		getInt(Listitem(argv[5],1)),

		getAddr(Listitem(argv[6],0)),
		getInt(Listitem(argv[7],0)),
		getInt(Listitem(argv[7],1)),
		getAddr(Listitem(argv[8],0)),
		getAddr(Listitem(argv[8],1)),
		getInt(Listitem(argv[9],0)),
		getInt(Listitem(argv[9],1)),

		getAddr(Listitem(argv[10],0)),
		getInt(Listitem(argv[11],0)),
		getInt(Listitem(argv[11],1)),
		getAddr(Listitem(argv[12],0)),
		getAddr(Listitem(argv[12],1)),
		getInt(Listitem(argv[13],0)),
		getInt(Listitem(argv[13],1)),

		getAddr(Listitem(argv[14],0)),
		getInt(Listitem(argv[15],0)),
		getInt(Listitem(argv[15],1)),
		getAddr(Listitem(argv[16],0)),
		getInt(Listitem(argv[17],0)),
		getInt(Listitem(argv[17],1))
		);

  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *isis_var_bkseg_rgb(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  checkCount(10, "segtable framesize \n\t faddrs fsteps \n\t baddrs bsteps \n\t vaddrs vsteps \n\t abufaddrs abufsteps \n");
  checkAddr(0, "segtable");
  checkIntList(1, "framesize");
  checkAddrList(2, "faddrs");
  checkIntList(3, "fsteps");
  checkAddrList(4, "baddrs");
  checkIntList(5, "bsteps");
  checkAddrList(6, "vaddrs");
  checkIntList(7, "vsteps");
  checkAddrList(8, "abufaddrs");
  checkIntList(9, "abufsteps");
 

  var_bkseg_rgb( 
		getAddr(argv[0]),
		getInt(Listitem(argv[1],  0)),
		getInt(Listitem(argv[1],  1)),
		getAddr(Listitem(argv[2], 0)),
		getAddr(Listitem(argv[2], 1)),
		getAddr(Listitem(argv[2], 2)),
		getInt(Listitem(argv[3],  0)),
		getInt(Listitem(argv[3],  1)),
		getAddr(Listitem(argv[4], 0)),
		getAddr(Listitem(argv[4], 1)),
		getAddr(Listitem(argv[4], 2)),
		getInt(Listitem(argv[5],  0)),
		getInt(Listitem(argv[5],  1)),
		getAddr(Listitem(argv[6], 0)),
		getAddr(Listitem(argv[6], 1)),
		getAddr(Listitem(argv[6], 2)),
		getInt(Listitem(argv[7],  0)),
		getInt(Listitem(argv[7],  1)),
		getAddr(Listitem(argv[8], 0)),
		getInt(Listitem(argv[9],  0)),
		getInt(Listitem(argv[9],  1))
	
		);

  return NullValue;
}


/*----------------------------------------------------------------------*/
void init_var_bkseg_table(unsigned char *segtable, 
			  double gain, 
			  double bias,
			  double vbias)
{
  int var, diff;
  double val, dvar;
  unsigned char *out, outval;
  
  out = segtable;
  
  for(var = 0; var < 256; var++) {
    for(diff = -256; diff < 256; diff++) {
      dvar = ((double) var) + vbias;
      if(dvar != 0.0) val = bias + (gain * 256 * ((double) abs(diff)) / dvar);
      else if(diff == 0.0) val = 0.0;
      else val = 255.0;
      if(val >= 255.0) outval = 255;
      else if (val <= 0.0) outval = 0;
      else outval = (unsigned char) val;
      *out++ = outval;
    }
  }
}

/*----------------------------------------------------------------------*/
void var_bkseg_yuv(unsigned char *segtable,
		   int xsize, int ysize,
		   unsigned char *fybuf, 
		   int fy_xstep, int fy_ystep,
		   unsigned char *fubuf, 
		   unsigned char *fvbuf, 
		   int fuv_xstep, int fuv_ystep,
		   unsigned char *bybuf, 
		   int by_xstep, int by_ystep,
		   unsigned char *bubuf, 
		   unsigned char *bvbuf,
		   int buv_xstep, int buv_ystep,
		   unsigned char *vybuf, 
		   int vy_xstep, int vy_ystep,
		   unsigned char *vubuf, 
		   unsigned char *vvbuf,
		   int vuv_xstep, int vuv_ystep,
		   unsigned char *abuf, 
		   int a_xstep, int a_ystep,
		   unsigned char *hbuf, 
		   int h_xstep, int h_ystep)
{
  int x, y;
  unsigned char *fyptr, *fuptr, *fvptr, *byptr, *buptr, *bvptr;
  unsigned char *vyptr, *vuptr, *vvptr, *aptr, *hptr;
  register int alpha1, alpha2;
  
  for(y = 0; y < ysize; y++) {

    fyptr = fybuf; fuptr = fubuf; fvptr = fvbuf;
    byptr = bybuf; buptr = bubuf; bvptr = bvbuf;
    vyptr = vybuf; vuptr = vubuf; vvptr = vvbuf;
    aptr = abuf; hptr = hbuf;
    
    for(x = 0; x < xsize; x += 2) {
      
      alpha1 = *(segtable + (*vuptr << 9) + 256 + (*fuptr) - (*buptr));  /* chroma U */
      alpha2 = *(segtable + (*vvptr << 9) + 256 + (*fvptr) - (*bvptr));  /* chroma V */
      if (alpha2 > alpha1) alpha1 = alpha2;   /* max of 2 chroma alphas */
      
      fuptr += fuv_xstep;
      buptr += buv_xstep;
      vuptr += vuv_xstep;
      fvptr += fuv_xstep;
      bvptr += buv_xstep;
      vvptr += vuv_xstep;

      alpha2 = *(segtable + (*vyptr << 9) + 256 + (*fyptr) - (*byptr));  /* luma 1 */
      *aptr = (alpha1 > alpha2) ? alpha1 : alpha2;  /* max of this and chroma alpha */
      
      fyptr += fy_xstep;
      byptr += by_xstep;
      vyptr += vy_xstep;
      aptr += a_xstep;
      
      alpha2 = *(segtable + (*vyptr << 9) + 256 + (*fyptr) - (*byptr));  /* luma 2 */
      *aptr = (alpha1 > alpha2) ? alpha1 : alpha2;  /* max of this and chroma alpha */
      
      fyptr += fy_xstep;
      byptr += by_xstep;
      vyptr += vy_xstep;
      aptr += a_xstep;

    }
  
    fybuf += fy_ystep;
    fubuf += fuv_ystep;
    fvbuf += fuv_ystep;
    bybuf += by_ystep;
    bubuf += buv_ystep;
    bvbuf += buv_ystep;
    vybuf += vy_ystep;
    vubuf += vuv_ystep;
    vvbuf += vuv_ystep;
    abuf += a_ystep;
    hbuf += h_ystep;
  }
}

/*----------------------------------------------------------------------*/
void var_bkseg_rgb(unsigned char *segtable,
		   int xsize, int ysize,
		   unsigned char *frbuf, 
		   unsigned char *fgbuf, 
		   unsigned char *fbbuf, 
		   int f_xstep, int f_ystep,
		   unsigned char *brbuf, 
		   unsigned char *bgbuf, 
		   unsigned char *bbbuf, 
		   int b_xstep, int b_ystep,
		   unsigned char *vrbuf, 
		   unsigned char *vgbuf,
		   unsigned char *vbbuf, 
		   int v_xstep, int v_ystep,
		   unsigned char *abuf, 
		   int a_xstep, int a_ystep)
		   
{
  int x, y;
  unsigned char *frptr, *fgptr, *fbptr, *brptr, *bgptr, *bbptr;
  unsigned char *vrptr, *vgptr, *vbptr, *aptr;
  register int alpha0, alpha1, alpha2;
  
  for(y = 0; y < ysize; y++) {

    frptr = frbuf; fgptr = fgbuf; fbptr = fbbuf;
    brptr = brbuf; bgptr = bgbuf; bbptr = bbbuf;
    vrptr = vrbuf; vgptr = vgbuf; vbptr = vbbuf;
    aptr = abuf; 
    
    for(x = 0; x < xsize; x ++) {
      
      alpha0 = *(segtable + (*vrptr << 9) + 256 + (*frptr) - (*brptr));
      alpha1 = *(segtable + (*vgptr << 9) + 256 + (*fgptr) - (*bgptr));
      alpha2 = *(segtable + (*vbptr << 9) + 256 + (*fbptr) - (*bbptr));
      if (alpha1 > alpha0) alpha0 = alpha1;
      if (alpha2 > alpha0) alpha0 = alpha2;
      
      frptr += f_xstep;
      brptr += b_xstep;
      vrptr += v_xstep;
      fgptr += f_xstep;
      bgptr += b_xstep;
      vgptr += v_xstep;
      fbptr += f_xstep;
      bbptr += b_xstep;
      vbptr += v_xstep;

      *aptr = (alpha0);
      
      aptr += a_xstep;

    }
  
    frbuf += f_ystep;
    fgbuf += f_ystep;
    fbbuf += f_ystep;
    brbuf += b_ystep;
    bgbuf += b_ystep;
    bbbuf += b_ystep;
    vrbuf += v_ystep;
    vgbuf += v_ystep;
    vbbuf += v_ystep;
    abuf += a_ystep;
    
  }
}
