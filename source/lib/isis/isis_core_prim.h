/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

#define ISIS_PI 3.1415926535897932385

enum { PRINT_TOKENS, PRINT_EXPS, QUIET_MODE, DEBUG_MODE };
enum { OPEN_INPUT, OPEN_OUTPUT, OPEN_UPDATE };
enum { INPUT_PORT, OUTPUT_PORT };
enum { TRACE_ON, TRACE_OFF, TRACE_NAME };

/*-------------------------------------------------------------*/
/* math primitives */

Value *prim_add(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_sub(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_mul(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_div(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_mod(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_abs(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_clamp(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_sin(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_cos(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_tan(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_asin(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_acos(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_atan(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_atan2(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_sinh(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_cosh(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_tanh(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_exp(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_log(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_log10(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_pow(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);
Value *prim_sqrt(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_ceil(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_floor(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_degrad(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_raddeg(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_bit_and(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *prim_bit_or(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_bit_xor(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *prim_bit_left(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv);
Value *prim_bit_right(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_bit_not(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *prim_rand(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_srand(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_linear_interpolate(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *prim_cubic_interpolate(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv);

/*-------------------------------------------------------------*/
/* compare primitives */

Value *prim_equality_op(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_not_op(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_lt(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv);
Value *prim_le(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv);
Value *prim_gt(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv);
Value *prim_ge(Script *script, char *proc_name, int call_data, 
	       int argc, Value **argv);

/*-------------------------------------------------------------*/
/* list primitives */

Value *prim_length(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_head(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_tail(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_first(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_last(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_allbutfirst(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_allbutlast(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_sublist(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *prim_head_insert(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_tail_insert(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_insert(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_change_item(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_remove_item(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_append(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_make_list(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_reverse(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *prim_foremost(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv);
Value *prim_sort(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_apply(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_map(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv);

/*-------------------------------------------------------------*/
/* types primitives */

Value *prim_cast_integer(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_cast_real(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_cast_character(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *prim_cast_boolean(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_cast_address(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_cast_procedure(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *prim_type_query(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_list_type_query(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);

/*-------------------------------------------------------------*/
/* memory primitives */

Value *prim_alloc(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_free(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_memcpy(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_memmove(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *prim_memcmp(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_memchr(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_memset(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_revbytes(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv);

/*-------------------------------------------------------------*/
/* port primitives */

Value *prim_open_FILE(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_new_port_FILE(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_new_port(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv);
Value *prim_current_port(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_set_port(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv);
Value *prim_query_port(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_interpret_port(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *prim_interpret_compiled_port(Script *script, char *proc_name, int call_data, 
				    int argc, Value **argv);
Value *prim_compile_port(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_port_open_filename(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *prim_port_close(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_port_read_raw(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_port_write_raw(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *prim_port_tell(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_port_seek(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_port_read_ready(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *prim_port_write_ready(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_port_read_string(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_port_write_string(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv);
Value *prim_port_read_coded(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *prim_port_write_coded(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);

/*-------------------------------------------------------------*/
/* misc primitives */

Value *prim_exit(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_eval(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);
Value *prim_uneval(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *prim_set_precision(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_option_query(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_option_set(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_display(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *prim_trace(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *prim_mem_stats(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
