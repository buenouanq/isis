/**********************************************************************
  isis_gif:  routines for reading GIF image files
  ********************************************************************* */

#include <stdio.h>
#include <stdlib.h>
#include <isis.h>

#include "isis_gif.h"
#include "isis_gif_private.h"  /* Local functions */

Value *prim_gif_create(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv);

Value *prim_gif_read_header(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv);

Value *prim_gif_read_image(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);

Value *prim_gif_destroy(Script *script, 
			char *proc_name, int call_data, 
			int argc, Value **argv);

/* ------------------------------------------------------------ */
void bind_gif_library(Script *script)
{
  bindBool(script, "isis_gif", 1);

  bindCfunc(script, "gif-create", prim_gif_create, 0);
  bindCfunc(script, "gif-read-header", prim_gif_read_header, 0);
  bindCfunc(script, "gif-read-image", prim_gif_read_image, 0);
  bindCfunc(script, "gif-destroy", prim_gif_destroy, 0);
}

/* ------------------------------------------------------------ */
Value *prim_gif_create(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  finfo_struct *pfinfo;

  checkCount(1, "gif buffer address");
  checkAddr(0, "gif buffer");

  pfinfo = (finfo_struct *) malloc(sizeof(finfo_struct));

  pfinfo->filebuf = getAddr(argv[0]);
  
  return newAddr(pfinfo);
}

/* ------------------------------------------------------------ */
Value *prim_gif_read_header(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  finfo_struct *pfinfo;

  checkCount(1, "gif handle");
  checkAddr(0, "gif handle");

  pfinfo = getAddr(argv[0]);

  if (gif_read_header (pfinfo))
    return newIntListva(2, pfinfo->width, pfinfo->height);
  
  else return NullValue;
}

/* ------------------------------------------------------------ */
Value *prim_gif_read_image(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  finfo_struct *pfinfo;
  int chans, res;
  int xsize, ysize;
  int xstep, ystep;
  unsigned char *r, *g, *b, *a;
  
  checkCount(4, "gif handle, size, bufaddrs, steps");
  checkAddr(0, "gif handle");
  checkIntList(1, "size");
  checkAddrList(2, "bufaddrs");
  checkIntList(3, "steps");
  
  pfinfo = getAddr(argv[0]);
  xsize = getInt(Listitem(argv[1], 0));
  ysize = getInt(Listitem(argv[1], 1));
  chans = Listsize(argv[2]);
  xstep = getInt(Listitem(argv[3], 0));
  ystep = getInt(Listitem(argv[3], 1));

  if (chans < 3 || chans > 4) {
    fprintf(stderr, "*** %s: expects three or four channel image.\n", 
	    proc_name);
    return NullValue;
  }

  if (xsize != pfinfo->width || ysize != pfinfo->height) {
    fprintf(stderr, "*** %s: image dimensions were not [ %d %d ]\n",
	    proc_name, pfinfo->width, pfinfo->height);
    return NullValue;
  }
  
  r = getAddr(Listitem(argv[2], 0));
  g = getAddr(Listitem(argv[2], 1));
  b = getAddr(Listitem(argv[2], 2));
  if (chans == 4) a = getAddr(Listitem(argv[2], 3));
  else a = NULL;

  if ((res = gif_read_image(pfinfo, r, g, b, a, xstep, ystep)) == 0)
    return NullValue;
  
  return newInt(res);
}

/* ------------------------------------------------------------ */
Value *prim_gif_destroy(Script *script, 
			char *proc_name, int call_data, 
			int argc, Value **argv)
{
  finfo_struct *pfinfo;

  checkCount(1, "gif handle");
  checkAddr(0, "gif handle");

  pfinfo = getAddr(argv[0]);
  gif_destroy(pfinfo);
  free(pfinfo);

  return NullValue;
}



