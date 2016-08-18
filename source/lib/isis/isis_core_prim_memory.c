/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "isis.h"
#include "isis_core_prim.h"

/*-------------------------------------------------------------*/
Value *prim_alloc(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  void *ptr;

  checkCount(1, "number of bytes");
  checkInt(0, "number of bytes");

  ptr = malloc(getInt(argv[0]));

  if(ptr == NULL) return NullValue;
  else return newAddr(ptr);
}

/*-------------------------------------------------------------*/
Value *prim_free(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  int i;

  for(i = 0; i < argc; i++) {
    if(!isAddr(argv[i])) 
      fprintf(stderr, "** %s: Arg %d was not an address.\n", proc_name, i + 1);
    else free( getAddr(argv[i]) );
  }

  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_memcpy(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  checkCount(3, "number of bytes, source buffer, destination buffer");
  checkInt(0, "number of bytes");
  checkAddr(1, "source buffer");
  checkAddr(2, "destination buffer");

  memcpy(getAddr(argv[2]), getAddr(argv[1]), getInt(argv[0]));

  return referValue(argv[2]);
}

/*-------------------------------------------------------------*/
Value *prim_memmove(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  checkCount(3, "number of bytes, source buffer, destination buffer");
  checkInt(0, "number of bytes");
  checkAddr(1, "source buffer");
  checkAddr(2, "destination buffer");

  memmove(getAddr(argv[2]), getAddr(argv[1]), getInt(argv[0]));

  return referValue(argv[2]);
}

/*-------------------------------------------------------------*/
Value *prim_memcmp(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  checkCount(3, "number of bytes, source 1 buffer, source 2 buffer");
  checkInt(0, "number of bytes");
  checkAddr(1, "source 1 buffer");
  checkAddr(2, "source 2 buffer");

  return newBool(memcmp( getAddr(argv[2]), 
			 getAddr(argv[1]), 
			 getInt(argv[0]) ) == 0);
}

/*-------------------------------------------------------------*/
Value *prim_memchr(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  void *ptr;

  checkCount(3, "number of bytes, search value (byte), search buffer");
  checkInt(0, "number of bytes");
  checkInt(1, "search value (byte)");
  checkAddr(2, "search buffer");
  
  ptr = memchr(getAddr(argv[2]), getInt(argv[1]), getInt(argv[0]));
  
  if(ptr) return newAddr(ptr);
  
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_memset(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  checkCount(3, "number of bytes, fill value (byte), buffer address");
  checkInt(0, "number of bytes");
  checkInt(1, "fill value (byte)");
  checkAddr(2, NULL);

  memset(getAddr(argv[2]), getInt(argv[1]), getInt(argv[0]));
  
  return referValue(argv[2]);
}

/*-------------------------------------------------------------*/
Value *prim_revbytes(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  int wordsize, numwords, i;
  unsigned char *inbuf, *outbuf;

  checkCount(4, "word size, number of words, inbuf, outbuf");
  checkInt(0, "word size");
  checkInt(1, "number of words");
  checkAddr(2, "inbuf");
  checkAddr(3, "outbuf");

  wordsize = getInt(argv[0]);
  numwords = getInt(argv[1]);
  inbuf = getAddr(argv[2]);
  outbuf = getAddr(argv[3]);

  switch(wordsize) {
    
  case 2: 
    for(i = 0; i < numwords; i++, inbuf += 2, outbuf += 2) {
      outbuf[0] = inbuf[1];
      outbuf[1] = inbuf[0];
    }
    break;
    
  case 4: 
    for(i = 0; i < numwords; i++, inbuf += 4, outbuf += 4) {
      outbuf[0] = inbuf[3];
      outbuf[1] = inbuf[2];
      outbuf[2] = inbuf[1];
      outbuf[3] = inbuf[0];
    }
    break;
    
  case 8: 
    for(i = 0; i < numwords; i++, inbuf += 8, outbuf += 8) {
      outbuf[0] = inbuf[7];
      outbuf[1] = inbuf[6];
      outbuf[2] = inbuf[5];
      outbuf[3] = inbuf[4];
      outbuf[4] = inbuf[3];
      outbuf[5] = inbuf[2];
      outbuf[6] = inbuf[1];
      outbuf[7] = inbuf[0];
    }
    break;

  default:
    fprintf(stderr, "** %s: Word size %d is invalid.\n", 
	    proc_name, wordsize);
    return NullValue;
  }

  return referValue(argv[3]);
}

