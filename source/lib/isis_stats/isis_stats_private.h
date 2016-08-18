/*  Prototypes for the Isis statistics functions  */

extern Value *image_stats_raw_2D_hist_byte(Script *script, char *proc_name,
					   int call_data, 
					   int argc, Value **argv);

extern Value *image_stats_raw_2D_hist_short(Script *script, char *proc_name,
					    int call_data, 
					    int argc, Value **argv);

extern Value *image_stats_raw_2D_hist_long( Script *script, char *proc_name,
					    int call_data, 
					    int argc, Value **argv );

extern Value *image_stats_raw_2D_hist_ushort(Script *script, char *proc_name,
					     int call_data, 
					     int argc, Value **argv);

extern Value *image_stats_raw_2D_hist_ulong(Script *script, char *proc_name,
					    int call_data, 
					    int argc, Value **argv);

extern Value *image_stats_raw_2D_hist_float(Script *script, char *proc_name,
					    int call_data, 
					    int argc, Value **argv);

extern Value *image_stats_raw_2D_hist_double(Script *script, char *proc_name,
					     int call_data, 
					     int argc, Value **argv);

/*  The 1D calls return a list of variable type.  These are the
    call_data values that indicate what type to return !  */

#define STATS_HIST_INTEGER   0
#define STATS_HIST_REAL      1

extern Value *image_stats_raw_1D_hist_byte(Script *script, char *proc_name,
					   int call_data, int argc,
					   Value **argv);

extern Value *image_stats_raw_1D_hist_float(Script *script,
					    char *proc_name,
					    int call_data,
					    int argc, Value **argv);

Value *image_stats_raw_fast_1D_hist_byte(Script *script, char *proc_name,
					 int call_data, 
					 int argc, Value **argv);

Value *image_stats_raw_centroid(Script *script, 
				char *proc_name, int call_data, 
				int argc, Value **argv);

typedef unsigned char byte;

