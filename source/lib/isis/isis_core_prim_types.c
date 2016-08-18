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
Value *prim_cast_integer(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  checkCount(1, NULL);
  
  switch(typeof(argv[0])) {    
  case IsisInt: return referValue(argv[0]);
  case IsisReal: return newInt((int) getReal(argv[0]));
  case IsisChar: return newInt((int) getChar(argv[0]));
  case IsisBool: return newInt((int) getBool(argv[0]));
  case IsisAddr: return newInt((int) getAddr(argv[0]));
  case IsisNull: return newInt(0);
  }
  
  fprintf(stderr, "** %s: Invalid cast.\n", proc_name);
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_cast_real(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  checkCount(1, NULL);
  
  switch(typeof(argv[0])) {    
  case IsisInt: return newReal((real) getInt(argv[0]));
  case IsisReal: return referValue(argv[0]);
  case IsisChar: return newReal((real) getChar(argv[0]));
  case IsisBool: return newReal((real) getBool(argv[0]));
  case IsisNull: return newReal(0.0);
  }
  
  fprintf(stderr, "** %s: Invalid cast.\n", proc_name);
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_cast_character(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  checkCount(1, NULL);
  
  switch(typeof(argv[0])) {    
  case IsisInt: return newChar((char) getInt(argv[0]));
  case IsisReal: return newChar((char) getReal(argv[0]));
  case IsisChar: return referValue(argv[0]);
  case IsisBool: return newChar((char) getBool(argv[0]));
  case IsisAddr: return newChar((char) ((int) getAddr(argv[0])));
  case IsisNull: return newChar(0);
  }
  
  fprintf(stderr, "** %s: Invalid cast.\n", proc_name);
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_cast_boolean(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  checkCount(1, NULL);
  
  switch(typeof(argv[0])) {    
  case IsisInt: return newBool(getInt(argv[0]) != 0);
  case IsisReal: return newBool(getReal(argv[0]) != 0.0);
  case IsisChar: return newBool(getChar(argv[0]) != 0);
  case IsisBool: return referValue(argv[0]);
  case IsisAddr: return newBool(getAddr(argv[0]) != NULL);
  case IsisNull: return newBool(0);
  }
  
  fprintf(stderr, "** %s: Invalid cast.\n", proc_name);
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_cast_address(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  checkCount(1, NULL);
  
  switch(typeof(argv[0])) {    
  case IsisInt: return newAddr((void *) getInt(argv[0]));
  case IsisReal: return newAddr((void *) ((unsigned long) getReal(argv[0])));
  case IsisChar: return newAddr((void *) ((unsigned long) getChar(argv[0])));
  case IsisBool: return newAddr((void *) getBool(argv[0]));
  case IsisAddr: return referValue(argv[0]);
  case IsisNull: return newAddr(NULL);
  }
  
  fprintf(stderr, "** %s: Invalid cast.\n", proc_name);
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_cast_procedure(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  /* might want to cast address to procedure some day */
  
  checkCount(1, NULL);
  
  switch(typeof(argv[0])) {    
  case IsisProc: return referValue(argv[0]);
  }
  
  fprintf(stderr, "** %s: Invalid cast.\n", proc_name);
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_type_query(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  checkCount(1, NULL);
  
  return newBool(call_data == typeof(argv[0]));
}

/*-------------------------------------------------------------*/
Value *prim_list_type_query(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  checkCount(1, NULL);
  
  return newBool( homogeneousList(argv[0], call_data) );
}
