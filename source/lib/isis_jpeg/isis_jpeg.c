/* *********************************************************************
   Template for writing C functions for Isis
   by Stefan agamanolis, June 1996
   
   This file holds the definitions of all the C functions you want to
   use inside of Isis (plus any other helper functions).  It also
   holds a special function (called bind_my_functions() below) whose
   job is to bind all of these C functions into the Isis interpreter
   when it is called.
   
   You must declare all of your C functions at the top of the file
   since are referred to inside of bind_my_functions().
   
   The header file "isis_cfunc_template.h" contains a single
   declaration of bind_my_functions().  Your main interpreter source
   file will include this file and call this function to bind your
   functions into Isis.  Be sure to edit both file appropriately if
   you change the names of functions.

   Added prim_jpeg_set_compress_quality(), 12/11/96  wad
   
   ********************************************************************* */

#include <stdio.h>                /* Must include before isis.h */
#include <stdlib.h>               /* Must include before isis.h */
#include <isis.h>                 /* Holds all Isis interpreter info */
#include <jpeglib.h>
#include <setjmp.h>

#include "isis_jpeg.h"  /* Holds a declaration bind_my_functions() */

enum { IO_FILE, IO_BUFFER };

/* Jpeg Error manager structures */

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;


/*-------------------------------------------------------------*/
/* Declare all C functions you intend to use inside Isis here */

Value *prim_jpeg_create_compress(Script *script, 
				 char *proc_name, int call_data, 
				 int argc, Value **argv);
Value *prim_jpeg_set_compress_destination(Script *script, 
					  char *proc_name, int call_data, 
					  int argc, Value **argv);
Value *prim_jpeg_set_compress_quality(Script *script, 
				      char *proc_name, int call_data, 
				      int argc, Value **argv);
Value *prim_jpeg_compress(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_jpeg_destroy_compress(Script *script, 
				  char *proc_name, int call_data, 
				  int argc, Value **argv);
Value *prim_jpeg_create_decompress(Script *script, 
				   char *proc_name, int call_data, 
				   int argc, Value **argv);
Value *prim_jpeg_set_decompress_source(Script *script, 
				       char *proc_name, int call_data, 
				       int argc, Value **argv);
Value *prim_jpeg_read_header(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_jpeg_decompress(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *prim_jpeg_destroy_decompress(Script *script, 
				    char *proc_name, int call_data, 
				    int argc, Value **argv);

/* ------------------------------------------------------------ */
/* This function is called to bind all of the functions and other
   values defined in this source file */

void bind_jpeg_library(Script *script)
{
  bindBool(script, "isis_jpeg", 1);

  bindInt(script, "jpeg-colorspace-rgb", JCS_RGB);
  bindInt(script, "jpeg-colorspace-grayscale", JCS_GRAYSCALE);

  bindCfunc(script, "jpeg-create-compress", 
	    prim_jpeg_create_compress, 0);

  bindCfunc(script, "jpeg-set-compress-destination-file", 
	    prim_jpeg_set_compress_destination, IO_FILE);
  bindCfunc(script, "jpeg-set-compress-destination-buffer", 
	    prim_jpeg_set_compress_destination, IO_BUFFER);

  bindCfunc(script, "jpeg-set-compress-quality", 
	    prim_jpeg_set_compress_quality, 1);
  bindCfunc(script, "jpeg-set-compress-quality-best", 
	    prim_jpeg_set_compress_quality, 0);

  bindCfunc(script, "jpeg-compress", 
	    prim_jpeg_compress, 0);
  
  bindCfunc(script, "jpeg-destroy-compress", 
	    prim_jpeg_destroy_compress, 0);

  bindCfunc(script, "jpeg-create-decompress", 
	    prim_jpeg_create_decompress, 0);

  bindCfunc(script, "jpeg-set-decompress-source-file", 
	    prim_jpeg_set_decompress_source, IO_FILE);
  bindCfunc(script, "jpeg-set-decompress-source-buffer", 
	    prim_jpeg_set_decompress_source, IO_BUFFER);

  bindCfunc(script, "jpeg-read-header", 
	    prim_jpeg_read_header, 0);

  bindCfunc(script, "jpeg-decompress", 
	    prim_jpeg_decompress, 0);

  bindCfunc(script, "jpeg-destroy-decompress", 
	    prim_jpeg_destroy_decompress, 0);

}

/* ------------------------------------------------------------ */
/* Define your C functions below.  All C functions that will be bound
   in Isis must follow this prototype. */


/* ------------------------------------------------------------ */
METHODDEF(void) my_init_source(j_decompress_ptr cinfo)
{
  return;
}

/* ------------------------------------------------------------ */
METHODDEF(boolean) my_fill_input_buffer(j_decompress_ptr cinfo)
{
  return FALSE;
}

/* ------------------------------------------------------------ */
METHODDEF(void) my_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
  cinfo->src->next_input_byte += num_bytes;
  cinfo->src->bytes_in_buffer -= num_bytes;
  return;
}

/* ------------------------------------------------------------ */
METHODDEF(void) my_term_source(j_decompress_ptr cinfo)
{
  return;
}


/* ------------------------------------------------------------ */
METHODDEF(void) my_init_destination(j_compress_ptr cinfo)
{
  return;
}

/* ------------------------------------------------------------ */
METHODDEF(boolean) my_empty_output_buffer(j_compress_ptr cinfo)
{
  return FALSE;
}

/* ------------------------------------------------------------ */
METHODDEF(void) my_term_destination(j_compress_ptr cinfo)
{
  return;
}

/* static data manager structures are used because of freeing problems */
/* so only one compress or decompress operation should occur at a time */
static struct jpeg_source_mgr my_source_mgr;
static struct jpeg_destination_mgr my_destination_mgr;

/* ------------------------------------------------------------ */
void jpeg_buffer_src(j_decompress_ptr cinfo, void *buf, size_t size)
{
  cinfo->src = &my_source_mgr;
  
  cinfo->src->next_input_byte = (JOCTET *) buf;
  cinfo->src->bytes_in_buffer = size;
  
  cinfo->src->init_source = my_init_source;
  cinfo->src->fill_input_buffer = my_fill_input_buffer;
  cinfo->src->skip_input_data = my_skip_input_data;
  cinfo->src->resync_to_restart = jpeg_resync_to_restart;
  cinfo->src->term_source = my_term_source;
}

/* ------------------------------------------------------------ */
void jpeg_buffer_dest(j_compress_ptr cinfo, void *buf, size_t size)
{
  cinfo->dest = &my_destination_mgr;
  
  cinfo->dest->next_output_byte = (JOCTET *) buf;
  cinfo->dest->free_in_buffer = size;
  
  cinfo->dest->init_destination = my_init_destination;
  cinfo->dest->empty_output_buffer = my_empty_output_buffer;
  cinfo->dest->term_destination = my_term_destination;
}

/* ------------------------------------------------------------ */
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* Here's the routine that will replace the standard JPEG error_exit
     method: (This code is straight from JPEG library example.c) */

  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  
  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/* ------------------------------------------------------------ */
METHODDEF(void)
my_emit_message (j_common_ptr cinfo, int msg_level)
{
  struct jpeg_error_mgr * err = cinfo->err;
  
  if (msg_level < 0) {
    /* It's a warning message.  Since corrupt files may generate many warnings,
     * the policy implemented here is to show only the first warning,
     * unless trace_level >= 3.
     */
    if (err->trace_level >= 3) (*err->output_message) (cinfo);
    /* Always count warnings in num_warnings. */
    err->num_warnings++;
  } else {
    /* It's a trace message.  Show it if trace_level >= msg_level. */
    if (err->trace_level >= msg_level)
      (*err->output_message) (cinfo);
  }
}

/* ------------------------------------------------------------ */
Value *prim_jpeg_create_compress(Script *script, 
				 char *proc_name, int call_data, 
				 int argc, Value **argv)
{
  int numchans;
  int dims[2];
  int colorspace;
  struct jpeg_compress_struct *cinfo;
  struct my_error_mgr *jerr;

  if(argc > 0 && isInt(argv[0])) 
    colorspace = getInt(argv[0]);
  else colorspace = JCS_RGB;
  
  if(argc > 1 && isInt(argv[1])) 
    numchans = getInt(argv[1]);
  else numchans = 3;
  
  if(argc > 2 && isIntList(argv[2])) 
    getIntList(argv[2], dims, 2);
  else { dims[0] = 0;  dims[1] = 0; }
  
  cinfo = malloc(sizeof(struct jpeg_compress_struct));
  jerr = malloc(sizeof(struct my_error_mgr));
  
  cinfo->err = jpeg_std_error(&jerr->pub);
  jerr->pub.error_exit = my_error_exit;
  jerr->pub.emit_message = my_emit_message;
  
  jpeg_create_compress(cinfo);
  
  cinfo->image_width = dims[0];
  cinfo->image_height = dims[1];
  cinfo->input_components = numchans;
  cinfo->in_color_space = (J_COLOR_SPACE) colorspace;
  
  jpeg_set_defaults(cinfo);
  
  return newAddr(cinfo);
}


/* ------------------------------------------------------------ */
Value *prim_jpeg_set_compress_destination(Script *script, 
					  char *proc_name, int call_data, 
					  int argc, Value **argv)
{
  switch(call_data) {
    
  case IO_FILE:
    checkCount(2, "jpeg handle, output file handle");
    checkAddr(0, "jpeg handle");
    checkAddr(1, "output file handle");
    jpeg_stdio_dest(getAddr(argv[0]), getAddr(argv[1]));
    return referValue(argv[1]);

  case IO_BUFFER:
    checkCount(3, "jpeg handle, output buffer, buffer size");
    checkAddr(0, "jpeg handle");
    checkAddr(1, "output buffer");
    checkInt(2, "buffer size");
    jpeg_buffer_dest(getAddr(argv[0]), getAddr(argv[1]), getInt(argv[2]));
    return referValue(argv[1]);
    
  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *prim_jpeg_set_compress_quality(Script *script, 
				      char *proc_name, int call_data, 
				      int argc, Value **argv)
{
  checkCount(2, "jpeg handle, quality 1-100");
  checkAddr(0, "jpeg handle");
  checkInt(1, "quality 1-100");
  
  /*  The quality should be a value from 1 (worst) to 100 (lossless)
      compression.  In addition, the call_data (if non-zero) mandates
      observance of the baseline standard, causing slightly worse images
      at high compression rates.  */
  
  jpeg_set_quality( getAddr(argv[0]), getInt(argv[1]), call_data );
  
  return referValue(argv[1]);
}

/* ------------------------------------------------------------ */
Value *prim_jpeg_compress(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  int chans, dims[2], skip, stride, interleaved, require_tables;
  int c, x, row_size;
  unsigned char *bufs[10], *buf, *cbuf;
  struct jpeg_compress_struct *cinfo;
  JSAMPROW row_pointer[1];

  checkCountLeast(4, "jpeg handle, chans, size, bufs, {steps, tables_flag}");
  checkAddr(0, "jpeg handle");
  checkInt(1, "chans");
  checkIntList(2, "frame size");

  if(!isAddr(argv[3]) && !isAddrList(argv[3])) {
    fprintf(stderr, "** %s: expects address or list of addresses for arg 4.", 
	    proc_name);
    return NullValue;
  }

  cinfo = getAddr(argv[0]);
  chans = getInt(argv[1]);
  getIntList(argv[2], dims, 2);

  if(chans > 10) {
    chans = 10;
    fprintf(stderr, "** %s: maximum of 10 channels.\n", proc_name);
  }
  
  if(isAddr(argv[3])) {
    interleaved = 1;
    buf = getAddr(argv[3]);
  }
  else {
    interleaved = 0;
    getAddrList(argv[3], (void **) bufs, 10);
  }

  if(argc > 4 && isIntList(argv[4])) {
    skip = getInt(Listitem(argv[4], 0));
    stride = getInt(Listitem(argv[4], 1));
  }
  else {
    skip = 1;
    stride = dims[0];
    if(interleaved) stride *= chans;
  }

  if(argc > 5)
    require_tables = (trueValue(argv[5]) ? TRUE : FALSE);
  else require_tables = TRUE;
  
  cinfo->image_width = dims[0];
  cinfo->image_height = dims[1];
  cinfo->input_components = chans;
  
  /* Error return point */
  if(setjmp(((my_error_ptr)(cinfo->err))->setjmp_buffer)) return NullValue;

  jpeg_start_compress(cinfo, require_tables);
  
  if(interleaved) {
    while(cinfo->next_scanline < cinfo->image_height) {
      row_pointer[0] = &buf[cinfo->next_scanline * stride];
      jpeg_write_scanlines(cinfo, row_pointer, 1);
    }
  }
  else {
    row_size = dims[0] * chans;
    row_pointer[0] = buf = malloc(row_size);
    
    while(cinfo->next_scanline < cinfo->image_height) {
      for(c = 0; c < chans; c++) {  /* interleave the data on one scanline */
	cbuf = bufs[c];
	for(x = c; x < row_size; x += chans) { buf[x] = *cbuf; cbuf += skip; }
	bufs[c] += stride;
      }
      jpeg_write_scanlines(cinfo, row_pointer, 1);
    }
    free(buf);
  }
  
  jpeg_finish_compress(cinfo);
  
  return newInt( cinfo->dest->free_in_buffer );
}


/* ------------------------------------------------------------ */
Value *prim_jpeg_destroy_compress(Script *script, 
				  char *proc_name, int call_data, 
				  int argc, Value **argv)
{
  struct jpeg_compress_struct *cinfo;

  checkCount(1, "jpeg handle");
  checkAddr(0, "jpeg handle");

  cinfo = getAddr(argv[0]);
 
  jpeg_destroy_compress(cinfo);
  
  free(cinfo->err);
  free(cinfo);

  return NullValue;
}


/* ------------------------------------------------------------ */
Value *prim_jpeg_create_decompress(Script *script, 
				   char *proc_name, int call_data, 
				   int argc, Value **argv)
{
  struct jpeg_decompress_struct *cinfo;
  struct my_error_mgr *jerr;

  cinfo = malloc(sizeof(struct jpeg_decompress_struct));
  jerr = malloc(sizeof(struct my_error_mgr));

  cinfo->err = jpeg_std_error(&jerr->pub);
  jerr->pub.error_exit = my_error_exit;
  jerr->pub.emit_message = my_emit_message;

  jpeg_create_decompress(cinfo);

  return newAddr(cinfo);
}

/* ------------------------------------------------------------ */
Value *prim_jpeg_set_decompress_source(Script *script, 
				       char *proc_name, int call_data, 
				       int argc, Value **argv)
{
  switch(call_data) {
    
  case IO_FILE:
    checkCount(2, "jpeg handle, input file handle");
    checkAddr(0, "jpeg handle");
    checkAddr(1, "input file handle");
    jpeg_stdio_src(getAddr(argv[0]), getAddr(argv[1]));
    return referValue(argv[1]);

  case IO_BUFFER:
    checkCount(3, "jpeg handle, input buffer, buffer size");
    checkAddr(0, "jpeg handle");
    checkAddr(1, "input buffer");
    checkInt(2, "buffer size");
    jpeg_buffer_src(getAddr(argv[0]), getAddr(argv[1]), getInt(argv[2]));
    return referValue(argv[1]);

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *prim_jpeg_read_header(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int require_image;
  Value *dims, *chans, *ret, *colspace;
  struct jpeg_decompress_struct *cinfo;
  
  checkCountLeast(1, "jpeg handle");
  checkAddr(0, "jpeg handle");
  
  cinfo = getAddr(argv[0]);
  
  if(argc > 1) require_image = (trueValue(argv[1]) ? TRUE : FALSE);
  else require_image = TRUE;
  
  /* Error return point */
  if(setjmp(((my_error_ptr)(cinfo->err))->setjmp_buffer)) return NullValue;

  jpeg_read_header(cinfo, require_image);
  
  dims = newIntListva(2, cinfo->image_width, cinfo->image_height);
  chans = newInt(cinfo->num_components);
  colspace = newInt(cinfo->jpeg_color_space);
  ret = newListva(3, chans, dims, colspace);
  freeValue(dims);
  freeValue(chans);
  freeValue(colspace);
  
  return ret;
}

/* ------------------------------------------------------------ */
Value *prim_jpeg_decompress(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  int chans, stride, skip, interleaved;  
  int c, x, row_size;
  int dims[2];
  unsigned char *bufs[10], *buf, *cbuf;
  struct jpeg_decompress_struct *cinfo;
  JSAMPROW row_pointer[1];

  checkCountLeast(4, "jpeg handle, chans, size, bufs, {steps}");
  checkAddr(0, "jpeg handle");
  checkInt(1, "chans");
  checkIntList(2, "frame size");
  
  if(!isAddr(argv[3]) && !isAddrList(argv[3])) {
    fprintf(stderr, "** %s: expects address or list of addresses for arg 4.", 
	    proc_name);
    return NullValue;
  }
  
  cinfo = getAddr(argv[0]);
  chans = getInt(argv[1]);
  getIntList(argv[2], dims, 2);

  if(chans > 10) {
    chans = 10;
    fprintf(stderr, "** %s: maximum of 10 channels.\n", proc_name);
    return NullValue;
  }
  
  if(isAddr(argv[3])) {
    interleaved = 1;
    buf = getAddr(argv[3]);
  }
  else {
    interleaved = 0;
    getAddrList(argv[3], (void **) bufs, 10);
  }
  
  if(argc > 4 && isIntList(argv[4])) {
    skip = getInt(Listitem(argv[4], 0));
    stride = getInt(Listitem(argv[4], 1));
  }
  else {
    skip = 1;
    stride = dims[0];
    if(interleaved) stride *= chans;
  }

  /* Error return point */
  if(setjmp(((my_error_ptr)(cinfo->err))->setjmp_buffer)) return NullValue;

  jpeg_start_decompress(cinfo);

  if(chans != cinfo->output_components ||
     dims[0] != ((int) cinfo->output_width) ||
     dims[1] != ((int) cinfo->output_height)) {
    fprintf(stderr, "** %s: dimensions of buffer do not match image.\n", 
	    proc_name);
    fprintf(stderr, "** \t Image: %d x %d x %d.   Buffer: %d x %d x %d.\n", 
	    cinfo->output_components, 
	    cinfo->output_width, 
	    cinfo->output_height,
	    chans, dims[0], dims[1]);
  }

  if(interleaved) {
    while(cinfo->output_scanline < cinfo->output_height) {
      row_pointer[0] = &buf[cinfo->output_scanline * stride];
      jpeg_read_scanlines(cinfo, row_pointer, 1);
    }
  }
  else {
    row_size = dims[0] * chans;
    row_pointer[0] = buf = malloc(row_size);
    
    while(cinfo->output_scanline < cinfo->output_height) {
      jpeg_read_scanlines(cinfo, row_pointer, 1);
      for(c = 0; c < chans; c++) {  /* de-interleave the data on one line */
	cbuf = bufs[c];
	for(x = c; x < row_size; x += chans) { *cbuf = buf[x]; cbuf+= skip; }
	bufs[c] += stride;
      }
    }
    free(buf);
  }

  jpeg_finish_decompress(cinfo);
  
  return newInt( cinfo->src->bytes_in_buffer );
}

/* ------------------------------------------------------------ */
Value *prim_jpeg_destroy_decompress(Script *script, 
				    char *proc_name, int call_data, 
				    int argc, Value **argv)
{
  struct jpeg_decompress_struct *cinfo;

  checkCount(1, "jpeg handle");
  checkAddr(0, "jpeg handle");

  cinfo = getAddr(argv[0]);
 
  jpeg_destroy_decompress(cinfo);
  
  free(cinfo->err);
  free(cinfo);

  return NullValue;
}

