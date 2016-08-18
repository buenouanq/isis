/*   color_const.c

     Color Processing Isis interface routines

     This file contains the color relaxation routines

     J. Watlington, 12/96

*/

#include <stdio.h>                /* Must include before isis.h */
#include <isis.h>                 /* Holds all Isis interpreter info */

#include "isis_color.h"           /* Holds a declaration bind_my_funcs() */
#include "isis_color_private.h"

/********************************************************
  Color Correction Interface Routines
*/

#define EDGE_THRESHOLD        32

/*  (image-color-relax-chan chromaptr edgeptr outputptr
                              xsize ysize
		              chroma_xstep chroma_ystep
                              edge_xstep edge_ystep
			      num-iter )
*/

Value *image_color_relax_chan(Script *script, char *proc_name, int call_data, 
	              int argc, Value **argv)
{
  register int            c_xstep, e_xstep, num_pts;
  register signed short   *cptr, *prev_cptr, *next_cptr;
  register byte           *eptr, *prev_eptr, *next_eptr;
  register signed short   *optr, average;

  int            x, y, iter;
  int            xsize, ysize, num_iter;
  int            c_ystep, e_ystep;

  signed short   *cbuf, *obuf;
  byte           *ebuf;
  signed short   *clineptr, *prev_clineptr, *next_clineptr, *olineptr;
  byte           *elineptr, *prev_elineptr, *next_elineptr;

  /*  First, check to see if the arguments are all there !   */

  checkCount(10, NULL);
  checkAddr(0, "chromabuf");
  checkAddr(1, "edgebuf");
  checkAddr(2, "outputbuf");
  checkInt(3, "xsize");
  checkInt(4, "ysize");
  checkInt(5, "chroma-xstep");
  checkInt(6, "chroma-ystep");
  checkInt(7, "edge-xstep");
  checkInt(8, "edge-ystep");
  checkInt(9, "num-iterations");

  /*  Now extract the arguments from their Isis representation  */

  cbuf = (signed short *)getAddr( argv[0] );
  ebuf = (byte *)getAddr( argv[1] );
  obuf = (signed short *)getAddr( argv[2] );
  xsize = getInt( argv[3] );
  ysize = getInt( argv[4] );
  c_xstep = getInt( argv[5] );
  c_ystep = getInt( argv[6] );
  e_xstep = getInt( argv[7] );
  e_ystep = getInt( argv[8] );
  num_iter = getInt( argv[9] );

  /* Now start the relaxation  */

  if( !(num_iter & 1) )  /* make num_iter odd so output ends up in right buf */
    num_iter++;

  for( iter = 0; iter < num_iter; iter++ )
    {
      /*  Initialize the sample pointers.  We have several, since
	  there is a neighborhood of three lines in edge and chroma bufs that
	  is being accessed.  I use a pointer for each line...

	  The pointers with "line" in their name always point to the beginning
	  of a line of data.
      */

      elineptr = ebuf;
      next_elineptr = ebuf + e_ystep;

      if( iter & 1 )
	{
	  clineptr = obuf;
	  olineptr = cbuf;
	}
      else
	{
	  clineptr = cbuf;
	  olineptr = obuf;
	}
      next_clineptr = clineptr + c_ystep;

      /*  Process the first (top) line in the image separately  */
      
      cptr = clineptr;
      next_cptr = next_clineptr;
      eptr = elineptr;
      next_eptr = next_elineptr;
      optr = olineptr;
      
      /*  Process the first sample in the first line  */
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*next_eptr < EDGE_THRESHOLD) &&
	     (*next_cptr != 0) ) {
	    average += *next_cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr + c_xstep) != 0) ) {
	    average += *(cptr + c_xstep);
	    num_pts++;
	  }
	  
	  if( *cptr != 0 ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = average / num_pts;
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;
      
      /*  Process the middle samples in the first line  */
      
      for( x = 1; x < xsize; x++ )
	{
	  cptr += c_xstep;
	  next_cptr += c_xstep;
	  eptr += e_xstep;
	  next_eptr += e_xstep;

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*next_eptr < EDGE_THRESHOLD) &&
		 (*next_cptr != 0) ) {
		average += *next_cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr + c_xstep) != 0) ) {
		average += *(cptr + c_xstep);
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr - c_xstep) != 0) ) {
		average += *(cptr - c_xstep);
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = average / num_pts;
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;
	}
      
      /*  Process the last sample of the first line here  */
      
      cptr += c_xstep;
      next_cptr += c_xstep;
      eptr += e_xstep;
      next_eptr += e_xstep;
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*next_eptr < EDGE_THRESHOLD) &&
	     (*next_cptr != 0) ) {
	    average += *next_cptr;
	    num_pts++;
	  }
	  
	  if( *cptr != 0 ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr - c_xstep) != 0) ) {
	    average += *(cptr - c_xstep);
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = average / num_pts;
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;

      /*  Process all the middle lines in the image  */

      for( y = 1; y < ysize; y++ )
	{
	  prev_cptr = clineptr;
	  prev_clineptr = prev_cptr;

	  cptr = next_clineptr;
	  clineptr = cptr;

	  next_cptr = next_clineptr + c_ystep;
	  next_clineptr = next_cptr;

	  prev_eptr = elineptr;
	  prev_elineptr = prev_eptr;

	  eptr = next_elineptr;
	  elineptr = eptr;

	  next_eptr = next_elineptr + e_ystep;
	  next_elineptr = next_eptr;

	  optr = olineptr + xsize;
	  olineptr = optr;

	  /*  Process the first sample in a line separately  */

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*prev_eptr < EDGE_THRESHOLD) &&
		 (*prev_cptr != 0) ) {
		average += *prev_cptr;
		num_pts++;
	      }
	      
	      if( (*next_eptr < EDGE_THRESHOLD) &&
		 (*next_cptr != 0) ) {
		average += *next_cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr + c_xstep) != 0) ) {
		average += *(cptr + c_xstep);
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = average / num_pts;
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;

	  /*  Process the middle samples in a line  */

	  for( x = 1; x < xsize; x++ )
	    {
	      cptr += c_xstep;
	      next_cptr += c_xstep;
	      prev_cptr += c_xstep;

	      eptr += e_xstep;
	      next_eptr += e_xstep;
	      prev_eptr += e_xstep;

	      if( *eptr < EDGE_THRESHOLD )
		{
		  num_pts = 0;
		  average = 0;
		  if( (*prev_eptr < EDGE_THRESHOLD) &&
		     (*prev_cptr != 0) ) {
		    average += *prev_cptr;
		    num_pts++;
		  }
		  
		  if( (*next_eptr < EDGE_THRESHOLD) &&
		     (*next_cptr != 0) ) {
		    average += *next_cptr;
		    num_pts++;
		  }
		  
		  if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		     (*(cptr + c_xstep) != 0) ) {
		    average += *(cptr + c_xstep);
		    num_pts++;
		  }
		  
		  if( *cptr != 0 ) {
		    average += *cptr;
		    num_pts++;
		  }
		  
		  if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		     (*(cptr - c_xstep) != 0) ) {
		    average += *(cptr - c_xstep);
		    num_pts++;
		  }
		  
		  if( num_pts > 0 )
		    *optr++ = average / num_pts;
		  else
		    *optr++ = 0;
		}
	      else
		{
		  *optr++ = *cptr;
		}
	    }

	  /*  Process the last sample in a line separately  */

	  cptr += c_xstep;
	  next_cptr += c_xstep;
	  prev_cptr += c_xstep;
	  
	  eptr += e_xstep;
	  next_eptr += e_xstep;
	  prev_eptr += e_xstep;

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*prev_eptr < EDGE_THRESHOLD) &&
		 (*prev_cptr != 0) ) {
		average += *prev_cptr;
		num_pts++;
	      }
	      
	      if( (*next_eptr < EDGE_THRESHOLD) &&
		 (*next_cptr != 0) ) {
		average += *next_cptr;
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr - c_xstep) != 0) ) {
		average += *(cptr - c_xstep);
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = average / num_pts;
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;
	}

      /*  Process the last (bottom) line of the image separately  */

      prev_cptr = clineptr;
      cptr = next_clineptr;
      prev_eptr = elineptr;
      eptr = next_elineptr;
      optr = olineptr + xsize;
      
      /*  Process the first sample of the last line here  */
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*prev_eptr < EDGE_THRESHOLD) &&
	     (*prev_cptr != 0) ) {
	    average += *prev_cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr + c_xstep) != 0) ) {
	    average += *(cptr + c_xstep);
	    num_pts++;
	  }
	  
	  if( *cptr != 0 ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = average / num_pts;
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;
      
      /*  Process the middle samples of the last line  */
      
      for( x = 1; x < xsize; x++ )
	{
	  cptr += c_xstep;
	  prev_cptr += c_xstep;

	  eptr += e_xstep;
	  prev_eptr += e_xstep;

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*prev_eptr < EDGE_THRESHOLD) &&
		 (*prev_cptr != 0) ) {
		average += *prev_cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr + c_xstep) != 0) ) {
		average += *(cptr + c_xstep);
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr - c_xstep) != 0) ) {
		average += *(cptr - c_xstep);
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = average / num_pts;
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;
	}
      
      /*  Process the last sample of the last line here  */
      
      cptr += c_xstep;
      prev_cptr += c_xstep;
      
      eptr += e_xstep;
      prev_eptr += e_xstep;
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*prev_eptr < EDGE_THRESHOLD) &&
	     (*prev_cptr != 0) ) {
	    average += *prev_cptr;
	    num_pts++;
	  }
	  
	  if( (*eptr < EDGE_THRESHOLD) &&
	     (*cptr != 0) ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr - c_xstep) != 0) ) {
	    average += *(cptr - c_xstep);
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = average / num_pts;
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;
    }

  return NullValue;
}


/*  (image-color-relax-byte-chan chromaptr edgeptr outputptr
                              xsize ysize
		              chroma_xstep chroma_ystep
                              edge_xstep edge_ystep
			      num-iter )
*/

Value *image_color_relax_byte_chan(Script *script, char *proc_name,
				   int call_data, 
				   int argc, Value **argv)
{
  register int    c_xstep, e_xstep, num_pts;
  register byte   *cptr, *prev_cptr, *next_cptr;
  register byte   *eptr, *prev_eptr, *next_eptr;
  register byte   *optr;
  register unsigned long  average;

  int             x, y, iter;
  int             xsize, ysize, num_iter;
  int             c_ystep, e_ystep;

  byte            *cbuf, *obuf;
  byte            *ebuf;
  byte            *clineptr, *prev_clineptr, *next_clineptr, *olineptr;
  byte            *elineptr, *prev_elineptr, *next_elineptr;

  /*  First, check to see if the arguments are all there !   */

  checkCount(10, NULL);
  checkAddr(0, "chromabuf");
  checkAddr(1, "edgebuf");
  checkAddr(2, "outputbuf");
  checkInt(3, "xsize");
  checkInt(4, "ysize");
  checkInt(5, "chroma-xstep");
  checkInt(6, "chroma-ystep");
  checkInt(7, "edge-xstep");
  checkInt(8, "edge-ystep");
  checkInt(9, "num-iterations");

  /*  Now extract the arguments from their Isis representation  */

  cbuf = (byte *)getAddr( argv[0] );
  ebuf = (byte *)getAddr( argv[1] );
  obuf = (byte *)getAddr( argv[2] );
  xsize = getInt( argv[3] );
  ysize = getInt( argv[4] );
  c_xstep = getInt( argv[5] );
  c_ystep = getInt( argv[6] );
  e_xstep = getInt( argv[7] );
  e_ystep = getInt( argv[8] );
  num_iter = getInt( argv[9] );

  /* Now start the relaxation  */

  if( !(num_iter & 1) )  /* make num_iter odd so output ends up in right buf */
    num_iter++;

  for( iter = 0; iter < num_iter; iter++ )
    {
      /*  Initialize the sample pointers.  We have several, since
	  there is a neighborhood of three lines in edge and chroma bufs that
	  is being accessed.  I use a pointer for each line...

	  The pointers with "line" in their name always point to the beginning
	  of a line of data.
      */

      elineptr = ebuf;
      next_elineptr = ebuf + e_ystep;

      if( iter & 1 )
	{
	  clineptr = obuf;
	  olineptr = cbuf;
	}
      else
	{
	  clineptr = cbuf;
	  olineptr = obuf;
	}
      next_clineptr = clineptr + c_ystep;

      /*  Process the first (top) line in the image separately  */
      
      cptr = clineptr;
      next_cptr = next_clineptr;
      eptr = elineptr;
      next_eptr = next_elineptr;
      optr = olineptr;
      
      /*  Process the first sample in the first line  */
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*next_eptr < EDGE_THRESHOLD) &&
	     (*next_cptr != 0) ) {
	    average += *next_cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr + c_xstep) != 0) ) {
	    average += *(cptr + c_xstep);
	    num_pts++;
	  }
	  
	  if( *cptr != 0 ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = (byte)(average / num_pts);
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;
      
      /*  Process the middle samples in the first line  */
      
      for( x = 1; x < xsize; x++ )
	{
	  cptr += c_xstep;
	  next_cptr += c_xstep;
	  eptr += e_xstep;
	  next_eptr += e_xstep;

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*next_eptr < EDGE_THRESHOLD) &&
		 (*next_cptr != 0) ) {
		average += *next_cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr + c_xstep) != 0) ) {
		average += *(cptr + c_xstep);
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr - c_xstep) != 0) ) {
		average += *(cptr - c_xstep);
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = (byte)(average / num_pts);
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;
	}
      
      /*  Process the last sample of the first line here  */
      
      cptr += c_xstep;
      next_cptr += c_xstep;
      eptr += e_xstep;
      next_eptr += e_xstep;
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*next_eptr < EDGE_THRESHOLD) &&
	     (*next_cptr != 0) ) {
	    average += *next_cptr;
	    num_pts++;
	  }
	  
	  if( *cptr != 0 ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr - c_xstep) != 0) ) {
	    average += *(cptr - c_xstep);
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = (byte)(average / num_pts);
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;

      /*  Process all the middle lines in the image  */

      for( y = 1; y < ysize; y++ )
	{
	  prev_cptr = clineptr;
	  prev_clineptr = prev_cptr;

	  cptr = next_clineptr;
	  clineptr = cptr;

	  next_cptr = next_clineptr + c_ystep;
	  next_clineptr = next_cptr;

	  prev_eptr = elineptr;
	  prev_elineptr = prev_eptr;

	  eptr = next_elineptr;
	  elineptr = eptr;

	  next_eptr = next_elineptr + e_ystep;
	  next_elineptr = next_eptr;

	  optr = olineptr + xsize;
	  olineptr = optr;

	  /*  Process the first sample in a line separately  */

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*prev_eptr < EDGE_THRESHOLD) &&
		 (*prev_cptr != 0) ) {
		average += *prev_cptr;
		num_pts++;
	      }
	      
	      if( (*next_eptr < EDGE_THRESHOLD) &&
		 (*next_cptr != 0) ) {
		average += *next_cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr + c_xstep) != 0) ) {
		average += *(cptr + c_xstep);
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = (byte)(average / num_pts);
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;

	  /*  Process the middle samples in a line  */

	  for( x = 1; x < xsize; x++ )
	    {
	      cptr += c_xstep;
	      next_cptr += c_xstep;
	      prev_cptr += c_xstep;

	      eptr += e_xstep;
	      next_eptr += e_xstep;
	      prev_eptr += e_xstep;

	      if( *eptr < EDGE_THRESHOLD )
		{
		  num_pts = 0;
		  average = 0;
		  if( (*prev_eptr < EDGE_THRESHOLD) &&
		     (*prev_cptr != 0) ) {
		    average += *prev_cptr;
		    num_pts++;
		  }
		  
		  if( (*next_eptr < EDGE_THRESHOLD) &&
		     (*next_cptr != 0) ) {
		    average += *next_cptr;
		    num_pts++;
		  }
		  
		  if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		     (*(cptr + c_xstep) != 0) ) {
		    average += *(cptr + c_xstep);
		    num_pts++;
		  }
		  
		  if( *cptr != 0 ) {
		    average += *cptr;
		    num_pts++;
		  }
		  
		  if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		     (*(cptr - c_xstep) != 0) ) {
		    average += *(cptr - c_xstep);
		    num_pts++;
		  }
		  
		  if( num_pts > 0 )
		    *optr++ = (byte)(average / num_pts);
		  else
		    *optr++ = 0;
		}
	      else
		{
		  *optr++ = *cptr;
		}
	    }

	  /*  Process the last sample in a line separately  */

	  cptr += c_xstep;
	  next_cptr += c_xstep;
	  prev_cptr += c_xstep;
	  
	  eptr += e_xstep;
	  next_eptr += e_xstep;
	  prev_eptr += e_xstep;

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*prev_eptr < EDGE_THRESHOLD) &&
		 (*prev_cptr != 0) ) {
		average += *prev_cptr;
		num_pts++;
	      }
	      
	      if( (*next_eptr < EDGE_THRESHOLD) &&
		 (*next_cptr != 0) ) {
		average += *next_cptr;
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr - c_xstep) != 0) ) {
		average += *(cptr - c_xstep);
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = (byte)(average / num_pts);
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;
	}

      /*  Process the last (bottom) line of the image separately  */

      prev_cptr = clineptr;
      cptr = next_clineptr;
      prev_eptr = elineptr;
      eptr = next_elineptr;
      optr = olineptr + xsize;
      
      /*  Process the first sample of the last line here  */
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*prev_eptr < EDGE_THRESHOLD) &&
	     (*prev_cptr != 0) ) {
	    average += *prev_cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr + c_xstep) != 0) ) {
	    average += *(cptr + c_xstep);
	    num_pts++;
	  }
	  
	  if( *cptr != 0 ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = (byte)(average / num_pts);
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;
      
      /*  Process the middle samples of the last line  */
      
      for( x = 1; x < xsize; x++ )
	{
	  cptr += c_xstep;
	  prev_cptr += c_xstep;

	  eptr += e_xstep;
	  prev_eptr += e_xstep;

	  if( *eptr < EDGE_THRESHOLD )
	    {
	      num_pts = 0;
	      average = 0;
	      if( (*prev_eptr < EDGE_THRESHOLD) &&
		 (*prev_cptr != 0) ) {
		average += *prev_cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr + e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr + c_xstep) != 0) ) {
		average += *(cptr + c_xstep);
		num_pts++;
	      }
	      
	      if( *cptr != 0 ) {
		average += *cptr;
		num_pts++;
	      }
	      
	      if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
		 (*(cptr - c_xstep) != 0) ) {
		average += *(cptr - c_xstep);
		num_pts++;
	      }
	      
	      if( num_pts > 0 )
		*optr++ = (byte)(average / num_pts);
	      else
		*optr++ = 0;
	    }
	  else
	    *optr++ = *cptr;
	}
      
      /*  Process the last sample of the last line here  */
      
      cptr += c_xstep;
      prev_cptr += c_xstep;
      
      eptr += e_xstep;
      prev_eptr += e_xstep;
      
      if( *eptr < EDGE_THRESHOLD )
	{
	  num_pts = 0;
	  average = 0;
	  if( (*prev_eptr < EDGE_THRESHOLD) &&
	     (*prev_cptr != 0) ) {
	    average += *prev_cptr;
	    num_pts++;
	  }
	  
	  if( (*eptr < EDGE_THRESHOLD) &&
	     (*cptr != 0) ) {
	    average += *cptr;
	    num_pts++;
	  }
	  
	  if( (*(eptr - e_xstep) < EDGE_THRESHOLD) &&
	     (*(cptr - c_xstep) != 0) ) {
	    average += *(cptr - c_xstep);
	    num_pts++;
	  }
	  
	  if( num_pts > 0 )
	    *optr++ = (byte)(average / num_pts);
	  else
	    *optr++ = 0;
	}
      else
	*optr++ = *cptr;
    }

  return NullValue;
}


