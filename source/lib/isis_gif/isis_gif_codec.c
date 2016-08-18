/* Giflib has the underlying functionality to read GIF files.
   The LZ-reading engine and parts of the rest were derived 
   from the same source that the JPEG engine came.
   The reference used is the standard Gif89a.doc file that
   Compuserve released, that is all over the web. But it's not
   incredibly complete -- here is an example from the file:

       "Animation - The Graphics Interchange Format is not 
        intended as a platform for animation, even though 
	it can be done in a limited way."
*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "isis_gif_private.h"

#define	MAX_LZW_BITS	12	/* maximum LZW code size */
#define LZW_TABLE_SIZE	(1 << MAX_LZW_BITS) /* # of possible LZW symbols */

#define TRUE 1
#define FALSE 0

/* -------------------------------------------------------------------- */
int ReadByte (finfo_struct *pfinfo) {
  return((int) *pfinfo->filebuf++);
}

/* -------------------------------------------------------------------- */
int GetDataBlock (finfo_struct *pfinfo, unsigned char *buf)
{
  /* Read a GIF data block, which has a leading count byte */
  /* A zero-length block marks the end of a data block sequence */

  int count;
  
  count = (int) *pfinfo->filebuf++;
  if (count > 0) {
    memcpy(buf, pfinfo->filebuf, count);
    pfinfo->filebuf += count;
  }
  return count;
}

/* -------------------------------------------------------------------- */
void SkipDataBlocks(finfo_struct *pfinfo)
{
  /* Skip a series of data blocks, until a block terminator is found */
  
  int count;
  
  count = (int) *pfinfo->filebuf++;
  while(count > 0) {
    pfinfo->filebuf += count;
    count = (int) *pfinfo->filebuf++;
  }
}

/* -------------------------------------------------------------------- */
void ReInitLZW(finfo_struct *pfinfo)
{
  /* (Re)initialize LZW state */

  pfinfo->code_size = pfinfo->input_code_size + 1;
  pfinfo->limit_code = pfinfo->clear_code << 1;	/* 2^code_size */
  pfinfo->max_code = pfinfo->clear_code + 2;	/* first unused code value */
  pfinfo->sp = pfinfo->symbol_stack;		/* init stack to empty */
}

/* -------------------------------------------------------------------- */
void InitLZWCode (finfo_struct *pfinfo)
{
  /* Initialize for a series of LZWReadByte (and hence GetCode) calls */
  
  /* GetCode initialization */
  pfinfo->last_byte = 2;      /* make safe to "recopy last two bytes" */
  pfinfo->last_bit = 0;	      /* nothing in the buffer */
  pfinfo->cur_bit = 0;	      /* force buffer load on first call */
  pfinfo->out_of_blocks = 0;

  /* LZWReadByte initialization: */
  /* compute special code values (note that these do not change later) */
  pfinfo->clear_code = 1 << pfinfo->input_code_size;
  pfinfo->end_code = pfinfo->clear_code + 1;
  pfinfo->first_time = 1;
  ReInitLZW(pfinfo);
}

/* -------------------------------------------------------------------- */
int GetCode (finfo_struct *pfinfo)
{
  /* Fetch the next code_size bits from the GIF data */
  /* We assume code_size is less than 16 */

  int accum;
  int offs, ret, count;

  while ( (pfinfo->cur_bit + pfinfo->code_size) > pfinfo->last_bit) {
    /* Time to reload the buffer */
    if (pfinfo->out_of_blocks) {
      fprintf(stderr, "GIF Error: No more GIF data\n");
      return pfinfo->end_code;	/* fake something useful */
    }
    /* preserve last two bytes of what we have -- assume code_size <= 16 */
    pfinfo->code_buf[0] = pfinfo->code_buf[pfinfo->last_byte - 2];
    pfinfo->code_buf[1] = pfinfo->code_buf[pfinfo->last_byte - 1];
    /* Load more bytes; set flag if we reach the terminator block */
    if ((count = GetDataBlock(pfinfo, &pfinfo->code_buf[2])) == 0) {
      pfinfo->out_of_blocks = TRUE;
      fprintf(stderr, "GIF Error: No more GIF data\n");
      return pfinfo->end_code;	/* fake something useful */
    }
    /* Reset counters */
    pfinfo->cur_bit = (pfinfo->cur_bit - pfinfo->last_bit) + 16;
    pfinfo->last_byte = 2 + count;
    pfinfo->last_bit = pfinfo->last_byte * 8;
  }

  /* Form up next 24 bits in accum */
  offs = pfinfo->cur_bit >> 3;	/* byte containing cur_bit */
  accum = pfinfo->code_buf[offs + 2];
  accum <<= 8;
  accum |= pfinfo->code_buf[offs + 1];
  accum <<= 8;
  accum |= pfinfo->code_buf[offs];

  /* Right-align cur_bit in accum, then mask off desired number of bits */
  accum >>= (pfinfo->cur_bit & 7);
  ret = ((int) accum) & ((1 << pfinfo->code_size) - 1);
  
  pfinfo->cur_bit += pfinfo->code_size;
  return ret;
}

/* -------------------------------------------------------------------- */
int LZWReadByte (finfo_struct *pfinfo)
{
  /* Read an LZW-compressed byte */

  register int code;		/* current working code */
  int incode;			/* saves actual input code */

  /* First time, just eat the expected Clear code(s) and return next code, */
  /* which is expected to be a raw byte. */
  if (pfinfo->first_time) {
    pfinfo->first_time = 0;
    code = pfinfo->clear_code;	/* enables sharing code with Clear case */
  } else {
    
    /* If any codes are stacked from a previously read symbol, return them */
    if (pfinfo->sp > pfinfo->symbol_stack)
      return (int) *(-- pfinfo->sp);

    /* Time to read a new symbol */
    code = GetCode(pfinfo);

  }

  if (code == pfinfo->clear_code) {
    /* Reinit state, swallow any extra Clear codes, and */
    /* return next code, which is expected to be a raw byte. */
    ReInitLZW(pfinfo);
    do {
      code = GetCode(pfinfo);
    } while (code == pfinfo->clear_code);
    if (code > pfinfo->clear_code) { /* make sure it is a raw byte */
      fprintf(stderr, "GIF Error: Bad data\n");
      code = 0;			/* use something valid */
      return -1;
    }
    /* make firstcode, oldcode valid! */
    pfinfo->firstcode = pfinfo->oldcode = code;
    return code;
  }

  if (code == pfinfo->end_code) {
    /* Skip the rest of the image, unless GetCode already read terminator */
    if (! pfinfo->out_of_blocks) {
      SkipDataBlocks(pfinfo);
      pfinfo->out_of_blocks = TRUE;
    }
    /* Complain that there's not enough data */
    fprintf(stderr, "GIF Error: Not enough data\n");
    /* Pad data with 0's */
    return -1;			/* fake something usable */
  }

  /* Got normal raw byte or LZW symbol */
  incode = code;		/* save for a moment */
  
  if (code >= pfinfo->max_code) { /* special case for not-yet-defined symbol */
    /* code == max_code is OK; anything bigger is bad data */
    if (code > pfinfo->max_code) {
      fprintf(stderr, "GIF Error: LZ Code out of range\n");
      incode = 0;	    /* prevent creation of loops in symbol table */
      return -1;
    }
    /* this symbol will be defined as oldcode/firstcode */
    *(pfinfo->sp++) = (unsigned char) pfinfo->firstcode;
    code = pfinfo->oldcode;
  }

  /* If it's a symbol, expand it into the stack */
  while (code >= pfinfo->clear_code) {
    *(pfinfo->sp++) = pfinfo->symbol_tail[code]; /* tail is a byte value */
    code = pfinfo->symbol_head[code]; /* head is another LZW symbol */
  }
  /* At this point code just represents a raw byte */
  pfinfo->firstcode = code;	/* save for possible future use */

  /* If there's room in table, */
  if ((code = pfinfo->max_code) < LZW_TABLE_SIZE) {
    /* Define a new symbol = prev sym + head of this sym's expansion */
    pfinfo->symbol_head[code] = pfinfo->oldcode;
    pfinfo->symbol_tail[code] = (unsigned char) pfinfo->firstcode;
    pfinfo->max_code++;
    /* Is it time to increase code_size? */
    if ((pfinfo->max_code >= pfinfo->limit_code) &&
	(pfinfo->code_size < MAX_LZW_BITS)) {
      pfinfo->code_size++;
      pfinfo->limit_code <<= 1;	/* keep equal to 2^code_size */
    }
  }
  
  pfinfo->oldcode = incode;	/* save last input symbol for future use */
  return pfinfo->firstcode;	/* return first byte of symbol's expansion */
}


/* -------------------------------------------------------------------- */
int gif_read_header (finfo_struct *pfinfo)
{
  /* Read the file header */

  char hdrbuf[3];
  unsigned char code;
  int colormaplen;

  /* determine if it is a GIF file */

  hdrbuf[0] = (char) ReadByte(pfinfo);
  hdrbuf[1] = (char) ReadByte(pfinfo);
  hdrbuf[2] = (char) ReadByte(pfinfo);

  if(strncmp(hdrbuf, "GIF", 3) != 0) {
    fprintf(stderr, "GIF Error: File is not a GIF!\n");
    return 0;
  }
  
  hdrbuf[0] = (char) ReadByte(pfinfo);
  hdrbuf[1] = (char) ReadByte(pfinfo);
  hdrbuf[2] = (char) ReadByte(pfinfo);

  if(strncmp(hdrbuf, "87a", 3) != 0 && 
     strncmp(hdrbuf, "89a", 3) != 0)
    fprintf(stderr, "GIF Warning: Unsupported GIF version\n");
  
  /* Read and decipher Logical Screen Descriptor */

  pfinfo->width = ReadByte(pfinfo);
  pfinfo->width += (256 * ReadByte(pfinfo));
  pfinfo->height = ReadByte(pfinfo);
  pfinfo->height += (256 * ReadByte(pfinfo));
  
  code = ReadByte(pfinfo);
  colormaplen = 2 << (code & 0x07);
  
  pfinfo->back_color = ReadByte(pfinfo);  /* background color */
  pfinfo->aspect_ratio = ReadByte(pfinfo);  /* ignore aspect ratio */

  /* Read global colormap if header indicates it is present */
  if(code & 0x80) {
    pfinfo->colormap = (unsigned char *) malloc(colormaplen * 3);
    memcpy(pfinfo->colormap, pfinfo->filebuf, colormaplen * 3);
    pfinfo->filebuf += colormaplen * 3;
  }
  else pfinfo->colormap = NULL;
  
  return 1;
}


/* -------------------------------------------------------------------- */
int gif_read_image (finfo_struct *pfinfo, unsigned char *r, 
		     unsigned char *g, unsigned char *b, unsigned char *a,
		     int skip, int stride)
{
  /* This function actually loads the current image into buffers
     (and goes to the next image) */
  
  int width, height, startx, starty;
  int c, col, row, newrow, colormaplen, interlaced, pass;
  int trans_flag, trans_color;
  unsigned char code;
  unsigned char *colormap;
  unsigned char *rr, *gg, *bb, *aa;
  unsigned char buf[256];
  
  trans_flag = 0;
  trans_color = 0;
  colormap = pfinfo->colormap;
  
  for (;;) {    
    code = ReadByte(pfinfo);
    
    if (code == 0x3b) return 0;   /* GIF terminator */

    if (code == 0x21) {		  /* Extension */
      code = ReadByte(pfinfo);    /* Read extension label byte */

      if (code == 0xf9) {         /* Graphic control extension */
	GetDataBlock(pfinfo, buf);
	trans_flag = buf[0] & 0x1;
	trans_color = buf[3];
	SkipDataBlocks(pfinfo);
      } 
      else SkipDataBlocks(pfinfo);
      continue; 
    }
    
    if (code == 0x2c) break;    /* beginning of image */

    fprintf(stderr, "GIF Warning: Expected start of image (0x2c)\n");
  }
    
  /* Read and decipher Local Image Descriptor */

  startx = ReadByte(pfinfo);
  startx += (256 * ReadByte(pfinfo));
  starty = ReadByte(pfinfo);
  starty += (256 * ReadByte(pfinfo));
  width = ReadByte(pfinfo);
  width += (256 * ReadByte(pfinfo));
  height = ReadByte(pfinfo);
  height += (256 * ReadByte(pfinfo));
  
  code = ReadByte(pfinfo);
  interlaced = code & 0x40;
  
  /* Read local colormap if header indicates it is present */
  if (code & 0x80) {
    colormaplen = 2 << (code & 0x07);
    colormap = (unsigned char *) malloc(colormaplen * 3);
    memcpy(colormap, pfinfo->filebuf, colormaplen * 3);
    pfinfo->filebuf += colormaplen * 3;
  }

  pfinfo->input_code_size = ReadByte(pfinfo); /* get min-code-size byte */
  if (pfinfo->input_code_size < 2 || 
      pfinfo->input_code_size >= MAX_LZW_BITS) { 
    fprintf(stderr, "GIF Error: LZW code size wrong\n");
    return 0;
  }

  /* Prepare to read selected image: first initialize LZW decompressor */
  pfinfo->symbol_head = (unsigned short *)
    malloc(LZW_TABLE_SIZE * sizeof(unsigned short));
  pfinfo->symbol_tail = (unsigned char *)
    malloc(LZW_TABLE_SIZE * sizeof(unsigned char));
  pfinfo->symbol_stack = (unsigned char *)
    malloc(LZW_TABLE_SIZE * sizeof(unsigned char));
  InitLZWCode(pfinfo);

  /* print each row */
  pass = 0;
  for (row = 0; row < height; row++) {
    
    if(interlaced) {
      switch(pass) {
	
      case 0:
	pass = 1;
	newrow = 0;
	break;

      case 1:
	newrow += 8;
	if(newrow >= height) {
	  newrow = 4;
	  pass = 2;
	}
	break;

      case 2:
	newrow += 8;
	if(newrow >= height) {
	  newrow = 2;
	  pass = 3;
	}
	break;

      case 3:
	newrow += 4;
	if(newrow >= height) {
	  newrow = 1;
	  pass = 4;
	}
	break;

      case 4:
	newrow += 2;
	break;
      }
    }
    else newrow = row;  /* non interlaced */

    /* calculate the starting position for the row */
    rr = r + ((newrow + starty) * stride) + (startx * skip);
    gg = g + ((newrow + starty) * stride) + (startx * skip);
    bb = b + ((newrow + starty) * stride) + (startx * skip);
    if(a) aa = a + ((newrow + starty) * stride) + (startx * skip);
    
    for (col = 0; col < width; col++) {

      c = LZWReadByte(pfinfo);
      if (c == -1) {   
	if (! pfinfo->out_of_blocks) SkipDataBlocks(pfinfo);
	fprintf(stderr, "GIF Error: Early end to image data at [%d %d].\n",
		row, col);
	return 0;
      }
      *rr = colormap[c * 3];
      *gg = colormap[c * 3 + 1];
      *bb = colormap[c * 3 + 2];
      if (a) {
	if (trans_flag && c == trans_color) *aa = 0;
	else *aa = 255;
      }

      rr += skip; 
      gg += skip; 
      bb += skip;
      if(a) aa += skip;
    }
  }

  free(pfinfo->symbol_head);
  free(pfinfo->symbol_tail);
  free(pfinfo->symbol_stack);
  if(colormap != pfinfo->colormap) free(colormap);  /* free local colormap */
  if (! pfinfo->out_of_blocks) SkipDataBlocks(pfinfo);
  return (trans_flag ? 4 : 3);
}



/* -------------------------------------------------------------------- */
int gif_destroy (finfo_struct *pfinfo) 
{
  /* deallocate all the memory. Highly recommended. */

  if (pfinfo->colormap != NULL) {
    free(pfinfo->colormap);
    pfinfo->colormap=NULL;
  }

  return 1;
}







