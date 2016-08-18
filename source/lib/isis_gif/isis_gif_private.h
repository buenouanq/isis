/* isis_gif_private.h */

typedef struct {
  unsigned char * filebuf;
  unsigned char * colormap;

  int width;
  int height;
  unsigned char back_color;
  unsigned char aspect_ratio;

  /* State for GetCode and LZWReadByte */
  unsigned char code_buf[260];  /* current input data block */
  int last_byte;		/* # of bytes in code_buf */
  int last_bit;			/* # of bits in code_buf */
  int cur_bit;			/* next bit index to read */
  int out_of_blocks;		/* TRUE if hit terminator data block */

  int input_code_size;		/* codesize given in GIF file */
  int clear_code,end_code;	/* values for Clear and End codes */

  int code_size;		/* current actual code size */
  int limit_code;		/* 2^code_size */
  int max_code;			/* first unused code value */
  int first_time;		/* flags first call to LZWReadByte */

  /* Private state for LZWReadByte */
  int oldcode;			/* previous LZW symbol */
  int firstcode;		/* first byte of oldcode's expansion */

  /* LZW symbol table and expansion stack */
  unsigned short *symbol_head;	/* => table of prefix symbols */
  unsigned char *symbol_tail;	/* => table of suffix bytes */
  unsigned char *symbol_stack;	/* => stack for symbol expansions */
  unsigned char *sp;		/* stack pointer */
} finfo_struct;

int gif_read_header (finfo_struct *pfinfo);

int gif_read_image (finfo_struct *pfinfo, unsigned char *r, 
		    unsigned char *g, unsigned char *b, unsigned char *a,
		    int skip, int stride);

int gif_destroy (finfo_struct *pfinfo);

