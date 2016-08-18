/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* isis_core_helpers.c */

/* This file contains functions to help interfacing Isis with external
   C code.  Also see the set of helper macros in isis.h */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>   /* for variable length argument lists */
#include "isis.h"

/*-------------------------------------------------------------*/
int isNumberList(Value *val)
{
  /* checks if all items are real or int. */
  
  int i, len;
  
  if(val == NullValue) return 0;
  if(!isList(val)) return 0;
  
  len = Listsize(val);
  for(i = 0; i < len; i++) if(!isNumber(Listitem(val, i))) return 0;
  
  return 1;
}

/*-------------------------------------------------------------*/
int getIntListfromNumberList(Value *val, int *ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     contains only numbers, using isNumberList. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen) len = maxlen;
  for(i = 0; i < len; i++) ptr[i] = getIntfromNumber(Listitem(val, i));
  
  return len;
}

/*-------------------------------------------------------------*/
int getRealListfromNumberList(Value *val, real *ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     contains only numbers, using isNumberList. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen) len = maxlen;
  for(i = 0; i < len; i++) ptr[i] = getRealfromNumber(Listitem(val, i));
  
  return len;
}

/*-------------------------------------------------------------*/
int getIntList(Value *val, int *ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     is homogeneous first, using the provided macros. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen) len = maxlen;
  for(i = 0; i < len; i++) ptr[i] = getInt(Listitem(val, i));
  
  return len;
}

/*-------------------------------------------------------------*/
int getRealList(Value *val, real *ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     is homogeneous first, using the provided macros. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen) len = maxlen;
  for(i = 0; i < len; i++) ptr[i] = getReal(Listitem(val, i));
  
  return len;
}

/*-------------------------------------------------------------*/
int getBoolList(Value *val, int *ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     is homogeneous first, using the provided macros. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen) len = maxlen;
  for(i = 0; i < len; i++) ptr[i] = getBool(Listitem(val, i));
  
  return len;
}

/*-------------------------------------------------------------*/
int getCharList(Value *val, char *ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     is homogeneous first, using the provided macros. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen - 1) len = maxlen - 1;  /* leave space for \0 at end */
  for(i = 0; i < len; i++) ptr[i] = getChar(Listitem(val, i));
  ptr[i] = 0;
  
  return len;
}

/*-------------------------------------------------------------*/
int getAddrList(Value *val, void **ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     is homogeneous first, using the provided macros. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen) len = maxlen;
  for(i = 0; i < len; i++) ptr[i] = getAddr(Listitem(val, i));
  
  return len;
}

/*-------------------------------------------------------------*/
int getProcList(Value *val, Procedure **ptr, int maxlen)
{
  /* extracts items and puts them into the array you provide.  Will
     not put more than maxlen items.  Make sure to check if the list
     is homogeneous first, using the provided macros. */
  
  int i, len;
  
  len = Listsize(val);
  if(len > maxlen) len = maxlen;
  for(i = 0; i < len; i++) ptr[i] = getProc(Listitem(val, i));
  
  return len;
}

/*-------------------------------------------------------------*/
Value *newInt(int val)
{
  Value *newval;

  newval = newValue(IsisInt);
  newval->u.intval = val;
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newReal(real val)
{
  Value *newval;

  newval = newValue(IsisReal);
  newval->u.realval = val;
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newBool(int val)
{
  Value *newval;

  newval = newValue(IsisBool);
  newval->u.boolval = (val != 0);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newChar(char val)
{
  Value *newval;

  newval = newValue(IsisChar);
  newval->u.charval = val;
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newAddr(void *val)
{
  Value *newval;

  newval = newValue(IsisAddr);
  newval->u.addressval = val;
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newProc(Procedure *val)
{
  Value *newval;

  newval = newValue(IsisProc);
  newval->u.procval = referProcedure(val);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newList(int len, Value **vals)
{
  Value *newval;
  int i;

  newval = newValue(IsisList);
  newval->listsize = len;
  newval->homogtype = IsisUnknown;
  newval->u.list = (Value **) newPtrArray(len);

  if(vals != NULL)
    for(i = 0; i < len; i++) Listitem(newval, i) = referValue(vals[i]);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newIntList(int len, int *vals)
{
  Value *newval;
  int i;

  newval = newList(len, NULL);
  newval->homogtype = IsisInt;
  for(i = 0; i < len; i++) Listitem(newval, i) = newInt(vals[i]);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newRealList(int len, real *vals)
{
  Value *newval;
  int i;

  newval = newList(len, NULL);
  newval->homogtype = IsisReal;
  for(i = 0; i < len; i++) Listitem(newval, i) = newReal(vals[i]);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newBoolList(int len, int *vals)
{
  Value *newval;
  int i;

  newval = newList(len, NULL);
  newval->homogtype = IsisBool;
  for(i = 0; i < len; i++) Listitem(newval, i) = newBool(vals[i]);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newCharList(int len, char *vals)
{
  Value *newval;
  int i;

  newval = newList(len, NULL);
  newval->homogtype = IsisChar;
  for(i = 0; i < len; i++) Listitem(newval, i) = newChar(vals[i]);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newAddrList(int len, void **vals)
{
  Value *newval;
  int i;
  
  newval = newList(len, NULL);
  newval->homogtype = IsisAddr;
  for(i = 0; i < len; i++) Listitem(newval, i) = newAddr(vals[i]);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newProcList(int len, Procedure **vals)
{
  Value *newval;
  int i;
  
  newval = newList(len, NULL);
  newval->homogtype = IsisProc;
  for(i = 0; i < len; i++) Listitem(newval, i) = newProc(vals[i]);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *newListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = referValue(va_arg(ap, Value *));
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *newIntListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisInt;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newInt(va_arg(ap, int));
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *newRealListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisReal;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newReal(va_arg(ap, real));
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *newBoolListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisBool;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newBool(va_arg(ap, int));
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *newCharListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisChar;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newChar((char) va_arg(ap, int));
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *newAddrListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisAddr;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newAddr(va_arg(ap, void *));
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *newProcListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisProc;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newProc(va_arg(ap, Procedure *));
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *newBasicListva(int len, ...)
{
  va_list ap;
  Value *newval;
  int i, dum;  

  va_start(ap, len);

  newval = newList(len, NULL);

  for(i = 0; i < len; i++) {
    switch(va_arg(ap, int)) {
    case IsisNull:
      dum = va_arg(ap, int);
      Listitem(newval, i) = NullValue;
      break;
    case IsisInt:
      Listitem(newval, i) = newInt(va_arg(ap, int));
      break;
    case IsisReal:
      Listitem(newval, i) = newReal(va_arg(ap, real));
      break;
    case IsisBool:
      Listitem(newval, i) = newBool(va_arg(ap, int));
      break;
    case IsisChar:
      Listitem(newval, i) = newChar((char) va_arg(ap, int));
      break;
    case IsisAddr:
      Listitem(newval, i) = newAddr(va_arg(ap, void *));
      break;
    case IsisProc:
      Listitem(newval, i) = newProc(va_arg(ap, Procedure *));
      break;
    default:
      dum = va_arg(ap, int);
      Listitem(newval, i) = NullValue;
      break;
    }
  }

  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Binding *bindValue(Script *script, char *name, Value *val)
{
  /* the bind*() procedures do not check for duplicate variable names. */
  
  Binding *binding;
  Varname *vname;
  
  vname = newVarname(createStr(name));

  isisMutexLock(bindMutex);

  binding = hashBind(script->env, vname, val);

  isisMutexUnlock(bindMutex);

  freeVarname(vname);

  return binding;
}  

/*-------------------------------------------------------------*/
Binding *bindInt(Script *script, char *name, int val)
{
  Value *newval;
  Binding *binding;
  
  newval = newInt(val);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindReal(Script *script, char *name, real val)
{
  Value *newval;
  Binding *binding;
  
  newval = newReal(val);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindBool(Script *script, char *name, int val)
{
  Value *newval;
  Binding *binding;
  
  newval = newBool(val);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindChar(Script *script, char *name, char val)
{
  Value *newval;
  Binding *binding;
  
  newval = newChar(val);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindAddr(Script *script, char *name, void *val)
{
  Value *newval;
  Binding *binding;
  
  newval = newAddr(val);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindProc(Script *script, char *name, Procedure *val)
{
  Value *newval;
  Binding *binding;
  
  newval = newProc(val);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindList(Script *script, char *name, int len, Value **vals)
{
  Value *newval;
  Binding *binding;
  
  newval = newList(len, vals);
  binding = bindValue(script, name, newval);
  freeValue(newval);
  
  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindIntList(Script *script, char *name, int len, int *vals)
{
  Value *newval;
  Binding *binding;
  
  newval = newIntList(len, vals);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindRealList(Script *script, char *name, int len, real *vals)
{
  Value *newval;
  Binding *binding;
  
  newval = newRealList(len, vals);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindBoolList(Script *script, char *name, int len, int *vals)
{
  Value *newval;
  Binding *binding;
  
  newval = newBoolList(len, vals);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindCharList(Script *script, char *name, int len, char *vals)
{
  Value *newval;
  Binding *binding;
  
  newval = newCharList(len, vals);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindAddrList(Script *script, char *name, int len, void **vals)
{
  Value *newval;
  Binding *binding;
  
  newval = newAddrList(len, vals);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindProcList(Script *script, char *name, int len, Procedure **vals)
{
  Value *newval;
  Binding *binding;
  
  newval = newProcList(len, vals);
  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = referValue(va_arg(ap, Value *));
  
  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindIntListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisInt;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newInt(va_arg(ap, int));
  
  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindRealListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisReal;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newReal(va_arg(ap, real));
  
  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindBoolListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisBool;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newBool(va_arg(ap, int));
  
  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindCharListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisChar;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newChar((char) va_arg(ap, int));
  
  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindAddrListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisAddr;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newAddr(va_arg(ap, void *));
  
  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindProcListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i;

  va_start(ap, len);

  newval = newList(len, NULL);
  newval->homogtype = IsisProc;
  for(i = 0; i < len; i++) 
    Listitem(newval, i) = newProc(va_arg(ap, Procedure *));
  
  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Binding *bindBasicListva(Script *script, char *name, int len, ...)
{
  va_list ap;
  Value *newval;
  Binding *binding;
  int i, dum;  

  va_start(ap, len);

  newval = newList(len, NULL);

  for(i = 0; i < len; i++) {
    switch(va_arg(ap, int)) {
    case IsisNull:
      dum = va_arg(ap, int);
      Listitem(newval, i) = NullValue;
      break;
    case IsisInt:
      Listitem(newval, i) = newInt(va_arg(ap, int));
      break;
    case IsisReal:
      Listitem(newval, i) = newReal(va_arg(ap, real));
      break;
    case IsisBool:
      Listitem(newval, i) = newBool(va_arg(ap, int));
      break;
    case IsisChar:
      Listitem(newval, i) = newChar((char) va_arg(ap, int));
      break;
    case IsisAddr:
      Listitem(newval, i) = newAddr(va_arg(ap, void *));
      break;
    case IsisProc:
      Listitem(newval, i) = newProc(va_arg(ap, Procedure *));
      break;
    default:
      dum = va_arg(ap, int);
      Listitem(newval, i) = NullValue;
      break;
    }
  }

  va_end(ap);

  binding = bindValue(script, name, newval);
  freeValue(newval);

  return binding;
}

/*-------------------------------------------------------------*/
Procedure *newCfunc(Value *(*func)(), char *name, int call_data)
{
  Procedure *newproc;
 
  newproc = newProcedure(P_CFUNC);
  newproc->cfunc.call_data = call_data;
  newproc->cfunc.name = createStr(name);
  newproc->cfunc.func = func;

  return newproc;
}

/*-------------------------------------------------------------*/
Binding *bindCfunc(Script *script, char *name, 
		   Value *(*func)(), int call_data)
{
  Binding *binding;
  Procedure *newproc;

  newproc = newCfunc(func, name, call_data);
  binding = bindProc(script, name, newproc);
  freeProcedure(newproc);   /* bindProc created its own reference */

  return binding;
}

/*-------------------------------------------------------------*/
void errorCount(Script *script, char *proc_name, int n, char *s)
{
  if(n == 0) 
    fprintf(stderr, "** %s: Expects no arguments.\n", 
	    proc_name);
  else if(n == 1)
    fprintf(stderr, "** %s: Expects 1 argument.\n", 
	    proc_name);
  else
    fprintf(stderr, "** %s: Expects %d arguments.\n", 
	    proc_name, n);

  if(s) fprintf(stderr, "**   %s\n", s);
}

/*-------------------------------------------------------------*/
void errorCountLeast(Script *script, char *proc_name, int n, char *s)
{
  if(n <= 1)
    fprintf(stderr, "** %s: Expects at least 1 argument.\n", 
	    proc_name);
  else
    fprintf(stderr, "** %s: Expects at least %d arguments.\n", 
	    proc_name, n);

  if(s) fprintf(stderr, "**   %s\n", s);
}

/*-------------------------------------------------------------*/
void errorCountMost(Script *script, char *proc_name, int n, char *s)
{
  if(n == 0)
    fprintf(stderr, "** %s: Expects no arguments.\n", 
	    proc_name);
  else if(n == 1)
    fprintf(stderr, "** %s: Expects no more than 1 argument.\n", 
	    proc_name);
  else
    fprintf(stderr, "** %s: Expects no more than %d arguments.\n", 
	    proc_name, n);

  if(s) fprintf(stderr, "**   %s\n", s);
}

/*-------------------------------------------------------------*/
void errorInt(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, "** %s: Expects integer for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects integer for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorReal(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, "** %s: Expects real number for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects real number for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorBool(Script *script, char *proc_name, int n, char *s)
{
  
  if(s == NULL) fprintf(stderr, "** %s: Expects boolean for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects boolean for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorChar(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, "** %s: Expects character for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects character for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorAddr(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, "** %s: Expects address for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects address for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorProc(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, "** %s: Expects procedure for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects procedure for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorList(Script *script, char *proc_name, int n, char *s)
{
  
  if(s == NULL) fprintf(stderr, "** %s: Expects list for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects list for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorNumber(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, "** %s: Expects number for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, "** %s: Expects number for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorIntList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects list of integers for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects list of integers for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorRealList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects list of real numbers for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects list of real numbers for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorBoolList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects list of booleans for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects list of booleans for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorCharList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects string for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects string for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorAddrList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects list of addresses for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects list of addresses for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorProcList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects list of procedures for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects list of procedures for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorListList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects list of lists for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects list of lists for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

/*-------------------------------------------------------------*/
void errorNumberList(Script *script, char *proc_name, int n, char *s)
{

  if(s == NULL) fprintf(stderr, 
			"** %s: Expects list of numbers for arg %d.\n",
			proc_name, n + 1);
  else fprintf(stderr, 
	       "** %s: Expects list of numbers for %s (arg %d).\n",
	       proc_name, s, n + 1);
}

