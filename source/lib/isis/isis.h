#ifndef __ISIS_H__
#define __ISIS_H__

/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* isis.h
   Definitions and structures used in the Isis script interpreter */

/*-------------------------------------------------------------------------*/
/* Version number */

#define ISIS_VERSION 9
#define ISIS_VERSION_TEXT "9"

/*-------------------------------------------------------------------------*/
/* Global type information */

typedef double real;
typedef int bool;
typedef void *addr;

/*-------------------------------------------------------------------------*/
/* Global macros */

#define getnext(x) if((x)->next != NULL) x = (x)->next;
#define minimum(a, b) (((a) < (b)) ? (a) : (b))
#define maximum(a, b) (((a) > (b)) ? (a) : (b))

/*-------------------------------------------------------------------------*/
/* Threading macros */

#ifdef ISIS_MULTITHREAD

#ifdef ISIS_THREADTYPE_PTHREAD   /* using Unix pthreads */

/* on Unix systems, include pthread.h, 
   and include the -pthread option when compiling,
   and link with -lpthread -lexc */

#include <pthread.h>
#define isisMutexDef(m)        pthread_mutex_t m;
#define isisMutexExternDef(m)  extern pthread_mutex_t m;
#define isisMutexInit(m)       if(pthread_mutex_init(&(m), NULL) != 0) { \
                                 fprintf(stderr, "* Error initializing mutex.\n"); \
				 exit(1); }
#define isisMutexDestroy(m)    if(pthread_mutex_destroy(&(m)) != 0) { \
                                 fprintf(stderr, "* Error destroying mutex.\n"); \
				 exit(1); }
#define isisMutexLock(m)       pthread_mutex_lock(&(m))
#define isisMutexUnlock(m)     pthread_mutex_unlock(&(m))
#endif

/* add more sections here for other threading systems */

#else  /* ISIS_MULTITHREAD is not defined */

#define isisMutexDef(m)
#define isisMutexExternDef(m)
#define isisMutexInit(m)
#define isisMutexDestroy(m)
#define isisMutexLock(m)
#define isisMutexUnlock(m)

#endif

/*-------------------------------------------------------------------------*/
/* Global size definitions */

#define LINE_BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 1024
#define STRING_TABLE_SIZE 1024     /* stores file names for error messages */
#define TOP_LEVEL_HASH_SIZE 1024

/* String sizes include null ending character */

#define STRING_LOW_LEN 5
#define STRING_MED_LEN 30
#define STRING_HIGH_LEN 275

#define PTR_LOW_LEN 5
#define PTR_MED_LEN 30
#define PTR_HIGH_LEN 275

/*-------------------------------------------------------------------------*/
/* Parsing definitions */

#define COMMENT_CHAR '#'
#define LONG_COMMENT_OPEN_CHAR '{'
#define LONG_COMMENT_CLOSE_CHAR '}'
#define STRING_CHAR '"'
#define CHAR_CHAR '\''
#define OPEN_CHAR '('
#define CLOSE_CHAR ')'
#define LIST_OPEN_CHAR '['
#define LIST_CLOSE_CHAR ']'
#define NEWLINE_CHAR '\n'
#define ESCAPE_CHAR '\\'

#define SET_STR "set"
#define BOUND_STR "bound?"
#define LOCAL_STR "local"
#define PROC_STR "proc"
#define BEGIN_STR "begin"
#define IF_STR "if"
#define ELSE_STR "else"
#define COND_STR "cond"
#define SWITCH_STR "switch"
#define WHILE_STR "while"
#define FALSE_STR "False"
#define TRUE_STR "True"
#define NULLVAL_STR "Null"
#define AND_STR "and"
#define OR_STR "or"
#define NAND_STR "nand"
#define NOR_STR "nor"

/*-------------------------------------------------------------------------*/
/* Other definitions */

#define True 1
#define False 0
#define ISIS_COMMAND_LINE_ARGS_VARNAME "command-line-args"
#define ISIS_CONSOLE_NAME "Console"
#define ISIS_PROMPT "-> "
#define ISIS_PROMPT_LEN 3
#define ISIS_SEPARATOR "==================================================\n"
#define ISIS_SEPARATOR_LEN 51
#define ISIS_50SPACE "                                                  "
#define ISIS_TRACE_FILL "|  "
#define ISIS_TRACE_FILL_LEN 3
#define ISIS_BOOTSTRAP "/scripts/bootstrap.isis"

/*-------------------------------------------------------------------------*/
/* Global enumerations */

/* token kinds */

enum { TOK_UNUSED, TOK_STRING, TOK_ATOM, TOK_INT, 
       TOK_REAL, TOK_OPEN_PAREN, TOK_OPEN_LIST, TOK_CLOSE,
       TOK_CHAR, TOK_ADDR, TOK_BINARY, TOK_END };

/* type kinds */

enum { IsisInvalid, IsisNull, 
       IsisInt, IsisReal, IsisBool, IsisChar, 
       IsisAddr, IsisProc, IsisList, IsisUnknown };

/* expression kinds */

enum { E_INVALID, E_NULL, E_CONST, E_VAR, E_PROC, E_APP, 
       E_LIST, E_IF, E_COND, E_SWITCH, E_BEGIN, E_LOGIC,
       E_SET, E_LOCAL, E_WHILE, E_VAROP };

/* procedure kinds */

#define P_INVALID      0
#define P_SCRIPT       1
#define P_CFUNC        2
#define P_SCRIPT_TRACE 5  /* 1 + 4 */
#define P_CFUNC_TRACE  6  /* 2 + 4 */

#define P_TYPE_MASK  3
#define P_TRACE_MASK 4

/* logic operator kinds */

enum { L_INVALID, L_AND, L_OR, L_NAND, L_NOR };

/* variable operator kinds */

enum { V_INVALID, V_BOUND };

/* environment ref kinds */

enum { R_INVALID, R_ARRAY, R_HASH };

/* C type kinds */

enum { CTYPE_UNKNOWN, CTYPE_CHAR, CTYPE_UCHAR, 
       CTYPE_SHORT, CTYPE_USHORT, CTYPE_INT,
       CTYPE_UINT, CTYPE_LONG, CTYPE_ULONG,
       CTYPE_FLOAT, CTYPE_DOUBLE, CTYPE_POINTER, 
#ifdef _LONGLONG
       CTYPE_LONGLONG, CTYPE_ULONGLONG,
#endif
       CTYPE_COUNT };

/* string and pointer array kinds */

enum { STRING_EXTERN, STRING_LOW, STRING_MED, STRING_HIGH, 
       PTR_EXTERN, PTR_LOW, PTR_MED, PTR_HIGH };

/* port list items */

enum { PORT_NAME, PORT_TYPE, PORT_INTERNAL_ID, 
       PORT_INFLAG, PORT_OUTFLAG, PORT_CLOSE_PROC, 
       PORT_READ_PROC, PORT_READLINE_PROC, PORT_WRITE_PROC,
       PORT_TELL_PROC, PORT_SEEK_PROC, 
       PORT_READ_READY_PROC, PORT_WRITE_READY_PROC, PORT_CONFIGURE_PROC,
       PORT_LIST_LENGTH };

/*-------------------------------------------------------------------------*/
/* Structure definitions */

typedef struct Memory {      /* SHARED ACCESS - all fields read/write */
  char *name;                /* LOCK NEEDED to query or modify any instance */
  struct Block *blocks;
  void **addrs;
  struct Memory *next;
  struct Memory *prev;
  int size;
  int quantity;
  int increment;
  int num_used;
  int tot_used;
  int max_used;
} Memory;

typedef struct Block {
  void *memptr;
  struct Block *next;
} Block;

typedef struct Token {
  char *str;                 /* the exact character representation */
  struct Token *next;        /* next token in line */
  int colsnum;               /* file num (low 16 bits) and column (high 16 bits) */
  int line;                  /* line token originated from */
  int kind;                  /* type of token as defined above */
} Token;

typedef struct Parse {
  struct Script *script;     /* script to eval read functions within */
  struct Value *port;        /* port from which to read script items */
  char *s;                   /* the read string */
  char *ts;                  /* the token string */
  int snum;                  /* string table ref of name of file */
  int line;                  /* current line number */
  int col;                   /* current column number */
  int maxlen;                /* maximum length of read string */
  int pos;                   /* current position in read string */
  int maxtlen;               /* maximum length of token string */
  int tpos;                  /* current position in token string */
} Parse;

typedef struct Script {
  char *name;                /* name of script for identification */
  struct Value *inport;      /* input port for this program */
  struct Value *outport;     /* output port for printing results */
  struct Environment *env;   /* Top-level environment of the script */
  int debug;                 /* Debug mode */
  int printtokens;           /* Print token lists? */
  int printexps;             /* Print expressions? */
  int quiet;                 /* Quiet mode */
  int traceindent;           /* Indent for tracing prints */
  int prompt;                /* Prompt turned on? */
} Script;

typedef struct Value {       /* SHARED ACCESS - read-only, except for ref_count */
  int type;                  /* type of the value */
  int homogtype;             /* type of items if it is a homogeneous list */
  int ref_count;             /* reference count - LOCK NEEDED to modify */
  int listsize;              /* size of list if this value is a list */
  union {                    /* union for specific kinds of values */
    int intval;
    real realval;
    char charval;
    int boolval;
    union Procedure *procval;
    void *addressval;
    struct Value **list;
  } u;
} Value;

typedef union Expression {   /* SHARED ACCESS - read-only, except for ref_count and binding */
  int kind;                  /* Must be first in all structs below */
  
  struct {                   /* These must occur in order in each exp struct */
    int kind;                /* expression kind */
    int ref_count;           /* reference count - LOCK NEEDED to modify */
    int line;                /* line num of tokens where expression originated */
    int colsnum;             /* file num (low 16 bits) and column (high 16 bits) */
  } nullexp;
  
  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    struct Value *val;
  } constexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    struct Varname *vname;
    struct Binding *binding; /* top level binding ref - may change once if NULL */
    int fnum;
    int bnum;
  } varexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    struct Varname **formals;
    union Expression *bodyexp;
    int num_formals;
    int var_num_args;
  } procexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;
    
    struct Varname **vnames;
    union Expression *bodyexp;
    int num_vars;
  } localexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression **exps;
    int num_exps;
  } appexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression **exps;
    int num_exps;
  } listexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression *ifpart;
    union Expression *thenpart;
    union Expression *elsepart;
  } ifexp;
  
  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression *cond;
    union Expression *body;
  } whileexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression **conds;
    union Expression **vals;
    int num_conds;
  } condexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression *switchexp;
    union Expression **cases;
    union Expression **vals;
    int num_cases;
    int last_default;
  } switchexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression **exps;
    int num_exps;
  } beginexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    union Expression **exps;
    int num_exps;
    int logic_kind;
  } logicexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    struct Varname *vname;
    union Expression *exp;
    struct Binding *binding; /* top level binding ref - may change once if NULL */
    int fnum;
    int bnum;
  } setexp;

  struct {
    int kind;
    int ref_count;
    int line;
    int colsnum;

    struct Varname *vname;
    struct Binding *binding; /* top level binding ref - may change once if NULL */
    int fnum;
    int bnum;
    int op_kind;
  } varopexp;

} Expression;

typedef struct ExpList {
  struct ExpNode *head;
  struct ExpNode *tail;
  int num_items;
} ExpList;

typedef struct ExpNode {
  void *item;
  struct ExpNode *next;
} ExpNode;

typedef struct Varname {      /* SHARED ACCESS - read-only, except for ref_count */
  int ref_count;              /* reference count - LOCK NEEDED to modify */
  char *str;
} Varname;

typedef struct Binding {      /* SHARED ACCESS - read-only, except for val */
  struct Varname *vname;      /* identifier for this binding */
  struct Value *val;          /* value - LOCK NEEDED to MODIFY OR QUERY */
  struct Binding *next;       /* next binding in the list or hash list */
} Binding;

typedef struct Environment {  /* SHARED ACCESS - bindings and ref_count are read-write */
  void *bindings;             /* depends on ref_type - LOCK NEEDED to modify */
  struct Environment *parent; /* The parent environment */
  int ref_count;              /* reference count - LOCK NEEDED to modify */
  int size;                   /* number of bindings, or hash table size */
  int ref_type;               /* type of env: list, array, hash */
} Environment;

typedef union Procedure {     /* SHARED ACCESS - ref_count and tracename are read-write */
  int kind;                   /* kind of procedure -- must be first */
  struct {
    int kind;
    int ref_count;            /* reference count -- LOCK NEEDED to modify */
    char *tracename;          /* name for tracing -- LOCK NEEDED to modify */
  } genfunc;
  struct {
    int kind;
    int ref_count;
    char *tracename;
    struct Varname **formals; /* The formal parameter identifiers */
    struct Environment *env;  /* The environment in which it was created */
    union Expression *body;   /* The expression for the body */
    int num_formals;          /* The number of formals */
    int var_num_args;         /* Variable number of arguments? */
  } scriptfunc;
  struct {
    int kind;
    int ref_count;
    char *tracename;
    char *name;               /* Name of the C function */
    struct Value *(*func)();  /* The actual C function */
    int call_data;            /* This is sent as the call_data argument */
  } cfunc;
} Procedure;

/*-------------------------------------------------------------------------*/
/* Isis C interface function prototype */

typedef Value *(*IsisCfunc)(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);

/*-------------------------------------------------------------------------*/
/* isis_core_parse.c globals */

Token *newToken(void);
char skipComment(Parse *parse);
int readItem(Parse *parse, Token **token);
void printItem(Script *script, Value *port, Token *token);
void freeItem(Token *token);
int allowedSymbol(char c);
char controlChar(char c);

/*-------------------------------------------------------------------------*/
/* isis_core_interp.c globals */

extern Expression *NullExpression;    /* Use this instead of NULL */
extern Value *NullValue;              /* Use this instead of NULL */
extern int isis_sizeof[];
extern Value *port_type_FILE_val;
extern Value *prim_close_FILE_val;
extern Value *prim_read_FILE_val;
extern Value *prim_readline_FILE_val;
extern Value *prim_write_FILE_val;
extern Value *prim_tell_FILE_val;
extern Value *prim_seek_FILE_val;
extern Value *prim_read_ready_FILE_val;
extern Value *prim_write_ready_FILE_val;
extern Value *prim_configure_FILE_val;

isisMutexExternDef(vnameMutex);
isisMutexExternDef(envMutex);
isisMutexExternDef(bindMutex);
isisMutexExternDef(procMutex);
isisMutexExternDef(expMutex);
isisMutexExternDef(valMutex);
isisMutexExternDef(memMutex);
isisMutexExternDef(strMutex);

void parse_command_line_options(int argc, char *argv[]);
void initialize_interpreter();
Script *create_script(char *name, FILE *infile, FILE *outfile);
void close_script(Script *script);
int registerString(char *string);
char *lookupString(int snum);
void releaseString(int snum);
Parse *createParse(Script *script, int snum, Value *port, int maxlen, int maxtlen);
void freeParse(Parse *parse);
int interpret_command_line_files(Script *script, int argc, char *argv[]);
int interpret_filename(Script *script, char *filename);
int interpret_file(Script *script, char *name, FILE *fp, int quiet, int prompt);
int interpret_port(Script *script, Value *port, int quiet, int prompt);
int compile_port(Script *script, Value *inport, Value *outport);
int interpret_compiled_port(Script *script, Value *port);
int process_entire_compiled(Script *script, Value *port);
int read_eval_print_entire(Script *script, Parse *parse);
int read_eval_print_one_item(Script *script, Parse *parse);
int process_one_item(Script *script, Parse *parse, Value **value);
void process_this_item(Script *script, Token *item, Value **value);

/*-------------------------------------------------------------------------*/
/* isis_core_exp.c global functions */

extern void (*printExpFuncs[])(Script *script, Value *, Expression *, int);
extern void (*evalExpFuncs[])(Script *, Environment *, Expression *, Value **);
extern void (*freeExpFuncs[])(Expression *);

#define printExpression(script, port, exp, indent) (printExpFuncs[(exp)->kind])(script, port, exp, indent)

#define evalExpression(script, env, exp, val) (evalExpFuncs[(exp)->kind])(script, env, exp, val)

#define freeExpression(exp) (freeExpFuncs[(exp)->kind])(exp)

int formExpression(Script *script, Environment *env, Token **token, Expression **exp);

void printInvalidExpression(Script *script, Value *port, Expression *exp, int indent);
void printNullExpression(Script *script, Value *port, Expression *exp, int indent);
void printConstExpression(Script *script, Value *port, Expression *exp, int indent);
void printVaropExpression(Script *script, Value *port, Expression *exp, int indent);
void printVarExpression(Script *script, Value *port, Expression *exp, int indent);
void printProcExpression(Script *script, Value *port, Expression *exp, int indent);
void printLocalExpression(Script *script, Value *port, Expression *exp, int indent);
void printAppExpression(Script *script, Value *port, Expression *exp, int indent);
void printListExpression(Script *script, Value *port, Expression *exp, int indent);
void printIfExpression(Script *script, Value *port, Expression *exp, int indent);
void printWhileExpression(Script *script, Value *port, Expression *exp, int indent);
void printCondExpression(Script *script, Value *port, Expression *exp, int indent);
void printSwitchExpression(Script *script, Value *port, Expression *exp, int indent);
void printBeginExpression(Script *script, Value *port, Expression *exp, int indent);
void printLogicExpression(Script *script, Value *port, Expression *exp, int indent);
void printSetExpression(Script *script, Value *port, Expression *exp, int indent);

void evalInvalidExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalNullExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalConstExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalVaropExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalVarExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalProcExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalLocalExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalAppExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalListExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalIfExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalWhileExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalCondExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalSwitchExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalBeginExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalLogicExpression(Script *script, Environment *env, Expression *exp, Value **val);
void evalSetExpression(Script *script, Environment *env, Expression *exp, Value **val);

void freeInvalidExpression(Expression *exp);
void freeNullExpression(Expression *exp);
void freeConstExpression(Expression *exp);
void freeVaropExpression(Expression *exp);
void freeVarExpression(Expression *exp);
void freeProcExpression(Expression *exp);
void freeLocalExpression(Expression *exp);
void freeAppExpression(Expression *exp);
void freeListExpression(Expression *exp);
void freeIfExpression(Expression *exp);
void freeWhileExpression(Expression *exp);
void freeCondExpression(Expression *exp);
void freeSwitchExpression(Expression *exp);
void freeBeginExpression(Expression *exp);
void freeLogicExpression(Expression *exp);
void freeSetExpression(Expression *exp);

Expression *newExpression(int kind);
Expression *referExpression(Expression *exp);
int insertExpression(void *item, ExpList *explist);
ExpList *newExpList(void);
ExpNode *newExpNode(void *item);
void freeExpList(ExpList *explist);
void freeExpressionArray(Expression **exp_array, int num);

/*-------------------------------------------------------------------------*/
/* isis_core_environ.c global functions */

Varname *newVarname(char *str);
Varname *referVarname(Varname *varname);
Varname **copyVarnameArray(Varname **vnames, int num);
void freeVarname(Varname *varname);
void freeVarnameArray(Varname **vnames, int num);

Environment *arrayEnvironment(Environment *parent, int size);
Environment *hashEnvironment(Environment *parent, int tablesize);

Environment *referEnvironment(Environment *env);
Environment *freeEnvironment(Environment *env);
Binding *searchEnvironment(char *varname, Environment *env,
			   int *level, int *bnum);
Binding *searchTop(char *varname, Environment *env);
Binding *hashBind(Environment *env, Varname *vname, Value *val);
Binding *arrayBind(Environment *env, int bnum, Varname *vname, Value *val);
Binding *lookupBinding(Environment *env, int level, int bnum);

int hashStr(char *str, int len);

Procedure *newProcedure(int kind);
Procedure *referProcedure(Procedure *proc);
void freeProcedure(Procedure *proc);

Value *callProcedure(Script *script, Procedure *proc, int argc, ...);
Value *applyProcedure(Script *script, Procedure *proc, 
		      int argc, Value **argv);

/*-------------------------------------------------------------------------*/
/* isis_core_value.c global functions */

extern int (*measureValueFuncs[])(Value *);
extern char *(*sprintValueFuncs[])(char *, Value *);
extern void (*freeValueFuncs[])(Value *);
extern int (*equalValueFuncs[])(Value *, Value *);
extern int (*trueValueFuncs[])(Value *);

#define sprintValue(s, val) (sprintValueFuncs[(val)->type])(s, val)
#define measureValue(val) (measureValueFuncs[(val)->type])(val)
#define freeValue(val) (freeValueFuncs[(val)->type])(val)
#define equalValue(val1, val2) (equalValueFuncs[(val1)->type])(val1, val2)
#define trueValue(val) (trueValueFuncs[(val)->type])(val)

int significantDigits(int sigdig);

int measureInvalidValue(Value *val);
int measureNullValue(Value *val);
int measureIntValue(Value *val);
int measureRealValue(Value *val);
int measureBoolValue(Value *val);
int measureCharValue(Value *val);
int measureAddrValue(Value *val);
int measureProcValue(Value *val);
int measureListValue(Value *val);

char *sprintInvalidValue(char *str, Value *val);
char *sprintNullValue(char *str, Value *val);
char *sprintIntValue(char *str, Value *val);
char *sprintRealValue(char *str, Value *val);
char *sprintBoolValue(char *str, Value *val);
char *sprintCharValue(char *str, Value *val);
char *sprintAddrValue(char *str, Value *val);
char *sprintProcValue(char *str, Value *val);
char *sprintListValue(char *str, Value *val);

void freeInvalidValue(Value *val);
void freeNullValue(Value *val);
void freeIntValue(Value *val);
void freeRealValue(Value *val);
void freeBoolValue(Value *val);
void freeCharValue(Value *val);
void freeAddrValue(Value *val);
void freeProcValue(Value *val);
void freeListValue(Value *val);

int equalInvalidValue(Value *val1, Value *val2);
int equalNullValue(Value *val1, Value *val2);
int equalIntValue(Value *val1, Value *val2);
int equalRealValue(Value *val1, Value *val2);
int equalBoolValue(Value *val1, Value *val2);
int equalCharValue(Value *val1, Value *val2);
int equalAddrValue(Value *val1, Value *val2);
int equalProcValue(Value *val1, Value *val2);
int equalListValue(Value *val1, Value *val2);

int trueInvalidValue(Value *val);
int trueNullValue(Value *val);
int trueIntValue(Value *val);
int trueRealValue(Value *val);
int trueBoolValue(Value *val);
int trueCharValue(Value *val);
int trueAddrValue(Value *val);
int trueProcValue(Value *val);
int trueListValue(Value *val);

Value *newValue(int type);
int homogeneousList(Value *val, int type);
Value *referValue(Value *val);

/*-------------------------------------------------------------------------*/
/* isis_core_prim.c global functions */

void bind_primitives(Script *script);

/*-------------------------------------------------------------------------*/
/* isis_core_helpers.c global functions */

#define name_of(script) ((script)->name)
#define input_port_of(script) ((script)->inport)
#define output_port_of(script) ((script)->outport)
#define environment_of(script) ((script)->env)

#define debug_mode(script) ((script)->debug)
#define quiet_mode(script) ((script)->quiet)
#define prompt_mode(script) ((script)->prompt)
#define print_tokens(script) ((script)->printtokens)
#define print_exps(script) ((script)->printexps)

/* Type checking */

#define isNull(a) ((a)->type == IsisNull)
#define isInt(a) ((a)->type == IsisInt)
#define isReal(a) ((a)->type == IsisReal)
#define isChar(a) ((a)->type == IsisChar)
#define isBool(a) ((a)->type == IsisBool)
#define isProc(a) ((a)->type == IsisProc)
#define isAddr(a) ((a)->type == IsisAddr)
#define isList(a) ((a)->type == IsisList)

#define isNumber(a) ((a)->type == IsisInt || (a)->type == IsisReal)

#define isNullList(a) homogeneousList((a), IsisNull)
#define isIntList(a) homogeneousList((a), IsisInt)
#define isRealList(a) homogeneousList((a), IsisReal)
#define isString(a) homogeneousList((a), IsisChar)
#define isCharList(a) homogeneousList((a), IsisChar)
#define isBoolList(a) homogeneousList((a), IsisBool)
#define isProcList(a) homogeneousList((a), IsisProc)
#define isAddrList(a) homogeneousList((a), IsisAddr)
#define isListList(a) homogeneousList((a), IsisList)

int isNumberList(Value *val);

#define typeof(a) ((a)->type)

/* Extracting values */

#define getInt(a) ((a)->u.intval)
#define getReal(a) ((a)->u.realval)
#define getChar(a) ((a)->u.charval)
#define getBool(a) ((a)->u.boolval)
#define getProc(a) ((a)->u.procval)
#define getAddr(a) ((a)->u.addressval)
#define getList(a) ((a)->u.list)

#define getRealfromNumber(a) (((a)->type == IsisReal) ? (a)->u.realval : ((real) (a)->u.intval))
#define getIntfromNumber(a) (((a)->type == IsisReal) ? ((int) (a)->u.realval) : (a)->u.intval)

#define Listitem(a, b) ((a)->u.list[b])    /* does not create additional ref */
#define Listsize(a) ((a)->listsize)

int getIntList(Value *val, int *ptr, int maxlen);
int getRealList(Value *val, real *ptr, int maxlen);
int getBoolList(Value *val, int *ptr, int maxlen);
int getCharList(Value *val, char *ptr, int maxlen);
int getAddrList(Value *val, void **ptr, int maxlen);
int getProcList(Value *val, Procedure **ptr, int maxlen);

#define getString getCharList

int getIntListfromNumberList(Value *val, int *ptr, int maxlen);
int getRealListfromNumberList(Value *val, real *ptr, int maxlen);

/* Creating values */

Value *newInt(int val);
Value *newReal(real val);
Value *newBool(int val);
Value *newChar(char val);
Value *newAddr(void *val);
Value *newProc(Procedure *val);

Value *newList(int len, Value **vals);

Value *newIntList(int len, int *vals);
Value *newRealList(int len, real *vals);
Value *newBoolList(int len, int *vals);
Value *newCharList(int len, char *vals);
Value *newAddrList(int len, void **vals);
Value *newProcList(int len, Procedure **vals);

#define newString newCharList

Value *newListva(int len, ...);

Value *newIntListva(int len, ...);
Value *newRealListva(int len, ...);
Value *newBoolListva(int len, ...);
Value *newCharListva(int len, ...);
Value *newAddrListva(int len, ...);
Value *newProcListva(int len, ...);
     
Value *newBasicListva(int len, ...);

/* Binding values */

Binding *bindValue(Script *script, char *name, Value *val);

Binding *bindInt(Script *script, char *name, int val);
Binding *bindReal(Script *script, char *name, real val);
Binding *bindBool(Script *script, char *name, int val);
Binding *bindChar(Script *script, char *name, char val);
Binding *bindAddr(Script *script, char *name, void *val);
Binding *bindProc(Script *script, char *name, Procedure *val);

Binding *bindList(Script *script, char *name, int len, Value **vals);

Binding *bindIntList(Script *script, char *name, int len, int *vals);
Binding *bindRealList(Script *script, char *name, int len, real *vals);
Binding *bindBoolList(Script *script, char *name, int len, int *vals);
Binding *bindCharList(Script *script, char *name, int len, char *vals);
Binding *bindAddrList(Script *script, char *name, int len, void **vals);
Binding *bindProcList(Script *script, char *name, int len, Procedure **vals);

#define bindString bindCharList

Binding *bindListva(Script *script, char *name, int len, ...);

Binding *bindIntListva(Script *script, char *name, int len, ...);
Binding *bindRealListva(Script *script, char *name, int len, ...);
Binding *bindBoolListva(Script *script, char *name, int len, ...);
Binding *bindCharListva(Script *script, char *name, int len, ...);
Binding *bindAddrListva(Script *script, char *name, int len, ...);
Binding *bindProcListva(Script *script, char *name, int len, ...);
     
Binding *bindBasicListva(Script *script, char *name, int len, ...);

/* creating and binding C functions */

Procedure *newCfunc(Value *(*func)(), char *name, int call_data);

Binding *bindCfunc(Script *script, char *name, 
		   Value *(*func)(), int call_data);

/* Argument errors */

void errorCount(Script *script, char *proc_name, int n, char *s);
void errorCountLeast(Script *script, char *proc_name, int n, char *s);
void errorCountMost(Script *script, char *proc_name, int n, char *s);

void errorInt(Script *script, char *proc_name, int n, char *s);
void errorReal(Script *script, char *proc_name, int n, char *s);
void errorBool(Script *script, char *proc_name, int n, char *s);
void errorChar(Script *script, char *proc_name, int n, char *s);
void errorAddr(Script *script, char *proc_name, int n, char *s);
void errorProc(Script *script, char *proc_name, int n, char *s);
void errorList(Script *script, char *proc_name, int n, char *s);

void errorNumber(Script *script, char *proc_name, int n, char *s);

void errorIntList(Script *script, char *proc_name, int n, char *s);
void errorRealList(Script *script, char *proc_name, int n, char *s);
void errorBoolList(Script *script, char *proc_name, int n, char *s);
void errorCharList(Script *script, char *proc_name, int n, char *s);
void errorAddrList(Script *script, char *proc_name, int n, char *s);
void errorProcList(Script *script, char *proc_name, int n, char *s);
void errorListList(Script *script, char *proc_name, int n, char *s);

void errorNumberList(Script *script, char *proc_name, int n, char *s);

#define errorString errorCharList

/* Checking arguments */

#define checkCount(n,s) if(argc != n) \
                  { errorCount(script, proc_name, n, s); \
                    return NullValue; }
#define checkCountLeast(n,s) if(argc < n) \
                  { errorCountLeast(script, proc_name, n, s); \
                    return NullValue; }
#define checkCountMost(n,s) if(argc > n) \
                  { errorCountMost(script, proc_name, n, s); \
                    return NullValue; }

#define checkInt(n,s) if(!isInt(argv[n])) \
                  { errorInt(script, proc_name, n, s); return NullValue; }
#define checkReal(n,s) if(!isReal(argv[n])) \
                  { errorReal(script, proc_name, n, s); return NullValue; }
#define checkBool(n,s) if(!isBool(argv[n])) \
                  { errorBool(script, proc_name, n, s); return NullValue; }
#define checkChar(n,s) if(!isChar(argv[n])) \
                  { errorChar(script, proc_name, n, s); return NullValue; }
#define checkAddr(n,s) if(!isAddr(argv[n])) \
                  { errorAddr(script, proc_name, n, s); return NullValue; }
#define checkProc(n,s) if(!isProc(argv[n])) \
                  { errorProc(script, proc_name, n, s); return NullValue; }
#define checkList(n,s) if(!isList(argv[n])) \
                  { errorList(script, proc_name, n, s); return NullValue; }

#define checkNumber(n,s) if(!isNumber(argv[n])) \
                  { errorNumber(script, proc_name, n, s); return NullValue; }

#define checkIntList(n,s) if(!isIntList(argv[n])) \
                  { errorIntList(script, proc_name, n, s); return NullValue; }
#define checkRealList(n,s) if(!isRealList(argv[n])) \
                  { errorRealList(script, proc_name, n, s); return NullValue; }
#define checkBoolList(n,s) if(!isBoolList(argv[n])) \
                  { errorBoolList(script, proc_name, n, s); return NullValue; }
#define checkString(n,s) if(!isString(argv[n])) \
                  { errorString(script, proc_name, n, s); return NullValue; }
#define checkCharList(n,s) if(!isCharList(argv[n])) \
                  { errorCharList(script, proc_name, n, s); return NullValue; }
#define checkAddrList(n,s) if(!isAddrList(argv[n])) \
                  { errorAddrList(script, proc_name, n, s); return NullValue; }
#define checkProcList(n,s) if(!isProcList(argv[n])) \
                  { errorProcList(script, proc_name, n, s); return NullValue; }
#define checkListList(n,s) if(!isListList(argv[n])) \
                  { errorListList(script, proc_name, n, s); return NullValue; }

#define checkNumberList(n,s) if(!isNumberList(argv[n])) \
               { errorNumberList(script, proc_name, n, s); return NullValue; }

/*-------------------------------------------------------------------------*/
/* isis_core_codec.c global functions */

unsigned int reverseInteger(unsigned int x);
unsigned short reverseShort(unsigned short x);
int precodeValue(Value *val);
char *encodeValue(Value *val, char *vp);
Value *decodeValue(char **vpptr);
int encodeItem(Script *script, Value *port, Token *token);
int decodeItem(Script *script, Value *port, Token **token);
void encodeString(int code, int len, char *instr, char *outstr);
void decodeString(int code, int len, char *instr, char *outstr);

/* put system-dependent endianness definitions here: */
/* both alpha and pentium are little endian */

#ifdef ISIS_BIG_ENDIAN /* machine is big endian -- network byte order */
#define encodeInteger(x) (x)
#define decodeInteger(x) (x)
#else  /* machine is little endian -- must reverse bytes (DEFAULT) */
#define encodeInteger(x) reverseInteger(x)
#define decodeInteger(x) reverseInteger(x)
#endif

/*-------------------------------------------------------------------------*/
/* isis_core_port.c global functions */

Value *new_port_FILE(char *name, FILE *fp, int inflag, int outflag);

int printValue(Script *script, Value *port, Value *val);
int printStr(Script *script, Value *port, char *str);
int printStrn(Script *script, Value *port, char *str, int len);
int printSpace(Script *script, Value *port, int size);
int readStr(Script *script, Value *port, char *str, int len);
int readStrn(Script *script, Value *port, char *str, int len);

Value *prim_close_FILE(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_read_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_readline_FILE(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_write_FILE(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_tell_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_seek_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_read_ready_FILE(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *prim_write_ready_FILE(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_configure_FILE(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);

/*-------------------------------------------------------------------------*/
/* isis_core_memory.c global functions */

extern Memory *Token_mem;
extern Memory *Value_mem;
extern Memory *Parse_mem;
extern Memory *Script_mem;
extern Memory *Expression_mem;
extern Memory *ExpList_mem;
extern Memory *ExpNode_mem;
extern Memory *Varname_mem;
extern Memory *Binding_mem;
extern Memory *Environment_mem;
extern Memory *Procedure_mem;
extern Memory *StringLow_mem;
extern Memory *StringMed_mem;
extern Memory *StringHigh_mem;
extern Memory *PtrLow_mem;
extern Memory *PtrMed_mem;
extern Memory *PtrHigh_mem;

void initMemory(void);
char *newStr(int len);
void freeStr(char *str);
char *copyStr(char *str);
char *createStr(char *str);
char *createStrn(char *str, int len);
void **newPtrArray(int len);
void freePtrArray(void **ptrarray);

void *allocBuffer(int size);
void *expandBuffer(void *oldbuf, int newsize);

Value *memPeek(void *genptr, int ctype);
Value *memPoke(void *genptr, int ctype, Value *val);
Value *memPeekArray(void *genptr, int arraytype, int size);
Value *memPokeArray(void *genptr, int arraytype, int arraysize, Value *val);

/*-------------------------------------------------------------------------*/
/* isis_core_memman.c global functions */

void *mem_alloc(int n);
Memory *mem_new_memory(char *name, int size, int quantity, int increment);
void mem_free_all(void);
void mem_free_memory(Memory *mem);
int mem_increase_quantity(Memory *mem, int quantity_to_add);
void *mem_alloc_item(Memory *mem);
void mem_freeItem(Memory *mem, void *ptr);
void mem_print_stats(FILE *fd);
void mem_reset_all_stats(void);

#endif /* __ISIS_H__ */
