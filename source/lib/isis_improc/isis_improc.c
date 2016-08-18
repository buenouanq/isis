/* *********************************************************************
   isis_improc.c
   improc image processing interface functions for Isis
   cleaned by stefan agamanolis, march 1998
   ********************************************************************* */

#include <stdio.h>                /* Must include before isis.h */
#include <isis.h>                 /* Holds all Isis interpreter info */

#include "isis_improc_private.h"
#include "isis_improc.h"

enum { PROCESS_BYTE, 
       PROCESS_CHAR,
       PROCESS_SHORT,
       PROCESS_USHORT,
       PROCESS_INT, 
       PROCESS_UINT,
       PROCESS_LONG, 
       PROCESS_ULONG,
       PROCESS_DOUBLE, 
       PROCESS_FLOAT, 
       PROCESS_BYTE_INT_BYTE,
       PROCESS_BYTE_INT_INT };

/*-------------------------------------------------------------*/
/* Declare all C functions you intend to use inside Isis here */

Value *improc_transfer(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *improc_range(Script *script, 
		    char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *improc_range_3ch(Script *script,
			char *proc_name, int call_data, 
			int argc, Value **argv);
Value *improc_composite(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *improc_composite_constalpha(Script *script, 
				   char *proc_name, int call_data, 
				   int argc, Value **argv);
Value *improc_composite_remapalpha(Script *script, 
				   char *proc_name, int call_data, 
				   int argc, Value **argv);
Value *improc_scale(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv);
Value *improc_scale_range(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *improc_scale_composite(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv);
Value *improc_scale_composite_constalpha(Script *script, 
					 char *proc_name, int call_data, 
					 int argc, Value **argv);
Value *improc_scale_composite_remapalpha(Script *script, 
					 char *proc_name, int call_data, 
					 int argc, Value **argv);
Value *improc_convert_byte(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *improc_convert_short(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *improc_convert_int(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *improc_convert_long(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *improc_convert_float(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *improc_convert_double(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *improc_gainbias_byte(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *improc_gainbias_short(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *improc_gainbias_int(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *improc_gainbias_long(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *improc_gainbias_float(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *improc_gainbias_double(Script *script, 
			      char *proc_name, int call_data, 
			      int argc, Value **argv);
Value *improc_remap_intensity(Script *script, 
			      char *proc_name, int call_data, 
			      int argc, Value **argv);
Value *improc_fill_constant(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *improc_add(Script *script, 
		  char *proc_name, int call_data, 
		  int argc, Value **argv);
Value *improc_subtract(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *improc_diff(Script *script, 
		   char *proc_name, int call_data, 
		   int argc, Value **argv);
Value *improc_multiply(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *improc_divide(Script *script, 
		     char *proc_name, int call_data, 
		     int argc, Value **argv);
Value *improc_maximum(Script *script, 
		      char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *improc_minimum(Script *script, 
		      char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *improc_filter_1d(Script *script, 
			char *proc_name, int call_data, 
			int argc, Value **argv);
Value *improc_filter_2d(Script *script, 
			char *proc_name, int call_data, 
			int argc, Value **argv);
Value *improc_affine_trans(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *improc_affine_warp(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *improc_warp_image(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *improc_bounding_box(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *improc_bounding_box_rgb(Script *script, 
			       char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *improc_fuzzy_bounding_box(Script *script, 
				 char *proc_name, int call_data, 
				 int argc, Value **argv);

Value *improc_flood_fill(Script *script, 
			 char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *improc_optical_flow(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv);

/* ------------------------------------------------------------ */
/* This function is called to bind all of the functions and other
   values defined in this source file */

void bind_improc_library(Script *script)
{
  bindBool(script, "isis_improc", 1);

  bindCfunc(script, "improc-transfer-byte", 
	    improc_transfer, PROCESS_BYTE);
  bindCfunc(script, "improc-transfer-int", 
	    improc_transfer, PROCESS_INT);
  bindCfunc(script, "improc-transfer-short", 
	    improc_transfer, PROCESS_SHORT);
  bindCfunc(script, "improc-transfer-long", 
	    improc_transfer, PROCESS_LONG);
  bindCfunc(script, "improc-transfer-float", 
	    improc_transfer, PROCESS_FLOAT);
  bindCfunc(script, "improc-transfer-double", 
	    improc_transfer, PROCESS_DOUBLE);

  bindCfunc(script, "improc-transfer-range-byte", 
	    improc_range, PROCESS_BYTE);
  bindCfunc(script, "improc-transfer-range-int", 
	    improc_range, PROCESS_INT);
  bindCfunc(script, "improc-transfer-range-long", 
	    improc_range, PROCESS_LONG);
  bindCfunc(script, "improc-transfer-range-float", 
	    improc_range, PROCESS_FLOAT);
  bindCfunc(script, "improc-transfer-range-double", 
	    improc_range, PROCESS_DOUBLE);

  bindCfunc(script, "improc-transfer-range-3ch-byte", 
	    improc_range_3ch, PROCESS_BYTE);
  bindCfunc(script, "improc-transfer-range-3ch-int", 
	    improc_range_3ch, PROCESS_INT);
  bindCfunc(script, "improc-transfer-range-3ch-long", 
	    improc_range_3ch, PROCESS_LONG);
  bindCfunc(script, "improc-transfer-range-3ch-float", 
	    improc_range_3ch, PROCESS_FLOAT);
  bindCfunc(script, "improc-transfer-range-3ch-double", 
	    improc_range_3ch, PROCESS_DOUBLE);

  bindCfunc(script, "improc-composite", 
	    improc_composite, 0);
  bindCfunc(script, "improc-composite-constalpha", 
	    improc_composite_constalpha, 0);
  bindCfunc(script, "improc-composite-remapalpha", 
	    improc_composite_remapalpha, 0);

  bindCfunc(script, "improc-scale-byte", 
	    improc_scale, PROCESS_BYTE);
  bindCfunc(script, "improc-scale-range-byte", 
	    improc_scale_range, PROCESS_BYTE);
  bindCfunc(script, "improc-scale-composite", 
	    improc_scale_composite, 0);
  bindCfunc(script, "improc-scale-composite-constalpha", 
	    improc_scale_composite_constalpha, 0);
  bindCfunc(script, "improc-scale-composite-remapalpha", 
	    improc_scale_composite_remapalpha, 0);
  
  bindCfunc(script, "improc-convert-byte-byte", 
	    improc_convert_byte, PROCESS_BYTE);
  bindCfunc(script, "improc-convert-byte-short", 
	    improc_convert_byte, PROCESS_SHORT);
  bindCfunc(script, "improc-convert-byte-int", 
	    improc_convert_byte, PROCESS_INT);
  bindCfunc(script, "improc-convert-byte-long", 
	    improc_convert_byte, PROCESS_LONG);
  bindCfunc(script, "improc-convert-byte-float",
	    improc_convert_byte, PROCESS_FLOAT);
  bindCfunc(script, "improc-convert-byte-double",
	    improc_convert_byte, PROCESS_DOUBLE);

  bindCfunc(script, "improc-convert-short-byte", 
	    improc_convert_short, PROCESS_BYTE);
  bindCfunc(script, "improc-convert-short-short", 
	    improc_convert_short, PROCESS_SHORT);
  bindCfunc(script, "improc-convert-short-int", 
	    improc_convert_short, PROCESS_INT);
  bindCfunc(script, "improc-convert-short-long", 
	    improc_convert_short, PROCESS_LONG);
  bindCfunc(script, "improc-convert-short-float",
	    improc_convert_short, PROCESS_FLOAT);
  bindCfunc(script, "improc-convert-short-double",
	    improc_convert_short, PROCESS_DOUBLE);

  bindCfunc(script, "improc-convert-int-byte", 
	    improc_convert_int, PROCESS_BYTE);
  bindCfunc(script, "improc-convert-int-short", 
	    improc_convert_int, PROCESS_SHORT);
  bindCfunc(script, "improc-convert-int-int", 
	    improc_convert_int, PROCESS_INT);
  bindCfunc(script, "improc-convert-int-long", 
	    improc_convert_int, PROCESS_LONG);
  bindCfunc(script, "improc-convert-int-float",
	    improc_convert_int, PROCESS_FLOAT);
  bindCfunc(script, "improc-convert-int-double",
	    improc_convert_int, PROCESS_DOUBLE);

  bindCfunc(script, "improc-convert-long-byte", 
	    improc_convert_long, PROCESS_BYTE);
  bindCfunc(script, "improc-convert-long-short", 
	    improc_convert_long, PROCESS_SHORT);
  bindCfunc(script, "improc-convert-long-int", 
	    improc_convert_long, PROCESS_INT);
  bindCfunc(script, "improc-convert-long-long", 
	    improc_convert_long, PROCESS_LONG);
  bindCfunc(script, "improc-convert-long-float",
	    improc_convert_long, PROCESS_FLOAT);
  bindCfunc(script, "improc-convert-long-double",
	    improc_convert_long, PROCESS_DOUBLE);

  bindCfunc(script, "improc-convert-float-byte", 
	    improc_convert_float, PROCESS_BYTE);
  bindCfunc(script, "improc-convert-float-short", 
	    improc_convert_float, PROCESS_SHORT);
  bindCfunc(script, "improc-convert-float-int", 
	    improc_convert_float, PROCESS_INT);
  bindCfunc(script, "improc-convert-float-long", 
	    improc_convert_float, PROCESS_LONG);
  bindCfunc(script, "improc-convert-float-float",
	    improc_convert_float, PROCESS_FLOAT);
  bindCfunc(script, "improc-convert-float-double",
	    improc_convert_float, PROCESS_DOUBLE);

  bindCfunc(script, "improc-convert-double-byte", 
	    improc_convert_double, PROCESS_BYTE);
  bindCfunc(script, "improc-convert-double-short", 
	    improc_convert_double, PROCESS_SHORT);
  bindCfunc(script, "improc-convert-double-int", 
	    improc_convert_double, PROCESS_INT);
  bindCfunc(script, "improc-convert-double-long", 
	    improc_convert_double, PROCESS_LONG);
  bindCfunc(script, "improc-convert-double-float",
	    improc_convert_double, PROCESS_FLOAT);
  bindCfunc(script, "improc-convert-double-double",
	    improc_convert_double, PROCESS_DOUBLE);

  bindCfunc(script, "improc-gainbias-byte",
	    improc_gainbias_byte, PROCESS_BYTE);
  bindCfunc(script, "improc-gainbias-byte-short",
	    improc_gainbias_byte, PROCESS_SHORT);
  bindCfunc(script, "improc-gainbias-byte-int",
	    improc_gainbias_byte, PROCESS_INT);
  bindCfunc(script, "improc-gainbias-byte-long",
	    improc_gainbias_byte, PROCESS_LONG);
  bindCfunc(script, "improc-gainbias-byte-float",
	    improc_gainbias_byte, PROCESS_FLOAT);
  bindCfunc(script, "improc-gainbias-byte-double",
	    improc_gainbias_byte, PROCESS_DOUBLE);

  bindCfunc(script, "improc-gainbias-short-byte",
	    improc_gainbias_short, PROCESS_BYTE);
  bindCfunc(script, "improc-gainbias-short",
	    improc_gainbias_short, PROCESS_SHORT);
  bindCfunc(script, "improc-gainbias-short-int",
	    improc_gainbias_short, PROCESS_INT);
  bindCfunc(script, "improc-gainbias-short-long",
	    improc_gainbias_short, PROCESS_LONG);
  bindCfunc(script, "improc-gainbias-short-float",
	    improc_gainbias_short, PROCESS_FLOAT);
  bindCfunc(script, "improc-gainbias-short-double",
	    improc_gainbias_short, PROCESS_DOUBLE);

  bindCfunc(script, "improc-gainbias-int",
	    improc_gainbias_int, PROCESS_INT);
  bindCfunc(script, "improc-gainbias-long",
	    improc_gainbias_long, PROCESS_LONG);
  bindCfunc(script, "improc-gainbias-float",
	    improc_gainbias_float, PROCESS_FLOAT);
  bindCfunc(script, "improc-gainbias-double",
	    improc_gainbias_double, PROCESS_DOUBLE);

  bindCfunc(script, "improc-fill-constant-byte", 
	    improc_fill_constant, PROCESS_BYTE);
  bindCfunc(script, "improc-fill-constant-int", 
	    improc_fill_constant, PROCESS_INT);
  bindCfunc(script, "improc-fill-constant-long", 
	    improc_fill_constant, PROCESS_LONG);
  bindCfunc(script, "improc-fill-constant-float", 
	    improc_fill_constant, PROCESS_FLOAT);
  bindCfunc(script, "improc-fill-constant-double", 
	    improc_fill_constant, PROCESS_DOUBLE);

  bindCfunc(script, "improc-add-byte", 
	    improc_add, PROCESS_BYTE);
  bindCfunc(script, "improc-add-int", 
	    improc_add, PROCESS_INT);
  bindCfunc(script, "improc-add-double", 
	    improc_add, PROCESS_DOUBLE);
  bindCfunc(script, "improc-add-byte-int-byte", 
	    improc_add, PROCESS_BYTE_INT_BYTE);
  bindCfunc(script, "improc-add-byte-int-int", 
	    improc_add, PROCESS_BYTE_INT_INT);

  bindCfunc(script, "improc-subtract-byte", 
	    improc_subtract, PROCESS_BYTE);
  bindCfunc(script, "improc-subtract-int", 
	    improc_subtract, PROCESS_INT);
  bindCfunc(script, "improc-subtract-double", 
	    improc_subtract, PROCESS_DOUBLE);
  bindCfunc(script, "improc-subtract-byte-int-byte", 
	    improc_subtract, PROCESS_BYTE_INT_BYTE);

  bindCfunc(script, "improc-diff-byte", 
	    improc_diff, PROCESS_BYTE);
  bindCfunc(script, "improc-diff-int", 
	    improc_diff, PROCESS_INT);
  bindCfunc(script, "improc-diff-double", 
	    improc_diff, PROCESS_DOUBLE);
  bindCfunc(script, "improc-diff-byte-int-byte", 
	    improc_diff, PROCESS_BYTE_INT_BYTE);

  bindCfunc(script, "improc-multiply-byte", 
	    improc_multiply, PROCESS_BYTE);
  bindCfunc(script, "improc-multiply-int", 
	    improc_multiply, PROCESS_INT);
  bindCfunc(script, "improc-multiply-double", 
	    improc_multiply, PROCESS_DOUBLE);
  bindCfunc(script, "improc-multiply-byte-int-byte", 
	    improc_multiply, PROCESS_BYTE_INT_BYTE);

  bindCfunc(script, "improc-divide-byte", 
	    improc_divide, PROCESS_BYTE);
  bindCfunc(script, "improc-divide-int", 
	    improc_divide, PROCESS_INT);
  bindCfunc(script, "improc-divide-double", 
	    improc_divide, PROCESS_DOUBLE);
  bindCfunc(script, "improc-divide-byte-int-byte", 
	    improc_divide, PROCESS_BYTE_INT_BYTE);

  bindCfunc(script, "improc-maximum-byte", 
	    improc_maximum, PROCESS_BYTE);
  bindCfunc(script, "improc-maximum-int", 
	    improc_maximum, PROCESS_INT);
  bindCfunc(script, "improc-maximum-double", 
	    improc_maximum, PROCESS_DOUBLE);

  bindCfunc(script, "improc-minimum-byte", 
	    improc_minimum, PROCESS_BYTE);
  bindCfunc(script, "improc-minimum-int", 
	    improc_minimum, PROCESS_INT);
  bindCfunc(script, "improc-minimum-double", 
	    improc_minimum, PROCESS_DOUBLE);

  bindCfunc(script, "improc-filter-1d-byte", 
	    improc_filter_1d, PROCESS_BYTE);
  bindCfunc(script, "improc-filter-1d-short", 
	    improc_filter_1d, PROCESS_SHORT);
  bindCfunc(script, "improc-filter-2d-byte", 
	    improc_filter_2d, PROCESS_BYTE);
  bindCfunc(script, "improc-affine-trans-byte", 
	    improc_affine_trans, PROCESS_BYTE);
  bindCfunc(script, "improc-affine-warp-map", 
	    improc_affine_warp, PROCESS_BYTE);

  bindCfunc(script, "improc-warp-image", improc_warp_image, 0);

  bindCfunc(script, "improc-remap-intensity", improc_remap_intensity, 0);

  bindCfunc(script, "improc-bounding-box", 
	    improc_bounding_box, 0);
  bindCfunc(script, "improc-bounding-box-rgb", 
	    improc_bounding_box_rgb, 0);
  bindCfunc(script, "improc-fuzzy-bounding-box", 
	    improc_fuzzy_bounding_box, PROCESS_BYTE);

  bindCfunc(script, "improc-flood-fill-byte", 
	    improc_flood_fill, PROCESS_BYTE);
  bindCfunc(script, "improc-flood-fill-short", 
	    improc_flood_fill, PROCESS_SHORT);
  bindCfunc(script, "improc-optical-flow-byte", 
	    improc_optical_flow, PROCESS_BYTE);
  bindCfunc(script, "improc-optical-flow-int", 
	    improc_optical_flow, PROCESS_INT);
}

/* ------------------------------------------------------------ */
/* Define your C functions below.  All C functions that will be bound
   in Isis must follow this prototype. */

/* ------------------------------------------------------------ */
Value *improc_transfer(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  int i, chans;
  
  checkCount(5, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");
  
  chans = Listsize(argv[1]);
  
  switch(call_data) {
    
  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      transfer_byte(
		    getInt(Listitem(argv[0], 0)),
		    getInt(Listitem(argv[0], 1)),
		    
		    getAddr(Listitem(argv[1], i)),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1))
		    );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      transfer_short(
		     getInt(Listitem(argv[0], 0)),
		     getInt(Listitem(argv[0], 1)),
		    
		     getAddr(Listitem(argv[1], i)),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1))
		     );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      transfer_int(
		   getInt(Listitem(argv[0], 0)),
		   getInt(Listitem(argv[0], 1)),
		    
		   getAddr(Listitem(argv[1], i)),
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		    
		   getAddr(Listitem(argv[3], i)),
		   getInt(Listitem(argv[4], 0)),
		   getInt(Listitem(argv[4], 1))
		   );
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      transfer_long(
		    getInt(Listitem(argv[0], 0)),
		    getInt(Listitem(argv[0], 1)),
		    
		    getAddr(Listitem(argv[1], i)),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1))
		    );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      transfer_float(
		     getInt(Listitem(argv[0], 0)),
		     getInt(Listitem(argv[0], 1)),
		    
		     getAddr(Listitem(argv[1], i)),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1))
		     );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      transfer_double(
		      getInt(Listitem(argv[0], 0)),
		      getInt(Listitem(argv[0], 1)),
		    
		      getAddr(Listitem(argv[1], i)),
		      getInt(Listitem(argv[2], 0)),
		      getInt(Listitem(argv[2], 1)),
		    
		      getAddr(Listitem(argv[3], i)),
		      getInt(Listitem(argv[4], 0)),
		      getInt(Listitem(argv[4], 1))
		      );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_range(Script *script, 
		    char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkNumber(0, "lowval");
  checkNumber(1, "highval");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");

  chans = Listsize(argv[3]);
  
  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      range_1ch_byte(
		     (unsigned char) getIntfromNumber(argv[0]),
		     (unsigned char) getIntfromNumber(argv[1]),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1)),
		    
		     getAddr(Listitem(argv[5], i)),
		     getInt(Listitem(argv[6], 0)),
		     getInt(Listitem(argv[6], 1))
		     );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      range_1ch_int(
		    getIntfromNumber(argv[0]),
		    getIntfromNumber(argv[1]),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1)),
		    
		    getAddr(Listitem(argv[5], i)),
		    getInt(Listitem(argv[6], 0)),
		    getInt(Listitem(argv[6], 1))
		    );
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      range_1ch_long(
		     getIntfromNumber(argv[0]),
		     getIntfromNumber(argv[1]),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1)),
		    
		     getAddr(Listitem(argv[5], i)),
		     getInt(Listitem(argv[6], 0)),
		     getInt(Listitem(argv[6], 1))
		     );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      range_1ch_float(
		      (float) getRealfromNumber(argv[0]),
		      (float) getRealfromNumber(argv[1]),
		      getInt(Listitem(argv[2], 0)),
		      getInt(Listitem(argv[2], 1)),
		    
		      getAddr(Listitem(argv[3], i)),
		      getInt(Listitem(argv[4], 0)),
		      getInt(Listitem(argv[4], 1)),
		    
		      getAddr(Listitem(argv[5], i)),
		      getInt(Listitem(argv[6], 0)),
		      getInt(Listitem(argv[6], 1))
		      );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      range_1ch_double(
		       getRealfromNumber(argv[0]),
		       getRealfromNumber(argv[1]),
		       getInt(Listitem(argv[2], 0)),
		       getInt(Listitem(argv[2], 1)),
		    
		       getAddr(Listitem(argv[3], i)),
		       getInt(Listitem(argv[4], 0)),
		       getInt(Listitem(argv[4], 1)),
		    
		       getAddr(Listitem(argv[5], i)),
		       getInt(Listitem(argv[6], 0)),
		       getInt(Listitem(argv[6], 1))
		       );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_range_3ch(Script *script, 
			char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int chans;
  
  checkCount(11, NULL); 
  checkNumber(0, "lowval0");
  checkNumber(1, "highval0");
  checkNumber(2, "lowval1");
  checkNumber(3, "highval1");
  checkNumber(4, "lowval2");
  checkNumber(5, "highval2");
  checkIntList(6, "size");
  checkAddrList(7, "inbufs");
  checkIntList(8, "instep");
  checkAddrList(9, "outbufs");
  checkIntList(10, "outstep");

  chans = Listsize(argv[7]);
  
  switch(call_data) {

  case PROCESS_BYTE:
    range_3ch_byte(
		   (unsigned char) getIntfromNumber(argv[0]),
		   (unsigned char) getIntfromNumber(argv[1]),
		   (unsigned char) getIntfromNumber(argv[2]),
		   (unsigned char) getIntfromNumber(argv[3]),
		   (unsigned char) getIntfromNumber(argv[4]),
		   (unsigned char) getIntfromNumber(argv[5]),
		   getInt(Listitem(argv[6], 0)),
		   getInt(Listitem(argv[6], 1)),
		    
		   getAddr(Listitem(argv[7], 0)),
		   getAddr(Listitem(argv[7], 1)),
		   getAddr(Listitem(argv[7], 2)),
		   getInt(Listitem(argv[8], 0)),
		   getInt(Listitem(argv[8], 1)),
		    
		   getAddr(Listitem(argv[9], 0)),
		   getAddr(Listitem(argv[9], 1)),
		   getAddr(Listitem(argv[9], 2)),
		   getInt(Listitem(argv[10], 0)),
		   getInt(Listitem(argv[10], 1))
		   );
    break;

  case PROCESS_INT:
    range_3ch_int(
		  getIntfromNumber(argv[0]),
		  getIntfromNumber(argv[1]),
		  getIntfromNumber(argv[2]),
		  getIntfromNumber(argv[3]),
		  getIntfromNumber(argv[4]),
		  getIntfromNumber(argv[5]),
		  getInt(Listitem(argv[6], 0)),
		  getInt(Listitem(argv[6], 1)),
		    
		  getAddr(Listitem(argv[7], 0)),
		  getAddr(Listitem(argv[7], 1)),
		  getAddr(Listitem(argv[7], 2)),
		  getInt(Listitem(argv[8], 0)),
		  getInt(Listitem(argv[8], 1)),
		    
		  getAddr(Listitem(argv[9], 0)),
		  getAddr(Listitem(argv[9], 1)),
		  getAddr(Listitem(argv[9], 2)),
		  getInt(Listitem(argv[10], 0)),
		  getInt(Listitem(argv[10], 1))
		  );
    break;

  case PROCESS_LONG:
    range_3ch_long(
		   getIntfromNumber(argv[0]),
		   getIntfromNumber(argv[1]),
		   getIntfromNumber(argv[2]),
		   getIntfromNumber(argv[3]),
		   getIntfromNumber(argv[4]),
		   getIntfromNumber(argv[5]),
		   getInt(Listitem(argv[6], 0)),
		   getInt(Listitem(argv[6], 1)),
		    
		   getAddr(Listitem(argv[7], 0)),
		   getAddr(Listitem(argv[7], 1)),
		   getAddr(Listitem(argv[7], 2)),
		   getInt(Listitem(argv[8], 0)),
		   getInt(Listitem(argv[8], 1)),
		    
		   getAddr(Listitem(argv[9], 0)),
		   getAddr(Listitem(argv[9], 1)),
		   getAddr(Listitem(argv[9], 2)),
		   getInt(Listitem(argv[10], 0)),
		   getInt(Listitem(argv[10], 1))
		   );
    break;

  case PROCESS_FLOAT:
    range_3ch_float(
		    (float) getRealfromNumber(argv[0]),
		    (float) getRealfromNumber(argv[1]),
		    (float) getRealfromNumber(argv[2]),
		    (float) getRealfromNumber(argv[3]),
		    (float) getRealfromNumber(argv[4]),
		    (float) getRealfromNumber(argv[5]),
		    getInt(Listitem(argv[6], 0)),
		    getInt(Listitem(argv[6], 1)),
		    
		    getAddr(Listitem(argv[7], 0)),
		    getAddr(Listitem(argv[7], 1)),
		    getAddr(Listitem(argv[7], 2)),
		    getInt(Listitem(argv[8], 0)),
		    getInt(Listitem(argv[8], 1)),
		    
		    getAddr(Listitem(argv[9], 0)),
		    getAddr(Listitem(argv[9], 1)),
		    getAddr(Listitem(argv[9], 2)),
		    getInt(Listitem(argv[10], 0)),
		    getInt(Listitem(argv[10], 1))
		    );
    break;

  case PROCESS_DOUBLE:
    range_3ch_double(
		     getRealfromNumber(argv[0]),
		     getRealfromNumber(argv[1]),
		     getRealfromNumber(argv[2]),
		     getRealfromNumber(argv[3]),
		     getRealfromNumber(argv[4]),
		     getRealfromNumber(argv[5]),
		     getInt(Listitem(argv[6], 0)),
		     getInt(Listitem(argv[6], 1)),
		    
		     getAddr(Listitem(argv[7], 0)),
		     getAddr(Listitem(argv[7], 1)),
		     getAddr(Listitem(argv[7], 2)),
		     getInt(Listitem(argv[8], 0)),
		     getInt(Listitem(argv[8], 1)),
		    
		     getAddr(Listitem(argv[9], 0)),
		     getAddr(Listitem(argv[9], 1)),
		     getAddr(Listitem(argv[9], 2)),
		     getInt(Listitem(argv[10], 0)),
		     getInt(Listitem(argv[10], 1))
		     );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_composite(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbuf");
  checkIntList(2, "instep");
  checkAddrList(3, "abuf");
  checkIntList(4, "astep");
  checkAddrList(5, "outbuf");
  checkIntList(6, "outstep");
  
  switch(Listsize(argv[1])) {
    
  case 3:
    composite_3ch(
		  getInt(Listitem(argv[0], 0)),
		  getInt(Listitem(argv[0], 1)),
		  
		  getAddr(Listitem(argv[1], 0)),
		  getAddr(Listitem(argv[1], 1)),
		  getAddr(Listitem(argv[1], 2)),
		  getInt(Listitem(argv[2], 0)),
		  getInt(Listitem(argv[2], 1)),
		  
		  getAddr(Listitem(argv[3], 0)),
		  getInt(Listitem(argv[4], 0)),
		  getInt(Listitem(argv[4], 1)),
		  
		  getAddr(Listitem(argv[5], 0)),
		  getAddr(Listitem(argv[5], 1)),
		  getAddr(Listitem(argv[5], 2)),
		  getInt(Listitem(argv[6], 0)),
		  getInt(Listitem(argv[6], 1))
		  );
    break;

  case 2:
    composite_2ch(
		  getInt(Listitem(argv[0], 0)),
		  getInt(Listitem(argv[0], 1)),
		  
		  getAddr(Listitem(argv[1], 0)),
		  getAddr(Listitem(argv[1], 1)),
		  getInt(Listitem(argv[2], 0)),
		  getInt(Listitem(argv[2], 1)),
		  
		  getAddr(Listitem(argv[3], 0)),
		  getInt(Listitem(argv[4], 0)),
		  getInt(Listitem(argv[4], 1)),
		  
		  getAddr(Listitem(argv[5], 0)),
		  getAddr(Listitem(argv[5], 1)),
		  getInt(Listitem(argv[6], 0)),
		  getInt(Listitem(argv[6], 1))
		  );
    break;
    
  case 1:
    composite_1ch(
		  getInt(Listitem(argv[0], 0)),
		  getInt(Listitem(argv[0], 1)),
		  
		  getAddr(Listitem(argv[1], 0)),
		  getInt(Listitem(argv[2], 0)),
		  getInt(Listitem(argv[2], 1)),
		  
		  getAddr(Listitem(argv[3], 0)),
		  getInt(Listitem(argv[4], 0)),
		  getInt(Listitem(argv[4], 1)),
		  
		  getAddr(Listitem(argv[5], 0)),
		  getInt(Listitem(argv[6], 0)),
		  getInt(Listitem(argv[6], 1))
		  );
    break;
    
  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_composite_constalpha(Script *script, 
				   char *proc_name, int call_data, 
				   int argc, Value **argv)
{
  checkCount(6, NULL); 
  checkInt(0, "alpha");
  checkIntList(1, "size");
  checkAddrList(2, "inbuf");
  checkIntList(3, "instep");
  checkAddrList(4, "outbuf");
  checkIntList(5, "outstep");

  switch(Listsize(argv[2])) {

  case 3:
    composite_constalpha_3ch(
			     getInt(argv[0]),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getAddr(Listitem(argv[2], 1)),
			     getAddr(Listitem(argv[2], 2)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getAddr(Listitem(argv[4], 1)),
			     getAddr(Listitem(argv[4], 2)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1))
			     );
    break;

  case 2:
    composite_constalpha_2ch(
			     getInt(argv[0]),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getAddr(Listitem(argv[2], 1)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getAddr(Listitem(argv[4], 1)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1))
			     );
    break;

  case 1:
    composite_constalpha_1ch(
			     getInt(argv[0]),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1))
			     );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_composite_remapalpha(Script *script, 
				   char *proc_name, int call_data, 
				   int argc, Value **argv)
{
  checkCount(8, NULL); 
  checkAddr(0, "remapbuf");
  checkIntList(1, "size");
  checkAddrList(2, "inbuf");
  checkIntList(3, "instep");
  checkAddrList(4, "abuf");
  checkIntList(5, "astep");
  checkAddrList(6, "outbuf");
  checkIntList(7, "outstep");

  switch(Listsize(argv[2])) {

  case 3:
    composite_remapalpha_3ch(
			     getAddr(argv[0]),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getAddr(Listitem(argv[2], 1)),
			     getAddr(Listitem(argv[2], 2)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),

			     getAddr(Listitem(argv[6], 0)),
			     getAddr(Listitem(argv[6], 1)),
			     getAddr(Listitem(argv[6], 2)),
			     getInt(Listitem(argv[7], 0)),
			     getInt(Listitem(argv[7], 1))
			     );
    break;

  case 2:
    composite_remapalpha_2ch(
			     getAddr(argv[0]),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getAddr(Listitem(argv[2], 1)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),

			     getAddr(Listitem(argv[6], 0)),
			     getAddr(Listitem(argv[6], 1)),
			     getInt(Listitem(argv[7], 0)),
			     getInt(Listitem(argv[7], 1))
			     );
    break;

  case 1:
    composite_remapalpha_1ch(
			     getAddr(argv[0]),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),

			     getAddr(Listitem(argv[6], 0)),
			     getInt(Listitem(argv[7], 0)),
			     getInt(Listitem(argv[7], 1))
			     );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_scale(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  checkCount(8, NULL); 
  checkIntList(0, "insize");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkIntList(3, "outsize");
  checkAddrList(4, "outbufs");
  checkIntList(5, "outstep");
  checkAddr(6, "xlineremapbuf");
  checkAddr(7, "ylineremapbuf");
  
  switch(Listsize(argv[1])) {

  case 1:
    scale_byte_1ch(
		   getInt(Listitem(argv[0], 0)),
		   getInt(Listitem(argv[0], 1)),		     
		   getAddr(Listitem(argv[1], 0)),
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		     
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),		     
		   getAddr(Listitem(argv[4], 0)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),
		     
		   getAddr(argv[6]),
		   getAddr(argv[7])
		   );
    break;

  case 2:
    scale_byte_2ch(
		   getInt(Listitem(argv[0], 0)),
		   getInt(Listitem(argv[0], 1)),		     
		   getAddr(Listitem(argv[1], 0)),
		   getAddr(Listitem(argv[1], 1)),
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		     
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),		     
		   getAddr(Listitem(argv[4], 0)),
		   getAddr(Listitem(argv[4], 1)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),
		     
		   getAddr(argv[6]),
		   getAddr(argv[7])
		   );
    break;

  case 3:
    scale_byte_3ch(
		   getInt(Listitem(argv[0], 0)),
		   getInt(Listitem(argv[0], 1)),		     
		   getAddr(Listitem(argv[1], 0)),
		   getAddr(Listitem(argv[1], 1)),
		   getAddr(Listitem(argv[1], 2)),
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		     
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),		     
		   getAddr(Listitem(argv[4], 0)),
		   getAddr(Listitem(argv[4], 1)),
		   getAddr(Listitem(argv[4], 2)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),
		     
		   getAddr(argv[6]),
		   getAddr(argv[7])
		   );
    break;

  case 4:
    scale_byte_4ch(
		   getInt(Listitem(argv[0], 0)),
		   getInt(Listitem(argv[0], 1)),		     
		   getAddr(Listitem(argv[1], 0)),
		   getAddr(Listitem(argv[1], 1)),
		   getAddr(Listitem(argv[1], 2)),
		   getAddr(Listitem(argv[1], 3)),
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		     
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),		     
		   getAddr(Listitem(argv[4], 0)),
		   getAddr(Listitem(argv[4], 1)),
		   getAddr(Listitem(argv[4], 2)),
		   getAddr(Listitem(argv[4], 3)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),
		     
		   getAddr(argv[6]),
		   getAddr(argv[7])
		   );
    break;

  }
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_scale_range(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  checkCount(10, NULL); 
  checkInt(0, "lowval");
  checkInt(1, "highval");
  checkIntList(2, "insize");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkIntList(5, "outsize");
  checkAddrList(6, "outbufs");
  checkIntList(7, "outstep");
  checkAddr(8, "xlineremapbuf");
  checkAddr(9, "ylineremapbuf");

  switch(Listsize(argv[3])) {

  case 1:
    scale_range_byte_1ch(
			 (unsigned char) getInt(argv[0]),
			 (unsigned char) getInt(argv[1]),
			   
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),		     
			 getAddr(Listitem(argv[3], 0)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),
			   
			 getInt(Listitem(argv[5], 0)),
			 getInt(Listitem(argv[5], 1)),		     
			 getAddr(Listitem(argv[6], 0)),
			 getInt(Listitem(argv[7], 0)),
			 getInt(Listitem(argv[7], 1)),
			   
			 getAddr(argv[8]),
			 getAddr(argv[9])
			 );
    break;

  case 2:
    scale_range_byte_2ch(
			 (unsigned char) getInt(argv[0]),
			 (unsigned char) getInt(argv[1]),
			   
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),		     
			 getAddr(Listitem(argv[3], 0)),
			 getAddr(Listitem(argv[3], 1)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),
			   
			 getInt(Listitem(argv[5], 0)),
			 getInt(Listitem(argv[5], 1)),		     
			 getAddr(Listitem(argv[6], 0)),
			 getAddr(Listitem(argv[6], 1)),
			 getInt(Listitem(argv[7], 0)),
			 getInt(Listitem(argv[7], 1)),
			   
			 getAddr(argv[8]),
			 getAddr(argv[9])
			 );
    break;

  case 3:
    scale_range_byte_3ch(
			 (unsigned char) getInt(argv[0]),
			 (unsigned char) getInt(argv[1]),
			   
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),		     
			 getAddr(Listitem(argv[3], 0)),
			 getAddr(Listitem(argv[3], 1)),
			 getAddr(Listitem(argv[3], 2)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),
			   
			 getInt(Listitem(argv[5], 0)),
			 getInt(Listitem(argv[5], 1)),		     
			 getAddr(Listitem(argv[6], 0)),
			 getAddr(Listitem(argv[6], 1)),
			 getAddr(Listitem(argv[6], 2)),
			 getInt(Listitem(argv[7], 0)),
			 getInt(Listitem(argv[7], 1)),
			   
			 getAddr(argv[8]),
			 getAddr(argv[9])
			 );
    break;

  }
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_scale_composite(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  checkCount(10, NULL); 
  checkIntList(0, "insize");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "abufs");
  checkIntList(4, "astep");
  checkIntList(5, "outsize");
  checkAddrList(6, "outbufs");
  checkIntList(7, "outstep");
  checkAddr(8, "xlineremapbuf");
  checkAddr(9, "ylineremapbuf");
  
  switch(Listsize(argv[6])) {
    
  case 1:
    scale_composite_byte_1ch(
			     getInt(Listitem(argv[0], 0)),
			     getInt(Listitem(argv[0], 1)),		     
			     getAddr(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[2], 0)),
			     getInt(Listitem(argv[2], 1)),

			     getAddr(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[4], 1)),
			       
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),		     
			     getAddr(Listitem(argv[6], 0)),
			     getInt(Listitem(argv[7], 0)),
			     getInt(Listitem(argv[7], 1)),
			       
			     getAddr(argv[8]),
			     getAddr(argv[9])
			     );
    break;

  case 2:
    scale_composite_byte_2ch(
			     getInt(Listitem(argv[0], 0)),
			     getInt(Listitem(argv[0], 1)),		     
			     getAddr(Listitem(argv[1], 0)),
			     getAddr(Listitem(argv[1], 1)),
			     getInt(Listitem(argv[2], 0)),
			     getInt(Listitem(argv[2], 1)),

			     getAddr(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[4], 1)),
			       
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),		     
			     getAddr(Listitem(argv[6], 0)),
			     getAddr(Listitem(argv[6], 1)),
			     getInt(Listitem(argv[7], 0)),
			     getInt(Listitem(argv[7], 1)),
			       
			     getAddr(argv[8]),
			     getAddr(argv[9])
			     );
    break;

  case 3:
    scale_composite_byte_3ch(
			     getInt(Listitem(argv[0], 0)),
			     getInt(Listitem(argv[0], 1)),		     
			     getAddr(Listitem(argv[1], 0)),
			     getAddr(Listitem(argv[1], 1)),
			     getAddr(Listitem(argv[1], 2)),
			     getInt(Listitem(argv[2], 0)),
			     getInt(Listitem(argv[2], 1)),

			     getAddr(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[4], 1)),
			       
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),		     
			     getAddr(Listitem(argv[6], 0)),
			     getAddr(Listitem(argv[6], 1)),
			     getAddr(Listitem(argv[6], 2)),
			     getInt(Listitem(argv[7], 0)),
			     getInt(Listitem(argv[7], 1)),
			       
			     getAddr(argv[8]),
			     getAddr(argv[9])
			     );
    break;

  }
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_scale_composite_constalpha(Script *script, 
					 char *proc_name, int call_data, 
					 int argc, Value **argv)
{
  checkCount(9, NULL); 
  checkInt(0, "alpha");
  checkIntList(1, "insize");
  checkAddrList(2, "inbufs");
  checkIntList(3, "instep");
  checkIntList(4, "outsize");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  checkAddr(7, "xlineremapbuf");
  checkAddr(8, "ylineremapbuf");
  
  switch(Listsize(argv[5])) {
    
  case 1:
    scale_composite_constalpha_byte_1ch(
					getInt(argv[0]),

					getInt(Listitem(argv[1], 0)),
					getInt(Listitem(argv[1], 1)),
					getAddr(Listitem(argv[2], 0)),
					getInt(Listitem(argv[3], 0)),
					getInt(Listitem(argv[3], 1)),
					  
					getInt(Listitem(argv[4], 0)),
					getInt(Listitem(argv[4], 1)),
					getAddr(Listitem(argv[5], 0)),
					getInt(Listitem(argv[6], 0)),
					getInt(Listitem(argv[6], 1)),
					  
					getAddr(argv[7]),
					getAddr(argv[8])
					);
    break;

  case 2:
    scale_composite_constalpha_byte_2ch(
					getInt(argv[0]),

					getInt(Listitem(argv[1], 0)),
					getInt(Listitem(argv[1], 1)),
					getAddr(Listitem(argv[2], 0)),
					getAddr(Listitem(argv[2], 1)),
					getInt(Listitem(argv[3], 0)),
					getInt(Listitem(argv[3], 1)),
					  
					getInt(Listitem(argv[4], 0)),
					getInt(Listitem(argv[4], 1)),
					getAddr(Listitem(argv[5], 0)),
					getAddr(Listitem(argv[5], 1)),
					getInt(Listitem(argv[6], 0)),
					getInt(Listitem(argv[6], 1)),
					  
					getAddr(argv[7]),
					getAddr(argv[8])
					);
    break;

  case 3:
    scale_composite_constalpha_byte_3ch(
					getInt(argv[0]),

					getInt(Listitem(argv[1], 0)),
					getInt(Listitem(argv[1], 1)),
					getAddr(Listitem(argv[2], 0)),
					getAddr(Listitem(argv[2], 1)),
					getAddr(Listitem(argv[2], 2)),
					getInt(Listitem(argv[3], 0)),
					getInt(Listitem(argv[3], 1)),
					  
					getInt(Listitem(argv[4], 0)),
					getInt(Listitem(argv[4], 1)),
					getAddr(Listitem(argv[5], 0)),
					getAddr(Listitem(argv[5], 1)),
					getAddr(Listitem(argv[5], 2)),
					getInt(Listitem(argv[6], 0)),
					getInt(Listitem(argv[6], 1)),
					  
					getAddr(argv[7]),
					getAddr(argv[8])
					);
    break;

  }
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_scale_composite_remapalpha(Script *script, 
					 char *proc_name, int call_data, 
					 int argc, Value **argv)
{
  checkCount(11, NULL); 
  checkAddr(0, "remapbuf");
  checkIntList(1, "insize");
  checkAddrList(2, "inbufs");
  checkIntList(3, "instep");
  checkAddrList(4, "abufs");
  checkIntList(5, "astep");
  checkIntList(6, "outsize");
  checkAddrList(7, "outbufs");
  checkIntList(8, "outstep");
  checkAddr(9, "xlineremapbuf");
  checkAddr(10, "ylineremapbuf");
  
  switch(Listsize(argv[7])) {

  case 1:
    scale_composite_remapalpha_byte_1ch(
					getAddr(argv[0]),

					getInt(Listitem(argv[1], 0)),
					getInt(Listitem(argv[1], 1)),
					getAddr(Listitem(argv[2], 0)),
					getInt(Listitem(argv[3], 0)),
					getInt(Listitem(argv[3], 1)),

					getAddr(Listitem(argv[4], 0)),
					getInt(Listitem(argv[5], 0)),
					getInt(Listitem(argv[5], 1)),
			       
					getInt(Listitem(argv[6], 0)),
					getInt(Listitem(argv[6], 1)),
					getAddr(Listitem(argv[7], 0)),
					getInt(Listitem(argv[8], 0)),
					getInt(Listitem(argv[8], 1)),
			       
					getAddr(argv[9]),
					getAddr(argv[10])
					);
    break;

  case 2:
    scale_composite_remapalpha_byte_2ch(
					getAddr(argv[0]),

					getInt(Listitem(argv[1], 0)),
					getInt(Listitem(argv[1], 1)),
					getAddr(Listitem(argv[2], 0)),
					getAddr(Listitem(argv[2], 1)),
					getInt(Listitem(argv[3], 0)),
					getInt(Listitem(argv[3], 1)),

					getAddr(Listitem(argv[4], 0)),
					getInt(Listitem(argv[5], 0)),
					getInt(Listitem(argv[5], 1)),
			       
					getInt(Listitem(argv[6], 0)),
					getInt(Listitem(argv[6], 1)),
					getAddr(Listitem(argv[7], 0)),
					getAddr(Listitem(argv[7], 1)),
					getInt(Listitem(argv[8], 0)),
					getInt(Listitem(argv[8], 1)),
			       
					getAddr(argv[9]),
					getAddr(argv[10])
					);
    break;

  case 3:
    scale_composite_remapalpha_byte_3ch(
					getAddr(argv[0]),

					getInt(Listitem(argv[1], 0)),
					getInt(Listitem(argv[1], 1)),
					getAddr(Listitem(argv[2], 0)),
					getAddr(Listitem(argv[2], 1)),
					getAddr(Listitem(argv[2], 2)),
					getInt(Listitem(argv[3], 0)),
					getInt(Listitem(argv[3], 1)),

					getAddr(Listitem(argv[4], 0)),
					getInt(Listitem(argv[5], 0)),
					getInt(Listitem(argv[5], 1)),
			       
					getInt(Listitem(argv[6], 0)),
					getInt(Listitem(argv[6], 1)),
					getAddr(Listitem(argv[7], 0)),
					getAddr(Listitem(argv[7], 1)),
					getAddr(Listitem(argv[7], 2)),
					getInt(Listitem(argv[8], 0)),
					getInt(Listitem(argv[8], 1)),
			       
					getAddr(argv[9]),
					getAddr(argv[10])
					);
    break;

  }
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_convert_byte(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  int i, chans;
  
  checkCount(5, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");

  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      transfer_byte(
		    getInt(Listitem(argv[0], 0)),
		    getInt(Listitem(argv[0], 1)),
		    
		    getAddr(Listitem(argv[1], i)),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1))
		    );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      convert_byte_short(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      convert_byte_int(
		       getInt(Listitem(argv[0], 0)),
		       getInt(Listitem(argv[0], 1)),
		       
		       getAddr(Listitem(argv[1], i)),
		       getInt(Listitem(argv[2], 0)),
		       getInt(Listitem(argv[2], 1)),
		       
		       getAddr(Listitem(argv[3], i)),
		       getInt(Listitem(argv[4], 0)),
		       getInt(Listitem(argv[4], 1))
		       );
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      convert_byte_long(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
			
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
			
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1))
			);
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      convert_byte_float(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      convert_byte_double(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1))
			  );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_convert_short(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  int i, chans;
  
  checkCount(5, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");

  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      convert_short_byte(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      transfer_short(
		     getInt(Listitem(argv[0], 0)),
		     getInt(Listitem(argv[0], 1)),
		    
		     getAddr(Listitem(argv[1], i)),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1))
		     );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      convert_short_int(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		       
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		       
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1))
			);
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      convert_short_long(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
			
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
			
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      convert_short_float(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1))
			  );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      convert_short_double(
			   getInt(Listitem(argv[0], 0)),
			   getInt(Listitem(argv[0], 1)),
		    
			   getAddr(Listitem(argv[1], i)),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1))
			   );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_convert_int(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  int i, chans;
  
  checkCount(5, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");

  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      convert_int_byte(
		       getInt(Listitem(argv[0], 0)),
		       getInt(Listitem(argv[0], 1)),
		    
		       getAddr(Listitem(argv[1], i)),
		       getInt(Listitem(argv[2], 0)),
		       getInt(Listitem(argv[2], 1)),
		    
		       getAddr(Listitem(argv[3], i)),
		       getInt(Listitem(argv[4], 0)),
		       getInt(Listitem(argv[4], 1))
		       );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      convert_int_short(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		    
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1))
			);
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      transfer_int(
		   getInt(Listitem(argv[0], 0)),
		   getInt(Listitem(argv[0], 1)),
		       
		   getAddr(Listitem(argv[1], i)),
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		       
		   getAddr(Listitem(argv[3], i)),
		   getInt(Listitem(argv[4], 0)),
		   getInt(Listitem(argv[4], 1))
		   );
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      convert_int_long(
		       getInt(Listitem(argv[0], 0)),
		       getInt(Listitem(argv[0], 1)),
			
		       getAddr(Listitem(argv[1], i)),
		       getInt(Listitem(argv[2], 0)),
		       getInt(Listitem(argv[2], 1)),
			
		       getAddr(Listitem(argv[3], i)),
		       getInt(Listitem(argv[4], 0)),
		       getInt(Listitem(argv[4], 1))
		       );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      convert_int_float(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		    
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1))
			);
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      convert_int_double(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_convert_long(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  int i, chans;
  
  checkCount(5, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");

  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      convert_long_byte(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		    
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1))
			);
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      convert_long_short(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      convert_long_int(
		       getInt(Listitem(argv[0], 0)),
		       getInt(Listitem(argv[0], 1)),
		       
		       getAddr(Listitem(argv[1], i)),
		       getInt(Listitem(argv[2], 0)),
		       getInt(Listitem(argv[2], 1)),
		       
		       getAddr(Listitem(argv[3], i)),
		       getInt(Listitem(argv[4], 0)),
		       getInt(Listitem(argv[4], 1))
		       );
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      transfer_long(
		    getInt(Listitem(argv[0], 0)),
		    getInt(Listitem(argv[0], 1)),
			
		    getAddr(Listitem(argv[1], i)),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
			
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1))
		    );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      convert_long_float(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      convert_long_double(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1))
			  );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_convert_float(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  int i, chans;
  
  checkCount(5, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");

  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      convert_float_byte(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      convert_float_short(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1))
			  );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      convert_float_int(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		       
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		       
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1))
			);
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      convert_float_long(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
			
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
			
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      transfer_float(
		     getInt(Listitem(argv[0], 0)),
		     getInt(Listitem(argv[0], 1)),
		    
		     getAddr(Listitem(argv[1], i)),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1))
		     );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      convert_float_double(
			   getInt(Listitem(argv[0], 0)),
			   getInt(Listitem(argv[0], 1)),
		    
			   getAddr(Listitem(argv[1], i)),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1))
			   );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_convert_double(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int i, chans;
  
  checkCount(5, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");

  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      convert_double_byte(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1))
			  );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      convert_double_short(
			   getInt(Listitem(argv[0], 0)),
			   getInt(Listitem(argv[0], 1)),
		    
			   getAddr(Listitem(argv[1], i)),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1))
			   );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      convert_double_int(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		       
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		       
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1))
			 );
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      convert_double_long(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
			
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
			
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1))
			  );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      convert_double_float(
			   getInt(Listitem(argv[0], 0)),
			   getInt(Listitem(argv[0], 1)),
		    
			   getAddr(Listitem(argv[1], i)),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1))
			   );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      transfer_double(
		      getInt(Listitem(argv[0], 0)),
		      getInt(Listitem(argv[0], 1)),
		    
		      getAddr(Listitem(argv[1], i)),
		      getInt(Listitem(argv[2], 0)),
		      getInt(Listitem(argv[2], 1)),
		    
		      getAddr(Listitem(argv[3], i)),
		      getInt(Listitem(argv[4], 0)),
		      getInt(Listitem(argv[4], 1))
		      );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_gainbias_byte(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkNumber(0, "gain");
  checkNumber(1, "bias");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");

  chans = Listsize(argv[3]);
  
  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      gainbias_byte(
		    getRealfromNumber(argv[0]),
		    getRealfromNumber(argv[1]),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1)),
		    
		    getAddr(Listitem(argv[5], i)),
		    getInt(Listitem(argv[6], 0)),
		    getInt(Listitem(argv[6], 1))
		    );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      gainbias_byte_short(
			  getRealfromNumber(argv[0]),
			  getRealfromNumber(argv[1]),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1)),
		    
			  getAddr(Listitem(argv[5], i)),
			  getInt(Listitem(argv[6], 0)),
			  getInt(Listitem(argv[6], 1))
			  );
    break;


  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      gainbias_byte_int(
			getRealfromNumber(argv[0]),
			getRealfromNumber(argv[1]),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1)),
		    
			getAddr(Listitem(argv[5], i)),
			getInt(Listitem(argv[6], 0)),
			getInt(Listitem(argv[6], 1))
			);
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      gainbias_byte_long(
			 getRealfromNumber(argv[0]),
			 getRealfromNumber(argv[1]),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),
		    
			 getAddr(Listitem(argv[5], i)),
			 getInt(Listitem(argv[6], 0)),
			 getInt(Listitem(argv[6], 1))
			 );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      gainbias_byte_float(
			  getRealfromNumber(argv[0]),
			  getRealfromNumber(argv[1]),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1)),
		    
			  getAddr(Listitem(argv[5], i)),
			  getInt(Listitem(argv[6], 0)),
			  getInt(Listitem(argv[6], 1))
			  );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      gainbias_byte_double(
			   getRealfromNumber(argv[0]),
			   getRealfromNumber(argv[1]),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1)),
		    
			   getAddr(Listitem(argv[5], i)),
			   getInt(Listitem(argv[6], 0)),
			   getInt(Listitem(argv[6], 1))
			   );
    break;

  default:
    fprintf(stderr, "** %s : Unsupported type conversion\n", proc_name);
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_gainbias_short(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkNumber(0, "gain");
  checkNumber(1, "bias");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");

  chans = Listsize(argv[3]);
  
  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      gainbias_short_byte(
			  getRealfromNumber(argv[0]),
			  getRealfromNumber(argv[1]),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1)),
		    
			  getAddr(Listitem(argv[5], i)),
			  getInt(Listitem(argv[6], 0)),
			  getInt(Listitem(argv[6], 1))
			  );
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      gainbias_short(
		     getRealfromNumber(argv[0]),
		     getRealfromNumber(argv[1]),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1)),
		    
		     getAddr(Listitem(argv[5], i)),
		     getInt(Listitem(argv[6], 0)),
		     getInt(Listitem(argv[6], 1))
		     );
    break;


  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      gainbias_short_int(
			 getRealfromNumber(argv[0]),
			 getRealfromNumber(argv[1]),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),
		    
			 getAddr(Listitem(argv[5], i)),
			 getInt(Listitem(argv[6], 0)),
			 getInt(Listitem(argv[6], 1))
			 );
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      gainbias_short_long(
			  getRealfromNumber(argv[0]),
			  getRealfromNumber(argv[1]),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1)),
		    
			  getAddr(Listitem(argv[5], i)),
			  getInt(Listitem(argv[6], 0)),
			  getInt(Listitem(argv[6], 1))
			  );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      gainbias_short_float(
			   getRealfromNumber(argv[0]),
			   getRealfromNumber(argv[1]),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1)),
		    
			   getAddr(Listitem(argv[5], i)),
			   getInt(Listitem(argv[6], 0)),
			   getInt(Listitem(argv[6], 1))
			   );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      gainbias_short_double(
			    getRealfromNumber(argv[0]),
			    getRealfromNumber(argv[1]),
			    getInt(Listitem(argv[2], 0)),
			    getInt(Listitem(argv[2], 1)),
		    
			    getAddr(Listitem(argv[3], i)),
			    getInt(Listitem(argv[4], 0)),
			    getInt(Listitem(argv[4], 1)),
		    
			    getAddr(Listitem(argv[5], i)),
			    getInt(Listitem(argv[6], 0)),
			    getInt(Listitem(argv[6], 1))
			    );
    break;

  default:
    fprintf(stderr, "** %s : Unsupported type conversion\n", proc_name);
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_gainbias_int(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkNumber(0, "gain");
  checkNumber(1, "bias");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");

  chans = Listsize(argv[3]);
  
  switch(call_data) {

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      gainbias_int(
		   getRealfromNumber(argv[0]),
		   getRealfromNumber(argv[1]),
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		    
		   getAddr(Listitem(argv[3], i)),
		   getInt(Listitem(argv[4], 0)),
		   getInt(Listitem(argv[4], 1)),
		    
		   getAddr(Listitem(argv[5], i)),
		   getInt(Listitem(argv[6], 0)),
		   getInt(Listitem(argv[6], 1))
		   );
    break;

  default:
    fprintf(stderr, "** %s : Unsupported type conversion\n", proc_name);
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_gainbias_long(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkNumber(0, "gain");
  checkNumber(1, "bias");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");

  chans = Listsize(argv[3]);
  
  switch(call_data) {

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      gainbias_long(
		    getRealfromNumber(argv[0]),
		    getRealfromNumber(argv[1]),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1)),
		    
		    getAddr(Listitem(argv[5], i)),
		    getInt(Listitem(argv[6], 0)),
		    getInt(Listitem(argv[6], 1))
		    );
    break;

  default:
    fprintf(stderr, "** %s : Unsupported type conversion\n", proc_name);
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_gainbias_float(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkNumber(0, "gain");
  checkNumber(1, "bias");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");

  chans = Listsize(argv[3]);
  
  switch(call_data) {

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      gainbias_float(
		     (float) getRealfromNumber(argv[0]),
		     (float) getRealfromNumber(argv[1]),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1)),
		    
		     getAddr(Listitem(argv[5], i)),
		     getInt(Listitem(argv[6], 0)),
		     getInt(Listitem(argv[6], 1))
		     );
    break;

  default:
    fprintf(stderr, "** %s : Unsupported type conversion\n", proc_name);
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_gainbias_double(Script *script, 
			      char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkCount(7, NULL); 
  checkNumber(0, "gain");
  checkNumber(1, "bias");
  checkIntList(2, "size");
  checkAddrList(3, "inbufs");
  checkIntList(4, "instep");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");

  chans = Listsize(argv[3]);
  
  switch(call_data) {

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      gainbias_double(
		      getRealfromNumber(argv[0]),
		      getRealfromNumber(argv[1]),
		      getInt(Listitem(argv[2], 0)),
		      getInt(Listitem(argv[2], 1)),
		    
		      getAddr(Listitem(argv[3], i)),
		      getInt(Listitem(argv[4], 0)),
		      getInt(Listitem(argv[4], 1)),
		    
		      getAddr(Listitem(argv[5], i)),
		      getInt(Listitem(argv[6], 0)),
		      getInt(Listitem(argv[6], 1))
		      );
    break;

  default:
    fprintf(stderr, "** %s : Unsupported type conversion\n", proc_name);
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_remap_intensity(Script *script, 
			      char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  int i, chans;
  
  checkCount(6, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");
  checkAddr(5, "remap");

  chans = Listsize(argv[1]);

  for(i = 0; i < chans; i++)
    remap_intensity(
		    getInt(Listitem(argv[0], 0)),
		    getInt(Listitem(argv[0], 1)),
		    
		    getAddr(Listitem(argv[1], i)),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1)),
		    
		    getAddr(argv[5])
		    );

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_fill_constant(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  int i, chans;
  
  checkCount(4, NULL); 
  checkNumber(0, "constant");
  checkIntList(1, "size");
  checkAddrList(2, "bufs");
  checkIntList(3, "step");

  chans = Listsize(argv[2]);
  
  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      fill_constant_byte(
			 (unsigned char) getIntfromNumber(argv[0]),
			 getInt(Listitem(argv[1], 0)),
			 getInt(Listitem(argv[1], 1)),
		    
			 getAddr(Listitem(argv[2], i)),
			 getInt(Listitem(argv[3], 0)),
			 getInt(Listitem(argv[3], 1))
			 );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      fill_constant_int(
			getIntfromNumber(argv[0]),
			getInt(Listitem(argv[1], 0)),
			getInt(Listitem(argv[1], 1)),
		    
			getAddr(Listitem(argv[2], i)),
			getInt(Listitem(argv[3], 0)),
			getInt(Listitem(argv[3], 1))
			);
    break;

  case PROCESS_LONG:
    for(i = 0; i < chans; i++)
      fill_constant_long(
			 getIntfromNumber(argv[0]),
			 getInt(Listitem(argv[1], 0)),
			 getInt(Listitem(argv[1], 1)),
		    
			 getAddr(Listitem(argv[2], i)),
			 getInt(Listitem(argv[3], 0)),
			 getInt(Listitem(argv[3], 1))
			 );
    break;

  case PROCESS_FLOAT:
    for(i = 0; i < chans; i++)
      fill_constant_float(
			  (float) getRealfromNumber(argv[0]),
			  getInt(Listitem(argv[1], 0)),
			  getInt(Listitem(argv[1], 1)),
		    
			  getAddr(Listitem(argv[2], i)),
			  getInt(Listitem(argv[3], 0)),
			  getInt(Listitem(argv[3], 1))
			  );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      fill_constant_double(
			   getRealfromNumber(argv[0]),
			   getInt(Listitem(argv[1], 0)),
			   getInt(Listitem(argv[1], 1)),
		    
			   getAddr(Listitem(argv[2], i)),
			   getInt(Listitem(argv[3], 0)),
			   getInt(Listitem(argv[3], 1))
			   );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_add(Script *script, 
		  char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "in1bufs");
  checkIntList(2, "in1step");
  checkAddrList(3, "in2bufs");
  checkIntList(4, "in2step");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  
  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      image_add_byte(
		     getInt(Listitem(argv[0], 0)),
		     getInt(Listitem(argv[0], 1)),
		    
		     getAddr(Listitem(argv[1], i)),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1)),

		     getAddr(Listitem(argv[5], i)),
		     getInt(Listitem(argv[6], 0)),
		     getInt(Listitem(argv[6], 1))
		     );
    break;

  case PROCESS_BYTE_INT_BYTE:
    for(i = 0; i < chans; i++)
      image_add_byte_int_byte(
			      getInt(Listitem(argv[0], 0)),
			      getInt(Listitem(argv[0], 1)),
		    
			      getAddr(Listitem(argv[1], i)),
			      getInt(Listitem(argv[2], 0)),
			      getInt(Listitem(argv[2], 1)),
		    
			      getAddr(Listitem(argv[3], i)),
			      getInt(Listitem(argv[4], 0)),
			      getInt(Listitem(argv[4], 1)),

			      getAddr(Listitem(argv[5], i)),
			      getInt(Listitem(argv[6], 0)),
			      getInt(Listitem(argv[6], 1))
			      );
    break;

  case PROCESS_BYTE_INT_INT:
    for(i = 0; i < chans; i++)
      image_add_byte_int_int(
			     getInt(Listitem(argv[0], 0)),
			     getInt(Listitem(argv[0], 1)),
			     
			     getAddr(Listitem(argv[1], i)),
			     getInt(Listitem(argv[2], 0)),
			     getInt(Listitem(argv[2], 1)),
			     
			     getAddr(Listitem(argv[3], i)),
			     getInt(Listitem(argv[4], 0)),
			     getInt(Listitem(argv[4], 1)),
			     
			     getAddr(Listitem(argv[5], i)),
			     getInt(Listitem(argv[6], 0)),
			     getInt(Listitem(argv[6], 1))
			     );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      image_add_int(
		    getInt(Listitem(argv[0], 0)),
		    getInt(Listitem(argv[0], 1)),
		    
		    getAddr(Listitem(argv[1], i)),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1)),

		    getAddr(Listitem(argv[5], i)),
		    getInt(Listitem(argv[6], 0)),
		    getInt(Listitem(argv[6], 1))
		    );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      image_add_double(
		       getInt(Listitem(argv[0], 0)),
		       getInt(Listitem(argv[0], 1)),
		    
		       getAddr(Listitem(argv[1], i)),
		       getInt(Listitem(argv[2], 0)),
		       getInt(Listitem(argv[2], 1)),
		    
		       getAddr(Listitem(argv[3], i)),
		       getInt(Listitem(argv[4], 0)),
		       getInt(Listitem(argv[4], 1)),

		       getAddr(Listitem(argv[5], i)),
		       getInt(Listitem(argv[6], 0)),
		       getInt(Listitem(argv[6], 1))
		       );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_subtract(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "in1bufs");
  checkIntList(2, "in1step");
  checkAddrList(3, "in2bufs");
  checkIntList(4, "in2step");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  
  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      image_subtract_byte(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1)),

			  getAddr(Listitem(argv[5], i)),
			  getInt(Listitem(argv[6], 0)),
			  getInt(Listitem(argv[6], 1))
			  );
    break;

  case PROCESS_BYTE_INT_BYTE:
    for(i = 0; i < chans; i++)
      image_subtract_byte_int_byte(
				   getInt(Listitem(argv[0], 0)),
				   getInt(Listitem(argv[0], 1)),
		    
				   getAddr(Listitem(argv[1], i)),
				   getInt(Listitem(argv[2], 0)),
				   getInt(Listitem(argv[2], 1)),
		    
				   getAddr(Listitem(argv[3], i)),
				   getInt(Listitem(argv[4], 0)),
				   getInt(Listitem(argv[4], 1)),

				   getAddr(Listitem(argv[5], i)),
				   getInt(Listitem(argv[6], 0)),
				   getInt(Listitem(argv[6], 1))
				   );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      image_subtract_int(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),

			 getAddr(Listitem(argv[5], i)),
			 getInt(Listitem(argv[6], 0)),
			 getInt(Listitem(argv[6], 1))
			 );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      image_subtract_double(
			    getInt(Listitem(argv[0], 0)),
			    getInt(Listitem(argv[0], 1)),
		    
			    getAddr(Listitem(argv[1], i)),
			    getInt(Listitem(argv[2], 0)),
			    getInt(Listitem(argv[2], 1)),
		    
			    getAddr(Listitem(argv[3], i)),
			    getInt(Listitem(argv[4], 0)),
			    getInt(Listitem(argv[4], 1)),

			    getAddr(Listitem(argv[5], i)),
			    getInt(Listitem(argv[6], 0)),
			    getInt(Listitem(argv[6], 1))
			    );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_diff(Script *script, 
		   char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "in1bufs");
  checkIntList(2, "in1step");
  checkAddrList(3, "in2bufs");
  checkIntList(4, "in2step");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  
  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      image_diff_byte(
		      getInt(Listitem(argv[0], 0)),
		      getInt(Listitem(argv[0], 1)),
		    
		      getAddr(Listitem(argv[1], i)),
		      getInt(Listitem(argv[2], 0)),
		      getInt(Listitem(argv[2], 1)),
		    
		      getAddr(Listitem(argv[3], i)),
		      getInt(Listitem(argv[4], 0)),
		      getInt(Listitem(argv[4], 1)),

		      getAddr(Listitem(argv[5], i)),
		      getInt(Listitem(argv[6], 0)),
		      getInt(Listitem(argv[6], 1))
		      );
    break;

  case PROCESS_BYTE_INT_BYTE:
    for(i = 0; i < chans; i++)
      image_diff_byte_int_byte(
			       getInt(Listitem(argv[0], 0)),
			       getInt(Listitem(argv[0], 1)),
		    
			       getAddr(Listitem(argv[1], i)),
			       getInt(Listitem(argv[2], 0)),
			       getInt(Listitem(argv[2], 1)),
		    
			       getAddr(Listitem(argv[3], i)),
			       getInt(Listitem(argv[4], 0)),
			       getInt(Listitem(argv[4], 1)),

			       getAddr(Listitem(argv[5], i)),
			       getInt(Listitem(argv[6], 0)),
			       getInt(Listitem(argv[6], 1))
			       );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      image_diff_int(
		     getInt(Listitem(argv[0], 0)),
		     getInt(Listitem(argv[0], 1)),
		    
		     getAddr(Listitem(argv[1], i)),
		     getInt(Listitem(argv[2], 0)),
		     getInt(Listitem(argv[2], 1)),
		    
		     getAddr(Listitem(argv[3], i)),
		     getInt(Listitem(argv[4], 0)),
		     getInt(Listitem(argv[4], 1)),

		     getAddr(Listitem(argv[5], i)),
		     getInt(Listitem(argv[6], 0)),
		     getInt(Listitem(argv[6], 1))
		     );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      image_diff_double(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		    
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1)),

			getAddr(Listitem(argv[5], i)),
			getInt(Listitem(argv[6], 0)),
			getInt(Listitem(argv[6], 1))
			);
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_multiply(Script *script, 
		       char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "in1bufs");
  checkIntList(2, "in1step");
  checkAddrList(3, "in2bufs");
  checkIntList(4, "in2step");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  
  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      image_multiply_byte(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1)),

			  getAddr(Listitem(argv[5], i)),
			  getInt(Listitem(argv[6], 0)),
			  getInt(Listitem(argv[6], 1))
			  );
    break;

  case PROCESS_BYTE_INT_BYTE:
    for(i = 0; i < chans; i++)
      image_multiply_byte_int_byte(
				   getInt(Listitem(argv[0], 0)),
				   getInt(Listitem(argv[0], 1)),
		    
				   getAddr(Listitem(argv[1], i)),
				   getInt(Listitem(argv[2], 0)),
				   getInt(Listitem(argv[2], 1)),
		    
				   getAddr(Listitem(argv[3], i)),
				   getInt(Listitem(argv[4], 0)),
				   getInt(Listitem(argv[4], 1)),

				   getAddr(Listitem(argv[5], i)),
				   getInt(Listitem(argv[6], 0)),
				   getInt(Listitem(argv[6], 1))
				   );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      image_multiply_int(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),

			 getAddr(Listitem(argv[5], i)),
			 getInt(Listitem(argv[6], 0)),
			 getInt(Listitem(argv[6], 1))
			 );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      image_multiply_double(
			    getInt(Listitem(argv[0], 0)),
			    getInt(Listitem(argv[0], 1)),
		    
			    getAddr(Listitem(argv[1], i)),
			    getInt(Listitem(argv[2], 0)),
			    getInt(Listitem(argv[2], 1)),
		    
			    getAddr(Listitem(argv[3], i)),
			    getInt(Listitem(argv[4], 0)),
			    getInt(Listitem(argv[4], 1)),

			    getAddr(Listitem(argv[5], i)),
			    getInt(Listitem(argv[6], 0)),
			    getInt(Listitem(argv[6], 1))
			    );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_divide(Script *script, 
		     char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "in1bufs");
  checkIntList(2, "in1step");
  checkAddrList(3, "in2bufs");
  checkIntList(4, "in2step");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  
  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      image_divide_byte(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		    
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1)),

			getAddr(Listitem(argv[5], i)),
			getInt(Listitem(argv[6], 0)),
			getInt(Listitem(argv[6], 1))
			);
    break;

  case PROCESS_BYTE_INT_BYTE:
    for(i = 0; i < chans; i++)
      image_divide_byte_int_byte(
				 getInt(Listitem(argv[0], 0)),
				 getInt(Listitem(argv[0], 1)),
		    
				 getAddr(Listitem(argv[1], i)),
				 getInt(Listitem(argv[2], 0)),
				 getInt(Listitem(argv[2], 1)),
		    
				 getAddr(Listitem(argv[3], i)),
				 getInt(Listitem(argv[4], 0)),
				 getInt(Listitem(argv[4], 1)),

				 getAddr(Listitem(argv[5], i)),
				 getInt(Listitem(argv[6], 0)),
				 getInt(Listitem(argv[6], 1))
				 );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      image_divide_int(
		       getInt(Listitem(argv[0], 0)),
		       getInt(Listitem(argv[0], 1)),
		    
		       getAddr(Listitem(argv[1], i)),
		       getInt(Listitem(argv[2], 0)),
		       getInt(Listitem(argv[2], 1)),
		    
		       getAddr(Listitem(argv[3], i)),
		       getInt(Listitem(argv[4], 0)),
		       getInt(Listitem(argv[4], 1)),

		       getAddr(Listitem(argv[5], i)),
		       getInt(Listitem(argv[6], 0)),
		       getInt(Listitem(argv[6], 1))
		       );
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      image_divide_double(
			  getInt(Listitem(argv[0], 0)),
			  getInt(Listitem(argv[0], 1)),
		    
			  getAddr(Listitem(argv[1], i)),
			  getInt(Listitem(argv[2], 0)),
			  getInt(Listitem(argv[2], 1)),
		    
			  getAddr(Listitem(argv[3], i)),
			  getInt(Listitem(argv[4], 0)),
			  getInt(Listitem(argv[4], 1)),

			  getAddr(Listitem(argv[5], i)),
			  getInt(Listitem(argv[6], 0)),
			  getInt(Listitem(argv[6], 1))
			  );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_maximum(Script *script, 
		      char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "in1bufs");
  checkIntList(2, "in1step");
  checkAddrList(3, "in2bufs");
  checkIntList(4, "in2step");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  
  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      image_maximum_byte(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),

			 getAddr(Listitem(argv[5], i)),
			 getInt(Listitem(argv[6], 0)),
			 getInt(Listitem(argv[6], 1))
			 );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      image_maximum_int(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		    
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1)),

			getAddr(Listitem(argv[5], i)),
			getInt(Listitem(argv[6], 0)),
			getInt(Listitem(argv[6], 1))
			);
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      image_maximum_double(
			   getInt(Listitem(argv[0], 0)),
			   getInt(Listitem(argv[0], 1)),
		    
			   getAddr(Listitem(argv[1], i)),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1)),

			   getAddr(Listitem(argv[5], i)),
			   getInt(Listitem(argv[6], 0)),
			   getInt(Listitem(argv[6], 1))
			   );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_minimum(Script *script, 
		      char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  int i, chans;
  
  checkCount(7, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "in1bufs");
  checkIntList(2, "in1step");
  checkAddrList(3, "in2bufs");
  checkIntList(4, "in2step");
  checkAddrList(5, "outbufs");
  checkIntList(6, "outstep");
  
  chans = Listsize(argv[1]);

  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      image_minimum_byte(
			 getInt(Listitem(argv[0], 0)),
			 getInt(Listitem(argv[0], 1)),
		    
			 getAddr(Listitem(argv[1], i)),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
		    
			 getAddr(Listitem(argv[3], i)),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),

			 getAddr(Listitem(argv[5], i)),
			 getInt(Listitem(argv[6], 0)),
			 getInt(Listitem(argv[6], 1))
			 );
    break;

  case PROCESS_INT:
    for(i = 0; i < chans; i++)
      image_minimum_int(
			getInt(Listitem(argv[0], 0)),
			getInt(Listitem(argv[0], 1)),
		    
			getAddr(Listitem(argv[1], i)),
			getInt(Listitem(argv[2], 0)),
			getInt(Listitem(argv[2], 1)),
		    
			getAddr(Listitem(argv[3], i)),
			getInt(Listitem(argv[4], 0)),
			getInt(Listitem(argv[4], 1)),

			getAddr(Listitem(argv[5], i)),
			getInt(Listitem(argv[6], 0)),
			getInt(Listitem(argv[6], 1))
			);
    break;

  case PROCESS_DOUBLE:
    for(i = 0; i < chans; i++)
      image_minimum_double(
			   getInt(Listitem(argv[0], 0)),
			   getInt(Listitem(argv[0], 1)),
		    
			   getAddr(Listitem(argv[1], i)),
			   getInt(Listitem(argv[2], 0)),
			   getInt(Listitem(argv[2], 1)),
		    
			   getAddr(Listitem(argv[3], i)),
			   getInt(Listitem(argv[4], 0)),
			   getInt(Listitem(argv[4], 1)),

			   getAddr(Listitem(argv[5], i)),
			   getInt(Listitem(argv[6], 0)),
			   getInt(Listitem(argv[6], 1))
			   );
    break;

  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_filter_1d(Script *script, 
			char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int i, chans, numtaps;
  double taplist[256];

  checkCount(8, NULL); 
  checkRealList(0, "taplist");
  checkInt(1, "centertap");
  checkBool(2, "mirror?");
  checkIntList(3, "size");
  checkAddrList(4, "inbuf");
  checkIntList(5, "instep");
  checkAddrList(6, "outbuf");
  checkIntList(7, "outstep");

  chans = Listsize(argv[4]);
  
  numtaps = Listsize(argv[0]);
  if(numtaps > 256) {
    fprintf(stderr, "** %s: Maximum number of taps is 256.\n", proc_name);
    numtaps = 256;
  }
  getRealList(argv[0], taplist, 256);

  switch(call_data) {
    
  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      filter_1d(
		numtaps,
		taplist,
		getInt(argv[1]),
		trueValue(argv[2]),
		
		getInt(Listitem(argv[3], 0)),
		getInt(Listitem(argv[3], 1)),
		
		getAddr(Listitem(argv[4], i)),
		getInt(Listitem(argv[5], 0)),
		getInt(Listitem(argv[5], 1)),
		
		getAddr(Listitem(argv[6], i)),
		getInt(Listitem(argv[7], 0)),
		getInt(Listitem(argv[7], 1))
		);
    break;

  case PROCESS_SHORT:
    for(i = 0; i < chans; i++)
      filter_1d_short(
		      numtaps,
		      taplist,
		      getInt(argv[1]),
		      trueValue(argv[2]),
		
		      getInt(Listitem(argv[3], 0)),
		      getInt(Listitem(argv[3], 1)),
		
		      getAddr(Listitem(argv[4], i)),
		      getInt(Listitem(argv[5], 0)),
		      getInt(Listitem(argv[5], 1)),
		
		      getAddr(Listitem(argv[6], i)),
		      getInt(Listitem(argv[7], 0)),
		      getInt(Listitem(argv[7], 1))
		      );
    break;
  }
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_filter_2d(Script *script, 
			char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int i, chans, numxtaps, numytaps;
  double taplist[256];

  checkCount(8, NULL); 
  checkList(0, "2d taplist");
  checkIntList(1, "centertap");
  checkBool(2, "mirror?");
  checkIntList(3, "size");
  checkAddrList(4, "inbuf");
  checkIntList(5, "instep");
  checkAddrList(6, "outbuf");
  checkIntList(7, "outstep");

  chans = Listsize(argv[4]);

  numytaps = Listsize(argv[0]);
  if( !isRealList(Listitem(argv[0], 0)) ) {
    fprintf(stderr, "** %s: Taps should be list of lists of REAL numbers\n", 
	    proc_name);
    return NullValue;
  }
  numxtaps = Listsize(Listitem(argv[0], 0));
  
  if(numxtaps * numytaps > 256) {
    fprintf(stderr, "** %s : Maximum number of taps is 256.\n", proc_name);
    return NullValue;
  }
  
  for(i = 0; i < Listsize(argv[0]); i++) {
    if( !isRealList(Listitem(argv[0], i)) ||
        Listsize(Listitem(argv[0], i)) != numxtaps ) {
      fprintf(stderr, "** %s : Horizontal tap lists are not same length\n", 
	      proc_name);
      return NullValue;
    }
  }
  
  for(i = 0; i < Listsize(argv[0]); i++)
    getRealList(Listitem(argv[0], i), &taplist[i*numxtaps], numxtaps);

  /* for(i = 0; i < numxtaps * numytaps; i++) printf("%d ", taplist[i]); */
  
  switch(call_data) {

  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      filter_2d(
		numxtaps,
		numytaps,
		taplist,
		getInt(Listitem(argv[1], 0)),
		getInt(Listitem(argv[1], 1)),
		trueValue(argv[2]),
		
		getInt(Listitem(argv[3], 0)),
		getInt(Listitem(argv[3], 1)),
		
		getAddr(Listitem(argv[4], i)),
		getInt(Listitem(argv[5], 0)),
		getInt(Listitem(argv[5], 1)),
		
		getAddr(Listitem(argv[6], i)),
		getInt(Listitem(argv[7], 0)),
		getInt(Listitem(argv[7], 1))
		);
    break;

  }
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_affine_trans(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  int i, chans;

  checkCount(8, NULL); 
  checkRealList(0, "affine parameters [a b c d e f]");
  checkBool(1, "reverse?");
  checkIntList(2, "insize");
  checkAddrList(3, "inbufs");
  checkIntList(4, "insteps");
  checkIntList(5, "outsize");
  checkAddrList(6, "outbufs");
  checkIntList(7, "outsteps");

  if(Listsize(argv[0]) != 6) {
    fprintf(stderr, "** %s: parameter list should be 6 real numbers\n", 
	    proc_name);
    return NullValue;
  }

  chans = Listsize(argv[3]);

  switch(call_data) {
    
  case PROCESS_BYTE:
    for(i = 0; i < chans; i++)
      affine_trans(
		   getReal(Listitem(argv[0], 0)),
		   getReal(Listitem(argv[0], 1)),
		   getReal(Listitem(argv[0], 2)),
		   getReal(Listitem(argv[0], 3)),
		   getReal(Listitem(argv[0], 4)),
		   getReal(Listitem(argv[0], 5)),
		   trueValue(argv[1]),
		   
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),
		   getAddr(Listitem(argv[3], i)),
		   getInt(Listitem(argv[4], 0)),
		   getInt(Listitem(argv[4], 1)),
		   
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),
		   getAddr(Listitem(argv[6], i)),
		   getInt(Listitem(argv[7], 0)),
		   getInt(Listitem(argv[7], 1))
		   );
    break;

  }
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_affine_warp(Script *script, 
			  char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  checkCount(5, NULL); 
  checkRealList(0, "affine parameters [a b c d e f]");
  checkIntList(1, "mapsize");
  checkAddrList(2, "[xbuf ybuf]");
  checkIntList(3, "mapstep");
  checkIntList(4, "sourcesize");
  
  if(Listsize(argv[0]) != 6) {
    fprintf(stderr, "** %s: parameter list should be 6 real numbers\n", 
	    proc_name);
    return NullValue;
  }

  if(Listsize(argv[2]) != 2) {
    fprintf(stderr, "** %s: expects 2 output buffers\n", proc_name);
    return NullValue;
  }
  
  affine_warp_safe(
		   getReal(Listitem(argv[0], 0)),
		   getReal(Listitem(argv[0], 1)),
		   getReal(Listitem(argv[0], 2)),
		   getReal(Listitem(argv[0], 3)),
		   getReal(Listitem(argv[0], 4)),
		   getReal(Listitem(argv[0], 5)),
		   
		   getInt(Listitem(argv[1], 0)),
		   getInt(Listitem(argv[1], 1)),
		   getAddr(Listitem(argv[2], 0)),
		   getAddr(Listitem(argv[2], 1)),
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),
		   
		   getInt(Listitem(argv[4], 0)),
		   getInt(Listitem(argv[4], 1))
		   );

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_warp_image(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  checkCount(8, NULL);
  checkAddrList(0, "inbufs");
  checkIntList(1, "insteps");
  checkAddrList(2, "outbufs");
  checkIntList(3, "outsteps");
  checkAddrList(4, "warpmaps");
  checkIntList(5, "mapskips");
  checkIntList(6, "size");
  /* arg 7 is "interpolate?" flag */
  
  if(!trueValue(argv[7])) {  /* no interpolation */
    
    switch(Listsize(argv[0])) {
      
    case 1:
      warp_buffer1_no_interp(
			     getAddr(Listitem(argv[0], 0)),
			     getInt(Listitem(argv[1], 0)), 
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getInt(Listitem(argv[3], 0)), 
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)), 
			     getAddr(Listitem(argv[4], 1)),
			     getInt(Listitem(argv[5], 0)), 
			     getInt(Listitem(argv[5], 1)),

			     getInt(Listitem(argv[6], 0)), 
			     getInt(Listitem(argv[6], 1))
			     );
      break;

    case 2:
      warp_buffer2_no_interp(
			     getAddr(Listitem(argv[0], 0)),
			     getAddr(Listitem(argv[0], 1)),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getAddr(Listitem(argv[2], 1)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getAddr(Listitem(argv[4], 1)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),

			     getInt(Listitem(argv[6], 0)),
			     getInt(Listitem(argv[6], 1))
			     );
      break;

    case 3:
      warp_buffer3_no_interp(
			     getAddr(Listitem(argv[0], 0)),
			     getAddr(Listitem(argv[0], 1)),
			     getAddr(Listitem(argv[0], 2)),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getAddr(Listitem(argv[2], 1)),
			     getAddr(Listitem(argv[2], 2)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getAddr(Listitem(argv[4], 1)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),

			     getInt(Listitem(argv[6], 0)),
			     getInt(Listitem(argv[6], 1))
			     );
      break;

    case 4:
      warp_buffer4_no_interp(
			     getAddr(Listitem(argv[0], 0)),
			     getAddr(Listitem(argv[0], 1)),
			     getAddr(Listitem(argv[0], 2)),
			     getAddr(Listitem(argv[0], 3)),
			     getInt(Listitem(argv[1], 0)),
			     getInt(Listitem(argv[1], 1)),

			     getAddr(Listitem(argv[2], 0)),
			     getAddr(Listitem(argv[2], 1)),
			     getAddr(Listitem(argv[2], 2)),
			     getAddr(Listitem(argv[2], 3)),
			     getInt(Listitem(argv[3], 0)),
			     getInt(Listitem(argv[3], 1)),

			     getAddr(Listitem(argv[4], 0)),
			     getAddr(Listitem(argv[4], 1)),
			     getInt(Listitem(argv[5], 0)),
			     getInt(Listitem(argv[5], 1)),

			     getInt(Listitem(argv[6], 0)),
			     getInt(Listitem(argv[6], 1))
			     );
      break;
    }
  }
  else {   /* tri-linear interpolation */
    switch(Listsize(argv[0])) {

    case 1:
      warp_buffer1(
		   getAddr(Listitem(argv[0], 0)),
		   getInt(Listitem(argv[1], 0)),
		   getInt(Listitem(argv[1], 1)),

		   getAddr(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),

		   getAddr(Listitem(argv[4], 0)),
		   getAddr(Listitem(argv[4], 1)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),

		   getInt(Listitem(argv[6], 0)),
		   getInt(Listitem(argv[6], 1))
		   );
      break;

    case 2:
      warp_buffer2(
		   getAddr(Listitem(argv[0], 0)),
		   getAddr(Listitem(argv[0], 1)),
		   getInt(Listitem(argv[1], 0)),
		   getInt(Listitem(argv[1], 1)),

		   getAddr(Listitem(argv[2], 0)),
		   getAddr(Listitem(argv[2], 1)),
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),

		   getAddr(Listitem(argv[4], 0)),
		   getAddr(Listitem(argv[4], 1)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),

		   getInt(Listitem(argv[6], 0)),
		   getInt(Listitem(argv[6], 1))
		   );
      break;

    case 3:
      warp_buffer3(
		   getAddr(Listitem(argv[0], 0)),
		   getAddr(Listitem(argv[0], 1)),
		   getAddr(Listitem(argv[0], 2)),
		   getInt(Listitem(argv[1], 0)),
		   getInt(Listitem(argv[1], 1)),

		   getAddr(Listitem(argv[2], 0)),
		   getAddr(Listitem(argv[2], 1)),
		   getAddr(Listitem(argv[2], 2)),
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),

		   getAddr(Listitem(argv[4], 0)),
		   getAddr(Listitem(argv[4], 1)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),

		   getInt(Listitem(argv[6], 0)),
		   getInt(Listitem(argv[6], 1))
		   );
      break;

    case 4:
      warp_buffer4(
		   getAddr(Listitem(argv[0], 0)),
		   getAddr(Listitem(argv[0], 1)),
		   getAddr(Listitem(argv[0], 2)),
		   getAddr(Listitem(argv[0], 3)),
		   getInt(Listitem(argv[1], 0)),
		   getInt(Listitem(argv[1], 1)),

		   getAddr(Listitem(argv[2], 0)),
		   getAddr(Listitem(argv[2], 1)),
		   getAddr(Listitem(argv[2], 2)),
		   getAddr(Listitem(argv[2], 3)),
		   getInt(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[3], 1)),

		   getAddr(Listitem(argv[4], 0)),
		   getAddr(Listitem(argv[4], 1)),
		   getInt(Listitem(argv[5], 0)),
		   getInt(Listitem(argv[5], 1)),

		   getInt(Listitem(argv[6], 0)),
		   getInt(Listitem(argv[6], 1))
		   );
      break;
    }
  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_bounding_box(Script *script, 
			   char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  int x1, y1, x2, y2;
  
  checkCount(5, NULL); 
  checkInt(0, "low");
  checkInt(1, "high");
  checkIntList(2, "size");
  checkAddrList(3, "addrs");
  checkIntList(4, "steps");
  
  if( bounding_box((unsigned char) getInt(argv[0]), 
		   (unsigned char) getInt(argv[1]), 
		   getInt(Listitem(argv[2], 0)),
		   getInt(Listitem(argv[2], 1)),		     
		   getAddr(Listitem(argv[3], 0)),
		   getInt(Listitem(argv[4], 0)),
		   getInt(Listitem(argv[4], 1)),
		   &x1, &y1, &x2, &y2) )
    return newIntListva(4, x1, y1, x2, y2);
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_bounding_box_rgb(Script *script, 
			       char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  int x1, y1, x2, y2;
  
  checkCount(9, NULL); 
  checkInt(0, "rlow");
  checkInt(1, "rhigh");
  checkInt(2, "glow");
  checkInt(3, "ghigh");
  checkInt(4, "blow");
  checkInt(5, "bhigh");
  checkIntList(6, "size");
  checkAddrList(7, "addrs");
  checkIntList(8, "steps");
  
  if( bounding_box_rgb((unsigned char) getInt(argv[0]), 
		       (unsigned char) getInt(argv[1]), 
		       (unsigned char) getInt(argv[2]), 
		       (unsigned char) getInt(argv[3]), 
		       (unsigned char) getInt(argv[4]), 
		       (unsigned char) getInt(argv[5]), 
		       getInt(Listitem(argv[6], 0)),
		       getInt(Listitem(argv[6], 1)),		     
		       getAddr(Listitem(argv[7], 0)),
		       getAddr(Listitem(argv[7], 1)),
		       getAddr(Listitem(argv[7], 2)),
		       getInt(Listitem(argv[8], 0)),
		       getInt(Listitem(argv[8], 1)),
		       &x1, &y1, &x2, &y2) )
    return newIntListva(4, x1, y1, x2, y2);
  
  return NullValue;
}

/* ------------------------------------------------------------ */
Value *improc_fuzzy_bounding_box(Script *script, 
				 char *proc_name, int call_data, 
				 int argc, Value **argv)
{
  int box[8];
  
  checkCount(5, NULL); 
  checkInt(0, "threshold");
  checkInt(1, "pixcount");
  checkIntList(2, "size");
  checkAddr(3, "buf");
  checkIntList(4, "steps");
  
  if( fuzzy_bounding_box(
			 getInt(argv[0]),
			 getInt(argv[1]),
			 getInt(Listitem(argv[2], 0)),
			 getInt(Listitem(argv[2], 1)),
			 getAddr(argv[3]),
			 getInt(Listitem(argv[4], 0)),
			 getInt(Listitem(argv[4], 1)),
			 box
			 )) return newIntList(8, box);

  return NullValue;
}




/* ------------------------------------------------------------ */
Value *improc_flood_fill(Script *script, 
			 char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  Value                  *retval;

  checkCount(6, NULL);
  checkAddrList(0, "inbuf");
  checkAddrList(1, "outbuf");
  checkInt(2, "height");
  checkInt(3, "width");
  checkInt(4, "y seed point");
  checkInt(5, "x seed point");
  
  retval = NullValue;
  switch(call_data) {
    
  case PROCESS_BYTE:
    if (flood_fill_byte (getAddr (Listitem(argv[0], 0)),
			 getAddr (Listitem(argv[1], 0)),
			 getInt (argv[2]),
			 getInt (argv[3]),
			 getInt (argv[4]),
			 getInt (argv[5])))
      retval = newBool(1);
    break;
    
  case PROCESS_SHORT:
    if (flood_fill_short (getAddr (Listitem(argv[0], 0)),
			  getAddr (Listitem(argv[1], 0)),
			  getInt (argv[2]),
			  getInt (argv[3]),
			  getInt (argv[4]),
			  getInt (argv[5])))
      retval = newBool(1);
    break;
  }
  
  return (retval);
}

/* ------------------------------------------------------------ */
/* Compute the 2D warp between a model and it's image           */

Value *improc_optical_flow (Script *script, 
			    char *proc_name, 
			    int call_data, 
			    int argc,         Value **argv)
{
  checkCount(6, NULL);
  checkAddrList(0, "image");
  checkAddrList(1, "model");
  checkInt(2, "height");
  checkInt(3, "width");
  checkAddrList(4, "plane with horizontal motion") ;
  checkAddrList(5, "plane with vertical motion");
  
  if (Listsize(argv[0]) != 1) {
    fprintf (stderr, "** %s : Unexpected number of channels in input image \n", proc_name);
    return NullValue;  }

  if (Listsize(argv[0]) != Listsize(argv[1])) {
    fprintf (stderr, "** %s : image and model should have equal number of channels \n", proc_name);
    return NullValue;  }

  if ((Listsize(argv[4]) != 1)  ||  
      (Listsize(argv[4]) != Listsize(argv[5]))) {
    fprintf (stderr, "** %s : Unexpected number of channels in output displacement vector fields \n", proc_name);
    return NullValue;  }

  switch (call_data) {

  case PROCESS_BYTE:
    calc_mvf_byte_C (getAddr (Listitem (argv[0], 0)),    /** image buffer address  **/
		     getAddr (Listitem (argv[1], 0)),    /** model buffer address  **/
		     getInt (argv[2]),                   /**      height           **/
		     getInt (argv[3]),                   /**      width            **/
		     getAddr (Listitem (argv[4], 0)),    /** buffer address for vertical displacement field    **/
		     getAddr (Listitem (argv[5], 0)) );  /** buffer address for horizontal displacement field  **/
    break;

  case PROCESS_INT:
    calc_mvf_int_C (getAddr (Listitem (argv[0], 0)),    /** image buffer address  **/
		    getAddr (Listitem (argv[1], 0)),    /** model buffer address  **/
		    getInt (argv[2]),                   /**      height           **/
		    getInt (argv[3]),                   /**      width            **/
		    getAddr (Listitem (argv[4], 0)),    /** buffer address for vertical displacement field    **/
		    getAddr (Listitem (argv[5], 0)) );  /** buffer address for horizontal displacement field  **/
    break;
  }

  return (newBool(1));
}

