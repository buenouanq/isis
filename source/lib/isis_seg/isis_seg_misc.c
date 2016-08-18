/* Miscellaneous segmentation utilities */
/* Stefan Agamanolis, September 1998 */
/* Cian Cullinan contributed to this library */

#include <stdlib.h>
#include <stdio.h>
#include <isis.h>
#include "isis_seg_private.h"

/* ------------------------------------------------------------ */
Value *isis_fill_holes(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  checkCount(10, NULL); 
  checkInt(0, "edgethresh");
  checkInt(1, "holethresh");
  checkInt(2, "maxpixels");
  checkInt(3, "fillval");
  checkBool(4, "fillsidesflag");
  checkIntList(5, "size");
  checkAddrList(6, "inbufs");
  checkIntList(7, "insteps");
  checkAddrList(8, "outbufs");
  checkIntList(9, "outsteps");
  
  fill_holes(
	     getInt(argv[0]),
	     getInt(argv[1]),
	     getInt(argv[2]),
	     getInt(argv[3]),
	     getBool(argv[4]),
	     getInt(Listitem(argv[5], 0)),
	     getInt(Listitem(argv[5], 1)),
	     getAddr(Listitem(argv[6], 0)),
	     getInt(Listitem(argv[7], 0)),
	     getInt(Listitem(argv[7], 1)),
	     getAddr(Listitem(argv[8], 0)),
	     getInt(Listitem(argv[9], 0)),
	     getInt(Listitem(argv[9], 1))
	     );
  
  return NullValue;
}

/*----------------------------------------------------------------------*/
void fill_holes(int edgethresh,
		int holethresh,
		int maxpixels,
		int fillval,
		int fillsides,
		int xsize,
		int ysize,
		unsigned char *inbuf, 
		int inskip,
		int instride,
		unsigned char *outbuf, 
		int outskip,
		int outstride)
{
  unsigned char *in, *out, *temp;
  int x, y;
  int hole, holeval, pix, numpixels;
  
  for(y = 0; y < ysize; y++) {
    hole = 0;
    holeval = 0;
    numpixels = 0;

    in = temp = inbuf;
    out = outbuf;
    
    if(fillsides) x = 0;
    else {   /* find first edge pixel so side isn't filled to border */
      for(x = 0; *in < edgethresh && x < xsize; x++) {
	*out = *in;
	in += inskip;
	temp += inskip;
	out += inskip;
      }
    }
      
    for(; x < xsize; x++) {
      pix = *in;

      if(pix < edgethresh) {     /* we are in a hole */
	hole = 1;
	holeval += (255 - pix);  /* keep track of hole statistics */
	numpixels++;
      }
      else {         /* we are on an edge */
	if(hole) {   /* if we were in a hole before */
	  if(numpixels <= maxpixels && holeval < holethresh) {
	    while(temp < in) {   /* fill hole */
	      *out = fillval; 
	      temp += inskip; 
	      out += outskip; 
	    } 
	  }
	  else { 
	    while(temp < in) {   /* leave hole alone */
	      *out = *temp; 
	      temp += inskip;
	      out += outskip;
	    }
	  }
	  hole = 0;       /* reset hole statistics */
	  holeval = 0;	  
	  numpixels = 0;
	}
	*out = fillval;   /* change edge to fillval */
	temp += inskip;
	out += outskip;
      }
      
      in += inskip;
    }
	
    if(fillsides && hole) {  /* if we were in a hole at the end */
      if(numpixels <= maxpixels && holeval < holethresh) {
	while(temp < in) {   /* fill hole */
	  *out = fillval; 
	  temp += inskip; 
	  out += outskip; 
	} 
      }
      else { 
	while(temp < in) {   /* leave hole alone */
	  *out = *temp; 
	  temp += inskip;
	  out += outskip;
	}
      }
    }
    
    inbuf += instride;
    outbuf += outstride;
  }
}



/* ------------------------------------------------------------ */
Value *isis_init_encode_image_yuv(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv)
{
  checkCount(4, NULL); 
  checkInt(0, "ybits");
  checkInt(1, "ubits");
  checkInt(2, "vbits");
  checkInt(3, "abits");
  
  init_encode_image_yuv(
			getInt(argv[0]),
			getInt(argv[1]),
			getInt(argv[2]),
			getInt(argv[3])
			);
  
  return NullValue;
  
}

//CIAN
/* ------------------------------------------------------------ */
Value *isis_init_encode_image_rgb(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv)
{
  checkCount(4, NULL); 
  checkInt(0, "rbits");
  checkInt(1, "gbits");
  checkInt(2, "bbits");
  checkInt(3, "abits");
  
  init_encode_image_rgb(
			getInt(argv[0]),
			getInt(argv[1]),
			getInt(argv[2]),
			getInt(argv[3])
			);
  
  return NullValue;
  
}


/* ------------------------------------------------------------ */
Value *isis_encode_image_yuv(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  checkCount(8, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "ybufs");
  checkIntList(2, "ysteps");
  checkAddrList(3, "uvbufs");
  checkIntList(4, "uvsteps");
  checkAddrList(5, "abufs");
  checkIntList(6, "asteps");
  checkAddr(7, "outbuf");
  
  return newInt( encode_image_yuv(
				  getInt(Listitem(argv[0], 0)),
				  getInt(Listitem(argv[0], 1)),
				  getAddr(Listitem(argv[1], 0)),
				  getInt(Listitem(argv[2], 0)),
				  getInt(Listitem(argv[2], 1)),
				  getAddr(Listitem(argv[3], 0)),
				  getAddr(Listitem(argv[3], 1)),
				  getInt(Listitem(argv[4], 0)),
				  getInt(Listitem(argv[4], 1)),
				  getAddr(Listitem(argv[5], 0)),
				  getInt(Listitem(argv[6], 0)),
				  getInt(Listitem(argv[6], 1)),
				  getAddr(argv[7])
				  ) );
}

//CIAN
/* ------------------------------------------------------------ */
Value *isis_encode_image_rgb(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  checkCount(6, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "bufs");
  checkIntList(2, "steps");
  checkAddrList(3, "abufs");
  checkIntList(4, "asteps");
  checkAddr(5, "outbuf");
  
  return newInt( encode_image_rgb(
				  getInt(Listitem(argv[0], 0)),
				  getInt(Listitem(argv[0], 1)),
				  getAddr(Listitem(argv[1], 0)),
				  getAddr(Listitem(argv[1], 1)),
				  getAddr(Listitem(argv[1], 2)),
				  getInt(Listitem(argv[2], 0)),
				  getInt(Listitem(argv[2], 1)),
				  getAddr(Listitem(argv[3], 0)),
				  getInt(Listitem(argv[4], 0)),
				  getInt(Listitem(argv[4], 1)),
				  getAddr(argv[5])
				  ) );
}


static unsigned char *ytable = NULL, *utable = NULL, *vtable = NULL, *atable = NULL;
static int enc_ybits, enc_ubits, enc_vbits, enc_abits;
//CIAN
static unsigned char *rtable = NULL, *gtable = NULL, *btable = NULL;
static int enc_rbits, enc_gbits, enc_bbits, enc_abits;

/*----------------------------------------------------------------------*/
void init_encode_image_yuv(int ybits,
			   int ubits,
			   int vbits,
			   int abits)
{
  int yregs, uregs, vregs, aregs, i;  

  enc_ybits = ybits;
  enc_ubits = ubits;
  enc_vbits = vbits;
  enc_abits = abits;

  yregs = 1 << ybits;
  uregs = 1 << ubits;
  vregs = 1 << vbits;
  aregs = 1 << abits;

  if(ytable) free(ytable);
  if(utable) free(utable);
  if(vtable) free(vtable);
  if(atable) free(atable);

  if(ybits) {
    ytable = (unsigned char *) malloc(yregs);
    utable = (unsigned char *) malloc(uregs);
    vtable = (unsigned char *) malloc(vregs);
    atable = (unsigned char *) malloc(aregs);
    
    for(i = 0; i < yregs; i++) ytable[i] = (255 * i) / (yregs - 1);
    for(i = 0; i < uregs; i++) utable[i] = (255 * i) / (uregs - 1);
    for(i = 0; i < vregs; i++) vtable[i] = (255 * i) / (vregs - 1);
    for(i = 0; i < aregs; i++) atable[i] = (255 * i) / (aregs - 1);
  }
}

//CIAN
/* ------------------------------------------------------------ */
void init_encode_image_rgb(int rbits,
			   int gbits,
			   int bbits,
			   int abits)
{
  int rregs, gregs, bregs, aregs, i;  

  enc_rbits = rbits;
  enc_gbits = gbits;
  enc_bbits = bbits;
  enc_abits = abits;

  rregs = 1 << rbits;
  gregs = 1 << gbits;
  bregs = 1 << bbits;
  aregs = 1 << abits;

  if(rtable) free(rtable);
  if(gtable) free(gtable);
  if(btable) free(btable);
  if(atable) free(atable);

  if(rbits) {
    rtable = (unsigned char *) malloc(rregs);
    gtable = (unsigned char *) malloc(gregs);
    btable = (unsigned char *) malloc(bregs);
    atable = (unsigned char *) malloc(aregs);
    
    for(i = 0; i < rregs; i++) rtable[i] = (255 * i) / (rregs - 1);
    for(i = 0; i < gregs; i++) gtable[i] = (255 * i) / (gregs - 1);
    for(i = 0; i < bregs; i++) btable[i] = (255 * i) / (bregs - 1);
    for(i = 0; i < aregs; i++) atable[i] = (255 * i) / (aregs - 1);
  }
}

/*----------------------------------------------------------------------*/
int encode_image_yuv(int xsize, int ysize,
		     unsigned char *ybuf, 
		     int y_xstep, int y_ystep,
		     unsigned char *ubuf, 
		     unsigned char *vbuf, 
		     int uv_xstep, int uv_ystep,
		     unsigned char *abuf, 
		     int a_xstep, int a_ystep,
		     unsigned char *outbuf)
{
  register unsigned char *yptr, *uptr, *vptr, *aptr, *optr, *ibuf, *iptr;
  int ydiff, uvdiff, adiff;
  int yshift, ushift, vshift, ashift;
  int curval, yval, uval, vval, aval;
  int yct, uct, vct, act, thisct;
  int x, y;

  ydiff = y_ystep - (xsize * y_xstep);
  uvdiff = uv_ystep - ((xsize / 2) * uv_xstep);
  adiff = a_ystep - (xsize * a_xstep);

  yshift = 8 - enc_ybits;
  ushift = 8 - enc_ubits;
  vshift = 8 - enc_vbits;
  ashift = 8 - enc_abits;

  optr = outbuf;
  *optr++ = 42;
  *optr++ = (xsize >> 8) & 255;
  *optr++ = xsize & 255;
  *optr++ = (ysize >> 8) & 255;
  *optr++ = ysize & 255;
  ibuf = optr;

  act = 0;
  aptr = abuf;
  aval = *aptr >> ashift;
  for(y = 0; y < ysize; y++, aptr += adiff) {
    for(x = 0; x < xsize; x++, aptr += a_xstep) {
      if(aval == (curval = *aptr >> ashift)) act++;
      else {
	if(act < 255) { 
	  *optr++ = act;
	  *optr++ = atable[aval];
	}
	else if(act < 65536) {
	  *optr++ = 255;
	  *optr++ = act >> 8;
	  *optr++ = act & 255;
	  *optr++ = atable[aval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = atable[aval];
	  act -= 65535;
	  *optr++ = 255;
	  *optr++ = act >> 8;
	  *optr++ = act & 255;
	  *optr++ = atable[aval];
	}
	act = 1; aval = curval;
      }
    }
  }
    
  if(act < 255) { 
    *optr++ = act;
    *optr++ = atable[aval];
  }
  else if(act < 65536) {
    *optr++ = 255;
    *optr++ = act >> 8;
    *optr++ = act & 255;
    *optr++ = atable[aval];
  }
  else {
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = atable[aval];
    act -= 65535;
    *optr++ = 255;
    *optr++ = act >> 8;
    *optr++ = act & 255;
    *optr++ = atable[aval];
  }


  yct = 0;
  yptr = ybuf;
  curval = yval = *yptr >> yshift;
  act = 0;
  iptr = ibuf;
  for(y = 0; y < ysize; y++, yptr += ydiff) {
    for(x = 0; x < xsize; x++, yptr += y_xstep) {
      
      if(act == 0) {
	thisct = *iptr++;
	if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
	act += thisct;
	aval = *iptr++;
      }
      if(aval) curval = *yptr >> yshift;
      act--;
      
      if(yval == curval) yct++;
      else {
	if(yct < 255) { 
	  *optr++ = yct;
	  *optr++ = ytable[yval];
	}
	else if(act < 65536) {
	  *optr++ = 255;
	  *optr++ = yct >> 8;
	  *optr++ = yct & 255;
	  *optr++ = ytable[yval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = ytable[yval];
	  act -= 65535;
	  *optr++ = 255;
	  *optr++ = yct >> 8;
	  *optr++ = yct & 255;
	  *optr++ = ytable[yval];
	}
	yct = 1; yval = curval;
      }
    }
  }
  
  if(yct < 255) { 
    *optr++ = yct;
    *optr++ = ytable[yval];
  }
  else if(act < 65536) {
    *optr++ = 255;
    *optr++ = yct >> 8;
    *optr++ = yct & 255;
    *optr++ = ytable[yval];
  }
  else {
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = ytable[yval];
    act -= 65535;
    *optr++ = 255;
    *optr++ = yct >> 8;
    *optr++ = yct & 255;
    *optr++ = ytable[yval];
  }


  uct = 0;
  uptr = ubuf;
  curval = uval = *uptr >> ushift;
  act = 0;
  iptr = ibuf;
  for(y = 0; y < ysize; y++, uptr += uvdiff) {
    for(x = 0; x < xsize; x += 2, uptr += uv_xstep) {
      
      while(act <= 0) {
	thisct = *iptr++;
	if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
	act += thisct;
	aval = *iptr++;
      }
      if(aval) curval = *uptr >> ushift;
      act -= 2;
      
      if(uval == curval) uct++;
      else {
	if(uct < 255) { 
	  *optr++ = uct;
	  *optr++ = utable[uval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = uct >> 8;
	  *optr++ = uct & 255;
	  *optr++ = utable[uval];
	}
	uct = 1; uval = curval;
      }
    }
  }
  
  if(uct < 255) { 
    *optr++ = uct;
    *optr++ = utable[uval];
  }
  else {
    *optr++ = 255;
    *optr++ = uct >> 8;
    *optr++ = uct & 255;
    *optr++ = utable[uval];
  }


  vct = 0;
  vptr = vbuf;
  curval = vval = *vptr >> vshift;
  act = 0;
  iptr = ibuf;
  for(y = 0; y < ysize; y++, vptr += uvdiff) {
    for(x = 0; x < xsize; x += 2, vptr += uv_xstep) {
      
      while(act <= 0) {
	thisct = *iptr++;
	if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
	act += thisct;
	aval = *iptr++;
      }
      if(aval) curval = *vptr >> vshift;
      act -= 2;
      
      if(vval == curval) vct++;
      else {
	if(vct < 255) { 
	  *optr++ = vct;
	  *optr++ = vtable[vval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = vct >> 8;
	  *optr++ = vct & 255;
	  *optr++ = vtable[vval];
	}
	vct = 1; vval = curval;
      }
    }
  }
  
  if(vct < 255) { 
    *optr++ = vct;
    *optr++ = vtable[vval];
  }
  else {
    *optr++ = 255;
    *optr++ = vct >> 8;
    *optr++ = vct & 255;
    *optr++ = vtable[vval];
  }

  return optr - outbuf;
}

//CIAN
/* ------------------------------------------------------------ */
int encode_image_rgb(int xsize, int ysize,
		     unsigned char *rbuf, 
		     unsigned char *gbuf,
		     unsigned char *bbuf, 
		     int xstep, int ystep,
		     unsigned char *abuf, 
		     int a_xstep, int a_ystep,
		     unsigned char *outbuf)
{
  register unsigned char *rptr, *gptr, *bptr, *aptr, *optr, *ibuf, *iptr;
  int rgbdiff, adiff;
  int rshift, gshift, bshift, ashift;
  int curval, rval, gval, bval, aval;
  int rct, gct, bct, act, thisct;
  int x, y;

  rgbdiff = ystep - (xsize * xstep);
  adiff = a_ystep - (xsize * a_xstep);

  rshift = 8 - enc_rbits;
  gshift = 8 - enc_gbits;
  bshift = 8 - enc_bbits;
  ashift = 8 - enc_abits;

  optr = outbuf;
  *optr++ = 42;
  *optr++ = (xsize >> 8) & 255;
  *optr++ = xsize & 255;
  *optr++ = (ysize >> 8) & 255;
  *optr++ = ysize & 255;
  ibuf = optr;

  act = 0;
  aptr = abuf;
  aval = *aptr >> ashift;
  for(y = 0; y < ysize; y++, aptr += adiff) {
    for(x = 0; x < xsize; x++, aptr += a_xstep) {
      if(aval == (curval = *aptr >> ashift)) act++;
      else {
	if(act < 255) { 
	  *optr++ = act;
	  *optr++ = atable[aval];
	}
	else if(act < 65536) {
	  *optr++ = 255;
	  *optr++ = act >> 8;
	  *optr++ = act & 255;
	  *optr++ = atable[aval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = atable[aval];
	  act -= 65535;
	  *optr++ = 255;
	  *optr++ = act >> 8;
	  *optr++ = act & 255;
	  *optr++ = atable[aval];
	}
	act = 1; aval = curval;
      }
    }
  }
    
  if(act < 255) { 
    *optr++ = act;
    *optr++ = atable[aval];
  }
  else if(act < 65536) {
    *optr++ = 255;
    *optr++ = act >> 8;
    *optr++ = act & 255;
    *optr++ = atable[aval];
  }
  else {
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = atable[aval];
    act -= 65535;
    *optr++ = 255;
    *optr++ = act >> 8;
    *optr++ = act & 255;
    *optr++ = atable[aval];
  }


  rct = 0;
  rptr = rbuf;
  curval = rval = *rptr >> rshift;
  act = 0;
  iptr = ibuf;
  for(y = 0; y < ysize; y++, rptr += rgbdiff) {
    for(x = 0; x < xsize; x++, rptr += xstep) {
      
      if(act == 0) {
	thisct = *iptr++;
	if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
	act += thisct;
	aval = *iptr++;
      }
      if(aval) curval = *rptr >> rshift;
      act--;
      
      if(rval == curval) rct++;
      else {
	if(rct < 255) { 
	  *optr++ = rct;
	  *optr++ = rtable[rval];
	}
	else if(act < 65536) {
	  *optr++ = 255;
	  *optr++ = rct >> 8;
	  *optr++ = rct & 255;
	  *optr++ = rtable[rval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = 255;
	  *optr++ = rtable[rval];
	  act -= 65535;
	  *optr++ = 255;
	  *optr++ = rct >> 8;
	  *optr++ = rct & 255;
	  *optr++ = rtable[rval];
	}
	rct = 1; rval = curval;
      }
    }
  }
  
  if(rct < 255) { 
    *optr++ = rct;
    *optr++ = rtable[rval];
  }
  else if(act < 65536) {
    *optr++ = 255;
    *optr++ = rct >> 8;
    *optr++ = rct & 255;
    *optr++ = rtable[rval];
  }
  else {
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = rtable[rval];
    act -= 65535;
    *optr++ = 255;
    *optr++ = rct >> 8;
    *optr++ = rct & 255;
    *optr++ = rtable[rval];
  }


  gct = 0;
  gptr = gbuf;
  curval = gval = *gptr >> gshift;
  act = 0;
  iptr = ibuf;
  for(y = 0; y < ysize; y++, gptr += rgbdiff) {
    for(x = 0; x < xsize; x ++, gptr += xstep) {
      
      while(act <= 0) {
	thisct = *iptr++;
	if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
	act += thisct;
	aval = *iptr++;
      }
      if(aval) curval = *gptr >> gshift;
      act --;
      
      if(gval == curval) gct++;
      else {
	if(gct < 255) { 
	  *optr++ = gct;
	  *optr++ = gtable[gval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = gct >> 8;
	  *optr++ = gct & 255;
	  *optr++ = gtable[gval];
	}
	gct = 1; gval = curval;
      }
    }
  }
  
  if(gct < 255) { 
    *optr++ = gct;
    *optr++ = gtable[gval];
  }
  else if (act < 65536) {
    *optr++ = 255;
    *optr++ = gct >> 8;
    *optr++ = gct & 255;
    *optr++ = gtable[gval];
  }

  else {
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = gtable[gval];
    act -= 65535;
    *optr++ = 255;
    *optr++ = gct >> 8;
    *optr++ = gct & 255;
    *optr++ = gtable[gval];
  }

  bct = 0;
  bptr = bbuf;
  curval = bval = *bptr >> bshift;
  act = 0;
  iptr = ibuf;
  for(y = 0; y < ysize; y++, bptr += rgbdiff) {
    for(x = 0; x < xsize; x ++, bptr += xstep) {
      
      while(act <= 0) {
	thisct = *iptr++;
	if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
	act += thisct;
	aval = *iptr++;
      }
      if(aval) curval = *bptr >> bshift;
      act --;
      
      if(bval == curval) bct++;
      else {
	if(bct < 255) { 
	  *optr++ = bct;
	  *optr++ = btable[bval];
	}
	else {
	  *optr++ = 255;
	  *optr++ = bct >> 8;
	  *optr++ = bct & 255;
	  *optr++ = btable[bval];
	}
	bct = 1; bval = curval;
      }
    }
  }
  
  if(bct < 255) { 
    *optr++ = bct;
    *optr++ = btable[bval];
  }
  else if(act < 65536) {
    *optr++ = 255;
    *optr++ = bct >> 8;
    *optr++ = bct & 255;
    *optr++ = btable[bval];
  }

  else {
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = 255;
    *optr++ = btable[bval];
    act -= 65535;
    *optr++ = 255;
    *optr++ = bct >> 8;
    *optr++ = bct & 255;
    *optr++ = btable[bval];
  }



  return optr - outbuf;
}




/* ------------------------------------------------------------ */
Value *isis_decode_image_yuv(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int res, xsize, ysize;

  checkCount(2, NULL); 
  checkAddr(0, "inbuf");
  checkAddr(1, "outbuf");
  
  res = decode_image_yuv(
			 getAddr(argv[0]),
			 getAddr(argv[1]),
			 &xsize, 
			 &ysize
			 );
  
  if(res) return newIntListva(2, xsize, ysize);
  else return NullValue;
}

//CIAN
/* ------------------------------------------------------------ */
Value *isis_decode_image_rgb_planar(Script *script, char *proc_name, int call_data, 
				    int argc, Value **argv)
{
  int res, xsize, ysize;

  checkCount(2, NULL); 
  checkAddr(0, "inbuf");
  checkAddr(1, "outbuf");
  
  res = decode_image_rgb_planar(
				getAddr(argv[0]),
				getAddr(argv[1]),
				&xsize, 
				&ysize
				);
  
  if(res) return newIntListva(2, xsize, ysize);
  else return NullValue;
}

//CIAN
/* ------------------------------------------------------------ */
Value *isis_decode_image_rgb_interleaved(Script *script, char *proc_name, int call_data, 
					 int argc, Value **argv)
{
  int res, xsize, ysize;
  
  checkCount(2, NULL); 
  checkAddr(0, "inbuf");
  checkAddr(1, "outbuf");
  
  res = decode_image_rgb_interleaved(
				     getAddr(argv[0]),
				     getAddr(argv[1]),
				     &xsize, 
				     &ysize
				     );
  
  if(res) return newIntListva(2, xsize, ysize);
  else return NullValue;
}

/*----------------------------------------------------------------------*/
int decode_image_yuv(unsigned char *ibuf,
		     unsigned char *outbuf, 
		     int *xsize, int *ysize)
{
  int totpixels, ct, thisct, i;
  unsigned char *iptr, *optr, val;
  
  iptr = ibuf;
  optr = outbuf;

  if(*iptr++ != 42) return 0;

  *xsize = (*iptr++ << 8); *xsize += *iptr++;
  *ysize = (*iptr++ << 8); *ysize += *iptr++;

  totpixels = *xsize * *ysize;

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct;
  }

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct;
  }

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct * 2;
  }

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct * 2;
  }

  return 1;
}

//CIAN
/* ------------------------------------------------------------ */
int decode_image_rgb_planar(unsigned char *ibuf,
			    unsigned char *outbuf, 
			    int *xsize, int *ysize)
{
  int totpixels, ct, thisct, i;
  unsigned char *iptr, *optr, val;
  
  iptr = ibuf;
  optr = outbuf;

  if(*iptr++ != 42) return 0;

  *xsize = (*iptr++ << 8); *xsize += *iptr++;
  *ysize = (*iptr++ << 8); *ysize += *iptr++;

  totpixels = *xsize * *ysize;

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct;
  }

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct;
  }

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct;
  }

  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) *optr++ = val;
    ct += thisct;
  }

  return 1;
}


//CIAN
/* ------------------------------------------------------------ */
int decode_image_rgb_interleaved(unsigned char *ibuf,
				 unsigned char *outbuf, 
				 int *xsize, int *ysize)
{
  int totpixels, ct, thisct, i;
  unsigned char *iptr, *optr, val;
  
  iptr = ibuf;
  
  if(*iptr++ != 42) return 0;
  
  *xsize = (*iptr++ << 8); *xsize += *iptr++;
  *ysize = (*iptr++ << 8); *ysize += *iptr++;
  
  totpixels = *xsize * *ysize;

  optr = (outbuf + 3);
  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) {*optr = val; optr += 4;}
    ct += thisct;
  }

  optr = outbuf;
  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) {*optr = val; optr += 4;}
    ct += thisct;
  }

  optr = (outbuf + 1);
  ct = 0;
  while(ct < totpixels) {
    thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) {*optr = val; optr += 4;}
    ct += thisct;
  }

  optr = (outbuf + 2);
  ct = 0;
  while(ct < totpixels) {
  thisct = *iptr++;
    if(thisct == 255) { thisct = *iptr++ << 8; thisct += *iptr++; }
    val = *iptr++;
    for(i = 0; i < thisct; i++) {*optr = val; optr += 4;}
    ct += thisct;
  }

  return 1;
}


/* ------------------------------------------------------------ */
Value *isis_seg_color_rgb(Script *script, char *proc_name, int call_data,
			  int argc, Value **argv)
{
  checkCount(11, NULL);
  checkInt(0, "rlow");
  checkInt(1, "rhigh");
  checkInt(2, "glow");
  checkInt(3, "ghigh");
  checkInt(4, "blow");
  checkInt(5, "bhigh");
  checkIntList(6, "size");
  checkAddrList(7, "rgbbuf");
  checkIntList(8, "rgbstep");
  checkAddr(9, "abuf");
  checkIntList(10, "astep");
  
  seg_color_rgb(
		getInt(argv[0]),
		getInt(argv[1]),
		getInt(argv[2]),
		getInt(argv[3]),
		getInt(argv[4]),
		getInt(argv[5]),

		getInt(Listitem(argv[6], 0)),
		getInt(Listitem(argv[6], 1)),
		
		getAddr(Listitem(argv[7], 0)),
		getAddr(Listitem(argv[7], 1)),
		getAddr(Listitem(argv[7], 2)),
		getInt(Listitem(argv[8], 0)),
		getInt(Listitem(argv[8], 1)),
		
		getAddr(Listitem(argv[9], 0)),
		getInt(Listitem(argv[10], 0)),
		getInt(Listitem(argv[10], 1))
		
		);
  
  return NullValue;
}


/* ------------------------------------------------------------ */
/* Creates an alpha image where the 'ON' pixels are those whose 
   corresponding rgb pixels were within a certain rgb range. */
void seg_color_rgb(unsigned char r_min, 
		   unsigned char r_max,
		   unsigned char g_min, 
		   unsigned char g_max,
		   unsigned char b_min, 
		   unsigned char b_max,
		   int xsize, int ysize,
		   unsigned char *rbuf,
		   unsigned char *gbuf,
		   unsigned char *bbuf,
		   int skip, int stride,
		   unsigned char *abuf,
		   int askip, int astride)
{
  int x, y;
  int diff, adiff;
  
  diff = stride - (xsize * skip);
  adiff = astride - (xsize * askip);
  
  for(y = 0; y < ysize; y++)
    {
      for(x =0; x < xsize; x++)
        {
	  if(*rbuf >= r_min && *rbuf <= r_max &&
	     *gbuf >= g_min && *gbuf <= g_max &&
	     *bbuf >= b_min && *bbuf <= b_max) *abuf = 255;
          else *abuf = 0;
          
          rbuf += skip;
          gbuf += skip;
          bbuf += skip;
          abuf += askip;
        }
      rbuf += diff;
      gbuf += diff;
      bbuf += diff;
      abuf += adiff;
    }
}

