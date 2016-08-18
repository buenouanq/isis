/*  Private prototypes for Isis color routines  */

extern Value *image_color_raw_rgb_to_br(Script *script, 
					char *proc_name, int call_data, 
					int argc, Value **argv);

extern Value *image_color_raw_rgb_to_y(Script *script, 
				       char *proc_name, int call_data, 
				       int argc, Value **argv);

extern Value *image_color_raw_ybr_to_rgb(Script *script, 
					 char *proc_name, int call_data, 
					 int argc, Value **argv);

extern Value *image_color_raw_rgb_to_yiq(Script *script, 
				       char *proc_name, int call_data, 
				       int argc, Value **argv);

extern Value *image_color_raw_yiq_to_rgb(Script *script, 
				       char *proc_name, int call_data, 
				       int argc, Value **argv);

extern Value *image_color_relax_chan(Script *script, 
				     char *proc_name, int call_data, 
				     int argc, Value **argv);

extern Value *image_color_relax_byte_chan(Script *script, char *proc_name,
					  int call_data, 
					  int argc, Value **argv);

typedef unsigned char byte;

