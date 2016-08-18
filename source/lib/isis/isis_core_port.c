/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* Port manipulation functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isis.h"

/*----------------------------------------------------------------------*/
Value *new_port_FILE(char *name, FILE *fp, int inflag, int outflag)
{
  Value *portval;
  
  if(fp == NULL) return NullValue;

  portval = newList(PORT_LIST_LENGTH, NULL);

  Listitem(portval, PORT_NAME) = newString(strlen(name), name);
  Listitem(portval, PORT_TYPE) = referValue(port_type_FILE_val);
  Listitem(portval, PORT_INTERNAL_ID) = newAddr(fp);
  Listitem(portval, PORT_INFLAG) = newBool(inflag);
  Listitem(portval, PORT_OUTFLAG) = newBool(outflag);
  Listitem(portval, PORT_CLOSE_PROC) = referValue(prim_close_FILE_val);
  Listitem(portval, PORT_READ_PROC) = referValue(prim_read_FILE_val);
  Listitem(portval, PORT_READLINE_PROC) = referValue(prim_readline_FILE_val);
  Listitem(portval, PORT_WRITE_PROC) = referValue(prim_write_FILE_val);
  Listitem(portval, PORT_TELL_PROC) = referValue(prim_tell_FILE_val);
  Listitem(portval, PORT_SEEK_PROC) = referValue(prim_seek_FILE_val);
  Listitem(portval, PORT_READ_READY_PROC) = referValue(prim_read_ready_FILE_val);
  Listitem(portval, PORT_WRITE_READY_PROC) = referValue(prim_write_ready_FILE_val);
  Listitem(portval, PORT_CONFIGURE_PROC) = referValue(prim_configure_FILE_val);
  
  return portval;
}

/*----------------------------------------------------------------------*/
int printValue(Script *script, Value *port, Value *val)
{
  Value *newargv[3], *retval;
  char *str, *buf;
  int len;
    
  len = measureValue(val);
  buf = allocBuffer(len);
  str = sprintValue(buf, val);
  len = str - buf;

  if(len) {
    newargv[0] = Listitem(port, PORT_INTERNAL_ID);
    newargv[1] = newInt(len);
    newargv[2] = newAddr(buf);
    
    retval = applyProcedure(script, getProc(Listitem(port, PORT_WRITE_PROC)), 
			    3, newargv);
    
    freeValue(newargv[1]);
    freeValue(newargv[2]);
    
    len = getInt(retval);
    freeValue(retval);
  }
  
  free(buf);
  
  return len;
}

/*----------------------------------------------------------------------*/
int printStr(Script *script, Value *port, char *str)
{
  Value *newargv[3], *retval;
  int len;
    
  newargv[0] = Listitem(port, PORT_INTERNAL_ID);
  newargv[1] = newInt(strlen(str));
  newargv[2] = newAddr(str);
    
  retval = applyProcedure(script, getProc(Listitem(port, PORT_WRITE_PROC)), 
			  3, newargv);
    
  freeValue(newargv[1]);
  freeValue(newargv[2]);
  
  len = getInt(retval);
  freeValue(retval);
  
  return len;
}

/*----------------------------------------------------------------------*/
int printStrn(Script *script, Value *port, char *str, int len)
{
  Value *newargv[3], *retval;
  
  newargv[0] = Listitem(port, PORT_INTERNAL_ID);
  newargv[1] = newInt(len);
  newargv[2] = newAddr(str);
  
  retval = applyProcedure(script, getProc(Listitem(port, PORT_WRITE_PROC)), 
			  3, newargv);
  
  freeValue(newargv[1]);
  freeValue(newargv[2]);
  
  len = getInt(retval);
  freeValue(retval);
  
  return len;
}

/*----------------------------------------------------------------------*/
int printSpace(Script *script, Value *port, int size)
{
  while(size > 50) size -= printStrn(script, port, ISIS_50SPACE, 50);
  printStrn(script, port, ISIS_50SPACE, size);
  
  return size;
}

/*----------------------------------------------------------------------*/
int readStr(Script *script, Value *port, char *str, int len)
{
  Value *newargv[4], *retval;
  
  newargv[0] = Listitem(port, PORT_INTERNAL_ID);
  newargv[1] = newInt(len - 1);
  newargv[2] = newAddr(str);
  newargv[3] = newInt((int)'\n');
  
  retval = applyProcedure(script, getProc(Listitem(port, PORT_READLINE_PROC)), 
			  4, newargv);
  
  freeValue(newargv[1]);
  freeValue(newargv[2]);
  freeValue(newargv[3]);
  
  len = getInt(retval);
  freeValue(retval);
  
  str[len] = 0;

  return len;
}

/*----------------------------------------------------------------------*/
int readStrn(Script *script, Value *port, char *str, int len)
{
  Value *newargv[3], *retval;
  
  newargv[0] = Listitem(port, PORT_INTERNAL_ID);
  newargv[1] = newInt(len);
  newargv[2] = newAddr(str);
  
  retval = applyProcedure(script, getProc(Listitem(port, PORT_READ_PROC)), 
			  3, newargv);
  
  freeValue(newargv[1]);
  freeValue(newargv[2]);
  
  len = getInt(retval);
  freeValue(retval);
  
  return len;
}

/*----------------------------------------------------------------------*/
Value *prim_close_FILE(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  return newBool( fclose(getAddr(argv[0])) == 0 );
}

/*----------------------------------------------------------------------*/
Value *prim_read_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  int thisturn, size, tot = 0;
  FILE *fp;
  char *ptr;
  
  fp = getAddr(argv[0]);
  size = getInt(argv[1]);
  ptr = getAddr(argv[2]);
  
  while(size) {
    thisturn = fread(ptr, 1, size, fp);
    if(thisturn == 0) return newInt(tot);
    if(thisturn < 0) continue;
    size -= thisturn;
    tot += thisturn;
    ptr += thisturn;
  }
  
  return newInt(tot);
}

/*----------------------------------------------------------------------*/
Value *prim_readline_FILE(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  int thisturn, size, tot = 0;
  FILE *fp;
  char *ptr, stopchar;
  
  fp = getAddr(argv[0]);
  size = getInt(argv[1]);
  ptr = getAddr(argv[2]);
  stopchar = (char) getInt(argv[3]);

  if(stopchar == '\n') {              /* optimize using fgets */
    if(fgets(ptr, size, fp) == NULL) return newInt(0);
    return newInt(strlen(ptr));
  }
  else { 
    while(size) {
      thisturn = fread(ptr, 1, 1, fp);
      if(thisturn == 0) return newInt(tot);
      if(thisturn < 0) continue;
      size -= thisturn;
      tot += thisturn;
      if(*ptr == stopchar) return newInt(tot);
      ptr += thisturn;
    } 
  }
  
  return newInt(tot);
}

/*----------------------------------------------------------------------*/
Value *prim_write_FILE(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  int thisturn, size, tot = 0;
  FILE *fp;
  char *ptr;
  
  fp = getAddr(argv[0]);
  size = getInt(argv[1]);
  ptr = getAddr(argv[2]);
  
  while(size) {
    thisturn = fwrite(ptr, 1, size, fp);
    if(thisturn == 0) return newInt(tot);
    if(thisturn < 0) continue;
    size -= thisturn;
    tot += thisturn;
    ptr += thisturn;
  }
  fflush(fp);
  
  return newInt(tot);
}

/*----------------------------------------------------------------------*/
Value *prim_tell_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  return newInt( ftell(getAddr(argv[0])) );
}

/*----------------------------------------------------------------------*/
Value *prim_seek_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  return newBool( fseek( getAddr(argv[0]), getInt(argv[1]), SEEK_SET ) == 0 );
}

/*----------------------------------------------------------------------*/
Value *prim_read_ready_FILE(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  fprintf(stderr, "* %s: Not supported for FILE ports.\n", proc_name);
  
  return newBool(1);
}

/*----------------------------------------------------------------------*/
Value *prim_write_ready_FILE(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  fprintf(stderr, "* %s: Not supported for FILE ports.\n", proc_name);
  
  return newBool(1);
}

/*----------------------------------------------------------------------*/
Value *prim_configure_FILE(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  return NullValue;
}

