/* Image Segmentation library */
/* contributors: Stefan Agamanolis, Cian Cullinan, John Watlington */

/*----------------------------------------------------------------------*/
/* Isis interface functions */

Value *isis_init_bkseg_yuv(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *isis_bkseg_yuv(Script *script, 
		      char *proc_name, int call_data, 
		      int argc, Value **argv);

Value *isis_init_chromakey(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *isis_chromakey(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);

Value *isis_simple_bkseg(Script *script, 
			 char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *isis_simple_bkseg_yuv(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv);

Value *isis_init_var_bkseg_table(Script *script, 
				 char *proc_name, int call_data, 
				 int argc, Value **argv);
Value *isis_var_bkseg_yuv(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *isis_var_bkseg_rgb(Script *script,
			  char *proc_name, int call_data,
			  int argc, Value **argv);

Value *isis_fill_holes(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);

Value *isis_init_encode_image_yuv(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv);

Value *isis_init_encode_image_rgb(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv);

Value *isis_encode_image_yuv(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);

Value *isis_encode_image_rgb(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv);

Value *isis_decode_image_yuv(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);

Value *isis_decode_image_rgb_planar(Script *script, char *proc_name, int call_data, 
				    int argc, Value **argv);

Value *isis_decode_image_rgb_interleaved(Script *script, char *proc_name, int call_data,
					 int argc, Value **argv);

Value *isis_seg_color_rgb(Script *script, char *proc_name, int call_data,
			  int argc, Value **argv);

/*----------------------------------------------------------------------*/
/* CHROMAKEY */

/* This chromakeyer works on rgb images only, but you can key off of
   either r g or b */

/* First call init_chromakey() with an image of only the color you are
   keying off of (usually a background image of all blue).  Pass the
   color channel to be keyed as the 'chank' argument (usually the blue
   channel).  Pass the x and y sizes of your image accordingly.  Pass
   1 for 'reset' if you are starting a new key, or 0 if you want to
   add to a previous one. */

/* After initializing the chromakeyer to your background color image,
   you can do the actual chromakey by calling chromakey(...).  Pass
   your input buffers in 'inchan0', 'inchan1', and 'inchank'
   ('inchank' must be the channel you are keying on).  Pass your
   source x and y sizes and stride.  Also pass output buffers for the
   new keyed channel ('outchank') and the alpha buffer ('outalpha').
   IMPORTANT: 'inchank' and 'outchank' CANNOT BE THE SAME BUFFER.
   Pass a value below which the image will be FULLY background
   ('maxbg' -- usually just 0).  Also pass a value for the alpha
   channel above which you would like the image to be considered FULLY
   foreground ('minfg' -- usually 255).  */

/* init_chromakey modes: */

void init_chromakey(int reset,   /* 1 to start new key, 0 to add to old */
		    int xsize,
		    int ysize,
		    unsigned char *chan0, 
		    unsigned char *chan1, 
		    unsigned char *chank,
		    int skip,
		    int stride);

void chromakey(int maxbg,
	       int minfg,
	       int xsize,
	       int ysize,
	       unsigned char *inchan0, 
	       unsigned char *inchan1, 
	       unsigned char *inchank,
	       int inskip,
	       int instride,
	       unsigned char *outchank, 
	       int keyskip,
	       int keystride,
	       unsigned char *outalpha, 
	       int outskip,
	       int outstride);

/*----------------------------------------------------------------------*/
/* BACKGROUND SEGMENTATION with VARIANCE */

void init_var_bkseg_table(unsigned char *segtable, 
			  double gain, 
			  double bias,
			  double vbias);

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
		   int h_xstep, int h_ystep);

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
		   int a_xstep, int a_ystep);

/*----------------------------------------------------------------------*/
/* BACKGROUND SEGMENTATION */

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
		    unsigned char *segmaps);

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
	       int astep_x, int astep_y);

void simple_bkseg_3ch(int xdim,
		      int ydim,
		      unsigned char *fbuf1, 
		      unsigned char *fbuf2, 
		      unsigned char *fbuf3, 
		      int fskip,
		      int fstride,
		      unsigned char *bbuf1, 
		      unsigned char *bbuf2, 
		      unsigned char *bbuf3,
		      int bskip,
		      int bstride,
		      int axdim,
		      int aydim,
		      unsigned char *abuf, 
		      int askip,
		      int astride, 
		      unsigned char *gbmap);

void simple_bkseg_1ch(int xdim,
		      int ydim,
		      unsigned char *fbuf1, 
		      int fskip,
		      int fstride,
		      unsigned char *bbuf1, 
		      int bskip,
		      int bstride,
		      int axdim,
		      int aydim,
		      unsigned char *abuf, 
		      int askip,
		      int astride, 
		      unsigned char *gbmap);

void simple_bkseg_yuv(int xdim,
		      int ydim,
		      unsigned char *fybuf, 
		      int fskip,
		      int fstride,
		      unsigned char *fubuf, 
		      unsigned char *fvbuf, 
		      int fuvskip,
		      int fuvstride,
		      unsigned char *bybuf, 
		      int bskip,
		      int bstride,
		      unsigned char *bubuf, 
		      unsigned char *bvbuf,
		      int buvskip,
		      int buvstride,
		      int axdim,
		      int aydim,
		      unsigned char *abuf, 
		      int askip,
		      int astride, 
		      unsigned char *ygbmap,
		      unsigned char *ugbmap,
		      unsigned char *vgbmap);

/*----------------------------------------------------------------------*/
/* MISCELLANEOUS */

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
		int outstride);

void init_encode_image_yuv(int ybits,
			   int ubits,
			   int vbits,
			   int abits);

void init_encode_image_rgb(int rbits,
			   int gbits,
			   int bbits,
			   int abits);

int encode_image_yuv(int xsize, int ysize,
		     unsigned char *ybuf, 
		     int y_xstep, int y_ystep,
		     unsigned char *ubuf, 
		     unsigned char *vbuf, 
		     int uv_xstep, int uv_ystep,
		     unsigned char *abuf, 
		     int a_xstep, int a_ystep,
		     unsigned char *outbuf);

int encode_image_rgb(int xsize, int ysize,
		     unsigned char *rbuf, 
		     unsigned char *gbuf, 
		     unsigned char *bbuf, 
		     int xstep, int ystep,
		     unsigned char *abuf, 
		     int a_xstep, int a_ystep,
		     unsigned char *outbuf);

int decode_image_yuv(unsigned char *ibuf,
		     unsigned char *outbuf, 
		     int *xsize, int *ysize);

int decode_image_rgb_planar(unsigned char *ibuf,
			    unsigned char *outbuf, 
			    int *xsize, int *ysize);

int decode_image_rgb_interleaved(unsigned char *ibuf,
				 unsigned char *outbuf, 
				 int *xsize, int *ysize);

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
		   int askip, int astride);
