/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <isis.h>
#include "isis_unix_private.h"

/*----------------------------------------------------------------------*/
Value *prim_unix_terminal_getattr(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv)
{
  struct termios tty;
  
  checkCount(1, "file descriptor");
  checkInt(0, "file descriptor");
  
  tcgetattr(getInt(argv[0]), &tty);

  return newIntListva(6, tty.c_iflag, tty.c_oflag, tty.c_cflag, tty.c_lflag,
		      cfgetispeed(&tty), cfgetospeed(&tty));
}

/*----------------------------------------------------------------------*/
Value *prim_unix_terminal_setattr(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv)
{
  struct termios tty;
  int ibaud, obaud;
  
  checkCount(7, "file descriptor, input flags, output flags, \n\t control flags, local flags, input speed, output speed");
  checkInt(0, "file descriptor");
  checkInt(1, "input flags");
  checkInt(2, "output flags");
  checkInt(3, "control flags");
  checkInt(4, "local flags");
  checkInt(5, "input speed");
  checkInt(6, "output speed");
  
  tcgetattr(getInt(argv[0]), &tty);

  ibaud = getInt(argv[5]);
  obaud = getInt(argv[6]);

  switch(ibaud) {
  case 0: ibaud = B0; break;
  case 50: ibaud = B50; break;
  case 75: ibaud = B75; break;
  case 110: ibaud = B110; break;
  case 134: ibaud = B134; break;
  case 150: ibaud = B150; break;
  case 200: ibaud = B200; break;
  case 300: ibaud = B300; break;
  case 600: ibaud = B600; break;
  case 1200: ibaud = B1200; break;
  case 1800: ibaud = B1800; break;
  case 2400: ibaud = B2400; break;
  case 4800: ibaud = B4800; break;
  case 9600: ibaud = B9600; break;
  case 19200: ibaud = B19200; break;
  case 38400: ibaud = B38400; break;
  case 57600: ibaud = B57600; break;
  case 115200: ibaud = B115200; break;
  case 230400: ibaud = B230400; break;
  default: 
    fprintf(stderr, "** Unsupported input baud rate: %d\n", ibaud);
    break;
  }

  switch(obaud) {
  case 0: obaud = B0; break;
  case 50: obaud = B50; break;
  case 75: obaud = B75; break;
  case 110: obaud = B110; break;
  case 134: obaud = B134; break;
  case 150: obaud = B150; break;
  case 200: obaud = B200; break;
  case 300: obaud = B300; break;
  case 600: obaud = B600; break;
  case 1200: obaud = B1200; break;
  case 1800: obaud = B1800; break;
  case 2400: obaud = B2400; break;
  case 4800: obaud = B4800; break;
  case 9600: obaud = B9600; break;
  case 19200: obaud = B19200; break;
  case 38400: obaud = B38400; break;
  case 57600: obaud = B57600; break;
  case 115200: obaud = B115200; break;
  case 230400: obaud = B230400; break;
  default: 
    fprintf(stderr, "** Unsupported baud rate: %d\n", obaud);
    break;
  }

  tty.c_iflag	= getInt(argv[1]);
  tty.c_oflag	= getInt(argv[2]);
  tty.c_cflag	= getInt(argv[3]);
  tty.c_lflag	= getInt(argv[4]);
  cfsetispeed(&tty, ibaud);
  cfsetospeed(&tty, obaud);

  tcsetattr(getInt(argv[0]), TCSANOW, &tty);

  return newBool(1);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_terminal_sendbreak(Script *script, char *proc_name, int call_data, 
				    int argc, Value **argv)
{
  int duration = 0;
  
  checkCountLeast(1, "file descriptor, duration in milliseconds");
  checkInt(0, "file descriptor");
  
  if(argc == 2) {
    checkInt(1, "duration in milliseconds");
    duration = getInt(argv[1]);
  }
  
  return newBool( tcsendbreak(getInt(argv[0]), duration) == 0 );
}


/*----------------------------------------------------------------------*/
Value *prim_unix_terminal_drain(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv)
{
  checkCount(1, "file descriptor");
  checkInt(0, "file descriptor");
  
  return newBool( tcdrain(getInt(argv[0])) == 0 );
}

