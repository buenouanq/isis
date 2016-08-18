/* image warping - written by Chris Dodge (cdodge@media.mit.edu) */

#include <stdlib.h>
#include "isis_improc_private.h"

void warp_buffer1(unsigned char *in,
		  int inskip,
		  int instr,
		  unsigned char *out,
		  int outskip,
		  int outstr,
		  unsigned int *xmap,
		  unsigned int *ymap,
		  int mapskip,
		  int mapstr,
		  int w,
		  int h)
{
  int i,j,odiff,x,y,blend_y,blend_x,x1,x2,p1,p2,p3,p4,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out+=outskip,ymap+=mapskip,xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_y = y & 0xff;
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	p1 = in[p];
	p2 = in[p+inskip];
	p3 = in[p+instr];
	p4 = in[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;
      }
}

/* added by stefan 12/3/96 */
void warp_buffer2(unsigned char *in1,
		  unsigned char *in2,
		  int inskip,
		  int instr,
		  unsigned char *out1,
		  unsigned char *out2,
		  int outskip,
		  int outstr,
		  unsigned int *xmap,
		  unsigned int *ymap,
		  int mapskip,
		  int mapstr,
		  int w,
		  int h)
{
  int i,j,odiff,x,y,blend_y,blend_x,x1,x2,p1,p2,p3,p4,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_y = y & 0xff;
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;

	p = (y * instr) + (x * inskip);
	  
	p1 = in1[p];
	p2 = in1[p+inskip];
	p3 = in1[p+instr];
	p4 = in1[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out1 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

	p1 = in2[p];
	p2 = in2[p+inskip];
	p3 = in2[p+instr];
	p4 = in2[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out2 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

      }
}



void warp_buffer3(unsigned char *in1,
		  unsigned char *in2,
		  unsigned char *in3,
		  int inskip,
		  int instr,
		  unsigned char *out1,
		  unsigned char *out2,
		  unsigned char *out3,
		  int outskip,
		  int outstr,
		  unsigned int *xmap,
		  unsigned int *ymap,
		  int mapskip,
		  int mapstr,
		  int w,
		  int h)
{
  int i,j,odiff,x,y,blend_y,blend_x,x1,x2,p1,p2,p3,p4,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,out3+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,out3+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_y = y & 0xff;
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;

	p = (y * instr) + (x * inskip);
	  
	p1 = in1[p];
	p2 = in1[p+inskip];
	p3 = in1[p+instr];
	p4 = in1[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out1 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

	p1 = in2[p];
	p2 = in2[p+inskip];
	p3 = in2[p+instr];
	p4 = in2[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out2 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

	p1 = in3[p];
	p2 = in3[p+inskip];
	p3 = in3[p+instr];
	p4 = in3[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out3 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

      }
}

/* added by stefan 12/3/96 */
void warp_buffer4(unsigned char *in1,
		  unsigned char *in2,
		  unsigned char *in3,
		  unsigned char *in4,
		  int inskip,
		  int instr,
		  unsigned char *out1,
		  unsigned char *out2,
		  unsigned char *out3,
		  unsigned char *out4,
		  int outskip,
		  int outstr,
		  unsigned int *xmap,
		  unsigned int *ymap,
		  int mapskip,
		  int mapstr,
		  int w,
		  int h)
{
  int i,j,odiff,x,y,blend_y,blend_x,x1,x2,p1,p2,p3,p4,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,out3+=odiff,out4+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,out3+=outskip,out4+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_y = y & 0xff;
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;

	p = (y * instr) + (x * inskip);
	  
	p1 = in1[p];
	p2 = in1[p+inskip];
	p3 = in1[p+instr];
	p4 = in1[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out1 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

	p1 = in2[p];
	p2 = in2[p+inskip];
	p3 = in2[p+instr];
	p4 = in2[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out2 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

	p1 = in3[p];
	p2 = in3[p+inskip];
	p3 = in3[p+instr];
	p4 = in3[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out3 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

	p1 = in4[p];
	p2 = in4[p+inskip];
	p3 = in4[p+instr];
	p4 = in4[p+instr+inskip];
    
	x1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
	x2 = ((p3 * (256 - blend_x)) + (p4 * blend_x)) >> 8;
	*out4 = ((x1 * (256 - blend_y)) + (x2 * blend_y)) >> 8;

      }
}



void warp_buffer1_no_interp(unsigned char *in,
			    int inskip,
			    int instr,
			    unsigned char *out,
			    int outskip,
			    int outstr,
			    unsigned int *xmap,
			    unsigned int *ymap,
			    int mapskip,
			    int mapstr,
			    int w,
			    int h)
{
  int i,j,odiff,x,y,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out+=outskip,ymap+=mapskip,xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
       	
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	*out = in[p];
    
      }
}


/* added by stefan 12/3/96 */
void warp_buffer2_no_interp(unsigned char *in1,
			    unsigned char *in2,
			    int inskip,
			    int instr,
			    unsigned char *out1,
			    unsigned char *out2,
			    int outskip,
			    int outstr,
			    unsigned int *xmap,
			    unsigned int *ymap,
			    int mapskip,
			    int mapstr,
			    int w,
			    int h)
{
  int i,j,odiff,x,y,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
		
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	*out1 = in1[p];
	*out2 = in2[p];
      }
}

void warp_buffer3_no_interp(unsigned char *in1,
			    unsigned char *in2,
			    unsigned char *in3,
			    int inskip,
			    int instr,
			    unsigned char *out1,
			    unsigned char *out2,
			    unsigned char *out3,
			    int outskip,
			    int outstr,
			    unsigned int *xmap,
			    unsigned int *ymap,
			    int mapskip,
			    int mapstr,
			    int w,
			    int h)
{
  int i,j,odiff,x,y,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,out3+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,out3+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
		
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	*out1 = in1[p];
	*out2 = in2[p];
	*out3 = in3[p];   
      }
}

/* added by stefan 12/3/96 */
void warp_buffer4_no_interp(unsigned char *in1,
			    unsigned char *in2,
			    unsigned char *in3,
			    unsigned char *in4,
			    int inskip,
			    int instr,
			    unsigned char *out1,
			    unsigned char *out2,
			    unsigned char *out3,
			    unsigned char *out4,
			    int outskip,
			    int outstr,
			    unsigned int *xmap,
			    unsigned int *ymap,
			    int mapskip,
			    int mapstr,
			    int w,
			    int h)
{
  int i,j,odiff,x,y,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,out3+=odiff,out4+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,out3+=outskip,out4+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
		
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	*out1 = in1[p];
	*out2 = in2[p];
	*out3 = in3[p];   
	*out4 = in4[p];   
      }
}



void warp_buffer1_xinterp(unsigned char *in,
			  int inskip,
			  int instr,
			  unsigned char *out,
			  int outskip,
			  int outstr,
			  unsigned int *xmap,
			  unsigned int *ymap,
			  int mapskip,
			  int mapstr,
			  int w,
			  int h)
{
  int i,j,odiff,x,y,blend_x,p1,p2,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);
  
  for(i=0;i<h;i++,out+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out+=outskip,ymap+=mapskip,xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	p1 = in[p];
	p2 = in[p+inskip];
    
	*out = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;
      }
}



/* added by stefan 12/3/96 */
void warp_buffer2_xinterp(unsigned char *in1,
			  unsigned char *in2,
			  int inskip,
			  int instr,
			  unsigned char *out1,
			  unsigned char *out2,
			  int outskip,
			  int outstr,
			  unsigned int *xmap,
			  unsigned int *ymap,
			  int mapskip,
			  int mapstr,
			  int w,
			  int h)
{
  int i,j,odiff,x,y,blend_x,p1,p2,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	p1 = in1[p];
	p2 = in1[p+inskip];
    
	*out1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

	p1 = in2[p];
	p2 = in2[p+inskip];
    
	*out2 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

      }
}


void warp_buffer3_xinterp(unsigned char *in1,
			  unsigned char *in2,
			  unsigned char *in3,
			  int inskip,
			  int instr,
			  unsigned char *out1,
			  unsigned char *out2,
			  unsigned char *out3,
			  int outskip,
			  int outstr,
			  unsigned int *xmap,
			  unsigned int *ymap,
			  int mapskip,
			  int mapstr,
			  int w,
			  int h)
{
  int i,j,odiff,x,y,blend_x,p1,p2,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,out3+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,out3+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	p1 = in1[p];
	p2 = in1[p+inskip];
    
	*out1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

	p1 = in2[p];
	p2 = in2[p+inskip];
    
	*out2 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

	p1 = in3[p];
	p2 = in3[p+inskip];
    
	*out3 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

      }
}



/* added by stefan 12/3/96 */
void warp_buffer4_xinterp(unsigned char *in1,
			  unsigned char *in2,
			  unsigned char *in3,
			  unsigned char *in4,
			  int inskip,
			  int instr,
			  unsigned char *out1,
			  unsigned char *out2,
			  unsigned char *out3,
			  unsigned char *out4,
			  int outskip,
			  int outstr,
			  unsigned int *xmap,
			  unsigned int *ymap,
			  int mapskip,
			  int mapstr,
			  int w,
			  int h)
{
  int i,j,odiff,x,y,blend_x,p1,p2,p,mdiff;

  odiff = outstr - (w * outskip);
  mdiff = mapstr - (w * mapskip);

  for(i=0;i<h;i++,out1+=odiff,out2+=odiff,out3+=odiff,out4+=odiff,xmap+=mdiff,ymap+=mdiff)
    for(j=0;j<w;j++,out1+=outskip,out2+=outskip,out3+=outskip,out4+=outskip,ymap+=mapskip,
	  xmap+=mapskip)
      {
	y = *ymap;
	x = *xmap;
	
	blend_x = x & 0xff;
	
	y >>= 8;
	x >>= 8;
	
	p = (y * instr) + (x * inskip);
	  
	p1 = in1[p];
	p2 = in1[p+inskip];
    
	*out1 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

	p1 = in2[p];
	p2 = in2[p+inskip];
    
	*out2 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

	p1 = in3[p];
	p2 = in3[p+inskip];
    
	*out3 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

	p1 = in4[p];
	p2 = in4[p+inskip];
    
	*out4 = ((p1 * (256 - blend_x)) + (p2 * blend_x)) >> 8;

      }
}
