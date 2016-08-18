/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* Unix port functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <isis.h>
#include "isis_unix_private.h"

/*----------------------------------------------------------------------*/
Value *prim_unix_fileno(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  checkCount(1, "FILE pointer");
  checkAddr(0, "FILE pointer");
  
  return newInt( fileno((FILE *)getAddr(argv[0])) );
}

/*----------------------------------------------------------------------*/
Value *prim_unix_pipe(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  int filedes[2];
  
  checkCount(0, NULL);
  
  if(pipe(filedes) == 0) return newIntList(2, filedes);
  
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_dup(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  checkCountLeast(1, NULL);
  checkInt(0, NULL);
  
  if(argc == 1) return newInt( dup(getInt(argv[0])) );
  
  checkInt(1, NULL);
  return newBool( dup2(getInt(argv[0]), getInt(argv[1])) == 0);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_open(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  int len, oflag, mode;
  char *path;
  int fd;

  checkCount(3, "filename, oflag, mode");
  checkString(0, "filename");
  checkInt(1, "oflag");
  checkInt(2, "mode");

  len = Listsize(argv[0]) + 1;
  path = malloc(len);
  getString(argv[0], path, len);
  
  oflag = getInt(argv[1]);
  mode = getInt(argv[2]);

  fd = open(path, oflag, mode);

  free(path);

  if(fd >= 0) return newInt(fd);
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_new_port_FD(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  Value *portval;
  
  checkCount(4, "name, file descriptor, inflag, outflag");
  checkString(0, "name");
  checkInt(1, "file descriptor");
  checkBool(2, "inflag");
  checkBool(3, "outflag");
  
  portval = newList(PORT_LIST_LENGTH, NULL);

  Listitem(portval, PORT_NAME) = referValue(argv[0]);
  Listitem(portval, PORT_TYPE) = referValue(isis_unix_port_type_FD_val);
  Listitem(portval, PORT_INTERNAL_ID) = referValue(argv[1]);
  Listitem(portval, PORT_INFLAG) = referValue(argv[2]);
  Listitem(portval, PORT_OUTFLAG) = referValue(argv[3]);
  Listitem(portval, PORT_CLOSE_PROC) = referValue(prim_unix_close_FD_val);
  Listitem(portval, PORT_READ_PROC) = referValue(prim_unix_read_FD_val);
  Listitem(portval, PORT_READLINE_PROC) = referValue(prim_unix_readline_FD_val);
  Listitem(portval, PORT_WRITE_PROC) = referValue(prim_unix_write_FD_val);
  Listitem(portval, PORT_TELL_PROC) = referValue(prim_unix_tell_FD_val);
  Listitem(portval, PORT_SEEK_PROC) = referValue(prim_unix_seek_FD_val);
  Listitem(portval, PORT_READ_READY_PROC) = referValue(prim_unix_read_ready_FD_val);
  Listitem(portval, PORT_WRITE_READY_PROC) = referValue(prim_unix_write_ready_FD_val);
  Listitem(portval, PORT_CONFIGURE_PROC) = referValue(prim_unix_configure_FD_val);

  return portval;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_close_FD(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  return newBool( close(getInt(argv[0])) == 0 );
}

/*----------------------------------------------------------------------*/
Value *prim_unix_read_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  int thisturn, size, tot = 0;
  int fd;
  char *ptr;
  
  fd = getInt(argv[0]);
  size = getInt(argv[1]);
  ptr = getAddr(argv[2]);
  
  while(size) {
    thisturn = read(fd, ptr, size);
    if(thisturn <= 0) return newInt(tot);
    /* if(thisturn < 0) continue; */
    size -= thisturn;
    tot += thisturn;
    ptr += thisturn;
  }
  
  return newInt(tot);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_readline_FD(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int thisturn, size, tot = 0;
  int fd;
  char *ptr, stopchar;
  
  fd = getInt(argv[0]);
  size = getInt(argv[1]);
  ptr = getAddr(argv[2]);
  stopchar = (char) getInt(argv[3]);

  while(size) {
    thisturn = read(fd, ptr, 1);
    if(thisturn <= 0) return newInt(tot);
    /* if(thisturn < 0) continue; */
    size -= thisturn;
    tot += thisturn;
    if(*ptr == stopchar) return newInt(tot);
    ptr += thisturn;
  }
  
  return newInt(tot);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_write_FD(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  int actual;
  
  actual = write( getInt(argv[0]), getAddr(argv[2]), getInt(argv[1]) );
  if(actual >= 0) return newInt(actual);
  else return newInt(0);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_tell_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  return newInt( lseek( getInt(argv[0]), 0, SEEK_CUR ) );
}

/*----------------------------------------------------------------------*/
Value *prim_unix_seek_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  return newBool( lseek( getInt(argv[0]), getInt(argv[1]), SEEK_SET ) >= 0 );
}

/*----------------------------------------------------------------------*/
Value *prim_unix_ready_FD(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  struct timeval tv;
  fd_set fdset;
  int fd, res;
  
  if(!isNull(argv[1])) {
    tv.tv_usec = (int) getInt(argv[1]);
    tv.tv_sec = tv.tv_usec / 1000000;
    tv.tv_usec = tv.tv_usec % 1000000;
  }
  else {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }
  
  fd = getInt(argv[0]);
  FD_ZERO(&fdset);
  FD_SET(fd, &fdset);
  if(call_data) res = select(fd + 1, NULL, &fdset, NULL, &tv);
  else res = select(fd + 1, &fdset, NULL, NULL, &tv);
  if(res >= 0) return newBool(res);
  else return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_ready_list_FD(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  Value *newval;
  struct timeval tv;
  fd_set fdset;
  int numfds, i, fd, res, maxfd;
  
  if(argc > 1 && isInt(argv[1])) {
    tv.tv_usec = (int) getInt(argv[1]);
    tv.tv_sec = tv.tv_usec / 1000000;
    tv.tv_usec = tv.tv_usec % 1000000;
  }
  else {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }
  
  numfds = Listsize(argv[0]);
  maxfd = 0;
  FD_ZERO(&fdset);
  for(i = 0; i < numfds; i++) {
    fd = getInt(Listitem(argv[0], i));
    FD_SET(fd, &fdset);
    if(fd > maxfd) maxfd = fd;
  }

  if(call_data) res = select(maxfd + 1, NULL, &fdset, NULL, &tv);
  else res = select(maxfd + 1, &fdset, NULL, NULL, &tv);

  if(res < 0) return NullValue;
  newval = newList(numfds, NULL);
  if(res > 0) for(i = 0; i < numfds; i++) {
    fd = getInt(Listitem(argv[0], i));
    Listitem(newval, i) = newBool(FD_ISSET(fd, &fdset));
  }
  else for(i = 0; i < numfds; i++) Listitem(newval, i) = newBool(0);
  return newval;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_configure_FD(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_recv_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  int tot;
  
  tot = recv(getInt(argv[0]), getAddr(argv[2]), getInt(argv[1]), 0);
  if(tot < 0) return NullValue;
  return newInt(tot);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_send_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  int tot;
  
  tot = send(getInt(argv[0]), getAddr(argv[2]), getInt(argv[1]), 0);
  if(tot < 0) return NullValue;
  return newInt(tot);
}

