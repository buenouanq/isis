#include <X11/Xlib.h>
#include <X11/Xutil.h>
/* #include <X11/StringDefs.h> */
/* StringDefs appears to be deprecated as of August 2004 -stefan */

/*----------------------------------------------------------------------*/
/* DITHERING */

/* These functions handle color->color, color->gray, and gray->gray
   dithering.  The stride of the data must be the same as the xsize of
   the data in calls to these functions (for now) */

/* You do NOT need to know about this structure:. */

typedef struct DitherChannelStruct {
  int dither_bits;		/* #bits of display used for this channel */
  int data_bits;		/* #bits of data used for this channel */
  int size;			/* power of 4 size of the dither matrix */
  int dim;			/* power of 4 */
  int max_data;			/* max data value */
  int num_data;			/* max data value + 1 */
  int num_regions;		/* number of matrices that fill the space */
  int reg_div_data;		/* num_regions / num_data */
  int data_div_reg;		/* num_data / num_regions */
  int error_help;		/* 255 / POWER2(dither_bits)-1 */
  unsigned char dither_matrix[256]; /* holds the 2D dither map */
} DitherChannelStruct;

/* This is the Dither structure.  You are responsible for actually
   allocating one of these structures to use in passing to the
   functions below.  */

typedef struct Dither {
  DitherChannelStruct dr,dg,db;
  int red_plane_size;
  int green_row_size;
  int dither_from_color;	/* dithering from 24bit color image? */
  int dither_to_color;		/* dithering to color image? */
  int dither_bits;		/* max bits of display to use */
} Dither;

/* Dithering quality--use in calls to DitherData() */

#define Decimate 0
#define OrderedDither 1
#define FloydSteinbergErrorDiffusion 2

/* This function initializes a 'Dither' structure that YOU must
   allocated or defined in your program to display 1 or 3 channels of
   8 bits per channel data.  dither_from_color = 0 if original image
   is greyscale, dither_to_color = 0 if you are dithering to
   greyscale, dither_bits = however many bits are available to dither
   to, lut will be filled with the colormap values that should be
   installed on your screen */

void InitDither(Dither *dith,	/* the structure to initialize */
		int dither_from_color, /* going from color image? */
		int dither_to_color, /* going to color image? */
		int dither_bits, /* number of bits to dither to */
		unsigned char lut[256][3]); /* lut for the output buffer */

/* This function is called after InitDither to dither the 1 or 3
   channels of data into the output buffer. The values left in the
   output buffer correspond to lut values returned by InitDither.  If
   dithering 1 channel greyscale images, gbuf and bbuf should be
   NULL.  */

void DitherData(Dither *dith,
		int dither_quality,
		int xsize,	/* image dimensions */
		int ysize,
		unsigned char *rbuf, /* 8 bits/channel data */
		unsigned char *gbuf,
		unsigned char *bbuf,
		int inskip,
		int instride,
		unsigned char *out, /* the output buffer */
		int outskip,    
		int outstride);

/* This function can be used if you have a 24 bit display.  The r g b
   data for each pixel is interleaved in 4-byte units in the form
   0x00RRGGBB.  FOR THIS REASON, THE OUTPUT BUFFER MUST BE 4 TIMES THE
   SIZE OF AN INDIVIDUAL COLOR CHANNEL BUFFER.  If using only 1 color
   channel gbuf and bbuf should be NULL.  Use msb_flag for altering
   byte order.  This function added by Stefan Agamanolis */

void InterleaveData(int xsize,	/* image dimensions */
		    int ysize,
		    unsigned char *r, /* R */
		    unsigned char *g, /* G */
		    unsigned char *b, /* B */
		    int inskip,
		    int instride,
		    unsigned char *op, /* usually 4X ORIG SIZE */
		    int outskip, /* usually 4 */
		    int outstride, /* usually 4 * xsize */
		    int msb_data, /* flag for MSB ordering */
		    int red_in_lowest_byte); /* flag for red in lowest byte */

/*----------------------------------------------------------------------*/
/* definitions for x display */

#define NUM_INPUTS 7
/*bool was defined as int in an Xlib.h*/

typedef struct ImageWindow {
  int child;
  Colormap defcmap, cmap;
  GC defgc, gc;
  Display *display;
  Visual *visual;
  int screen;
  Window rootwindow, imagewindow, subwindows[1];
  XVisualInfo vinfo;
  XWMHints xwmh;
  XSizeHints size;
  XSetWindowAttributes winattr;
  XImage *image;
  XImage *tempimage;
  Dither *dith;
  int quality, display_bits, display_colors, gray, private_cmap;
  int bufsize;
  int num_inputs;
  int *inputs;
  long xeventmask;
} ImageWindow;

/* ------------------------- */
/* Global functions */

ImageWindow *new_image_window(char *displayname, 
			      char *title,
			      int xpos, int ypos,
			      int xsize, int ysize,
			      int max_xsize, 
			      int max_ysize, 
			      int dither_quality,
			      int map_at_creation);

ImageWindow *new_child_image_window(ImageWindow *parwin, 
				    int xpos, int ypos,
				    int xsize, int ysize,
				    int map_at_creation);

void show_image_window(ImageWindow *imgwin);

void hide_image_window(ImageWindow *imgwin);

void move_image_window(ImageWindow *imgwin, int x, int y);

void resize_image_window(ImageWindow *imgwin, int x, int y);

void set_image_window_title(ImageWindow *imgwin, char *title);

void destroy_image_window(ImageWindow *imgwin);

void display_image(ImageWindow *imgwin,
		   int xpos, int ypos,
		   int xsize, int ysize, 
		   int skip, int stride,
		   unsigned char *r,
		   unsigned char *g, 
		   unsigned char *b);

void display_32bit_interleaved_image(ImageWindow *imgwin,
				     int xpos, int ypos,
				     int xsize, int ysize, 
				     int skip, int stride,
				     unsigned char *buf);

int pointer_available(ImageWindow *imgwin);

int pointer_active(ImageWindow *imgwin);

int pointer_inside(ImageWindow *imgwin);

int pointer_location(ImageWindow *imgwin, int *x, int *y);

int number_of_inputs(ImageWindow *imgwin);

int *poll_all_inputs(ImageWindow *imgwin, int *num_inputs);

int poll_input(ImageWindow *imgwin, int input_number);

void get_image_window_size(ImageWindow *imgwin, int *x, int *y);

void get_image_window_location(ImageWindow *imgwin, int *x, int *y);

#define EVENT_NONE 0
#define EVENT_POINTER_INSIDE 1
#define EVENT_POINTER_LOCATION 2
#define EVENT_WINDOW_INPUT 4
#define EVENT_WINDOW_GEOMETRY 8
#define EVENT_CHARACTER 16
#define EVENT_EXPOSE 32
#define EVENT_ALL 255

typedef union imgwin_event {
  int type;   /* must be first */
  struct {
    int type;
    int inside;
    int x, y;
    int num_inputs;
    int *inputs;
  } pointer_inside;
  struct {
    int type;
    int x, y;
    int num_inputs;
    int *inputs;
  } pointer_location;
  struct {
    int type;
    int input_number;
    int value;
    int x, y;
    int num_inputs;
    int *inputs;
  } window_input;
  struct {
    int type;
    int xloc, yloc;
    int xsize, ysize;
  } window_geometry;
  struct {
    int type;
    char key;
    int x, y;
    int num_inputs;
    int *inputs;
  } character;
  struct {
    int type;
    int x, y;
    int w, h;
  } expose;
} imgwin_event;

void select_events(ImageWindow *imgwin, int mask);

void dump_events(ImageWindow *imgwin, int mask);
int event_pending(ImageWindow *imgwin, int mask);
int get_event(ImageWindow *imgwin, imgwin_event *event, 
	      int block, int mask);


