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
void bind_primitives(Script *script)
{
  /* Adds all primitives to the top-level environment in the script. */

  int int8type, int16type, int32type, int64type;
  int uint8type, uint16type, uint32type, uint64type;
  int real32type, real64type;

  if(!quiet_mode(script)) {
    printStr(script, script->outport, "*** Isis ");
    printStr(script, script->outport, ISIS_VERSION_TEXT);
    printStr(script, script->outport, "\n");
  }

  bindInt(script, "isis-version", ISIS_VERSION);
  
  /* math primitives */

  bindCfunc(script, "+", prim_add, 0);
  bindCfunc(script, "-", prim_sub, 0);
  bindCfunc(script, "*", prim_mul, 0);
  bindCfunc(script, "/", prim_div, 0);
  bindCfunc(script, "%", prim_mod, 0);

  bindCfunc(script, "add", prim_add, 0);
  bindCfunc(script, "sub", prim_sub, 0);
  bindCfunc(script, "mul", prim_mul, 0);
  bindCfunc(script, "div", prim_div, 0);
  bindCfunc(script, "mod", prim_mod, 0);

  bindCfunc(script, "abs", prim_abs, 0);

  bindCfunc(script, "clamp", prim_clamp, 0);
  
  bindCfunc(script, "sin", prim_sin, 0);
  bindCfunc(script, "cos", prim_cos, 0);
  bindCfunc(script, "tan", prim_tan, 0);
  bindCfunc(script, "asin", prim_asin, 0);
  bindCfunc(script, "acos", prim_acos, 0);
  bindCfunc(script, "atan", prim_atan, 0);
  bindCfunc(script, "atan2", prim_atan2, 0);
  bindCfunc(script, "sinh", prim_sinh, 0);
  bindCfunc(script, "cosh", prim_cosh, 0);
  bindCfunc(script, "tanh", prim_tanh, 0);
  bindCfunc(script, "exp", prim_exp, 0);
  bindCfunc(script, "log", prim_log, 0);
  bindCfunc(script, "log10", prim_log10, 0);
  bindCfunc(script, "pow", prim_pow, 0);
  bindCfunc(script, "sqrt", prim_sqrt, 0);
  bindCfunc(script, "ceil", prim_ceil, 0);
  bindCfunc(script, "floor", prim_floor, 0);
  bindCfunc(script, "degrad", prim_degrad, 0);
  bindCfunc(script, "raddeg", prim_raddeg, 0);

  bindCfunc(script, "&", prim_bit_and, 0);
  bindCfunc(script, "|", prim_bit_or, 0);
  bindCfunc(script, "^", prim_bit_xor, 0);
  bindCfunc(script, "<<", prim_bit_left, 0);
  bindCfunc(script, ">>", prim_bit_right, 0);
  bindCfunc(script, "~", prim_bit_not, 0);

  bindCfunc(script, "bit-and", prim_bit_and, 0);
  bindCfunc(script, "bit-or", prim_bit_or, 0);
  bindCfunc(script, "bit-xor", prim_bit_xor, 0);
  bindCfunc(script, "bit-left", prim_bit_left, 0);
  bindCfunc(script, "bit-right", prim_bit_right, 0);
  bindCfunc(script, "bit-not", prim_bit_not, 0);

  bindCfunc(script, "seed-random", prim_srand, 0);
  bindCfunc(script, "random", prim_rand, 0);

  bindCfunc(script, "linear-interpolate", 
		     prim_linear_interpolate, 0);
  bindCfunc(script, "cubic-interpolate", 
		     prim_cubic_interpolate, 0);

  /* compare primitives */

  bindCfunc(script, "eq?", prim_equality_op, 1);
  bindCfunc(script, "neq?", prim_equality_op, 0);

  bindCfunc(script, "=", prim_equality_op, 1);
  bindCfunc(script, "!=", prim_equality_op, 0);

  bindCfunc(script, "not", prim_not_op, 0);  

  bindCfunc(script, "lt?", prim_lt, 0);
  bindCfunc(script, "leq?", prim_le, 0);
  bindCfunc(script, "gt?", prim_gt, 0);
  bindCfunc(script, "geq?", prim_ge, 0);

  bindCfunc(script, "<", prim_lt, 0);
  bindCfunc(script, "<=", prim_le, 0);
  bindCfunc(script, ">", prim_gt, 0);
  bindCfunc(script, ">=", prim_ge, 0);

  /* list primitives */

  bindCfunc(script, "length", prim_length, 0);  

  bindCfunc(script, "head", prim_head, 0);
  bindCfunc(script, "tail", prim_tail, 0);

  bindCfunc(script, "first", prim_first, 0);  
  bindCfunc(script, "last", prim_last, 0);  
  bindCfunc(script, "allbutfirst", prim_allbutfirst, 0);  
  bindCfunc(script, "allbutlast", prim_allbutlast, 0);  
  bindCfunc(script, "sublist", prim_sublist, 0);

  bindCfunc(script, "head-insert", prim_head_insert, 0);
  bindCfunc(script, "tail-insert", prim_tail_insert, 0);

  bindCfunc(script, "insert-before", prim_insert, 0);
  bindCfunc(script, "insert-after", prim_insert, 1);

  bindCfunc(script, "change-item", prim_change_item, 0);
  bindCfunc(script, "remove-item", prim_remove_item, 0);

  bindCfunc(script, "append", prim_append, 0);  

  bindCfunc(script, "make-list", prim_make_list, 0);

  bindCfunc(script, "reverse", prim_reverse, 0);  

  bindCfunc(script, "foremost", prim_foremost, 1);  
  bindCfunc(script, "hindmost", prim_foremost, 0);  

  bindCfunc(script, "sort", prim_sort, 0);  

  bindCfunc(script, "apply", prim_apply, 0);

  bindCfunc(script, "map", prim_map, 0);

  /* types primitives */

  bindCfunc(script, "integer", prim_cast_integer, 0);
  bindCfunc(script, "real", prim_cast_real, 0);
  bindCfunc(script, "boolean", prim_cast_boolean, 0);
  bindCfunc(script, "character", prim_cast_character, 0);
  bindCfunc(script, "address", prim_cast_address, 0);
  bindCfunc(script, "procedure", prim_cast_procedure, 0);

  bindCfunc(script, "integer?", prim_type_query, IsisInt);
  bindCfunc(script, "real?", prim_type_query, IsisReal);
  bindCfunc(script, "boolean?", prim_type_query, IsisBool);
  bindCfunc(script, "character?", prim_type_query, IsisChar);
  bindCfunc(script, "address?", prim_type_query, IsisAddr);
  bindCfunc(script, "procedure?", prim_type_query, IsisProc);
  bindCfunc(script, "list?", prim_type_query, IsisList);

  bindCfunc(script, "integer-list?", prim_list_type_query, IsisInt);
  bindCfunc(script, "real-list?", prim_list_type_query, IsisReal);
  bindCfunc(script, "boolean-list?", prim_list_type_query, IsisBool);
  bindCfunc(script, "character-list?", prim_list_type_query, IsisChar);
  bindCfunc(script, "address-list?", prim_list_type_query, IsisAddr);
  bindCfunc(script, "procedure-list?", prim_list_type_query, IsisProc);
  bindCfunc(script, "list-list?", prim_list_type_query, IsisList);

  bindCfunc(script, "string?", prim_list_type_query, IsisChar);

  /* misc primitives */

  bindCfunc(script, "exit", prim_exit, 0);
  bindCfunc(script, "quit", prim_exit, 0);

  bindCfunc(script, "eval", prim_eval, 0);
  bindCfunc(script, "uneval", prim_uneval, 0);

  bindCfunc(script, "set-precision", prim_set_precision, 0);

  bindCfunc(script, "print-tokens", prim_option_set, PRINT_TOKENS);
  bindCfunc(script, "print-exps", prim_option_set, PRINT_EXPS);
  bindCfunc(script, "debug", prim_option_set, DEBUG_MODE);
  bindCfunc(script, "quiet", prim_option_set, QUIET_MODE);

  bindCfunc(script, "print-tokens?", prim_option_query, PRINT_TOKENS);
  bindCfunc(script, "print-exps?", prim_option_query, PRINT_EXPS);
  bindCfunc(script, "debug?", prim_option_query, DEBUG_MODE);
  bindCfunc(script, "quiet?", prim_option_query, QUIET_MODE);

  bindCfunc(script, "display", prim_display, 0);

  bindCfunc(script, "trace", prim_trace, TRACE_ON);
  bindCfunc(script, "untrace", prim_trace, TRACE_OFF);
  bindCfunc(script, "tracename", prim_trace, TRACE_NAME);

  bindCfunc(script, "memory", prim_mem_stats, 0);
  bindCfunc(script, "reset-memory-stats", prim_mem_stats, 1);

  /* port primitives */

  bindValue(script, "default-input-port", script->inport);
  bindValue(script, "default-output-port", script->outport);
  
  bindInt(script, "port-name", PORT_NAME);
  bindInt(script, "port-type", PORT_TYPE);
  bindInt(script, "port-inflag", PORT_INFLAG);
  bindInt(script, "port-outflag", PORT_OUTFLAG);
  bindInt(script, "port-internal-id", PORT_INTERNAL_ID);
  bindInt(script, "port-close-proc", PORT_CLOSE_PROC);
  bindInt(script, "port-read-proc", PORT_READ_PROC);
  bindInt(script, "port-readline-proc", PORT_READLINE_PROC);
  bindInt(script, "port-write-proc", PORT_WRITE_PROC);
  bindInt(script, "port-tell-proc", PORT_TELL_PROC);
  bindInt(script, "port-seek-proc", PORT_SEEK_PROC);
  bindInt(script, "port-read-ready-proc", PORT_READ_READY_PROC);
  bindInt(script, "port-write-ready-proc", PORT_WRITE_READY_PROC);
  bindInt(script, "port-configure-proc", PORT_CONFIGURE_PROC);
  
  bindValue(script, "port-type-FILE", port_type_FILE_val);
  
  bindValue(script, "close-FILE", prim_close_FILE_val);
  bindValue(script, "read-FILE", prim_read_FILE_val);
  bindValue(script, "readline-FILE", prim_readline_FILE_val);
  bindValue(script, "write-FILE", prim_write_FILE_val);
  bindValue(script, "tell-FILE", prim_tell_FILE_val);
  bindValue(script, "seek-FILE", prim_seek_FILE_val);
  bindValue(script, "read-ready-FILE", prim_read_ready_FILE_val);
  bindValue(script, "write-ready-FILE", prim_write_ready_FILE_val);
  bindValue(script, "configure-FILE", prim_configure_FILE_val);

  bindCfunc(script, "open-FILE", prim_open_FILE, 0);
  bindCfunc(script, "new-port-FILE", prim_new_port_FILE, 0);
  bindCfunc(script, "new-port", prim_new_port, 0);

  bindCfunc(script, "current-input-port", prim_current_port, INPUT_PORT);
  bindCfunc(script, "current-output-port", prim_current_port, OUTPUT_PORT);

  bindCfunc(script, "set-input-port", prim_set_port, INPUT_PORT);
  bindCfunc(script, "set-output-port", prim_set_port, OUTPUT_PORT);

  bindCfunc(script, "input-port?", prim_query_port, INPUT_PORT);
  bindCfunc(script, "output-port?", prim_query_port, OUTPUT_PORT);

  bindCfunc(script, "interpret-port", prim_interpret_port, 0);
  bindCfunc(script, "interactive", prim_interpret_port, 1);

  bindCfunc(script, "interpret-compiled-port", prim_interpret_compiled_port, 0);
  bindCfunc(script, "compile-port", prim_compile_port, 0);

  bindCfunc(script, "open-input", prim_port_open_filename, OPEN_INPUT);
  bindCfunc(script, "open-output", prim_port_open_filename, OPEN_OUTPUT);
  bindCfunc(script, "open-update", prim_port_open_filename, OPEN_UPDATE);

  bindCfunc(script, "close", prim_port_close, 0);
  bindCfunc(script, "read-raw", prim_port_read_raw, 0);  
  bindCfunc(script, "write-raw", prim_port_write_raw, 0);  
  bindCfunc(script, "tell", prim_port_tell, 0);
  bindCfunc(script, "seek", prim_port_seek, 0);
  bindCfunc(script, "read-ready", prim_port_read_ready, 0);
  bindCfunc(script, "write-ready", prim_port_write_ready, 0);

  bindCfunc(script, "pending", prim_port_read_ready, 0);

  bindCfunc(script, "read-string", prim_port_read_string, 0);  
  bindCfunc(script, "write-string", prim_port_write_string, 0);  
  bindCfunc(script, "print", prim_port_write_string, 1);

  bindCfunc(script, "read", prim_port_read_coded, 0);  
  bindCfunc(script, "write", prim_port_write_coded, 0);  
  
  /* character constants */

  bindChar(script, "alert", '\a');
  bindChar(script, "backspace", '\b');
  bindChar(script, "formfeed", '\f');
  bindChar(script, "newline", '\n');
  bindChar(script, "return", '\r');
  bindChar(script, "tab", '\t');
  bindChar(script, "vtab", '\v');
  bindChar(script, "squote", '\'');
  bindChar(script, "dquote", '"');
  bindChar(script, "backslash", '\\');

  /* numerical constants */

  bindReal(script, "pi", ISIS_PI);  
  bindInt(script, "rand-max", RAND_MAX);

  /* memory primitives */

  bindCfunc(script, "allocate-memory", prim_alloc, 0);
  bindCfunc(script, "free-memory", prim_free, 0);  
  
  bindCfunc(script, "copy-memory", prim_memcpy, 0);  
  bindCfunc(script, "move-memory", prim_memmove, 0);  
  bindCfunc(script, "compare-memory", prim_memcmp, 0);  
  bindCfunc(script, "search-memory", prim_memchr, 0);  
  bindCfunc(script, "set-memory", prim_memset, 0);  
  bindCfunc(script, "reverse-bytes", prim_revbytes, 0);  
  
  /* memory constants */

  bindIntList(script, "sizeof", CTYPE_COUNT, isis_sizeof);
  
  bindInt(script, "c-byte", CTYPE_UCHAR);
  bindInt(script, "c-char", CTYPE_CHAR);
  bindInt(script, "c-uchar", CTYPE_UCHAR);
  bindInt(script, "c-short", CTYPE_SHORT);
  bindInt(script, "c-ushort", CTYPE_USHORT);
  bindInt(script, "c-int", CTYPE_INT);
  bindInt(script, "c-uint", CTYPE_UINT);
  bindInt(script, "c-long", CTYPE_LONG);
  bindInt(script, "c-ulong", CTYPE_ULONG);
  bindInt(script, "c-float", CTYPE_FLOAT);
  bindInt(script, "c-double", CTYPE_DOUBLE);
  bindInt(script, "c-pointer", CTYPE_POINTER);
#ifdef _LONGLONG
  bindInt(script, "c-longlong", CTYPE_LONGLONG);
  bindInt(script, "c-ulonglong", CTYPE_ULONGLONG);
#endif

  int8type = int16type = int32type = int64type = CTYPE_UNKNOWN;
  uint8type = uint16type = uint32type = uint64type = CTYPE_UNKNOWN;
  real32type = real64type = CTYPE_UNKNOWN;

  if(isis_sizeof[CTYPE_SHORT] == 1) {
    int8type = CTYPE_SHORT;
    uint8type = CTYPE_USHORT;
  }
  else if(isis_sizeof[CTYPE_SHORT] == 2) {
    int16type = CTYPE_SHORT;
    uint16type = CTYPE_USHORT;
  }

  if(isis_sizeof[CTYPE_CHAR] == 1) {
    int8type = CTYPE_CHAR;
    uint8type = CTYPE_UCHAR;
  }

#ifdef _LONGLONG
  if(isis_sizeof[CTYPE_LONGLONG] == 8) {
    int64type = CTYPE_LONGLONG;
    uint64type = CTYPE_ULONGLONG;
  }
#endif    

  if(isis_sizeof[CTYPE_LONG] == 4) {
    int32type = CTYPE_LONG;
    uint32type = CTYPE_ULONG;
  }
  else if(isis_sizeof[CTYPE_LONG] == 8) {
    int64type = CTYPE_LONG;
    uint64type = CTYPE_ULONG;
  }

  if(isis_sizeof[CTYPE_INT] == 2) {
    int16type = CTYPE_INT;
    uint16type = CTYPE_UINT;
  }
  else if(isis_sizeof[CTYPE_INT] == 4) {
    int32type = CTYPE_INT;
    uint32type = CTYPE_UINT;
  }
  else if(isis_sizeof[CTYPE_INT] == 8) {
    int64type = CTYPE_INT;
    uint64type = CTYPE_UINT;
  }

  if(isis_sizeof[CTYPE_DOUBLE] == 4) real32type = CTYPE_DOUBLE;
  else if(isis_sizeof[CTYPE_DOUBLE] == 8) real64type = CTYPE_DOUBLE;
  if(isis_sizeof[CTYPE_FLOAT] == 4) real32type = CTYPE_FLOAT;
  else if(isis_sizeof[CTYPE_FLOAT] == 8) real64type = CTYPE_FLOAT;

  bindInt(script, "int8", int8type);
  bindInt(script, "int16", int16type);
  bindInt(script, "int32", int32type);
  bindInt(script, "int64", int64type);

  bindInt(script, "uint8", uint8type);
  bindInt(script, "uint16", uint16type);
  bindInt(script, "uint32", uint32type);
  bindInt(script, "uint64", uint64type);

  bindInt(script, "real32", real32type);
  bindInt(script, "real64", real64type);
}
