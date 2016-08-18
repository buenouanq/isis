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
Value *prim_equality_op(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  /* checks equality or non-equality of values */
  
  int i;
  
  for(i = 1; i < argc; i++)
    if(!equalValue(argv[0], argv[i])) return newBool(!call_data);
  
  return newBool(call_data);
}

/*-------------------------------------------------------------*/
Value *prim_not_op(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  checkCount(1, NULL);

  return newBool(!trueValue(argv[0]));
}

/*-------------------------------------------------------------*/
Value *prim_lt(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv)
{
  checkCount(2, NULL);
  checkNumber(0, NULL);
  checkNumber(1, NULL);
  
  if(isInt(argv[0]) && isInt(argv[1]))
    return newBool( getInt(argv[0]) < getInt(argv[1]) );

  return newBool( getRealfromNumber(argv[0]) < getRealfromNumber(argv[1]) );
}

/*-------------------------------------------------------------*/
Value *prim_le(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv)
{
  checkCount(2, NULL);
  checkNumber(0, NULL);
  checkNumber(1, NULL);
  
  if(isInt(argv[0]) && isInt(argv[1]))
    return newBool( getInt(argv[0]) <= getInt(argv[1]) );

  return newBool( getRealfromNumber(argv[0]) <= getRealfromNumber(argv[1]) );
}

/*-------------------------------------------------------------*/
Value *prim_gt(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv)
{
  checkCount(2, NULL);
  checkNumber(0, NULL);
  checkNumber(1, NULL);
  
  if(isInt(argv[0]) && isInt(argv[1]))
    return newBool( getInt(argv[0]) > getInt(argv[1]) );

  return newBool( getRealfromNumber(argv[0]) > getRealfromNumber(argv[1]) );
}

/*-------------------------------------------------------------*/
Value *prim_ge(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv)
{
  checkCount(2, NULL);
  checkNumber(0, NULL);
  checkNumber(1, NULL);
  
  if(isInt(argv[0]) && isInt(argv[1]))
    return newBool( getInt(argv[0]) >= getInt(argv[1]) );

  return newBool( getRealfromNumber(argv[0]) >= getRealfromNumber(argv[1]) );
}

