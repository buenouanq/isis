/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* isis_core_value.c */

/* This file contains functions to manipulate Isis values. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "isis.h"

static int significant_digits = 10;

int (*measureValueFuncs[])(Value *) = { measureInvalidValue,
					measureNullValue,
					measureIntValue,
					measureRealValue,
					measureBoolValue,
					measureCharValue,
					measureAddrValue,
					measureProcValue,
					measureListValue };

char *(*sprintValueFuncs[])(char *, Value *) = { sprintInvalidValue,
						 sprintNullValue,
						 sprintIntValue,
						 sprintRealValue,
						 sprintBoolValue,
						 sprintCharValue,
						 sprintAddrValue,
						 sprintProcValue,
						 sprintListValue };

void (*freeValueFuncs[])(Value *) = { freeInvalidValue,
				      freeNullValue,
				      freeIntValue,
				      freeRealValue,
				      freeBoolValue,
				      freeCharValue,
				      freeAddrValue,
				      freeProcValue,
				      freeListValue };

int (*equalValueFuncs[])(Value *, Value *) = { equalInvalidValue,
					       equalNullValue,
					       equalIntValue,
					       equalRealValue,
					       equalBoolValue,
					       equalCharValue,
					       equalAddrValue,
					       equalProcValue,
					       equalListValue };

int (*trueValueFuncs[])(Value *) = { trueInvalidValue,
				     trueNullValue,
				     trueIntValue,
				     trueRealValue,
				     trueBoolValue,
				     trueCharValue,
				     trueAddrValue,
				     trueProcValue,
				     trueListValue };

/* ************************************************************************ */
/* Function to set number of significant digits in real number prints */

/*-------------------------------------------------------------*/
int significantDigits(int sigdig) 
{ 
  if(sigdig < 1) significant_digits = 1;
  else significant_digits = sigdig;

  return significant_digits;
}

/* ************************************************************************ */
/* The measure*Value functions return the approximate number of
   characters that would be needed to print the value. */

/*-------------------------------------------------------------*/
int measureInvalidValue(Value *val)
{
  return 18;
}

/*-------------------------------------------------------------*/
int measureNullValue(Value *val)
{
  return 5;
}

/*-------------------------------------------------------------*/
int measureIntValue(Value *val)
{
  return 12;
}

/*-------------------------------------------------------------*/
int measureRealValue(Value *val)
{
  /* 1 sign, 1 dot, 1 space, 1 extra dig, 5 exponent, plus sig digs */
  return significant_digits + 9;  
}

/*-------------------------------------------------------------*/
int measureBoolValue(Value *val)
{
  return 6;
}

/*-------------------------------------------------------------*/
int measureCharValue(Value *val)
{
  return 4;
}

/*-------------------------------------------------------------*/
int measureAddrValue(Value *val)
{
  return 19;
}

/*-------------------------------------------------------------*/
int measureProcValue(Value *val)
{
  return 6;
}

/*-------------------------------------------------------------*/
int measureListValue(Value *val)
{
  int i, res;
  
  for(i = 0, res = 0; i < Listsize(val); i++) 
    res += measureValue(Listitem(val, i));

  return res + 4;
}

/* ************************************************************************ */
/* The sprint*Value functions write a string representation of the
   value to the specified string. */

/*-------------------------------------------------------------*/
char *sprintInvalidValue(char *str, Value *val)
{
  int offset;
  
  offset = sprintf(str, "< Invalid value > ");
  
  return str + offset;
}

/*-------------------------------------------------------------*/
char *sprintNullValue(char *str, Value *val)
{
  int offset;

  offset = sprintf(str, "Null ");
  
  return str + offset;
} 

/*-------------------------------------------------------------*/
char *sprintIntValue(char *str, Value *val)
{
  int offset;

  offset = sprintf(str, "%d ", getInt(val));

  return str + offset;
} 

/*-------------------------------------------------------------*/
char *sprintRealValue(char *str, Value *val)
{
  int offset, i;

  offset = sprintf(str, "%.*g ", significant_digits, getReal(val));

  for(i = 0; i < offset; i++) 
    if(str[i] == 'e' || str[i] == '.') return str + offset;

  strcpy(&str[offset - 1], ".0 "); 
  return str + offset + 2;
} 

/*-------------------------------------------------------------*/
char *sprintBoolValue(char *str, Value *val)
{
  int offset;
  
  offset = sprintf(str, "%s ", ((getBool(val)) ? "True" : "False"));
 
  return str + offset;
} 

/*-------------------------------------------------------------*/
char *sprintCharValue(char *str, Value *val)
{
  int offset;

  offset = sprintf(str, "'%c' ", getChar(val));

  return str + offset;
} 

/*-------------------------------------------------------------*/
char *sprintAddrValue(char *str, Value *val)
{
  int offset;

  offset = sprintf(str, "0x%lx ", (unsigned long) getAddr(val));

  return str + offset;
} 

/*-------------------------------------------------------------*/
char *sprintProcValue(char *str, Value *val)
{
  int offset;

  if(getProc(val) == NULL) offset = sprintf(str, "Proc ");
  else if((getProc(val))->kind & P_CFUNC) offset = sprintf(str, "Cfunc ");
  else offset = sprintf(str, "Proc ");

  return str + offset;
} 

/*-------------------------------------------------------------*/
char *sprintListValue(char *str, Value *val)
{
  int i, offset = 0;

  if(isString(val) && Listsize(val) > 0) {
    *str++ = STRING_CHAR;
    for(i = 0; i < Listsize(val); i++) *str++ = getChar(Listitem(val, i));
    *str++ = STRING_CHAR;
    *str++ = ' ';
  }
  else if(Listsize(val) == 0) offset += sprintf(str, "[] ");
  else {
    offset += sprintf(str, "[ ");
    for(i = 0; i < Listsize(val); i++) { 
      str = sprintValue(str + offset, Listitem(val, i));
      offset = 0;
    }
    offset += sprintf(str, "] ");
  }
  
  return str + offset;
} 

/* ************************************************************************ */
/* The free*Value functions decrease the reference count on the value
   and free the associated memory if the number of references is
   zero. */

/*-------------------------------------------------------------*/
void freeInvalidValue(Value *val)
{
  /* if this was called, there was a problem, so print an error message */
  
  fprintf(stderr, "* Tried to free an invalid value.\n");
}

/*-------------------------------------------------------------*/
void freeNullValue(Value *val)
{
  /* there will only ever be one and only one Null value, and it must
     not be freed at any time */

  return;
}

/*-------------------------------------------------------------*/
void freeIntValue(Value *val)
{
  isisMutexLock(valMutex);
  val->ref_count--;
  if(val->ref_count > 0) {
    isisMutexUnlock(valMutex);
    return;
  }
  isisMutexUnlock(valMutex);
  mem_freeItem(Value_mem, val);
}

/*-------------------------------------------------------------*/
void freeRealValue(Value *val)
{
  isisMutexLock(valMutex);
  val->ref_count--;
  if(val->ref_count > 0) {
    isisMutexUnlock(valMutex);
    return;
  }
  isisMutexUnlock(valMutex);
  mem_freeItem(Value_mem, val);
}

/*-------------------------------------------------------------*/
void freeBoolValue(Value *val)
{
  isisMutexLock(valMutex);
  val->ref_count--;
  if(val->ref_count > 0) {
    isisMutexUnlock(valMutex);
    return;
  }
  isisMutexUnlock(valMutex);
  mem_freeItem(Value_mem, val);
}

/*-------------------------------------------------------------*/
void freeCharValue(Value *val)
{
  isisMutexLock(valMutex);
  val->ref_count--;
  if(val->ref_count > 0) {
    isisMutexUnlock(valMutex);
    return;
  }
  isisMutexUnlock(valMutex);
  mem_freeItem(Value_mem, val);
}

/*-------------------------------------------------------------*/
void freeAddrValue(Value *val)
{
  isisMutexLock(valMutex);
  val->ref_count--;
  if(val->ref_count > 0) {
    isisMutexUnlock(valMutex);
    return;
  }
  isisMutexUnlock(valMutex);
  mem_freeItem(Value_mem, val);
}

/*-------------------------------------------------------------*/
void freeProcValue(Value *val)
{
  isisMutexLock(valMutex);
  val->ref_count--;
  if(val->ref_count > 0) {
    isisMutexUnlock(valMutex);
    return;
  }
  isisMutexUnlock(valMutex);
  freeProcedure(getProc(val));
  mem_freeItem(Value_mem, val);
}

/*-------------------------------------------------------------*/
void freeListValue(Value *val)
{
  int i;

  isisMutexLock(valMutex);
  val->ref_count--;
  if(val->ref_count > 0) {
    isisMutexUnlock(valMutex);
    return;
  }
  isisMutexUnlock(valMutex);

  for(i = 0; i < Listsize(val); i++)     /* free each item in list */
    freeValue(Listitem(val, i));
  freePtrArray((void **) getList(val));  /* free the pointer array */

  mem_freeItem(Value_mem, val);
}

/* ************************************************************************ */
/* The equal*Value functions test the equality of 2 values.  val1 must
   be of the type specified in the function name, and val2 is the
   other value.  Each returns 0 or 1. */

/*-------------------------------------------------------------*/
int equalInvalidValue(Value *val1, Value *val2)
{
  /* if this was called, there was a problem, so print an error message */
  
  fprintf(stderr, "* Tried to test equality on an invalid value.\n");

  return 0;
}

/*-------------------------------------------------------------*/
int equalNullValue(Value *val1, Value *val2)
{
  return val1 == val2;
}

/*-------------------------------------------------------------*/
int equalIntValue(Value *val1, Value *val2)
{
  if(val1 == val2) return 1;
  if(typeof(val2) != IsisInt) return 0;
  return getInt(val1) == getInt(val2);
}

/*-------------------------------------------------------------*/
int equalRealValue(Value *val1, Value *val2)
{
  if(val1 == val2) return 1;
  if(typeof(val2) != IsisReal) return 0;
  return getReal(val1) == getReal(val2);
}

/*-------------------------------------------------------------*/
int equalBoolValue(Value *val1, Value *val2)
{
  if(val1 == val2) return 1;
  if(typeof(val2) != IsisBool) return 0;
  return getBool(val1) == getBool(val2);
}

/*-------------------------------------------------------------*/
int equalCharValue(Value *val1, Value *val2)
{
  if(val1 == val2) return 1;
  if(typeof(val2) != IsisChar) return 0;
  return getChar(val1) == getChar(val2);
}

/*-------------------------------------------------------------*/
int equalAddrValue(Value *val1, Value *val2)
{
  if(val1 == val2) return 1;
  if(typeof(val2) != IsisAddr) return 0;
  return getAddr(val1) == getAddr(val2);
}

/*-------------------------------------------------------------*/
int equalProcValue(Value *val1, Value *val2)
{
  if(val1 == val2) return 1;
  if(typeof(val2) != IsisProc) return 0;
  return getProc(val1) == getProc(val2);
}

/*-------------------------------------------------------------*/
int equalListValue(Value *val1, Value *val2)
{
  int i, numcheck;

  if(val1 == val2) return 1;
  if(typeof(val2) != IsisList) return 0;
  if(Listsize(val1) != Listsize(val2)) return 0;
  numcheck = Listsize(val1);
  for(i = 0; i < numcheck; i++)
    if(!equalValue(Listitem(val1, i), Listitem(val2, i))) return 0;
  return 1;
}

/* ************************************************************************ */
/* The true*Value functions return 1 if the value is "True":
   Numeric/Character values: true = nonzero
   Boolean values: true = True 
   List values: true = list is not empty
   All others: true = value is not a null pointer */

/*-------------------------------------------------------------*/
int trueInvalidValue(Value *val)
{
  /* if this was called, there was a problem, so print an error message */
  
  fprintf(stderr, "* Tried to test truth on an invalid value.\n");
  
  return 0;
}

/*-------------------------------------------------------------*/
int trueNullValue(Value *val)
{
  return 0;
}

/*-------------------------------------------------------------*/
int trueIntValue(Value *val)
{
  return getInt(val) != 0;
}

/*-------------------------------------------------------------*/
int trueRealValue(Value *val)
{
  return getReal(val) != 0.0;
}

/*-------------------------------------------------------------*/
int trueBoolValue(Value *val)
{
  return getBool(val) != 0;
}

/*-------------------------------------------------------------*/
int trueCharValue(Value *val)
{
  return getChar(val) != 0;
}

/*-------------------------------------------------------------*/
int trueAddrValue(Value *val)
{
  return getAddr(val) != NULL;
}

/*-------------------------------------------------------------*/
int trueProcValue(Value *val)
{
  return getProc(val) != NULL;
}

/*-------------------------------------------------------------*/
int trueListValue(Value *val)
{
  return Listsize(val) > 0;
}

/*-------------------------------------------------------------*/
Value *newValue(int type)
{
  Value *value;
  
  value = mem_alloc_item(Value_mem);
  
  value->ref_count = 1;
  value->type = type;
  
  return value;
}

/*-------------------------------------------------------------*/
int homogeneousList(Value *val, int type)
{
  /* checks if all elements of a list are the same type */
  /* pass <= 0 for type to find the homog type of the list. */
  /* pass > 0 (the type number) to test if the list is homog of that type */

  int len, i;
  int findtype;

  if(!isList(val) || Listsize(val) == 0) return 0;

  if(val->homogtype == IsisUnknown) {
    len = Listsize(val);
    if(len == 0) val->homogtype = 0;
    else {
      findtype = typeof(Listitem(val, 0));
      for(i = 1; i < len; i++) if(typeof(Listitem(val, i)) != findtype) break;
      if(i == len) val->homogtype = findtype;
      else val->homogtype = 0;
    }
  }
  
  if(type == 0) return val->homogtype;
  else if(type == val->homogtype) return val->homogtype;
  else return 0;
}

/*-------------------------------------------------------------*/
Value *referValue(Value *val)
{
  /* increases reference count on value by 1 */

  isisMutexLock(valMutex);  
  val->ref_count++;
  isisMutexUnlock(valMutex);  
  return val;
}

