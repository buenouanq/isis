#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <isis.h>

#include "isis_xwin.h"
#include "isis_xwin_private.h"

Value *prim_xwin_create(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_xwin_child(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_xwin_show(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_xwin_hide(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_xwin_move(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_xwin_resize(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_xwin_set_title(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);
Value *prim_xwin_destroy(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_xwin_display(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_xwin_display_interleaved(Script *script, 
				     char *proc_name, int call_data, 
				     int argc, Value **argv);
Value *prim_xwin_point_inside(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv);
Value *prim_xwin_point_location(Script *script, 
				char *proc_name, int call_data, 
				int argc, Value **argv);
Value *prim_xwin_num_inputs(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *prim_xwin_poll_all(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_xwin_poll_inp(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_xwin_getsize(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_xwin_getloc(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_xwin_select_events(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *prim_xwin_dump_events(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_xwin_pending_event(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *prim_xwin_get_event(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);


/*----------------------------------------------------------------------*/
void bind_xwin_library(Script *script) 
{
  bindBool(script, "isis_xwin", 1);

  bindCfunc(script, "xwin-create", prim_xwin_create, 0);
  bindCfunc(script, "xwin-child", prim_xwin_child, 0);
  bindCfunc(script, "xwin-show", prim_xwin_show, 0);
  bindCfunc(script, "xwin-hide", prim_xwin_hide, 0);
  bindCfunc(script, "xwin-move", prim_xwin_move, 0);
  bindCfunc(script, "xwin-resize", prim_xwin_resize, 0);
  bindCfunc(script, "xwin-title", prim_xwin_set_title, 0);
  bindCfunc(script, "xwin-destroy", prim_xwin_destroy, 0);
  bindCfunc(script, "xwin-display", prim_xwin_display, 0);
  bindCfunc(script, "xwin-display-interleaved",
	    prim_xwin_display_interleaved, 0);
  bindCfunc(script, "xwin-get-size", prim_xwin_getsize, 0);
  bindCfunc(script, "xwin-get-location", prim_xwin_getloc, 0);
  bindCfunc(script, "xwin-pointer-inside", prim_xwin_point_inside, 0);
  bindCfunc(script, "xwin-pointer-location", prim_xwin_point_location, 0);
  bindCfunc(script, "xwin-number-of-actuators", prim_xwin_num_inputs, 0);
  bindCfunc(script, "xwin-poll-all-actuators", prim_xwin_poll_all, 0);
  bindCfunc(script, "xwin-poll-actuator", prim_xwin_poll_inp, 0);  
  bindCfunc(script, "xwin-select-events", prim_xwin_select_events, 0);  
  bindCfunc(script, "xwin-dump-events", prim_xwin_dump_events, 0);  
  bindCfunc(script, "xwin-event-pending", prim_xwin_pending_event, 0);  
  bindCfunc(script, "xwin-get-event", prim_xwin_get_event, 0);  
  bindCfunc(script, "xwin-wait-for-event", prim_xwin_get_event, 1);

  bindInt(script, "event-none", EVENT_NONE);
  bindInt(script, "event-pointer-in-window", EVENT_POINTER_INSIDE);
  bindInt(script, "event-pointer-location", EVENT_POINTER_LOCATION);
  bindInt(script, "event-actuator", EVENT_WINDOW_INPUT);
  bindInt(script, "event-window-geometry", EVENT_WINDOW_GEOMETRY);
  bindInt(script, "event-character", EVENT_CHARACTER);
  bindInt(script, "event-expose", EVENT_EXPOSE);
  bindInt(script, "event-all", EVENT_ALL);

  bindInt(script, "dither-decimate", Decimate);
  bindInt(script, "dither-ordered", OrderedDither);
  bindInt(script, "dither-diffusion", FloydSteinbergErrorDiffusion);
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_create(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int use_default_display, loc[2], dim[2], max[2], dither, show;
  char displayname[256], title[256];

  use_default_display = 0;

  if(argc > 0 && isString(argv[0]))
    getString(argv[0], displayname, 256);
  else use_default_display = 1;
  
  if(argc > 1 && isString(argv[1]))
    getString(argv[1], title, 256);
  else strcpy(title, "Isis");

  if(argc > 2 && isIntList(argv[2]))
    getIntList(argv[2], loc, 2);
  else loc[0] = loc[1] = 0;
  
  if(argc > 3 && isIntList(argv[3]))
    getIntList(argv[3], dim, 2);
  else { dim[0] = 640; dim[1] = 480; }
  
  if(argc > 4 && isIntList(argv[4]))
    getIntList(argv[4], max, 2);
  else { max[0] = dim[0]; max[1] = dim[1]; }
  
  if(argc > 5 && isInt(argv[5]))
    dither = getInt(argv[5]);
  else dither = Decimate;

  if(argc > 6)
    show = trueValue(argv[6]);
  else show = 1;
  
  return newAddr(new_image_window(use_default_display ? NULL : displayname, 
				  title, loc[0],
				  loc[1], dim[0], dim[1],
				  max[0], max[1], dither, show));
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_child(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  int loc[2], dim[2], show;

  checkCountLeast(1, "parent window handle, location, size");
  checkAddr(0, "parent window handle");
  
  if(argc > 1 && isIntList(argv[1]))
    getIntList(argv[1], loc, 2);
  else loc[0] = loc[1] = 10;
  
  if(argc > 2 && isIntList(argv[2]))
    getIntList(argv[2], dim, 2);
  else { dim[0] = 100; dim[1] = 100; }
  
  if(argc > 3)
    show = trueValue(argv[3]);
  else show = 1;
  
  return newAddr(new_child_image_window(getAddr(argv[0]),
					loc[0], loc[1], dim[0], dim[1], show));
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_show(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  show_image_window(getAddr(argv[0]));
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_hide(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  hide_image_window(getAddr(argv[0]));
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_move(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  checkCount(2, "window handle, window position");
  checkAddr(0, "window handle");
  checkIntList(1, "window position");

  move_image_window(getAddr(argv[0]), 
		    getInt(Listitem(argv[1], 0)), 
		    getInt(Listitem(argv[1], 1)));
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_resize(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)

{
  checkCount(2, "window handle, window size");
  checkAddr(0, "window handle");
  checkIntList(1, "window size");

  resize_image_window(getAddr(argv[0]), 
		      getInt(Listitem(argv[1], 0)), 
		      getInt(Listitem(argv[1], 1)));
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_set_title(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  char title[256];
  
  checkCount(2, "window handle, window title");
  checkAddr(0, "window handle");
  checkString(1, "window title");

  getString(argv[1], title, 256);
  set_image_window_title(getAddr(argv[0]), title);
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_destroy(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
     
{
  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  destroy_image_window(getAddr(argv[0]));
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_display(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  checkCount(5, "window handle, display pos, size, buflist, step factors");
  checkAddr(0, "window handle");
  checkIntList(1, "display pos");
  checkIntList(2, "size");
  checkAddrList(3, "buflist");
  checkIntList(4, "steps");

  display_image(getAddr(argv[0]), 
		getInt(Listitem(argv[1], 0)),
		getInt(Listitem(argv[1], 1)),
		getInt(Listitem(argv[2], 0)),
		getInt(Listitem(argv[2], 1)),
		getInt(Listitem(argv[4], 0)),
		getInt(Listitem(argv[4], 1)),
		getAddr(Listitem(argv[3], 0)),
		getAddr(Listitem(argv[3], 1)),
		getAddr(Listitem(argv[3], 2)));

  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_display_interleaved(Script *script, 
				     char *proc_name, int call_data, 
				     int argc, Value **argv)
{
  checkCount(5, "window handle, display pos, size, buffer, step factors");
  checkAddr(0, "window handle");
  checkIntList(1, "display pos");
  checkIntList(2, "size");
  checkAddr(3, "interleaved data buffer");
  checkIntList(4, "steps");

  display_32bit_interleaved_image(getAddr(argv[0]), 
				  getInt(Listitem(argv[1], 0)),
				  getInt(Listitem(argv[1], 1)),
				  getInt(Listitem(argv[2], 0)),
				  getInt(Listitem(argv[2], 1)),
				  getInt(Listitem(argv[4], 0)),
				  getInt(Listitem(argv[4], 1)),
				  getAddr(argv[3]));

  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_getsize(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  int dim[2];

  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  get_image_window_size(getAddr(argv[0]), &dim[0], &dim[1]);
  return newIntList(2, dim);
}   

/*----------------------------------------------------------------------*/
Value *prim_xwin_getloc(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int loc[2];

  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  get_image_window_location(getAddr(argv[0]), &loc[0], &loc[1]);
  return newIntList(2, loc);
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_point_inside(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  return newBool(pointer_inside(getAddr(argv[0])));
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_point_location(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv)
{
  int loc[2];

  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  pointer_location(getAddr(argv[0]), &loc[0], &loc[1]);
  return newIntList(2,loc);
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_poll_all(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  int num_inputs;
  int *inputs;

  checkCount(1, "window handle");
  checkAddr(0, "window handle");
  
  inputs = poll_all_inputs(getAddr(argv[0]), &num_inputs);
  return newIntList(num_inputs, inputs);
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_num_inputs(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  checkCount(1, "window handle");
  checkAddr(0, "window handle");

  return newInt(number_of_inputs(getAddr(argv[0])));
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_poll_inp(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv)
{
  checkCount(2, "window handle, input number");
  checkAddr(0, "window handle");
  checkInt(1, "input number");

  return newInt(poll_input(getAddr(argv[0]), getInt(argv[1])));
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_select_events(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  int i, mask;

  checkCountLeast(1, "window handle, event types ...");
  checkAddr(0, "window handle");
  
  for(i = 1; i < argc; i++) checkInt(i, "event type");
  
  mask = 0;
  if(argc > 1) for(i = 1; i < argc;i++) mask |= (getInt(argv[i]));
  else mask = EVENT_ALL;

  select_events(getAddr(argv[0]), mask);
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_dump_events(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int i, mask;

  checkCountLeast(1, "window handle, event types ...");
  checkAddr(0, "window handle");
  
  for(i = 1; i < argc; i++) checkInt(i, "event type");
  
  mask = 0;
  if(argc > 1) for(i = 1; i < argc;i++) mask |= (getInt(argv[i]));
  else mask = EVENT_ALL;

  dump_events(getAddr(argv[0]), mask);
  return NullValue;
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_pending_event(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  int i, mask;

  checkCountLeast(1, "window handle, event types ...");
  checkAddr(0, "window handle");
  
  for(i = 1; i < argc; i++) checkInt(i, "event type");
  
  mask = 0;
  if(argc > 1) for(i = 1; i < argc;i++) mask |= (getInt(argv[i]));
  else mask = EVENT_ALL;

  return newBool(event_pending(getAddr(argv[0]), mask));
}

/*----------------------------------------------------------------------*/
Value *prim_xwin_get_event(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)

{
  int i, mask, block;
  imgwin_event event;
  Value *val1, *val2, *val3, *val4, *val5, *retval;

  checkCountLeast(1, "window handle, event types ...");
  checkAddr(0, "window handle");
  
  for(i = 1; i < argc; i++) checkInt(i, "event type");
  
  mask = 0;
  if(argc > 1) for(i = 1; i < argc;i++) mask |= (getInt(argv[i]));
  else mask = EVENT_ALL;

  block = call_data;

  if(get_event(getAddr(argv[0]), &event, block, mask)) {
    
    switch(event.type) {
      
    case EVENT_POINTER_INSIDE:
      /* <event> <true or false> <pointer location> <inputs> */
      
      val1 = newInt(event.type);
      val2 = newBool(event.pointer_inside.inside);
      val3 = newIntListva(2, event.pointer_inside.x, event.pointer_inside.y);
      val4 = newIntList(7, event.pointer_inside.inputs);
      retval = newListva(4, val1, val2, val3, val4);
      freeValue(val1);
      freeValue(val2);
      freeValue(val3);
      freeValue(val4);
      return(retval);
      
    case EVENT_POINTER_LOCATION:
      /* <event> <pointer location> <inputs> */
      
      val1 = newInt(event.type);
      val2 = newIntListva(2, event.pointer_location.x,
			  event.pointer_location.y);
      val3 = newIntList(7, event.pointer_location.inputs);
      retval = newListva(3, val1, val2, val3);
      freeValue(val1);
      freeValue(val2);
      freeValue(val3);
      return(retval);

    case EVENT_WINDOW_INPUT:
      /* <event> <actuator#> <acutator val> <point pos> <inputs> */

      val1 = newInt(event.type);
      val2 = newInt(event.window_input.input_number);
      val3 = newInt(event.window_input.value);
      val4 = newIntListva(2, event.window_input.x, event.window_input.y);
      val5 = newIntList(7, event.window_input.inputs);
      retval = newListva(5, val1, val2, val3, val4, val5);
      freeValue(val1);
      freeValue(val2);
      freeValue(val3);
      freeValue(val4);
      freeValue(val5);
      return(retval);

    case EVENT_WINDOW_GEOMETRY:
      /* <event> <window location> <window size> */

      val1 = newInt(event.type);
      val2 = newIntListva(2, event.window_geometry.xloc,
			  event.window_geometry.yloc);
      val3 = newIntListva(2, event.window_geometry.xsize,
			  event.window_geometry.ysize);
      retval = newListva(3, val1, val2, val3);
      freeValue(val1);
      freeValue(val2);
      freeValue(val3);
      return(retval);

    case EVENT_CHARACTER:
      /* <event> <character> <pointer location> <inputs> */

      val1 = newInt(event.type);
      val2 = newChar(event.character.key);
      val3 = newIntListva(2, event.character.x, event.character.y);
      val4 = newIntList(7, event.character.inputs);
      retval = newListva(4, val1, val2, val3, val4);
      freeValue(val1);
      freeValue(val2);
      freeValue(val3);
      freeValue(val4);
      return(retval);

    case EVENT_EXPOSE:
      /* <event> <pos> <size> */

      val1 = newInt(event.type);
      val2 = newIntListva(2, event.expose.x, event.expose.y);
      val3 = newIntListva(2, event.expose.w, event.expose.h);
      retval = newListva(3, val1, val2, val3);
      freeValue(val1);
      freeValue(val2);
      freeValue(val3);
      return(retval);

    }
  }
  
  return newIntListva(1, EVENT_NONE);
}
