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
Value *prim_exit(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  int code;
  
  if(argc == 1 && isInt(argv[0])) code = getInt(argv[0]);
  else code = 0;
  
  exit(code);

  return NullValue;  /* never gets here */
}

/*-------------------------------------------------------------*/
Value *prim_eval(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  /* Gets an item from a string and evaluates it! */
  
  int len;
  Value *newval;
  Token *item;
  Parse *parse;
  
  checkCount(1, NULL);
  checkString(0, NULL);
  
  len = Listsize(argv[0]) + 1;
  
  parse = createParse(script, 0, NullValue, len, len);
  
  getString(argv[0], parse->s, len);
  
  newval = NullValue;
  
  if(readItem(parse, &item) > 1) process_this_item(script, item, &newval);
  
  freeItem(item);
  freeParse(parse);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *prim_uneval(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  int i, len = 0;
  char *str, *buf;
  
  for(i = 0; i < argc; i++) len += measureValue(argv[i]);
  buf = allocBuffer(len);
  str = buf;
  for(i = 0; i < argc; i++) str = sprintValue(str, argv[i]);
  len = str - buf;
  
  return newString(len - 1, buf);
}

/*-------------------------------------------------------------*/
Value *prim_set_precision(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  checkCount(1, "number of significant digits");
  checkInt(0, "number of significant digits");
  
  return newInt( significantDigits(getInt(argv[0])) );
}

/*-------------------------------------------------------------*/
Value *prim_option_query(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  /* query options in a script */
  
  checkCount(0, NULL);
  
  switch(call_data) {
  case DEBUG_MODE: return newBool(script->debug);
  case QUIET_MODE: return newBool(script->quiet);
  case PRINT_TOKENS: return newBool(script->printtokens);
  case PRINT_EXPS: return newBool(script->printexps);
  }
  
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *prim_option_set(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  /* sets options in a script */

  int turnon = 0;

  if(argc > 0) turnon = trueValue(argv[0]);

  switch(call_data) {
    
  case DEBUG_MODE:
    if(argc == 0) turnon = script->debug = !(script->debug);
    else script->debug = turnon;
    break;

  case PRINT_TOKENS:
    if(argc == 0) turnon = script->printtokens = !(script->printtokens);
    else script->printtokens = turnon;
    break;

  case PRINT_EXPS:
    if(argc == 0) turnon = script->printexps = !(script->printexps);
    else script->printexps = turnon;
    break;

  case QUIET_MODE:
    if(argc == 0) turnon = script->quiet = !(script->quiet);
    else script->quiet = turnon;
    break;

  }
  
  return newBool(turnon);
}

/*-------------------------------------------------------------*/  
Value *prim_display(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  int i;
  
  for(i = 0; i < argc; i++) printValue(script, script->outport, argv[i]);
  printStrn(script, script->outport, "\n", 1);
  
  if(argc > 0) return referValue(argv[0]);
  return NullValue;
}

/*-------------------------------------------------------------*/  
Value *prim_trace(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  Procedure *proc;
  char *str;
  int i, len, setname;
  
  for(i = 0; i < argc; i++) {
    proc = NULL;
    str = NULL;
    setname = 0;
    if(isProc(argv[i])) proc = getProc(argv[i]);
    else if(isList(argv[i]) && Listsize(argv[i]) == 2 && 
	    isProc(Listitem(argv[i], 0)) && (isString(Listitem(argv[i], 1)) || 
					     isNull(Listitem(argv[i], 1)))) {
      proc = getProc(Listitem(argv[i], 0));
      setname = 1;
      if(!isNull(Listitem(argv[i], 1))) {
	len = Listsize(Listitem(argv[i], 1)) + 1;
	str = newStr(len);
	getString(Listitem(argv[i], 1), str, len);
      }
    }
    else {
      fprintf(stderr, "** %s: Arg %d was not a procedure or a [procedure string] list.\n", 
	      proc_name, i + 1);
      continue;
    }

    if(setname) {   /* need mutex to change tracename */
      isisMutexLock(strMutex);
      if(proc->genfunc.tracename) freeStr(proc->genfunc.tracename);
      proc->genfunc.tracename = str;
      isisMutexUnlock(strMutex);
    }
    if(call_data == TRACE_ON) proc->kind |= P_TRACE_MASK;
    else if(call_data == TRACE_OFF) proc->kind &= ~P_TRACE_MASK;
  }
  
  return NullValue;
}

/*-------------------------------------------------------------*/  
Value *prim_mem_stats(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  if(call_data) mem_reset_all_stats();
  else mem_print_stats(stderr);

  return NullValue;
}

    

