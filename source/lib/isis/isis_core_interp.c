/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* This file contains high-level system operation functions */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "isis.h"

static int initialized = 0;
static int default_quiet = 0;
static int default_prompt = 0;
static int default_printtokens = 0;
static int default_printexps = 0;
static int default_debug = 0;
static char *command_line_filename = NULL;
static int command_line_args_start = -1;

static char **string_table;
static int string_table_pos;

/* exported globals: */

Expression *NullExpression;
Value *NullValue;
int isis_sizeof[CTYPE_COUNT];
Value *port_type_FILE_val = NULL;
Value *prim_close_FILE_val = NULL;
Value *prim_read_FILE_val = NULL;
Value *prim_readline_FILE_val = NULL;
Value *prim_write_FILE_val = NULL;
Value *prim_tell_FILE_val = NULL;
Value *prim_seek_FILE_val = NULL;
Value *prim_read_ready_FILE_val = NULL;
Value *prim_write_ready_FILE_val = NULL;
Value *prim_configure_FILE_val = NULL;

isisMutexDef(vnameMutex);
isisMutexDef(envMutex);
isisMutexDef(bindMutex);
isisMutexDef(procMutex);
isisMutexDef(expMutex);
isisMutexDef(valMutex);
isisMutexDef(memMutex);
isisMutexDef(strMutex);

/*-------------------------------------------------------------*/
void parse_command_line_options(int argc, char *argv[])
{
  int i = 1, stream = 0;
  int len;

  if(argc > 1) {
    if(argv[1][0] == '-') {  /* get command line options */
      len = strlen(argv[1]);
      for(i = 1; i < len; i++) {
        switch(argv[1][i]) {
        case 'q': default_quiet = 1; break;
        case 't': default_printtokens = 1; break;
        case 'e': default_printexps = 1; break;
        case 'd': default_debug = 1; break;
        case 's': stream = 1; break;
        }
      }
      if(stream) command_line_args_start = 2;
      else if(argc > 2) { 
        command_line_filename = argv[2];
        command_line_args_start = 3;
      }
      return;
    }
    else {	/* no command line options */
      command_line_filename = argv[1];
      command_line_args_start = 2;
    }
  }
}  

/*-------------------------------------------------------------*/
void initialize_interpreter()
{
  Procedure *proc;
  int i;

  if(!initialized) {
    
    /* initialize the mutexes */

    isisMutexInit(vnameMutex);
    isisMutexInit(envMutex);
    isisMutexInit(bindMutex);
    isisMutexInit(procMutex);
    isisMutexInit(expMutex);
    isisMutexInit(valMutex);
    isisMutexInit(memMutex);
    isisMutexInit(strMutex);

    /* initialize the memory manager */

    initMemory();    

    /* initialize the memory element size array */

    isis_sizeof[CTYPE_UNKNOWN] = 0;
    isis_sizeof[CTYPE_CHAR] = sizeof(char);
    isis_sizeof[CTYPE_UCHAR] = sizeof(unsigned char);
    isis_sizeof[CTYPE_SHORT] = sizeof(short);
    isis_sizeof[CTYPE_USHORT] = sizeof(unsigned short);
    isis_sizeof[CTYPE_INT] = sizeof(int);
    isis_sizeof[CTYPE_UINT] = sizeof(unsigned int);
    isis_sizeof[CTYPE_LONG] = sizeof(long);
    isis_sizeof[CTYPE_ULONG] = sizeof(unsigned long);
    isis_sizeof[CTYPE_FLOAT] = sizeof(float);
    isis_sizeof[CTYPE_DOUBLE] = sizeof(double);
    isis_sizeof[CTYPE_POINTER] = sizeof(void *);
#ifdef _LONGLONG
    isis_sizeof[CTYPE_LONGLONG] = sizeof(long long);
    isis_sizeof[CTYPE_ULONGLONG] = sizeof(unsigned long long);
#endif

    /* initialize internal string table */
  
    string_table = (char **) newPtrArray(STRING_TABLE_SIZE);
    for(i = 0; i < STRING_TABLE_SIZE; i++) string_table[i] = NULL;
    string_table_pos = 1;  /* reserve position 0 for NULL */
    string_table[0] = "[unknown]";
     
    /* create NullExpression and NullValue */

    NullExpression = newExpression(E_NULL);
    NullExpression->nullexp.line = 0;
    NullExpression->nullexp.colsnum = 0;

    NullValue = newValue(IsisNull);

    /* create a string value for FILE port type */
    
    port_type_FILE_val = newCharList(4, "FILE");
    
    /* create procedure values for standard FILE port routines */

    proc = newCfunc(prim_close_FILE, "close-FILE", 0);
    prim_close_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_read_FILE, "read-FILE", 0);
    prim_read_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_readline_FILE, "readline-FILE", 0);
    prim_readline_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_write_FILE, "write-FILE", 0);
    prim_write_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_tell_FILE, "tell-FILE", 0);
    prim_tell_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_seek_FILE, "seek-FILE", 0);
    prim_seek_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_read_ready_FILE, "read-ready-FILE", 0);
    prim_read_ready_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_write_ready_FILE, "write-ready-FILE", 0);
    prim_write_ready_FILE_val = newProc(proc);
    freeProcedure(proc);

    proc = newCfunc(prim_configure_FILE, "configure-FILE", 0);
    prim_configure_FILE_val = newProc(proc);
    freeProcedure(proc);

    /* mark the interpreter as initialized */

    initialized = 1;
  }
}

/*-------------------------------------------------------------*/
Script *create_script(char *name, FILE *infile, FILE *outfile)
{
  /* Create new script with input from the given FILE */

  Script *script;

  script = mem_alloc_item(Script_mem);

  script->name = createStr(name);

  script->inport = new_port_FILE("Standard Input", infile, 1, 0);
  script->outport = new_port_FILE("Standard Output", outfile, 0, 1);

  script->debug = default_debug;
  script->printtokens = default_printtokens;
  script->printexps = default_printexps;
  script->quiet = default_quiet;
  script->prompt = default_prompt;
  script->traceindent = 0;

  /* Set up initial environment */
  
  script->env = hashEnvironment(NULL, TOP_LEVEL_HASH_SIZE);
  
  /* Bind the language primitives (C functions) */
  
  bind_primitives(script);
  
  return script;
}

/*-------------------------------------------------------------*/
void close_script(Script *script)
{
  /* Free all script structures. The caller must close the input and
     output files itself. */
  
  if(script == NULL) return;

  freeStr(script->name);

  freeValue(script->inport);
  freeValue(script->outport);

  freeEnvironment(script->env);

  mem_freeItem(Script_mem, script);  
}

/*-------------------------------------------------------------*/
int registerString(char *string)
{
  int i, pos;

  if(string == NULL) return 0;   /* position 0 reserved for NULL */
  
  isisMutexLock(strMutex);
  
  for(i = 0; i < STRING_TABLE_SIZE; i++) 
    if(string_table[(string_table_pos + i) % STRING_TABLE_SIZE] == NULL) break;
  
  if(i == STRING_TABLE_SIZE) {
    fprintf(stderr, "* registerString: no more space in string table.\n");
    isisMutexUnlock(strMutex);
    return 0;
  }

  pos = string_table_pos = (string_table_pos + i) % STRING_TABLE_SIZE;
  string_table[pos] = createStr(string);
  
  isisMutexUnlock(strMutex);
  
  return pos;
}

/*-------------------------------------------------------------*/
char *lookupString(int snum)
{
  return string_table[snum];
}

/*-------------------------------------------------------------*/
void releaseString(int snum)
{
  /* assumes same string won't be released more than once */
  
  if(snum == 0) return;  /* reserved, don't release */
  
  isisMutexLock(strMutex);
  
  freeStr(string_table[snum]);
  string_table[snum] = NULL;
  string_table_pos = snum;

  isisMutexUnlock(strMutex);
}

/*-------------------------------------------------------------*/
Parse *createParse(Script *script, int snum, Value *port,
		   int maxlen, int maxtlen)
{
  Parse *parse;
  
  parse = mem_alloc_item(Parse_mem);

  parse->script = script;
  parse->port = referValue(port);
  parse->s = newStr(maxlen);
  parse->ts = newStr(maxtlen);
  parse->s[0] = 0;
  parse->snum = snum;
  parse->line = 1;
  parse->col = 0;
  parse->maxlen = maxlen;
  parse->pos = 0;
  parse->maxtlen = maxtlen;
  parse->tpos = 0;
  
  return parse;
}

/*-------------------------------------------------------------*/
void freeParse(Parse *parse)
{
  if(parse == NULL) return;

  if(parse->port) freeValue(parse->port);
  if(parse->s) freeStr(parse->s);
  if(parse->ts) freeStr(parse->ts);

  mem_freeItem(Parse_mem, parse);  
}

/*-------------------------------------------------------------*/
int interpret_command_line_files(Script *script, int argc, char *argv[])
{
  int i, j, len, items;
  Value *comline;
  
  i = command_line_args_start;
  if(i < 0) i = argc;
  
  len = argc - i;
  comline = newList(len, NULL);
  for(j = 0; j < len; j++, i++) 
    Listitem(comline, j) = newString(strlen(argv[i]), argv[i]);
  
  bindValue(script, ISIS_COMMAND_LINE_ARGS_VARNAME, comline);
  freeValue(comline);
  
  if(command_line_filename) {
    items = interpret_filename(script, command_line_filename);
  }
  else {   
    /* call 'interactive' directly to start an interactive shell */
    /* done this way in case 'interactive' has been redefined */
    /* used to be just: */
    /* items = interpret_file(script, ISIS_CONSOLE_NAME, stdin, 0, 1); */

    Binding *binding;
    Value *procval, *retval;
    
    binding = searchTop("interactive", script->env);
    isisMutexLock(bindMutex);
    procval = referValue(binding->val);    
    isisMutexUnlock(bindMutex);    

    retval = callProcedure(script, getProc(procval), 0);
    if(isInt(retval)) items = getInt(retval);
    else items = 0;

    freeValue(retval);
    freeValue(procval);
  }

  return items;
}

/*-------------------------------------------------------------*/
int interpret_filename(Script *script, char *filename)
{
  FILE *fp;
  int items = 0;
  
  if((fp = fopen(filename, "r")) != NULL) {
    items = interpret_file(script, filename, fp, 1, 0);
    fclose(fp);
  }
  else fprintf(stderr, "* Nonexistent or inaccessible file: %s\n", filename);
  
  return items;
}

/*-------------------------------------------------------------*/
int interpret_file(Script *script, char *name, FILE *fp, int quiet, int prompt)
{
  int items;
  Value *port;
  
  port = new_port_FILE(name, fp, 1, 0);
  items = interpret_port(script, port, quiet, prompt);
  freeValue(port);
  
  return items;
}
  
/*-------------------------------------------------------------*/
int interpret_port(Script *script, Value *port, int quiet, int prompt)
{
  int items, len, oldquiet, oldprompt;
  Parse *parse;
  char *name;

  if(!isList(port)) return 0;
  
  len = Listsize(Listitem(port, PORT_NAME)) + 1;
  name = mem_alloc(len);
  getString(Listitem(port, PORT_NAME), name, len);

  parse = createParse(script, registerString(name), port, 
		      LINE_BUFFER_SIZE, TOKEN_BUFFER_SIZE);
  
  oldquiet = script->quiet;
  oldprompt = script->prompt;
  script->quiet = quiet;
  script->prompt = prompt;

  items = read_eval_print_entire(script, parse);

  script->quiet = oldquiet;
  script->prompt = oldprompt;
  
  freeParse(parse);
  free(name);
  
  return items;
}
  
/*-------------------------------------------------------------*/
int compile_port(Script *script, Value *inport, Value *outport)
{
  Parse *parse;
  Token *item;
  int items = 0;
  
  if(!isList(inport) || !isList(outport)) return 0;

  parse = createParse(script, 0, inport, 
		      LINE_BUFFER_SIZE, TOKEN_BUFFER_SIZE);
  
  while( readItem(parse, &item) > 1 ) {
    encodeItem(script, outport, item);
    freeItem(item);
    items++;
  }  
  freeItem(item);

  freeParse(parse);
  
  return items;
}
  
/*-------------------------------------------------------------*/
int interpret_compiled_port(Script *script, Value *port)
{
  int items;
  
  if(!isList(port)) return 0;
  
  items = process_entire_compiled(script, port);
  
  return items;
}
  
/*-------------------------------------------------------------*/
int process_entire_compiled(Script *script, Value *port)
{
  /* Reads and evaluates all the items in compiled form.  No values
     are printed or returned.  Returns number of items processed. */
  
  int items = 0;
  Value *val;
  Token *item;

  while( decodeItem(script, port, &item) > 1) {
    if(script->printtokens) {
      printStrn(script, script->outport, ISIS_SEPARATOR, ISIS_SEPARATOR_LEN);
      printItem(script, script->outport, item);
      printStrn(script, script->outport, ISIS_SEPARATOR, ISIS_SEPARATOR_LEN);
    }
    process_this_item(script, item, &val);
    freeItem(item);
    freeValue(val);    
    items++;
  }
  freeItem(item);
  
  return items;
}

/*-------------------------------------------------------------*/
int read_eval_print_entire(Script *script, Parse *parse)
{
  /* Process the entire script, reading, evaluating, and printing
     results, until an EOF is reached on the input file.  Return
     number of items processed. */
  
  int items = 0;
  
  while( read_eval_print_one_item(script, parse) ) items++;

  return items;
}

/*-------------------------------------------------------------*/
int read_eval_print_one_item(Script *script, Parse *parse)
{
  /* Read, evaluate, and print one item from script.  Returns number
     of items processed (0 means end of file or error) */
  
  Value *val;

  if(process_one_item(script, parse, &val)) {
    if(!isNull(script->outport) && !script->quiet) {
      printValue(script, script->outport, val);
      printStrn(script, script->outport, "\n", 1);
    }
    freeValue(val);
    return 1;
  }
  
  return 0;  /* failure */
}

/*-------------------------------------------------------------*/
int process_one_item(Script *script, Parse *parse, Value **value)
{
  /* Read and evaluate one item from the specified script, the pointer
     to the value is returned through the 'value' argument.  Returns
     number of items processed (0 means end of file) */

  Token *item;

  if(!script->quiet && script->prompt) 
    printStrn(script, script->outport, ISIS_PROMPT, ISIS_PROMPT_LEN);
  if(readItem(parse, &item) > 1) {
    if(script->printtokens) {
      printStrn(script, script->outport, ISIS_SEPARATOR, ISIS_SEPARATOR_LEN);
      printItem(script, script->outport, item);
      printStrn(script, script->outport, ISIS_SEPARATOR, ISIS_SEPARATOR_LEN);
    }
    process_this_item(script, item, value);
    freeItem(item);
    return 1;  /* not end of file */
  }
  else freeItem(item);
  
  *value = NullValue;
  return 0;  /* end of file */
}

/*-------------------------------------------------------------*/
void process_this_item(Script *script, Token *item, Value **value)
{
  /* Form an expression from and evaluate the given item, the pointer
     to the value is returned through the 'value' argument. */
  
  Expression *exp;
  
  if(item != NULL && formExpression(script, script->env, &item, &exp)) {
    if(script->printexps) {
      printStrn(script, script->outport, ISIS_SEPARATOR, ISIS_SEPARATOR_LEN);
      printExpression(script, script->outport, exp, 0);
      printStrn(script, script->outport, ISIS_SEPARATOR, ISIS_SEPARATOR_LEN);
    }
    evalExpression(script, script->env, exp, value);
    freeExpression(exp);
    return;
  }
  
  *value = NullValue;
}
