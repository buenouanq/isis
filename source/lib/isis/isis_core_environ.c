/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* isis_core_environ.c */

/* This file contains functions to manipulate bindings and
   environments. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "isis.h"

/*-------------------------------------------------------------*/
Varname *newVarname(char *str)
{
  /* pass a copy of the str so it can be freed when varname is freed */

  Varname *vname;
  
  vname = mem_alloc_item(Varname_mem);
  vname->ref_count = 1;
  vname->str = str;   

  return vname;
}

/*-------------------------------------------------------------*/
Varname *referVarname(Varname *vname)
{
  if(vname != NULL) {
    isisMutexLock(vnameMutex);
    vname->ref_count++;
    isisMutexUnlock(vnameMutex);
  }
  return vname;
}

/*-------------------------------------------------------------*/
Varname **copyVarnameArray(Varname **vnames, int num)
{
  Varname **newvnames;
  int i;

  newvnames = (Varname **)newPtrArray(num);
  for(i = 0; i < num; i++) newvnames[i] = referVarname(vnames[i]);

  return newvnames;
}

/*-------------------------------------------------------------*/
void freeVarname(Varname *vname)
{
  if(vname == NULL) return;
  
  isisMutexLock(vnameMutex);
  
  vname->ref_count--;
  
  if(vname->ref_count > 0) {
    isisMutexUnlock(vnameMutex);
    return;
  }
  
  isisMutexUnlock(vnameMutex);

  /* no more references to this varname exist */

  freeStr(vname->str);
  mem_freeItem(Varname_mem, vname);
}

/*-------------------------------------------------------------*/
void freeVarnameArray(Varname **vnames, int num)
{
  int i;
  
  for(i = 0; i < num; i++) freeVarname(vnames[i]);
  freePtrArray((void **) vnames);
}

/*-------------------------------------------------------------*/
Environment *arrayEnvironment(Environment *parent, int size)
{
  /* creates an ARRAY referenced environment with specified size */
  /* but the actual bindings are UNINITIALIZED for efficiency */

  Environment *env;
  
  env = mem_alloc_item(Environment_mem);
  
  env->ref_count = 1;
  env->ref_type = R_ARRAY;
  env->size = size;
  env->parent = referEnvironment(parent);

  env->bindings = (Binding **) newPtrArray(size);
  
  return env;
}

/*-------------------------------------------------------------*/
Environment *hashEnvironment(Environment *parent, int tablesize)
{
  /* creates a HASH referenced environment with specified table size */

  int i;
  Environment *env;
  Binding **hashtable;
  
  env = mem_alloc_item(Environment_mem);
  
  env->ref_count = 1;
  env->ref_type = R_HASH;
  env->size = tablesize;
  env->parent = referEnvironment(parent);

  hashtable = (Binding **) newPtrArray(tablesize);
  for(i = 0; i < tablesize; i++) hashtable[i] = NULL;
  env->bindings = hashtable;

  return env;
}

/*-------------------------------------------------------------*/
Environment *referEnvironment(Environment *env)
{
  /* Increases ref count on an environment */
  
  if(env != NULL) {
    isisMutexLock(envMutex);
    env->ref_count++;
    isisMutexUnlock(envMutex);
  }
  return env;
}

/*-------------------------------------------------------------*/
Environment *freeEnvironment(Environment *env)
{
  /* Frees the top frame of the environment and returns parent. */

  int i;
  Binding *binding, *next;
  Environment *parent;
  
  if(env == NULL) return NULL;
  
  isisMutexLock(envMutex);

  env->ref_count--;
  
  parent = env->parent;

  if(env->ref_count > 0) {
    isisMutexUnlock(envMutex);
    return parent;
  }

  isisMutexUnlock(envMutex);

  /* no more references to this environment exist */

  switch(env->ref_type) {

  case R_HASH:
    for(i = 0; i < env->size; i++) {
      binding = ((Binding **) env->bindings)[i];
      while(binding != NULL) {
	freeVarname(binding->vname);
	freeValue(binding->val);
	next = binding->next;
	mem_freeItem(Binding_mem, binding);
	binding = next;
      }
    }
    freePtrArray((void **) env->bindings);
    break;
      
  case R_ARRAY:
    for(i = 0; i < env->size; i++) {
      binding = ((Binding **) env->bindings)[i];
      freeVarname(binding->vname);
      freeValue(binding->val);
      mem_freeItem(Binding_mem, binding);
    }
    freePtrArray((void **) env->bindings);
    break;
      
  }
    
  mem_freeItem(Environment_mem, env);
  
  freeEnvironment(parent);

  return parent;
}

/*-------------------------------------------------------------*/
Binding *searchEnvironment(char *varname, Environment *env,
			   int *fnum, int *bnum)
{
  /* Searches the entire environment chain for a named variable and
     returns its binding, or NULL if it was not found.  Also returns
     the lexical address of the binding if appropriate (fnum and
     bnum).  This function should only be called from
     formExpression(). */
  
  Environment *curenv;
  Binding *curbind;
  int pos;
  
  *fnum = 0;
  curenv = env;
  
  while(curenv != NULL) {
    
    switch(curenv->ref_type) {

    case R_ARRAY:
      for(pos = 0; pos < curenv->size; pos++) {
	curbind = ((Binding **) curenv->bindings)[pos];
	if(strcmp(varname, curbind->vname->str) == 0) { 
	  *bnum = pos; 
	  return curbind; 
	}
      }
      break;

    case R_HASH:
      *bnum = -1;
      if(curbind = searchTop(varname, curenv)) return curbind;
      break;
      
    }
    
    *fnum += 1;
    curenv = curenv->parent;
  }
  
  *fnum = -1;   /* means "unresolved" */
  return NULL;  /* unsucessful */

}

/*-------------------------------------------------------------*/
Binding *searchTop(char *varname, Environment *env)
{
  /* Searches a top level hash environment for a named variable and
     returns its binding, or NULL if it was not found.  Mutex lock not
     necessary.  Meant to be a fast function for searching the top
     level environment, for use by the eval*Expression() functions.
     Assumes env is not NULL! */
  
  Binding *curbind;
  int pos;
  
  pos = hashStr(varname, env->size);
  curbind = ((Binding **) env->bindings)[pos];
  while(curbind != NULL) {
    if(strcmp(varname, curbind->vname->str) == 0) return curbind;
    curbind = curbind->next;
  }
  
  return NULL;  /* unsucessful */
}

/*-------------------------------------------------------------*/
Binding *hashBind(Environment *env, Varname *vname, Value *val) 
{
  /* adds a new binding to the specified HASH environment.  the bind
     mutex should be locked before calling this function. */
  
  Binding *newbind;
  int pos;
  
  newbind = mem_alloc_item(Binding_mem);
  newbind->vname = referVarname(vname);
  newbind->val = referValue(val);
  
  pos = hashStr(newbind->vname->str, env->size);
  
  newbind->next = ((Binding **) env->bindings)[pos];
  ((Binding **) env->bindings)[pos] = newbind;

  return newbind;
}

/*-------------------------------------------------------------*/
Binding *arrayBind(Environment *env, int bnum, 
		   Varname *vname, Value *val) 
{
  /* adds a new binding to the specified ARRAY environment at the
     specified position.  not necessary to lock mutex, because this
     function will not be called more than once simultaneously for the
     same environment -- it's only called right after a new array
     environment is created.  */
  
  Binding *newbind;
  
  newbind = mem_alloc_item(Binding_mem);
  newbind->vname = referVarname(vname);
  newbind->val = referValue(val);
  
  return (((Binding **) env->bindings)[bnum] = newbind);
}

/*-------------------------------------------------------------*/
Binding *lookupBinding(Environment *env, int fnum, int bnum)
{
  /* Retrieves the binding at the specified lexical address in the
     ARRAY referenced environment chain.  not necessary to lock
     mutex -- binding reference will not change. */

  int i;
  
  for(i = 0; i < fnum; i++) env = env->parent;
  
  return ((Binding **) env->bindings)[bnum];
}

/*-------------------------------------------------------------*/
int hashStr(char *str, int len)
{
  int res = 0;

  while(*str) res += *str++ - 32;  /* no control characters! */

  return (res % len);
}

/*-------------------------------------------------------------*/
Procedure *newProcedure(int kind)
{
  Procedure *newproc;
  
  newproc = mem_alloc_item(Procedure_mem);
  newproc->kind = kind;
  newproc->genfunc.ref_count = 1;
  newproc->genfunc.tracename = NULL;

  return newproc;
}

/*-------------------------------------------------------------*/
Procedure *referProcedure(Procedure *proc)
{
  /* Increases reference count on a procedure */
  
  if(proc != NULL) {
    isisMutexLock(procMutex);
    proc->genfunc.ref_count++;
    isisMutexUnlock(procMutex);
  }
  return proc;
}

/*-------------------------------------------------------------*/
void freeProcedure(Procedure *proc)
{
  if(proc == NULL) return;

  isisMutexLock(procMutex);

  proc->genfunc.ref_count--;

  if(proc->genfunc.ref_count > 0) {
    isisMutexUnlock(procMutex);
    return;
  }

  isisMutexUnlock(procMutex);

  /* no more references to this procedure exist */

  if(proc->genfunc.tracename) freeStr(proc->genfunc.tracename);
  
  switch(proc->kind & P_TYPE_MASK) {
    
  case P_CFUNC:
    freeStr(proc->cfunc.name);
    break;
    
  case P_SCRIPT:
    freeVarnameArray(proc->scriptfunc.formals, proc->scriptfunc.num_formals);
    freeEnvironment(proc->scriptfunc.env);
    freeExpression(proc->scriptfunc.body);
    break;
    
  default:
    fprintf(stderr, "* Unknown procedure type %d in freeProcedure\n", 
	    proc->kind);
    break;
  }

  mem_freeItem(Procedure_mem, proc);
}

/*-------------------------------------------------------------*/
Value *callProcedure(Script *script, Procedure *proc, int argc, ...)
{
  va_list ap;
  Value **argv, *newval;
  int i;

  va_start(ap, argc);
  
  argv = (Value **) newPtrArray(argc);
  for(i = 0; i < argc; i++) argv[i] = va_arg(ap, Value *);
  
  newval = applyProcedure(script, proc, argc, argv);
  
  freePtrArray((void **) argv);
  
  va_end(ap);

  return newval;
}

/*-------------------------------------------------------------*/
Value *applyProcedure(Script *script, Procedure *proc, int argc, Value **argv)
{
  /* you must free each value in argv after calling this function */

  Value *newval, *arglistval;
  Environment *env;
  int i, numargs;
  
  if(proc == NULL) return NullValue;

  switch(proc->kind) {

  case P_CFUNC:
    newval = proc->cfunc.func(script, 
			      proc->cfunc.name,
			      proc->cfunc.call_data,
			      argc, argv);
    break;

  case P_SCRIPT:
    if(proc->scriptfunc.var_num_args) {
      env = arrayEnvironment(proc->scriptfunc.env, 1);
      arglistval = newList(argc, NULL);
      for(i = 0; i < argc; i++) Listitem(arglistval, i) = referValue(argv[i]);
      arrayBind(env, 0, proc->scriptfunc.formals[0], arglistval);
      freeValue(arglistval);
    }
    else {
      env = arrayEnvironment(proc->scriptfunc.env, 
			     proc->scriptfunc.num_formals);
      if(argc == proc->scriptfunc.num_formals) numargs = argc;
      else {
	numargs = minimum(argc, proc->scriptfunc.num_formals);
	fprintf(stderr, 
		"* Incorrect number of arguments in call (%d for %d).\n",
		argc, proc->scriptfunc.num_formals);
	for(i = numargs; i < proc->scriptfunc.num_formals; i++)
	  arrayBind(env, i, proc->scriptfunc.formals[i], NullValue);
      }
      for(i = 0; i < numargs; i++)
	arrayBind(env, i, proc->scriptfunc.formals[i], argv[i]);
    }
    evalExpression(script, env, proc->scriptfunc.body, &newval);
    env = freeEnvironment(env);
    break;

  case P_CFUNC_TRACE:
    for(i = 0; i < script->traceindent; i++) 
      printStrn(script, script->outport, ISIS_TRACE_FILL, ISIS_TRACE_FILL_LEN);
    printStrn(script, script->outport, "( ", 2);
    if(proc->cfunc.tracename) printStr(script, script->outport, proc->cfunc.tracename);
    else printStr(script, script->outport, proc->cfunc.name);
    printStrn(script, script->outport, " ", 1);
    for(i = 0; i < argc; i++) 
      printValue(script, script->outport, argv[i]);
    printStrn(script, script->outport, ")\n", 2);
    script->traceindent++;

    newval = proc->cfunc.func(script, 
			      proc->cfunc.name,
			      proc->cfunc.call_data,
			      argc, argv);

    script->traceindent--;
    for(i = 0; i < script->traceindent; i++) 
      printStrn(script, script->outport, ISIS_TRACE_FILL, ISIS_TRACE_FILL_LEN);
    printValue(script, script->outport, newval);
    printStrn(script, script->outport, "\n", 1);
    break;

  case P_SCRIPT_TRACE:
    for(i = 0; i < script->traceindent; i++) 
      printStrn(script, script->outport, ISIS_TRACE_FILL, ISIS_TRACE_FILL_LEN);
    printStrn(script, script->outport, "( ", 2);
    if(proc->scriptfunc.tracename) printStr(script, script->outport, proc->scriptfunc.tracename);
    else printStrn(script, script->outport, "{unnamed}", 9);
    printStrn(script, script->outport, " ", 1);
    for(i = 0; i < argc; i++) 
      printValue(script, script->outport, argv[i]);
    printStrn(script, script->outport, ")\n", 2);
    script->traceindent++;

    if(proc->scriptfunc.var_num_args) {
      env = arrayEnvironment(proc->scriptfunc.env, 1);
      arglistval = newList(argc, NULL);
      for(i = 0; i < argc; i++) Listitem(arglistval, i) = referValue(argv[i]);
      arrayBind(env, 0, proc->scriptfunc.formals[0], arglistval);
      freeValue(arglistval);
    }
    else {
      env = arrayEnvironment(proc->scriptfunc.env, 
			     proc->scriptfunc.num_formals);
      if(argc == proc->scriptfunc.num_formals) numargs = argc;
      else {
	numargs = minimum(argc, proc->scriptfunc.num_formals);
	fprintf(stderr, 
		"* Incorrect number of arguments in call (%d for %d).\n",
		argc, proc->scriptfunc.num_formals);
	for(i = numargs; i < proc->scriptfunc.num_formals; i++)
	  arrayBind(env, i, proc->scriptfunc.formals[i], NullValue);
      }
      for(i = 0; i < numargs; i++)
	arrayBind(env, i, proc->scriptfunc.formals[i], argv[i]);
    }
    evalExpression(script, env, proc->scriptfunc.body, &newval);
    env = freeEnvironment(env);

    script->traceindent--;
    for(i = 0; i < script->traceindent; i++) 
      printStrn(script, script->outport, ISIS_TRACE_FILL, ISIS_TRACE_FILL_LEN);
    printValue(script, script->outport, newval);
    printStrn(script, script->outport, "\n", 1);
    break;

  default:
    fprintf(stderr, "* Invalid procedure kind %d in applyProcedure.\n",
	    proc->kind);
    newval = NullValue;
  }
  
  return newval;
}


