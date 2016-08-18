/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isis.h"
#include "isis_core_prim.h"

/*----------------------------------------------------------------------*/
Value *prim_open_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  int len;
  char *path, *mode;
  FILE *fp;

  checkCount(2, "filename, mode");
  checkString(0, "filename");
  checkString(1, "mode");

  len = Listsize(argv[0]) + 1;
  path = mem_alloc(len);
  getString(argv[0], path, len);

  len = Listsize(argv[1]) + 1;
  mode = mem_alloc(len);
  getString(argv[1], mode, len);
  
  fp = fopen(path, mode);

  free(path);
  free(mode);

  if(fp) return newAddr(fp);
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_new_port_FILE(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  Value *portval;

  checkCount(4, "name, FILE pointer, inflag, outflag");
  checkString(0, "name");
  checkAddr(1, "FILE pointer");
  checkBool(2, "inflag");
  checkBool(3, "outflag");
  
  portval = newList(PORT_LIST_LENGTH, NULL);

  Listitem(portval, PORT_NAME) = referValue(argv[0]);
  Listitem(portval, PORT_TYPE) = referValue(port_type_FILE_val);
  Listitem(portval, PORT_INTERNAL_ID) = referValue(argv[1]);
  Listitem(portval, PORT_INFLAG) = referValue(argv[2]);
  Listitem(portval, PORT_OUTFLAG) = referValue(argv[3]);
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
Value *prim_new_port(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  return newList(argc, argv);
}

/*----------------------------------------------------------------------*/
Value *prim_current_port(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  if(call_data == INPUT_PORT) return referValue(script->inport);
  else return referValue(script->outport);
}

/*----------------------------------------------------------------------*/
Value *prim_set_port(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  checkCount(1, "port");
  checkList(0, "port");
  
  if(call_data == INPUT_PORT) {
    freeValue(script->inport);
    script->inport = referValue(argv[0]);
    return referValue(argv[0]);
  }
  else {
    freeValue(script->outport);
    script->outport = referValue(argv[0]);
    return referValue(argv[0]);
  }
}

/*----------------------------------------------------------------------*/
Value *prim_query_port(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  checkCount(1, "port");
  checkList(0, "port");

  if(call_data == INPUT_PORT) return referValue(Listitem(argv[0], PORT_INFLAG));
  else return referValue(Listitem(argv[0], PORT_OUTFLAG));
}

/*----------------------------------------------------------------------*/
Value *prim_interpret_port(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  Value *port;
  int items, quiet, prompt;
  
  if(call_data) {
    port = script->inport;
    quiet = 0;
    prompt = 1;
  }
  else {
    checkCountLeast(1, "port");
    checkList(0, "port");
    port = argv[0];
    if(argc > 1) quiet = trueValue(argv[1]);
    else quiet = 1;
    if(argc > 2) prompt = trueValue(argv[2]);
    else prompt = 0;
  }
  
  items = interpret_port(script, port, quiet, prompt);

  return newInt(items);
}

/*----------------------------------------------------------------------*/
Value *prim_interpret_compiled_port(Script *script, char *proc_name, int call_data, 
				    int argc, Value **argv)
{
  checkCount(1, "port");
  checkList(0, "port");
  
  return newInt( interpret_compiled_port(script, argv[0]) );
}

/*----------------------------------------------------------------------*/
Value *prim_compile_port(Script *script, char *proc_name, int call_data, 
				    int argc, Value **argv)
{
  checkCount(2, "input port, output port");
  checkList(0, "input port");
  checkList(1, "output port");
  
  return newInt( compile_port(script, argv[0], argv[1]) );
}

/*----------------------------------------------------------------------*/
Value *prim_port_open_filename(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  Value *retval;
  int len;
  char *path, *mode;
  
  checkCount(1, "filename");
  checkString(0, "filename");
  
  len = Listsize(argv[0]) + 1;
  path = mem_alloc(len);
  getString(argv[0], path, len);
  
  if(call_data == OPEN_INPUT) mode = "r";
  else if(call_data == OPEN_OUTPUT) mode = "w";
  else if(call_data == OPEN_UPDATE) mode = "r+";
  else mode = "r+";
  
  retval = new_port_FILE(path, fopen(path, mode), 
			 (call_data == OPEN_INPUT || call_data == OPEN_UPDATE), 
			 (call_data == OPEN_OUTPUT || call_data == OPEN_UPDATE));
  
  free(path);
  
  return retval;
}

/*----------------------------------------------------------------------*/
Value *prim_port_close(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  Value *newargv[1];
  
  checkCount(1, "port");
  checkList(0, "port");
  
  newargv[0] = Listitem(argv[0], PORT_INTERNAL_ID);

  return applyProcedure(script, getProc(Listitem(argv[0], PORT_CLOSE_PROC)), 
			1, newargv);
}

/*----------------------------------------------------------------------*/
Value *prim_port_read_raw(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  Value *newargv[3];
  
  checkCount(3, "port, number of bytes, buffer address");
  checkList(0, "port");
  checkInt(1, "number of bytes");
  checkAddr(2, "buffer address");
  
  newargv[0] = Listitem(argv[0], PORT_INTERNAL_ID);
  newargv[1] = argv[1];
  newargv[2] = argv[2];

  return applyProcedure(script, getProc(Listitem(argv[0], PORT_READ_PROC)), 
			3, newargv);
}

/*----------------------------------------------------------------------*/
Value *prim_port_write_raw(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  Value *newargv[3];
  
  checkCount(3, "port, number of bytes, buffer address");
  checkList(0, "port");
  checkInt(1, "number of bytes");
  checkAddr(2, "buffer address");
  
  newargv[0] = Listitem(argv[0], PORT_INTERNAL_ID);
  newargv[1] = argv[1];
  newargv[2] = argv[2];

  return applyProcedure(script, getProc(Listitem(argv[0], PORT_WRITE_PROC)), 
			3, newargv);
}

/*----------------------------------------------------------------------*/
Value *prim_port_tell(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  Value *newargv[1];
  
  checkCount(1, "port");
  checkList(0, "port");
  
  newargv[0] = Listitem(argv[0], PORT_INTERNAL_ID);

  return applyProcedure(script, getProc(Listitem(argv[0], PORT_TELL_PROC)), 
			1, newargv);
}

/*----------------------------------------------------------------------*/
Value *prim_port_seek(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  Value *newargv[2];
  
  checkCount(2, "port, position");
  checkList(0, "port");
  checkInt(1, "position");
  
  newargv[0] = Listitem(argv[0], PORT_INTERNAL_ID);
  newargv[1] = argv[1];

  return applyProcedure(script, getProc(Listitem(argv[0], PORT_SEEK_PROC)), 
			2, newargv);
}

/*----------------------------------------------------------------------*/
Value *prim_port_read_ready(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  Value *portval;
  Value *newargv[2];
  
  if(argc > 0) {
    checkList(0, "port");
    portval = argv[0];
    if(argc > 1) {
      checkInt(1, "microseconds");
      newargv[1] = argv[1];
    }
    else newargv[1] = NullValue;
  }
  else {
    portval = script->inport;
    newargv[1] = NullValue;
  }
  
  newargv[0] = Listitem(portval, PORT_INTERNAL_ID);
  
  return applyProcedure(script, getProc(Listitem(portval, PORT_READ_READY_PROC)), 
			2, newargv);
}

/*----------------------------------------------------------------------*/
Value *prim_port_write_ready(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  Value *portval;
  Value *newargv[2];
  
  if(argc > 0) {
    checkList(0, "port");
    portval = argv[0];
    if(argc > 1) {
      checkInt(1, "microseconds");
      newargv[1] = argv[1];
    }
    else newargv[1] = NullValue;
  }
  else {
    portval = script->inport;
    newargv[1] = NullValue;
  }
  
  newargv[0] = Listitem(portval, PORT_INTERNAL_ID);
  
  return applyProcedure(script, getProc(Listitem(portval, PORT_WRITE_READY_PROC)), 
			2, newargv);
}

/*----------------------------------------------------------------------*/
Value *prim_port_read_string(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  Value *port, *retval, *newargv[4];
  Procedure *proc;
  int bufsize = LINE_BUFFER_SIZE;
  char *buf;
  int len, thislen;
  
  if(argc > 0) {
    checkCountMost(1, "port");
    checkList(0, "port");
    port = argv[0];
  }
  else port = script->inport;

  /* allocate initial buffer */

  buf = allocBuffer(bufsize);
  proc = getProc(Listitem(port, PORT_READLINE_PROC));
  newargv[0] = Listitem(port, PORT_INTERNAL_ID);
  newargv[3] = newInt((int)'\n');
  len = 0;

  /* loop while still reading data and a newline has not been reached */

  while(len == 0 || buf[len - 1] != '\n') {
    
    /* increase size of buffer if it is nearly full */

    if(bufsize - len < 128) {
      buf = expandBuffer(buf, bufsize * 2);
      bufsize = bufsize * 2;
    }

    newargv[1] = newInt(bufsize - len);
    newargv[2] = newAddr(buf + len);
    
    retval = applyProcedure(script, proc, 4, newargv);
    
    freeValue(newargv[1]);
    freeValue(newargv[2]);
    
    thislen = getInt(retval);
    freeValue(retval);

    if(thislen == 0) break; /* no more data available */
    len += thislen;
  }

  freeValue(newargv[3]);

  /* return Null if nothing was read, or the string without the newline */

  if(len == 0) retval = NullValue;
  else retval = newString((buf[len - 1] == '\n') ? len - 1 : len, buf);

  /* free buffer */

  free(buf);

  return retval;
}


/*----------------------------------------------------------------------*/
Value *prim_port_write_string(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  Value *port, *newargv[3], *retval;
  int i, j, len, startarg;
  char *str, *buf;
    
  if(call_data == 1) {  /* print function -- no port */
    startarg = 0;
    port = script->outport;
  }
  else {
    checkCountLeast(1, "port, items to write");
    checkList(0, "port");
    startarg = 1;
    port = argv[0];
  }
  
  /* measure max length of string */
  
  len = 0;
  for(i = startarg; i < argc; i++) {
    if(isChar(argv[i])) len += 1;
    else if(isString(argv[i])) len += Listsize(argv[i]);
    else len += measureValue(argv[i]);
  }
  
  /* allocate buffer */

  buf = allocBuffer(len);

  /* generate string */

  str = buf;
  for(i = startarg; i < argc; i++) {
    if(isChar(argv[i])) *str++ = getChar(argv[i]);
    else if(isString(argv[i])) {
      for(j = 0; j < Listsize(argv[i]); j++) 
	*str++ = getChar(Listitem(argv[i], j));
    }
    else str = sprintValue(str, argv[i]);
  }
  len = str - buf;

  /* write it out */

  if(len) {
    newargv[0] = Listitem(port, PORT_INTERNAL_ID);
    newargv[1] = newInt(len);
    newargv[2] = newAddr(buf);
    
    retval = applyProcedure(script, getProc(Listitem(port, PORT_WRITE_PROC)), 
			    3, newargv);
    
    freeValue(newargv[1]);
    freeValue(newargv[2]);
  }
  else retval = newInt(0);
  
  /* free buffer */

  free(buf);

  /* return number of bytes written, or Null if this was "print" */

  if(call_data == 0) return retval;

  freeValue(retval);
  return NullValue;
}


/*----------------------------------------------------------------------*/
Value *prim_port_read_coded(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  Procedure *proc;
  Value *port, *newargv[3], *retval;
  unsigned int nl;
  int len, res;
  char *buf;
  char *vp;

  if(argc > 0) {
    checkCountMost(1, "port");
    checkList(0, "port");
    port = argv[0];
  }
  else port = script->inport;
  
  proc = getProc(Listitem(port, PORT_READ_PROC));
  newargv[0] = Listitem(port, PORT_INTERNAL_ID);
  len = 0;

  /* read size of packet */

  newargv[1] = newInt(4);
  newargv[2] = newAddr(&nl);
  retval = applyProcedure(script, proc, 3, newargv);
  freeValue(newargv[1]);
  freeValue(newargv[2]);

  /* if no bytes available, return Null */
  
  res = getInt(retval);
  freeValue(retval);
  if(res == 0) return NullValue;

  /* decode size of packet */
  
  len = decodeInteger(nl);
  if(len == 0) return NullValue;
  
  /* allocate buffer */

  buf = allocBuffer(len);

  /* read packet */

  newargv[1] = newInt(len);
  newargv[2] = newAddr(buf);
  retval = applyProcedure(script, proc, 3, newargv);
  freeValue(newargv[1]);
  freeValue(newargv[2]);

  res = getInt(retval);  
  freeValue(retval);

  /* decode value in packet */
  
  vp = buf;
  if(res != len) retval = NullValue;
  else retval = decodeValue(&vp);

  /* free buffer */

  free(buf);

  /* return decoded value */

  return retval;
}

/*----------------------------------------------------------------------*/
Value *prim_port_write_coded(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  Value *port, *newargv[3], *retval;
  Procedure *proc;
  unsigned int nl;
  int len, totlen = 0, i, startarg;
  char *vp, *temp, *buf = NULL;
  
  if(call_data == 1) {
    startarg = 0;
    port = script->outport;
  }
  else {
    checkCountLeast(1, "port, items to write");
    checkList(0, "port");
    startarg = 1;
    port = argv[0];
  }

  proc = getProc(Listitem(port, PORT_WRITE_PROC));
  newargv[0] = Listitem(port, PORT_INTERNAL_ID);

  /* loop for each argument */

  for(i = startarg; i < argc; i++) {

    /* find coded size of arg */

    len = precodeValue(argv[i]);
    nl = encodeInteger(len);
    len += 4;

    /* allocate buffer */

    buf = expandBuffer(buf, len);
    vp = buf;

    /* copy packet size and encode value */

    temp = (char *) &nl;
    *vp++ = *temp++; *vp++ = *temp++;
    *vp++ = *temp++; *vp++ = *temp++;
    encodeValue(argv[i], vp);

    /* write packet */

    newargv[1] = newInt(len);
    newargv[2] = newAddr(buf);
    
    retval = applyProcedure(script, proc, 3, newargv);
    
    freeValue(newargv[1]);
    freeValue(newargv[2]);

    /* keep track of total length */
    
    totlen += getInt(retval);
    freeValue(retval);
  }
  
  /* free buffer */

  free(buf);

  /* return total number of bytes written */

  return newInt(totlen);
}
