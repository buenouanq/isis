/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/*********************************************************/
/*  large pieces of this file from Ann Bui's original    */
/*********************************************************/  

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <isis.h>
#include "isis_unix_private.h"

/**************************************************************************/

/* ---------------------------------------------------------------------- */
Value *prim_unix_system(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int size, ret;
  char *command;
  
  if(argc == 0) return newBool(system(NULL)); 
  
  checkCountMost(1, "command");
  checkString(0, "command");
  
  size = Listsize(argv[0]) + 1;
  command = (char *) malloc(sizeof(char) * size);
  getString(argv[0], command, size);
  ret = system(command);
  free(command);
  return newInt(ret);
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_get_env(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  int size;
  char *getvar, *varval;
  
  checkCount(1, "environment variable name");
  checkString(0, "environment variable name");

  size = Listsize(argv[0]) + 1;
  getvar = (char *) malloc(sizeof(char) * size);
  getString(argv[0], getvar, size);
  varval = getenv(getvar);
  free(getvar);
  if(varval) return newString(strlen(varval), varval);
  else return NullValue;
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_set_env(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  int size, size1, size2;
  char *getvar;

  checkCount(2, "environment variable name, value");
  checkString(0, "environment variable name");
  checkString(1, "environment variable value");

  size1 = Listsize(argv[0]) + 1; 
  size2 = Listsize(argv[1]) + 1;
  size = size1 + size2;
  getvar = (char *) malloc(sizeof(char) * size);
  getString(argv[0], getvar, size1);
  getvar[size1 - 1] = '=';
  getString(argv[1], &getvar[size1], size2);
  putenv(getvar);
  /* should not free getvar -- see man page */
  return referValue(argv[1]);
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_get_time(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  struct timeval tp;
  struct timezone tzp;

  checkCount(0, NULL);
  gettimeofday(&tp, &tzp);  
  return newIntListva(2, (int) tp.tv_sec, (int) tp.tv_usec);
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_sleep(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  struct timeval tv;
  real rsec;
  
  checkCount(1, "number of seconds");
  checkNumber(0, "number of seconds");
    
  rsec = getRealfromNumber(argv[0]);
  tv.tv_sec = (int) rsec;
  tv.tv_usec = (int) (1000000.0 * (rsec - ((real) tv.tv_sec)));

  return newBool( select(0, NULL, NULL, NULL, &tv) == 0);
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_microsleep(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  struct timeval tv;
  
  checkCount(1, "number of microseconds");
  checkNumber(0, "number of microseconds");

  tv.tv_usec = (int) getRealfromNumber(argv[0]);
  tv.tv_sec = tv.tv_usec / 1000000;
  tv.tv_usec = tv.tv_usec % 1000000;

  return newBool( select(0, NULL, NULL, NULL, &tv) == 0);
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_fork(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  return newInt(fork());
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_exec(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  /* exec accepts each argument string separately, program name first.
     isis: (exec progname arg1 arg2 ...) */

  int i, len;
  char **args;
  
  checkCountLeast(1, "command and argument strings");
  
  /* Check that all args are strings */

  for(i = 0; i < argc; i++) checkString(i, NULL);
  
  /* Extract all strings into a pointer array */
  
  args = (char **) malloc((argc + 1) * sizeof(char *));
  for(i = 0; i < argc; i++) {
    len = Listsize(argv[i]) + 1;
    args[i] = (char *) malloc(len);
    getString(argv[i], args[i], len);
  }
  args[argc] = NULL;  /* last slot must be NULL to terminate arg list */

  execvp(args[0], args);

  /* execvp should never return unless there is any error */
  
  fprintf(stderr, "** %s: Unable to execute %s.\n", proc_name, args[0]);
  
  for(i = 0; i < argc; i++) free(args[i]);
  free(args);
  
  return NullValue;
} 

/* ---------------------------------------------------------------------- */
Value *prim_unix_execute(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  /* isis_execute is a version of exec that takes any number of strings.
     they that are concatenated together to form the command to run.
     simpler to use but less versatile than exec. */

  int size, i, numargs, p, stemp;
  char *progname, *temp;
  char **args;
  
  checkCountLeast(1, "command and argument strings");

  /* Check that all args are strings */
  
  for(i = 0; i < argc; i++) checkString(i, NULL);

  /* Get total size of strings */

  size = 0; 
  for(i = 0; i < argc; i++) size = size + Listsize(argv[i]) + 1;
  temp = (char *) malloc(sizeof(char) * size); 

  /* Put all the string arguments into one array */

  p = 0;
  for(i = 0; i < argc; i++) {
    stemp = Listsize(argv[i]) + 1;
    getString(argv[i], &temp[p], stemp);
    p += stemp;
    temp[p - 1] = ' ';
  }
  temp[p - 1] = 0;
  
  /* Count the number of arguments by looking for spaces.  There may
     be more than one space consecutively. */

  p = numargs = 0;
  size = size - 1;  /* ignore null char in length */
  while(isspace(temp[p])) p++;    /* skip over starting spaces */
  while(p < size) {
    while(isgraph(temp[p])) p++;
    numargs++;
    while(isspace(temp[p])) p++;
  }

  /* We now allocate memory for the array that we are going to send to
     execvp there should be the number of arguments plus one slot for
     termination of the argument array.  Thus the + 1. */
  
  args = (char **) malloc(sizeof(char *) * (numargs + 1));
  
  /* Build the argument array. */
  
  p = 0;
  while(isspace(temp[p])) p++;  /* skip over starting spaces */
  for(i = 0; i < numargs; i++) {
    args[i] = &temp[p];
    while(isgraph(temp[p])) p++;    
    while(isspace(temp[p])) temp[p++] = 0;
  }
  args[numargs] = NULL;   /* terminate args with NULL */

  progname = args[0];
  execvp(progname, args);

  /* execvp should never return unless there is any error */

  fprintf(stderr, "** %s: Unable to execute %s.\n", proc_name, progname);

  free(temp);
  free(args);

  return NullValue;

} 

/* ---------------------------------------------------------------------- */
Value *prim_unix_waitpid(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  int status, pid, nopid;  

  nopid = -1;
  status = 0;

  if(argc == 0) {       /* Call waitpid with pid = -1. */    
    
    if(call_data == 0) pid = waitpid(nopid, &status, WNOHANG);
    else if(call_data == 1) pid = waitpid(nopid, &status, 0);
    
    return newIntListva(2, pid, status);    
  } 
  
  checkCountMost(1, "process id");
  checkInt(0, "process id");
  
  if(call_data == 0) pid = waitpid(getInt(argv[0]), &status, WNOHANG);
  else if(call_data == 1) pid = waitpid(getInt(argv[0]), &status, 0);

  return newIntListva(2, pid, status);
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_send_signal(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  checkCount(2, "process id, signal");
  checkInt(0, "process id");
  checkInt(1, "signal");
  
  return newBool( kill(getInt(argv[0]), getInt(argv[1])) == 0 );
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_get_process_id(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv)
{
  checkCount(0, NULL);
  
  return newInt( getpid() );
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_get_parent_id(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  checkCount(0, NULL);
  
  return newInt( getppid() );
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_get_user_id(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  checkCount(0, NULL);
  
  return newInt( getuid() );
}

/* ---------------------------------------------------------------------- */
Value *prim_unix_get_group_id(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  checkCount(0, NULL);
  
  return newInt( getgid() );
}



