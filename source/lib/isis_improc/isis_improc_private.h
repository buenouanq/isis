#ifndef __IMPROC_H__
#define __IMPROC_H__

/******************************************************************
  
  Image processing library 
  
  by Stefan Agamanolis and Shawn Becker
  image warping (warp.c) by Chris Dodge
  started May 1995 
  
  Include this file in your program to use the routines, and
  link your object file with the image processing library
  
  Please consult with Stefan before and after making any changes.
  
  All routines have been fully tested.
  
  *******************************************************************/

/*----------------------------------------------------------------------*/
/* ARITHMETIC */

/* These functions perform arithmetic on buffers/arrays.  Choose the
   one appropriate for your sample type */

void image_add_byte(int xsize,
		    int ysize,
		    unsigned char *i1ptr,
		    int i1skip,
		    int i1stride,
		    unsigned char *i2ptr,
		    int i2skip,
		    int i2stride,
		    unsigned char *optr,
		    int oskip,
		    int ostride);

void image_add_byte_int_byte(int xsize,
			     int ysize,
			     unsigned char *i1ptr,
			     int i1skip,
			     int i1stride,
			     int *i2ptr,
			     int i2skip,
			     int i2stride,
			     unsigned char *optr,
			     int oskip,
			     int ostride);

void image_add_byte_int_int(int xsize,
			    int ysize,
			    unsigned char *i1ptr,
			    int i1skip,
			    int i1stride,
			    int *i2ptr,
			    int i2skip,
			    int i2stride,
			    int *optr,
			    int oskip,
			    int ostride);

void image_add_int(int xsize,
		   int ysize,
		   int *i1ptr,
		   int i1skip,
		   int i1stride,
		   int *i2ptr,
		   int i2skip,
		   int i2stride,
		   int *optr,
		   int oskip,
		   int ostride);

void image_add_double(int xsize,
		      int ysize,
		      double *i1ptr,
		      int i1skip,
		      int i1stride,
		      double *i2ptr,
		      int i2skip,
		      int i2stride,
		      double *optr,
		      int oskip,
		      int ostride);

void image_subtract_byte(int xsize,
			 int ysize,
			 unsigned char *i1ptr,
			 int i1skip,
			 int i1stride,
			 unsigned char *i2ptr,
			 int i2skip,
			 int i2stride,
			 unsigned char *optr,
			 int oskip,
			 int ostride);

void image_subtract_byte_int_byte(int xsize,
				  int ysize,
				  unsigned char *i1ptr,
				  int i1skip,
				  int i1stride,
				  int *i2ptr,
				  int i2skip,
				  int i2stride,
				  unsigned char *optr,
				  int oskip,
				  int ostride);

void image_subtract_int(int xsize,
			int ysize,
			int *i1ptr,
			int i1skip,
			int i1stride,
			int *i2ptr,
			int i2skip,
			int i2stride,
			int *optr,
			int oskip,
			int ostride);

void image_subtract_double(int xsize,
			   int ysize,
			   double *i1ptr,
			   int i1skip,
			   int i1stride,
			   double *i2ptr,
			   int i2skip,
			   int i2stride,
			   double *optr,
			   int oskip,
			   int ostride);

void image_diff_byte(int xsize,
		     int ysize,
		     unsigned char *i1ptr,
		     int i1skip,
		     int i1stride,
		     unsigned char *i2ptr,
		     int i2skip,
		     int i2stride,
		     unsigned char *optr,
		     int oskip,
		     int ostride);

void image_diff_byte_int_byte(int xsize,
			      int ysize,
			      unsigned char *i1ptr,
			      int i1skip,
			      int i1stride,
			      int *i2ptr,
			      int i2skip,
			      int i2stride,
			      unsigned char *optr,
			      int oskip,
			      int ostride);

void image_diff_int(int xsize,
		    int ysize,
		    int *i1ptr,
		    int i1skip,
		    int i1stride,
		    int *i2ptr,
		    int i2skip,
		    int i2stride,
		    int *optr,
		    int oskip,
		    int ostride);

void image_diff_double(int xsize,
		       int ysize,
		       double *i1ptr,
		       int i1skip,
		       int i1stride,
		       double *i2ptr,
		       int i2skip,
		       int i2stride,
		       double *optr,
		       int oskip,
		       int ostride);

void image_multiply_byte(int xsize,
			 int ysize,
			 unsigned char *i1ptr,
			 int i1skip,
			 int i1stride,
			 unsigned char *i2ptr,
			 int i2skip,
			 int i2stride,
			 unsigned char *optr,
			 int oskip,
			 int ostride);

void image_multiply_byte_int_byte(int xsize,
				  int ysize,
				  unsigned char *i1ptr,
				  int i1skip,
				  int i1stride,
				  int *i2ptr,
				  int i2skip,
				  int i2stride,
				  unsigned char *optr,
				  int oskip,
				  int ostride);

void image_multiply_int(int xsize,
			int ysize,
			int *i1ptr,
			int i1skip,
			int i1stride,
			int *i2ptr,
			int i2skip,
			int i2stride,
			int *optr,
			int oskip,
			int ostride);

void image_multiply_double(int xsize,
			   int ysize,
			   double *i1ptr,
			   int i1skip,
			   int i1stride,
			   double *i2ptr,
			   int i2skip,
			   int i2stride,
			   double *optr,
			   int oskip,
			   int ostride);

void image_divide_byte(int xsize,
		       int ysize,
		       unsigned char *i1ptr,
		       int i1skip,
		       int i1stride,
		       unsigned char *i2ptr,
		       int i2skip,
		       int i2stride,
		       unsigned char *optr,
		       int oskip,
		       int ostride);

void image_divide_byte_int_byte(int xsize,
				int ysize,
				unsigned char *i1ptr,
				int i1skip,
				int i1stride,
				int *i2ptr,
				int i2skip,
				int i2stride,
				unsigned char *optr,
				int oskip,
				int ostride);

void image_divide_int(int xsize,
		      int ysize,
		      int *i1ptr,
		      int i1skip,
		      int i1stride,
		      int *i2ptr,
		      int i2skip,
		      int i2stride,
		      int *optr,
		      int oskip,
		      int ostride);

void image_divide_double(int xsize,
			 int ysize,
			 double *i1ptr,
			 int i1skip,
			 int i1stride,
			 double *i2ptr,
			 int i2skip,
			 int i2stride,
			 double *optr,
			 int oskip,
			 int ostride);

void image_maximum_byte(int xsize,
			int ysize,
			unsigned char *i1ptr,
			int i1skip,
			int i1stride,
			unsigned char *i2ptr,
			int i2skip,
			int i2stride,
			unsigned char *optr,
			int oskip,
			int ostride);

void image_maximum_int(int xsize,
		       int ysize,
		       int *i1ptr,
		       int i1skip,
		       int i1stride,
		       int *i2ptr,
		       int i2skip,
		       int i2stride,
		       int *optr,
		       int oskip,
		       int ostride);

void image_maximum_double(int xsize,
			  int ysize,
			  double *i1ptr,
			  int i1skip,
			  int i1stride,
			  double *i2ptr,
			  int i2skip,
			  int i2stride,
			  double *optr,
			  int oskip,
			  int ostride);

void image_minimum_byte(int xsize,
			int ysize,
			unsigned char *i1ptr,
			int i1skip,
			int i1stride,
			unsigned char *i2ptr,
			int i2skip,
			int i2stride,
			unsigned char *optr,
			int oskip,
			int ostride);

void image_minimum_int(int xsize,
		       int ysize,
		       int *i1ptr,
		       int i1skip,
		       int i1stride,
		       int *i2ptr,
		       int i2skip,
		       int i2stride,
		       int *optr,
		       int oskip,
		       int ostride);

void image_minimum_double(int xsize,
			  int ysize,
			  double *i1ptr,
			  int i1skip,
			  int i1stride,
			  double *i2ptr,
			  int i2skip,
			  int i2stride,
			  double *optr,
			  int oskip,
			  int ostride);

/*----------------------------------------------------------------------*/
/* BOUNDING-BOXES */

/* 
   This routine takes a grey scale image and finds the smallest
   bounding box around all objects of intensity greater than or equal
   to 'low' and less than or equal to 'high'.  The rectangle is
   returned through the pointers 'topleftx' 'toplefty' 'botrightx' and
   'botrighty'.  Returns '1' if a bounding box was found, or 0 if
   nothing was found.  
   
   Example, to find a figure in an alpha channel, pass 1 as 'lowint'
   and 255 as 'highint'. */

int bounding_box(unsigned char low,
		 unsigned char high,
		 int xsize,
		 int ysize,
		 unsigned char *buf,
		 int skip,
		 int stride,
		 int *topleftx,
		 int *toplefty,
		 int *botrightx,
		 int *botrighty);

int bounding_box_rgb(unsigned char rlow,
		     unsigned char rhigh,
		     unsigned char glow,
		     unsigned char ghigh,
		     unsigned char blow,
		     unsigned char bhigh,
		     int xsize,
		     int ysize,
		     unsigned char *rbuf,
		     unsigned char *gbuf,
		     unsigned char *bbuf,
		     int skip,
		     int stride,
		     int *topleftx,
		     int *toplefty,
		     int *botrightx,
		     int *botrighty);

int fuzzy_bounding_box(int thresh, 
		       int pixcount,
		       int xdim, 
		       int ydim,
		       unsigned char *image, 
		       int skip,
		       int stride,
		       int *box);

/*----------------------------------------------------------------------*/
/* COMPOSITING / Z-BUFFER */

/* These routines perform alpha compositing, layering, and zbuffering.
   If your input image has an alpha channel, use a function that
   starts with "composite_".  If the input image has a depth buffer,
   use a function with "zbuffer_" in the name.  If the input image
   does not have depth, but the output image does, use a "layer_"
   function.  For parallel three channel processing (for rgb), use the
   "_3ch" functions.  All image and alpha channels should be
   byte-sampled (unsigned char), and all depth (z) buffers must be
   integer-sampled, the greater the integer the closer the object.
   
   You should call initialize_composite() once before doing any
   compositing.  This function creates lookup tables to speed up the
   compositing process (avoid doing divides for every pixel).  If you
   do not call it, it will be called automatically the first time you
   do a composite.
   
   In compositing functions that do layering or zbuffering, you must
   pass an "alpha_cutoff".  This value controls how the depth is
   copied from the input to the output z-buffers.  If the current
   pixel's alpha value is >= this threshold, that pixel's depth is
   copied into the output z-buffer.  Otherwise it is not, allowing for
   a farther object to be composited later behind this object and show
   through these areas.  If you don't want to use this feature, set it
   to 0.  */

void initialize_composite();

void composite_1ch(int xsize, 
		   int ysize,
		   unsigned char *iptr0, 
		   int inskip,
		   int instride,
		   unsigned char *iptra, 
		   int askip,
		   int astride,
		   unsigned char *optr0,
		   int outskip,
		   int outstride);

void composite_2ch(int xsize, 
		   int ysize,
		   unsigned char *iptr0, 
		   unsigned char *iptr1, 
		   int inskip,
		   int instride,
		   unsigned char *iptra, 
		   int askip,
		   int astride,
		   unsigned char *optr0,
		   unsigned char *optr1,
		   int outskip,
		   int outstride);

void composite_3ch(int xsize, 
		   int ysize,
		   unsigned char *iptr0, 
		   unsigned char *iptr1, 
		   unsigned char *iptr2, 
		   int inskip,
		   int instride,
		   unsigned char *iptra, 
		   int askip,
		   int astride,
		   unsigned char *optr0,
		   unsigned char *optr1,
		   unsigned char *optr2,
		   int outskip,
		   int outstride);

void scale_composite_byte_1ch(int inxsize,
			      int inysize,
			      unsigned char *iptr0,	     
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      int outxsize,
			      int outysize,
			      unsigned char *optr0,
			      int outskip,
			      int outstride, 
			      int *xremap,
			      int *yremap);

void scale_composite_byte_2ch(int inxsize,
			      int inysize,
			      unsigned char *iptr0,	     
			      unsigned char *iptr1,	     
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      int outxsize,
			      int outysize,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      int outskip,
			      int outstride, 
			      int *xremap,
			      int *yremap);

void scale_composite_byte_3ch(int inxsize,
			      int inysize,
			      unsigned char *iptr0,	     
			      unsigned char *iptr1,	     
			      unsigned char *iptr2,	     
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      int outxsize,
			      int outysize,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      unsigned char *optr2,
			      int outskip,
			      int outstride, 
			      int *xremap,
			      int *yremap);

void composite_remapalpha_1ch(unsigned char *remapalpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      unsigned char *optr0,
			      int outskip,
			      int outstride);

void composite_remapalpha_2ch(unsigned char *remapalpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      unsigned char *iptr1, 
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      int outskip,
			      int outstride);

void composite_remapalpha_3ch(unsigned char *remapalpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      unsigned char *iptr1, 
			      unsigned char *iptr2, 
			      int inskip,
			      int instride,
			      unsigned char *iptra, 
			      int askip,
			      int astride,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      unsigned char *optr2,
			      int outskip,
			      int outstride);

void scale_composite_remapalpha_byte_1ch(unsigned char *remapalpha,
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 int inskip,
					 int instride,
					 unsigned char *iptra, 
					 int askip,
					 int astride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap);

void scale_composite_remapalpha_byte_2ch(unsigned char *remapalpha,
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 unsigned char *iptr1,	     
					 int inskip,
					 int instride,
					 unsigned char *iptra, 
					 int askip,
					 int astride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 unsigned char *optr1,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap);

void scale_composite_remapalpha_byte_3ch(unsigned char *remapalpha,
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 unsigned char *iptr1,	     
					 unsigned char *iptr2,	     
					 int inskip,
					 int instride,
					 unsigned char *iptra, 
					 int askip,
					 int astride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 unsigned char *optr1,
					 unsigned char *optr2,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap);

void composite_constalpha_1ch(int alpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      int inskip,
			      int instride,
			      unsigned char *optr0,
			      int outskip,
			      int outstride);

void composite_constalpha_2ch(int alpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      unsigned char *iptr1, 
			      int inskip,
			      int instride,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      int outskip,
			      int outstride);

void composite_constalpha_3ch(int alpha,
			      int xsize, 
			      int ysize,
			      unsigned char *iptr0, 
			      unsigned char *iptr1, 
			      unsigned char *iptr2, 
			      int inskip,
			      int instride,
			      unsigned char *optr0,
			      unsigned char *optr1,
			      unsigned char *optr2,
			      int outskip,
			      int outstride);

void scale_composite_constalpha_byte_1ch(int alpha, 
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 int inskip,
					 int instride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap);

void scale_composite_constalpha_byte_2ch(int alpha, 
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 unsigned char *iptr1,	     
					 int inskip,
					 int instride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 unsigned char *optr1,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap);

void scale_composite_constalpha_byte_3ch(int alpha, 
					 int inxsize,
					 int inysize,
					 unsigned char *iptr0,	     
					 unsigned char *iptr1,	     
					 unsigned char *iptr2,	     
					 int inskip,
					 int instride,
					 int outxsize,
					 int outysize,
					 unsigned char *optr0,
					 unsigned char *optr1,
					 unsigned char *optr2,
					 int outskip,
					 int outstride, 
					 int *xremap,
					 int *yremap);

void composite_layer_1ch(int indepth,
			 int alpha_cutoff,
			 int xsize, 
			 int ysize,
			 unsigned char *iptr0, 
			 int inskip,
			 int instride,
			 unsigned char *iptra, 
			 int askip,
			 int astride,
			 unsigned char *optr0,
			 int outskip,
			 int outstride,
			 int *optrz,
			 int zskip,
			 int zstride);

void composite_layer_3ch(int indepth,
			 int alpha_cutoff,
			 int xsize, 
			 int ysize,
			 unsigned char *iptr0, 
			 unsigned char *iptr1, 
			 unsigned char *iptr2, 
			 int inskip,
			 int instride,
			 unsigned char *iptra, 
			 int askip,
			 int astride,
			 unsigned char *optr0,
			 unsigned char *optr1,
			 unsigned char *optr2,
			 int outskip,
			 int outstride,
			 int *optrz,
			 int zskip,
			 int zstride);

void composite_zbuffer_1ch(int zoffset,
			   int alpha_cutoff,
			   int xsize, 
			   int ysize,
			   unsigned char *iptr0, 
			   int inskip,
			   int instride,
			   unsigned char *iptra, 
			   int askip,
			   int astride,
			   int *iptrz,
			   int inzskip,
			   int inzstride,
			   unsigned char *optr0,
			   int outskip,
			   int outstride,
			   int *optrz,
			   int outzskip,
			   int outzstride);

void composite_zbuffer_3ch(int zoffset,
			   int alpha_cutoff,
			   int xsize, 
			   int ysize,
			   unsigned char *iptr0, 
			   unsigned char *iptr1, 
			   unsigned char *iptr2, 
			   int inskip,
			   int instride,
			   unsigned char *iptra, 
			   int askip,
			   int astride,
			   int *iptrz,
			   int inzskip,
			   int inzstride,
			   unsigned char *optr0,
			   unsigned char *optr1,
			   unsigned char *optr2,
			   int outskip,
			   int outstride,
			   int *optrz,
			   int outzskip,
			   int outzstride);

void zbuffer_1ch(int zoffset,
		 int xsize, 
		 int ysize,
		 unsigned char *iptr0, 
		 int inskip,
		 int instride,
		 int *iptrz,
		 int inzskip,
		 int inzstride,
		 unsigned char *optr0,
		 int outskip,
		 int outstride,
		 int *optrz,
		 int outzskip,
		 int outzstride);

void zbuffer_3ch(int zoffset,
		 int xsize, 
		 int ysize,
		 unsigned char *iptr0, 
		 unsigned char *iptr1, 
		 unsigned char *iptr2, 
		 int inskip,
		 int instride,
		 int *iptrz,
		 int inzskip,
		 int inzstride,
		 unsigned char *optr0,
		 unsigned char *optr1,
		 unsigned char *optr2,
		 int outskip,
		 int outstride,
		 int *optrz,
		 int outzskip,
		 int outzstride);

void layer_1ch(int indepth,
	       int xsize, 
	       int ysize,
	       unsigned char *iptr0, 
	       int inskip,
	       int instride,
	       unsigned char *optr0,
	       int outskip,
	       int outstride,
	       int *optrz,
	       int outzskip,
	       int outzstride);

void layer_3ch(int indepth,
	       int xsize, 
	       int ysize,
	       unsigned char *iptr0, 
	       unsigned char *iptr1, 
	       unsigned char *iptr2, 
	       int inskip,
	       int instride,
	       unsigned char *optr0,
	       unsigned char *optr1,
	       unsigned char *optr2,
	       int outskip,
	       int outstride,
	       int *optrz,
	       int outzskip,
	       int outzstride);

/*----------------------------------------------------------------------*/
/* SAMPLE TYPE CONVERSION */

void convert_byte_short(int xsize,
			int ysize,
			unsigned char *iptr,	     
			int inskip,
			int instride,
			short *optr,
			int outskip,
			int outstride);

void convert_byte_int(int xsize,
		      int ysize,
		      unsigned char *iptr,	     
		      int inskip,
		      int instride,
		      int *optr,
		      int outskip,
		      int outstride);

void convert_byte_long(int xsize,
		       int ysize,
		       unsigned char *iptr,	     
		       int inskip,
		       int instride,
		       long *optr,
		       int outskip,
		       int outstride);

void convert_byte_float(int xsize,
			int ysize,
			unsigned char *iptr,	     
			int inskip,
			int instride,
			float *optr,
			int outskip,
			int outstride);

void convert_byte_double(int xsize,
			 int ysize,
			 unsigned char *iptr,	     
			 int inskip,
			 int instride,
			 double *optr,
			 int outskip,
			 int outstride);

void convert_short_byte(int xsize,
			int ysize,
			short *iptr,	     
			int inskip,
			int instride,
			unsigned char *optr,
			int outskip,
			int outstride);

void convert_short_int(int xsize,
		       int ysize,
		       short *iptr,	     
		       int inskip,
		       int instride,
		       int *optr,
		       int outskip,
		       int outstride);

void convert_short_long(int xsize,
			int ysize,
			short *iptr,	     
			int inskip,
			int instride,
			long *optr,
			int outskip,
			int outstride);

void convert_short_float(int xsize,
			 int ysize,
			 short *iptr,	     
			 int inskip,
			 int instride,
			 float *optr,
			 int outskip,
			 int outstride);

void convert_short_double(int xsize,
			  int ysize,
			  short *iptr,	     
			  int inskip,
			  int instride,
			  double *optr,
			  int outskip,
			  int outstride);

void convert_int_byte(int xsize,
		      int ysize,
		      int *iptr,	     
		      int inskip,
		      int instride,
		      unsigned char *optr,
		      int outskip,
		      int outstride);

void convert_int_short(int xsize,
		       int ysize,
		       int *iptr,	     
		       int inskip,
		       int instride,
		       short *optr,
		       int outskip,
		       int outstride);

void convert_int_long(int xsize,
		      int ysize,
		      int *iptr,	     
		      int inskip,
		      int instride,
		      long *optr,
		      int outskip,
		      int outstride);

void convert_int_float(int xsize,
		       int ysize,
		       int *iptr,	     
		       int inskip,
		       int instride,
		       float *optr,
		       int outskip,
		       int outstride);

void convert_int_double(int xsize,
			int ysize,
			int *iptr,	     
			int inskip,
			int instride,
			double *optr,
			int outskip,
			int outstride);

void convert_long_byte(int xsize,
		       int ysize,
		       long *iptr,	     
		       int inskip,
		       int instride,
		       unsigned char *optr,
		       int outskip,
		       int outstride);

void convert_long_short(int xsize,
			int ysize,
			long *iptr,	     
			int inskip,
			int instride,
			short *optr,
			int outskip,
			int outstride);

void convert_long_int(int xsize,
		      int ysize,
		      long *iptr,	     
		      int inskip,
		      int instride,
		      int *optr,
		      int outskip,
		      int outstride);

void convert_long_float(int xsize,
			int ysize,
			long *iptr,	     
			int inskip,
			int instride,
			float *optr,
			int outskip,
			int outstride);

void convert_long_double(int xsize,
			 int ysize,
			 long *iptr,	     
			 int inskip,
			 int instride,
			 double *optr,
			 int outskip,
			 int outstride);

void convert_float_byte(int xsize,
			int ysize,
			float *iptr,	     
			int inskip,
			int instride,
			unsigned char *optr,
			int outskip,
			int outstride);

void convert_float_short(int xsize,
			 int ysize,
			 float *iptr,	     
			 int inskip,
			 int instride,
			 short *optr,
			 int outskip,
			 int outstride);

void convert_float_int(int xsize,
		       int ysize,
		       float *iptr,	     
		       int inskip,
		       int instride,
		       int *optr,
		       int outskip,
		       int outstride);

void convert_float_long(int xsize,
			int ysize,
			float *iptr,	     
			int inskip,
			int instride,
			long *optr,
			int outskip,
			int outstride);

void convert_float_double(int xsize,
			  int ysize,
			  float *iptr,	     
			  int inskip,
			  int instride,
			  double *optr,
			  int outskip,
			  int outstride);

void convert_double_byte(int xsize,
			 int ysize,
			 double *iptr,	     
			 int inskip,
			 int instride,
			 unsigned char *optr,
			 int outskip,
			 int outstride);

void convert_double_short(int xsize,
			  int ysize,
			  double *iptr,	     
			  int inskip,
			  int instride,
			  short *optr,
			  int outskip,
			  int outstride);

void convert_double_int(int xsize,
			int ysize,
			double *iptr,	     
			int inskip,
			int instride,
			int *optr,
			int outskip,
			int outstride);

void convert_double_long(int xsize,
			 int ysize,
			 double *iptr,	     
			 int inskip,
			 int instride,
			 long *optr,
			 int outskip,
			 int outstride);

void convert_double_float(int xsize,
			  int ysize,
			  double *iptr,	     
			  int inskip,
			  int instride,
			  float *optr,
			  int outskip,
			  int outstride);


/*----------------------------------------------------------------------*/
/* FILL CONSTANT */

/* These functions simply fill a buffer with a single value.  Use the
   function for the datatype you are using */

void fill_constant_byte(unsigned char val,
			int xsize,
			int ysize,
			unsigned char *ptr,
			int skip, 
			int stride);

void fill_constant_int(int val,
		       int xsize,
		       int ysize,
		       int *ptr,
		       int skip, 
		       int stride);

void fill_constant_long(long val,
			int xsize,
			int ysize,
			long *ptr,
			int skip, 
			int stride);

void fill_constant_float(float val,
			 int xsize,
			 int ysize,
			 float *ptr,
			 int skip, 
			 int stride);

void fill_constant_double(double val,
			  int xsize,
			  int ysize,
			  double *ptr,
			  int skip, 
			  int stride);

/*----------------------------------------------------------------------*/
/* FILTERS */

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
	      int outstride);

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
	      int outstride);

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
	      int outstride);

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
		 int outstride);

int affine_warp(double a, double b, double c,
		double d, double e, double f,
		int xsize,
		int ysize,
		int * xbuf,
		int * ybuf,
		int skip,
		int stride);

int affine_warp_safe(double a, double b, double c,
		     double d, double e, double f,
		     int xsize,
		     int ysize,
		     int * xbuf,
		     int * ybuf,
		     int skip,
		     int stride,
		     int inxsize,
		     int inysize);

/*----------------------------------------------------------------------*/
/* GAIN / BIAS */

/* These functions perform gain and bias on buffers.  The input and
   output buffers may be identical if you wish to perform the
   operation in place.  

   gainbias_byte() is for unsigned char samples.  It is highly optimized as
   a table is generated containing the correct output values for all
   256 possible input values, and the table is used to modify the
   image.  

   gainbias_byte_short()
   gainbias_byte_int()
   gainbias_byte_float()
   gainbias_byte_double()  take an unsigned char input and generate an
      output buffer of another type, clipping appropriately.
      These are all equally optimized as gainbias_byte()...

   gainbias_short_byte()
   gainbias_short() 
   gainbias_short_int()
   gainbias_short_float()
   gainbias_short_double()  take an signed short input and generate an
      output buffer of another type, clipping appropriately.

   gainbias_int() works on integer samples.  It is optimized so if you
   pass 1.0 for the gain, only integer addition will be done for the
   bias, possibly saving cycles.  If you pass 0.0 for the bias, only
   gaining arithmetic is done.  If you pass 1.0 and 0.0 for gain and
   bias, no arithmetic is done and the buffer is just transferred. 

   gainbias_double() works on double samples.  If you pass 1.0 for the
   gain or 0.0 for the bias, the corresponding arithmetic is not done
   for faster performance.  */

void gainbias_byte(double gain,
		   double bias,
		   int xsize,
		   int ysize,
		   unsigned char *iptr,
		   int inskip,
		   int instride,
		   unsigned char *optr,
		   int outskip,
		   int outstride);

void gainbias_byte_short(double gain, double bias, int xsize, int ysize,
			 unsigned char *iptr, int inskip,  int instride,
			 signed short *optr, int outskip, int outstride);

void gainbias_byte_int(double gain, double bias, int xsize, int ysize,
		       unsigned char *iptr, int inskip, int instride,
		       int *optr, int outskip, int outstride);

void gainbias_byte_long(double gain, double bias, int xsize, int ysize,
			unsigned char *iptr, int inskip, int instride,
			long *optr, int outskip, int outstride);

void gainbias_byte_float(double gain, double bias, int xsize, int ysize,
			 unsigned char *iptr, int inskip, int instride,
			 float *optr, int outskip, int outstride);

void gainbias_byte_double(double gain, double bias, int xsize, int ysize,
			  unsigned char *iptr, int inskip, int instride,
			  double *optr, int outskip, int outstride);


void gainbias_short(double gain,
		    double bias,
		    int xsize,
		    int ysize,
		    short *iptr,
		    int inskip,
		    int instride,
		    short *optr,
		    int outskip,
		    int outstride);

void gainbias_short_byte(double gain, double bias, int xsize, int ysize,
		   signed short *iptr, int inskip,  int instride,
		   unsigned char *optr, int outskip, int outstride);

void gainbias_short_int(double gain, double bias, int xsize, int ysize,
		   signed short *iptr, int inskip, int instride,
		   int *optr, int outskip, int outstride);

void gainbias_short_long(double gain, double bias, int xsize, int ysize,
		   signed short *iptr, int inskip, int instride,
		   long *optr, int outskip, int outstride);

void gainbias_short_float(double gain, double bias, int xsize, int ysize,
		   signed short *iptr, int inskip, int instride,
		   float *optr, int outskip, int outstride);

void gainbias_short_double(double gain, double bias, int xsize, int ysize,
		   signed short *iptr, int inskip, int instride,
		   double *optr, int outskip, int outstride);


void gainbias_int(double gain,
		  double bias,
		  int xsize,
		  int ysize,
		  int *iptr,
		  int inskip,
		  int instride,
		  int *optr,
		  int outskip,
		  int outstride);

void gainbias_long(double gain,
		   double bias,
		   int xsize,
		   int ysize,
		   long *iptr,
		   int inskip,
		   int instride,
		   long *optr,
		   int outskip,
		   int outstride);

void gainbias_float(float gain,
		    float bias,
		    int xsize,
		    int ysize,
		    float *iptr,
		    int inskip,
		    int instride,
		    float *optr,
		    int outskip,
		    int outstride);

void gainbias_double(double gain,
		     double bias,
		     int xsize,
		     int ysize,
		     double *iptr,
		     int inskip,
		     int instride,
		     double *optr,
		     int outskip,
		     int outstride);

/*----------------------------------------------------------------------*/
/* RANGE SELECTION */

/* Pixels that have a value within the given range are selected and
   copied to the output buffer.  All others are not copied. */

void range_1ch_byte(unsigned char low,
		    unsigned char high,
		    int xsize,
		    int ysize,
		    unsigned char *iptr,
		    int inskip,
		    int instride,
		    unsigned char *optr,
		    int outskip,
		    int outstride);

void range_3ch_byte(unsigned char low0,
		    unsigned char high0,
		    unsigned char low1,
		    unsigned char high1,
		    unsigned char low2,
		    unsigned char high2,
		    int xsize,
		    int ysize,
		    unsigned char *iptr0,
		    unsigned char *iptr1,
		    unsigned char *iptr2,
		    int inskip,
		    int instride,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    unsigned char *optr2,
		    int outskip,
		    int outstride);

void range_1ch_int(int low,
		   int high,
		   int xsize,
		   int ysize,
		   int *iptr,
		   int inskip,
		   int instride,
		   int *optr,
		   int outskip,
		   int outstride);

void range_3ch_int(int low0,
		   int high0,
		   int low1,
		   int high1,
		   int low2,
		   int high2,
		   int xsize,
		   int ysize,
		   int *iptr0,
		   int *iptr1,
		   int *iptr2,
		   int inskip,
		   int instride,
		   int *optr0,
		   int *optr1,
		   int *optr2,
		   int outskip,
		   int outstride);

void range_1ch_long(long low,
		    long high,
		    int xsize,
		    int ysize,
		    long *iptr,
		    int inskip,
		    int instride,
		    long *optr,
		    int outskip,
		    int outstride);

void range_3ch_long(long low0,
		    long high0,
		    long low1,
		    long high1,
		    long low2,
		    long high2,
		    int xsize,
		    int ysize,
		    long *iptr0,
		    long *iptr1,
		    long *iptr2,
		    int inskip,
		    int instride,
		    long *optr0,
		    long *optr1,
		    long *optr2,
		    int outskip,
		    int outstride);

void range_1ch_float(float low,
		     float high,
		     int xsize,
		     int ysize,
		     float *iptr,
		     int inskip,
		     int instride,
		     float *optr,
		     int outskip,
		     int outstride);

void range_3ch_float(float low0,
		     float high0,
		     float low1,
		     float high1,
		     float low2,
		     float high2,
		     int xsize,
		     int ysize,
		     float *iptr0,
		     float *iptr1,
		     float *iptr2,
		     int inskip,
		     int instride,
		     float *optr0,
		     float *optr1,
		     float *optr2,
		     int outskip,
		     int outstride);

void range_1ch_double(double low,
		      double high,
		      int xsize,
		      int ysize,
		      double *iptr,
		      int inskip,
		      int instride,
		      double *optr,
		      int outskip,
		      int outstride);

void range_3ch_double(double low0,
		      double high0,
		      double low1,
		      double high1,
		      double low2,
		      double high2,
		      int xsize,
		      int ysize,
		      double *iptr0,
		      double *iptr1,
		      double *iptr2,
		      int inskip,
		      int instride,
		      double *optr0,
		      double *optr1,
		      double *optr2,
		      int outskip,
		      int outstride);


/*----------------------------------------------------------------------*/
/* SCALING */

void scale_byte_1ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap);

void scale_byte_2ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    unsigned char *iptr1,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap);

void scale_byte_3ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    unsigned char *iptr1,	     
		    unsigned char *iptr2,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    unsigned char *optr2,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap);

void scale_byte_4ch(int inxsize,
		    int inysize,
		    unsigned char *iptr0,	     
		    unsigned char *iptr1,	     
		    unsigned char *iptr2,	     
		    unsigned char *iptr3,	     
		    int inskip,
		    int instride,
		    int outxsize,
		    int outysize,
		    unsigned char *optr0,
		    unsigned char *optr1,
		    unsigned char *optr2,
		    unsigned char *optr3,
		    int outskip,
		    int outstride, 
		    int *xremap,
		    int *yremap);

void scale_range_byte_1ch(unsigned char low,
			  unsigned char high,
			  int inxsize,
			  int inysize,
			  unsigned char *iptr0,	     
			  int inskip,
			  int instride,
			  int outxsize,
			  int outysize,
			  unsigned char *optr0,
			  int outskip,
			  int outstride, 
			  int *xremap,
			  int *yremap);

void scale_range_byte_2ch(unsigned char low,
			  unsigned char high,
			  int inxsize,
			  int inysize,
			  unsigned char *iptr0,	     
			  unsigned char *iptr1,	     
			  int inskip,
			  int instride,
			  int outxsize,
			  int outysize,
			  unsigned char *optr0,
			  unsigned char *optr1,
			  int outskip,
			  int outstride, 
			  int *xremap,
			  int *yremap);

void scale_range_byte_3ch(unsigned char low,
			  unsigned char high,
			  int inxsize,
			  int inysize,
			  unsigned char *iptr0,	     
			  unsigned char *iptr1,	     
			  unsigned char *iptr2,	     
			  int inskip,
			  int instride,
			  int outxsize,
			  int outysize,
			  unsigned char *optr0,
			  unsigned char *optr1,
			  unsigned char *optr2,
			  int outskip,
			  int outstride, 
			  int *xremap,
			  int *yremap);


/*----------------------------------------------------------------------*/
/* TRANSFERS */

/* These functions perform simple transfers from one region to
   another.  Choose the function for your sample type. */

void transfer_byte(int xsize,
		   int ysize,
		   unsigned char *iptr,	     
		   int inskip,
		   int instride,
		   unsigned char *optr,
		   int outskip,
		   int outstride);

void transfer_int(int xsize,
		  int ysize,
		  int *iptr,	     
		  int inskip,
		  int instride,
		  int *optr,
		  int outskip,
		  int outstride);

void transfer_short(int xsize,
		    int ysize,
		    short *iptr,	     
		    int inskip,
		    int instride,
		    short *optr,
		    int outskip,
		    int outstride);

void transfer_long(int xsize,
		   int ysize,
		   long *iptr,	     
		   int inskip,
		   int instride,
		   long *optr,
		   int outskip,
		   int outstride);

void transfer_float(int xsize,
		    int ysize,
		    float *iptr,	     
		    int inskip,
		    int instride,
		    float *optr,
		    int outskip,
		    int outstride);

void transfer_double(int xsize,
		     int ysize,
		     double *iptr,	     
		     int inskip,
		     int instride,
		     double *optr,
		     int outskip,
		     int outstride);

void remap_intensity(int xsize,
		     int ysize,
		     unsigned char *iptr,	     
		     int inskip,
		     int instride,
		     unsigned char *optr,
		     int outskip,
		     int outstride,
		     unsigned char *remap);


/*----------------------------------------------------------------------*/
/* IMAGE WARPING */

/* Image warping is basically a remapping of a 2D array of pixels based */
/* on a map of destination to source, tri-linear interpolation is performed */
/* unless otherwise noted */

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
		  int h);

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
		  int h);

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
		  int h);

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
		  int h);

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
			    int h);

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
			    int h);

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
			    int h);

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
			    int h);

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
			  int h);

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
			  int h);

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
			  int h);

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
			  int h);


/*----------------------------------------------------------------------*/
/* FLOOD FILL */

/* Given an alpha plane defining multiple foreground regions and a single x-y
   position in one of the foreground regions, this routine uses a flood fill to 
   mark all the pixels in the selected foreground region.

   This routine written and maintained by Bill Butera
 */

int flood_fill_byte (unsigned char *image, 
		     unsigned char *mask, 
		     int height, 
		     int width, 
		     int ycen, 
		     int xcen) ;


int flood_fill_short (short int *image, 
		      unsigned char *mask, 
		      int height, 
		      int width, 
		      int ycen, 
		      int xcen) ;


/*----------------------------------------------------------------------*/
/* OPTICAL FLOW                                                         */

/* Compute the warping function between two images using a varient of optical
   flow described by Martinez and Krause.

   This routine written and maintained by Bill Butera
 */

void calc_mvf_byte_C (unsigned char    *image, 
		      unsigned char    *model, 
		      int               height, 
		      int               width, 
		      float            *vy, 
		      float            *vx);

void calc_mvf_int_C (int              *image, 
		     int              *model, 
		     int               height, 
		     int               width, 
		     float            *vy, 
		     float            *vx);


#endif
