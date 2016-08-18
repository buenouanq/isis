
#define                        MIN_BLOBSIZE               50


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "isis_improc_private.h"

/* declarations of local functions, added by stefan */

static int update_block (unsigned long int ROS, unsigned long int *spread);
static void update_neighbors (unsigned long int *buf, int height, int width, int y, int x);
static void block_set_point (unsigned long int *spread, int height, int width, int ycen, int xcen);

static void block_pack (unsigned long int *ROS, 
			int ex_height, 
			int ex_width, 
			short int *image, 
			int height, 
			int width);

static void block_pack_byte (unsigned long int  *ROS, 
			     int                 ex_height, 
			     int                 ex_width, 
			     unsigned char      *image, 
			     int                 height, 
			     int                 width);

static void block_unpack (unsigned long int *spread, 
			  int ex_height, 
			  int ex_width, 
			  unsigned char *image, 
			  int height, 
			  int width);


/*
 *   f l o o d _ f i l l                                           Bill Butera
 *                                                                 August 4, 1998
 *
 *   Flood a region of interconnected pixels beginning at a given spatial position.
 *
 *   This routine is optimized for speed on a machine whose long integer word is 
 *   defined to be 64-bits.
 *
 *   Inputs :
 *              - an alpha plane where foreground pixels are defined by non-zero values
 *                 and background pixels are marked with a zero.
 *              - 2D dimensions of the alpha plane (height, width) 
 *              - 2D position from which to commence the flood-fill.
 *
 *   Outputs :
 *              - a mask with the flooded region markded with a 1 and the remainder 
 *                 marked with zeros.
 *
 *   Beginning at the 2D position given by ycen and xcen, the function iterativley activates 
 *   all those pixel positions which are both labeled as foreground in the alpha plane and 
 *   which are adjacent to previously actived pixels. The iteration terminates when pixels
 *   are no longer found which satisfy both conditions. The position of the activated pixels 
 *   are returned in the output mask.
 *
 *   The input alpha plane is expected to be an array of short int's. The output mask 
 *   is an array of unsigned bytes.
 *
 *   Function returns a 1 on success and a 0 on failure.
 */

flood_fill_short (short int *image, 
		  unsigned char *mask, 
		  int height,   int width, 
		  int ycen,     int xcen)
{
  int                   y, x, i, ptr, flag;
  int                   ex_height, ex_width, framesize;
  unsigned long int    *ROS, *spread;
  int                   max_iteration,  finished;
  

  if (sizeof(long int) != 8) {
    fprintf (stderr, "Deep doo doo. unexpected machine length for variable type 'long int'.");
    return (0);  }


       /*  Create a binary field for the selection data and pack into overlapping blocks. 
	*  Initially, this should be an empty bit field except for one point (ycen, xcen)      */


  ex_height = (int) ceil ((double)(height - 1) / 7.0);
  ex_width = (int) ceil ((double)(width - 1) / 6.0);
  framesize = ex_height * ex_width;
  if ((spread = (unsigned long *) calloc ((size_t)framesize, sizeof(unsigned long))) == (unsigned long *)NULL) {
    fprintf (stderr, "Croaked snarfing buffer space in function 'flood_fill'");
    return (0);  }
  block_set_point (spread, ex_height, ex_width, ycen, xcen);
  


        /*  Convert the image data into a binary field for the region of support (ROS). 
	 *  Pack this binary field into a series of overlapping blocks                           */

  if ((ROS = (unsigned long *) calloc ((size_t)framesize, sizeof(unsigned long))) == (unsigned long *)NULL) {
    fprintf (stderr, "Croaked snarfing buffer space in function 'flood_fill'");
    return (0);  }

  block_pack (ROS, ex_height, ex_width, image, height, width);



         /*  Loop over image executing the spread algorithm            */

  max_iteration = framesize;
  for (i=0; i<max_iteration; ++i)   {

    finished = 1;
    for (ptr=0, y=0; y<ex_height; ++y)
      for (x=0; x<ex_width; ++x, ++ptr) {
	flag = update_block (ROS[ptr], spread + ptr);
	if (flag == 1)  {
	  finished = 0;
	  update_neighbors (spread, ex_height, ex_width, y, x);  }
	else if (flag == 2)
	  update_neighbors (spread, ex_height, ex_width, y, x); }
    if (finished)
      break;

    finished = 1;
    for (ptr=framesize-1, y=ex_height-1; y>=0; --y)
      for (x=ex_width-1; x>=0; --x, --ptr)  {
	flag = update_block (ROS[ptr], spread + ptr);
	if (flag == 1)  {
	  finished = 0;
	  update_neighbors (spread, ex_height, ex_width, y, x);  }
	else if (flag == 3)
	  update_neighbors (spread, ex_height, ex_width, y, x); }
    if (finished)
      break;   }


             /*  Unpack the selection data into the output 'mask' structure  */

  block_unpack (spread, ex_height, ex_width, mask, height, width);

  free ((void *)ROS);
  free ((void *)spread);
  return (1);
  
}




/* 
 *  And here's the version that operates on unsigned char input
 */


flood_fill_byte (unsigned char *image, 
		 unsigned char *mask, 
		 int height,   int width, 
		 int ycen,     int xcen)
{
  int                   y, x, i, ptr, flag;
  int                   ex_height, ex_width, framesize;
  unsigned long int    *ROS, *spread;
  int                   max_iteration,  finished;
  

  if (sizeof(long int) != 8) {
    fprintf (stderr, "Deep doo doo. unexpected machine length for variable type 'long int'.");
    return (0);  }


       /*  Create a binary field for the selection data and pack into overlapping blocks. 
	*  Initially, this should be an empty bit field except for one point (ycen, xcen)      */


  ex_height = (int) ceil ((double)(height - 1) / 7.0);
  ex_width = (int) ceil ((double)(width - 1) / 6.0);
  framesize = ex_height * ex_width;
  if ((spread = (unsigned long *) calloc ((size_t)framesize, sizeof(unsigned long))) == (unsigned long *)NULL) {
    fprintf (stderr, "Croaked snarfing buffer space in function 'flood_fill'");
    return (0);  }
  block_set_point (spread, ex_height, ex_width, ycen, xcen);
  


        /*  Convert the image data into a binary field for the region of support (ROS). 
	 *  Pack this binary field into a series of overlapping blocks                           */

  if ((ROS = (unsigned long *) calloc ((size_t)framesize, sizeof(unsigned long))) == (unsigned long *)NULL) {
    fprintf (stderr, "Croaked snarfing buffer space in function 'flood_fill'");
    return (0);  }

  block_pack_byte (ROS, ex_height, ex_width, image, height, width);



         /*  Loop over image executing the spread algorithm            */

  max_iteration = framesize;
  for (i=0; i<max_iteration; ++i)   {

    finished = 1;
    for (ptr=0, y=0; y<ex_height; ++y)
      for (x=0; x<ex_width; ++x, ++ptr) {
	flag = update_block (ROS[ptr], spread + ptr);
	if (flag == 1)  {
	  finished = 0;
	  update_neighbors (spread, ex_height, ex_width, y, x);  }
	else if (flag == 2)
	  update_neighbors (spread, ex_height, ex_width, y, x); }
    if (finished)
      break;

    finished = 1;
    for (ptr=framesize-1, y=ex_height-1; y>=0; --y)
      for (x=ex_width-1; x>=0; --x, --ptr)  {
	flag = update_block (ROS[ptr], spread + ptr);
	if (flag == 1)  {
	  finished = 0;
	  update_neighbors (spread, ex_height, ex_width, y, x);  }
	else if (flag == 3)
	  update_neighbors (spread, ex_height, ex_width, y, x); }
    if (finished)
      break;   }


             /*  Unpack the selection data into the output 'mask' structure  */

  block_unpack (spread, ex_height, ex_width, mask, height, width);

  free ((void *)ROS);
  free ((void *)spread);
  return (1);
  
}







/*
 *  u p d a t e _ b l o c k 
 *
 *  Routine to do region fill withint a 8 x 7 block of data encoded
 *  into a single 64-bit long integer. Two blocks are passed as input
 *  with an altered version of one of the blocks returned as output.
 *
 *  Spatial organization : 
 *
 *  An 8 x 7 block of data from a binary 2D file is mapped to 64-bit
 *  unsigned long integer.
 *
 *  The 64-bit input words are divided into 8 8-bit gorups. Each 8-bit
 *  group consists of a gaurd bit followed by 7 bits of data assumed
 *  to originate from a single line from a 2D array. The guard bit
 *  must be set to zero. The 8-bit groups are assumed to be vertically
 *  aligned in the original image.
 *
 *  This routine uses bit shifts and boolean logic to file a region
 *  described in the region of support (ROS) variable and return it in
 *  the spread variable.
 *
 *   Mapping :
 *                        <--7-->
 *
 *                    1   ^^^^^^^
 *                    .   ^^^^^^^
 *                    .   ^^^^^^^
 *       block        .   ^^^^^^^
 *       from         .   ^^^^^^^
 *       image        .   ^^^^^^^
 *                    .   ^^^^^^^
 *                    8   ^^^^^^^
 *
 *       to 64-bit word
 *
 *      Line 1   Line 2   Line 3   Line 4   Line 5   Line 6   Line 7   Line 8 
 *
 *     X^^^^^^^ X^^^^^^^ X^^^^^^^ X^^^^^^^ X^^^^^^^ X^^^^^^^ X^^^^^^^ X^^^^^^^
 *
 *     MSB                                                                 LSB
 *
 */

static int update_block (unsigned long int ROS, unsigned long int *spread)
{
  unsigned long int                copy, prior, original, temp;
  int                              i;

  if (!ROS)               return (0);
  else if (!(*spread))    return (0);
  else if (!(~(*spread))) return (0);

  original = *spread;
  copy = *spread;
  for (i=0; i<36; ++i)  {
    prior = copy;
                                 /*  Horizontal spread  */
    temp = ROS & (copy << 1);
    copy = temp | copy;
    temp = ROS & (copy >> 1);
    copy = temp | copy;
                                 /*  Vertical spread  */
    temp = ROS & (copy << 8);
    copy = temp | copy;
    temp = ROS & (copy >> 8);
    copy = temp | copy;

    if (copy == prior)
      break;  }

  if (copy == original) {
    if ((copy & 0x0101010101010101) || (copy & 0x00000000000000ff))
      return (2);
    else if ((copy & 0x4040404040404040) || (copy & 0xff00000000000000))
      return (3);
    else
      return (0); }
  else {
    *spread = copy;
    return (1);    }
}






/*
 *    Function to update the overlapping pixels from neighboring block.
 *
 *    Each 64-bit word in 'buf' is assumed to contain a 8 x 7 block of 2D
 *    binarized data. Each block overlaps with its neighbors by one pixel
 *    depth along each border. 
 *
 *    The block at point (y,x) is assumed to have been altered. This function
 *    propagates those changes to its neighboring blocks in the four ordinal
 *    directions (north, south, east, and west).
 */

static void update_neighbors (unsigned long int *buf, int height, int width, int y, int x)
{
  unsigned long int            *source, *target, temp;

  source = &(buf[(y * width) + x]);
  if (y > 0) {                                   /* update to the north  */
    target = &(buf[((y-1) * width) + x]);
    temp = *source >> (7 * 8);
    *target = *target | temp;  }
  if (y < (height - 1)) {                        /* update to the south  */
    target = &(buf[((y+1) * width) + x]);
    temp = *source << (7 * 8);
    *target = *target | temp;  }
  if (x > 0) {                                   /* update to the west  */
    target = &(buf[(y * width) + x - 1]);
    temp = (*source >> 6) & (unsigned long) 0x0101010101010101;
    *target = *target | temp;  }
  if (x < (width - 1)) {                         /* update to the east  */
    target = &(buf[(y * width) + x + 1]);
    temp = (*source << 6) & (unsigned long) 0x4040404040404040;
    *target = *target | temp;  }

  return ;
}






static void block_set_point (unsigned long int *spread, int height, int width, int ycen, int xcen)
{
  int                      x, y, pix, line;
  unsigned long int        one=1;

  y = (int) floor ((double)ycen / 14.0) * 2;
  if ((line = ycen % 14) > 7)  {
    line -= 7;
    y++;   }

  x = (int) floor ((double)xcen / 12.0) * 2;
  if ((pix = xcen % 12) > 6)  {
    pix -= 6;
    x++;   }

  spread[(y * width) + x] = one << (((7 - line) * 8) + (6 - pix));

  return;
}







static void block_pack (unsigned long int *ROS, 
		 int ex_height, 
		 int ex_width, 
		 short int *image, 
		 int height, 
		 int width)
{
  short int              *imagePtr;
  int                     start_x, end_x;
  int                     start_y, end_y;
  int                     y, x, line, pix;
  int                     hslot, vslot, ptr, ptrOffset;
  unsigned long int      *target;

  for (ptr=0, y=0; y<ex_height; ++y)
    for (x=0; x<ex_width; ++x, ++ptr)   {             /*  Loop over each 8 x 7 block  */

      target = ROS + ptr;

      start_y = y * 7;
      end_y = start_y + 7;
      end_y = (end_y >= height) ? height - 1 : end_y;

      start_x = x * 6;
      end_x = start_x + 6;
      end_x = (end_x >= width) ? width - 1 : end_x;

      if ((y < (ex_height - 1)) && (x < (ex_width - 1)))  {
	ptrOffset = (int)width - 7;

	imagePtr = image + ((start_y * width) + start_x);
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 2  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 3  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 4  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 5 **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 6  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 7  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 8  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

      }

      else {
	vslot = 0;
	line = start_y;
	while (line <= end_y)  {

	  *target = *target << 1;            /* This shift inserts the gaurd bit which separates lines  */
	  hslot = 1;
	  imagePtr = image + ((line * width) + start_x);
	  pix = start_x;
	  while (pix <= end_x) {
	    *target = *target << 1;
	    if (*imagePtr++ != 0)
	      *target = *target | 0x01;
	    pix++;
	    hslot++;  }

	  *target = *target << (8 - hslot);

	  line++;
	  vslot++;  }
	
	*target = (*target) << ((8 - vslot) * 8);  }}

  return ;
}







static void block_pack_byte (unsigned long int  *ROS, 
		      int                 ex_height, 
		      int                 ex_width, 
		      unsigned char      *image, 
		      int                 height, 
		      int                 width)
{
  unsigned char          *imagePtr;
  int                     start_x, end_x;
  int                     start_y, end_y;
  int                     y, x, line, pix;
  int                     hslot, vslot, ptr, ptrOffset;
  unsigned long int      *target;

  for (ptr=0, y=0; y<ex_height; ++y)
    for (x=0; x<ex_width; ++x, ++ptr)   {             /*  Loop over each 8 x 7 block  */

      target = ROS + ptr;

      start_y = y * 7;
      end_y = start_y + 7;
      end_y = (end_y >= height) ? height - 1 : end_y;

      start_x = x * 6;
      end_x = start_x + 6;
      end_x = (end_x >= width) ? width - 1 : end_x;

      if ((y < (ex_height - 1)) && (x < (ex_width - 1)))  {
	ptrOffset = (int)width - 7;

	imagePtr = image + ((start_y * width) + start_x);
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 2  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 3  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 4  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 5 **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 6  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 7  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

	imagePtr += ptrOffset;               /**  Line 8  **/
	*target = *target << 1;              /* This shift inserts the gaurd bit which separates lines  */
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;
	*target = *target << 1;
	if (*imagePtr++ != 0)  *target = *target | 0x01;

      }

      else {
	vslot = 0;
	line = start_y;
	while (line <= end_y)  {

	  *target = *target << 1;            /* This shift inserts the gaurd bit which separates lines  */
	  hslot = 1;
	  imagePtr = image + ((line * width) + start_x);
	  pix = start_x;
	  while (pix <= end_x) {
	    *target = *target << 1;
	    if (*imagePtr++ != 0)
	      *target = *target | 0x01;
	    pix++;
	    hslot++;  }

	  *target = *target << (8 - hslot);

	  line++;
	  vslot++;  }
	
	*target = (*target) << ((8 - vslot) * 8);  }}

  return ;
}































static void block_unpack (unsigned long int *spread, 
		   int ex_height, 
		   int ex_width, 
		   unsigned char *image, 
		   int height, 
		   int width)
{
  unsigned char          *imagePtr;
  int                     start_x, end_x;
  int                     start_y, end_y;
  int                     y, x, line, pix;
  int                     hslot, vslot, ptr, ptrOffset;
  unsigned long int      *source, msb_mask;

  msb_mask = (unsigned long) 0x8000000000000000;
  for (ptr=0, y=0; y<ex_height; ++y)
    for (x=0; x<ex_width; ++x, ++ptr)   {             /*  Loop over each 8 x 7 block  */

      source = spread + ptr;

      start_y = y * 7;
      end_y = start_y + 7;
      end_y = (end_y >= height) ? height - 1 : end_y;

      start_x = x * 6;
      end_x = start_x + 6;
      end_x = (end_x >= width) ? width - 1 : end_x;

      if ((y < (ex_height - 1)) && (x < (ex_width - 1)))  {
	ptrOffset = (int)width - 7;

	imagePtr = image + ((start_y * width) + start_x);
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

	imagePtr += ptrOffset;                 /**  Line 2  **/
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

	imagePtr += ptrOffset;                 /**  Line 3  **/
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

	imagePtr += ptrOffset;                 /**  Line 4  **/
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

	imagePtr += ptrOffset;                 /**  Line 5  **/
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

	imagePtr += ptrOffset;                 /**  Line 6  **/
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

	imagePtr += ptrOffset;                 /**  Line 7  **/
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

	imagePtr += ptrOffset;                 /**  Line 8  **/
	*source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;
	if (*source & msb_mask)      *imagePtr++ = 1;
	else                         *imagePtr++ = 0;
	*source = *source << 1;

      }

      else  {
	vslot = 0;
	line = start_y;
	while (line <= end_y)  {

	  *source = *source << 1;                /* This shift extracts the gaurd bit which separates lines  */
	  hslot = 1;
	  imagePtr = image + ((line * width) + start_x);
	  pix = start_x;
	  while (pix <= end_x) {
	    if (*source & msb_mask)
	      *imagePtr++ = 1;
	    else
	      *imagePtr++ = 0;
	    *source = *source << 1;
	    pix++;
	    hslot++;  }

	  *source = *source << (8 - hslot);

	  line++;
	  vslot++; }}}

  return ;
}
