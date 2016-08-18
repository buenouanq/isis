/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <isis.h>
#include "isis_unix_private.h"

#ifdef ISIS_MULTITHREAD

typedef struct ThreadData {
  Procedure *proc;
  Script *script;
  Value **argv;
  int argc;
  Value *val;
} ThreadData;

/*----------------------------------------------------------------------*/
static void *isis_thread_start(void *arg)
{
  ThreadData *tdata;

  tdata = arg;
  tdata->val = applyProcedure(tdata->script, tdata->proc, tdata->argc, tdata->argv);
  return (void *) tdata;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_thread_spawn(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  ThreadData *tdata;
  Script *newscript;
  int newargc, i, status;
  Value **newargv;
  pthread_t tid;

  checkCountLeast(1, "procedure and arguments");
  checkProc(0, NULL);
  
  /* allocate new Script structure */
  
  newscript = mem_alloc_item(Script_mem);
  
  newscript->name = NULL;
  newscript->inport = referValue(script->inport);
  newscript->outport = referValue(script->outport);
  newscript->env = referEnvironment(script->env);
  newscript->debug = script->debug;
  newscript->printtokens = 0;
  newscript->printexps = 0;
  newscript->quiet = 1;
  newscript->prompt = 1;
  newscript->traceindent = 0;

  /* allocate argument pointer array */

  newargc = argc - 1;
  newargv = (Value **) newPtrArray(newargc);
  for(i = 0; i < newargc; i++) newargv[i] = referValue(argv[i + 1]);

  /* set up thread data structure */

  tdata = (void *) allocBuffer(sizeof(ThreadData));
  tdata->script = newscript;
  tdata->argc = argc - 1;
  tdata->argv = newargv;
  tdata->proc = referProcedure(getProc(argv[0]));
  tdata->val = NullValue;

  /* start the thread */

  status = pthread_create(&tid, NULL, isis_thread_start, tdata);
  if(status == 0) return newAddr(tid);

  /* thread failed to start */

  freeProcedure(tdata->proc);
  for(i = 0; i < tdata->argc; i++) freeValue(tdata->argv[i]);
  freePtrArray(tdata->argv);
  freeEnvironment(tdata->script->env);  
  freeValue(tdata->script->inport);
  freeValue(tdata->script->outport);
  mem_freeItem(Script_mem, tdata->script);
  free(tdata);

  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_thread_join(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  ThreadData *tdata;
  Value *val;
  pthread_t tid;
  int status, i;

  checkCount(1, "thread id");
  checkAddr(0, "thread id");

  tid = getAddr(argv[0]);

  status = pthread_join(tid, (void **) &tdata);
  if(status != 0) return NullValue;
  
  freeProcedure(tdata->proc);
  for(i = 0; i < tdata->argc; i++) freeValue(tdata->argv[i]);
  freePtrArray(tdata->argv);
  freeEnvironment(tdata->script->env);  
  freeValue(tdata->script->inport);
  freeValue(tdata->script->outport);
  mem_freeItem(Script_mem, tdata->script);
  val = tdata->val;
  free(tdata);

  return val;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_thread_self(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  checkCount(0, NULL);

  return newAddr( pthread_self() );
}


#endif /* ISIS_MULTITHREAD */

