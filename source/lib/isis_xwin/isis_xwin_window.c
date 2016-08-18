#include <stdio.h>
#include <stdlib.h>

#include "isis_xwin_private.h"

static XColor xcols[256];
static unsigned char lut[256][3];
static unsigned char *outbuf = NULL;
static int outbufsize = 0;

/*----------------------------------------------------------------------*/
static void expand_outbuf(int len)
{
  if(outbuf != NULL) free(outbuf);
  if((outbuf = malloc(len)) == NULL) {
    fprintf(stderr, "** Unable to allocate %d byte xwin output buffer.\n", 
	    len);
    exit(1);
  }
  outbufsize = len;
}

/*-------------------------------------------------------------*/
ImageWindow *new_image_window(char *displayname, 
			      char *title,
			      int xpos, int ypos,
			      int xsize, int ysize,
			      int max_xsize, 
			      int max_ysize, 
			      int dither_quality,
			      int map_at_creation)
{
  ImageWindow *imgwin;
  int i;
  
  imgwin = (ImageWindow *) malloc(sizeof(ImageWindow));
  if(imgwin == NULL) {
    fprintf(stderr, "** Can't allocate ImageWindow structure.\n");
    return NULL;
  }

  imgwin->child = 0;

  /* If displayname is NULL, the default display defined in the
     DISPLAY environment variable is used */

  if ((imgwin->display = XOpenDisplay(displayname)) == NULL) {
    if(displayname == NULL) fprintf(stderr, "** Can't open default display\n");
    else fprintf(stderr, "** Can't open display %s\n", displayname);
    return NULL;
  }
  imgwin->screen = DefaultScreen(imgwin->display);
  imgwin->defgc = DefaultGC(imgwin->display, imgwin->screen);
  imgwin->defcmap = DefaultColormap(imgwin->display, imgwin->screen);
  imgwin->rootwindow = RootWindow(imgwin->display, imgwin->screen);

  if(XMatchVisualInfo(imgwin->display, imgwin->screen, 
		      24, TrueColor, &imgwin->vinfo)) {
    imgwin->display_bits = 24;
    imgwin->gray = 0;
  }
  else if(XMatchVisualInfo(imgwin->display, imgwin->screen, 
			   8, PseudoColor, &imgwin->vinfo)) {
    imgwin->display_bits = 8;
    imgwin->gray = 0;
  }
  else if(XMatchVisualInfo(imgwin->display, imgwin->screen, 
			   8, GrayScale, &imgwin->vinfo)) {
    imgwin->display_bits = 8;
    imgwin->gray = 1;
  }
  else {
    fprintf(stderr, 
	    "** No 24 or 8 bit color or gray scale visuals available\n");
    return NULL;
  }

  imgwin->visual = imgwin->vinfo.visual;
  imgwin->display_colors = 1 << imgwin->display_bits;
  
  imgwin->quality = dither_quality;
  if(imgwin->display_bits != 24) {
    if((imgwin->cmap = XCreateColormap(imgwin->display, imgwin->rootwindow, 
				       imgwin->visual, AllocAll)) == 0) {
      fprintf(stderr, "** Unable to create private colormap\n");
      return NULL;
    }
    imgwin->dith = malloc(sizeof(Dither));
    InitDither(imgwin->dith, 1,
	       !imgwin->gray, imgwin->display_bits, lut);
    for(i = 0; i < imgwin->display_colors; i++) {
      xcols[i].pixel = i;
      xcols[i].red = (short)lut[i][0] << 8;
      xcols[i].green = (short)lut[i][1] << 8;
      xcols[i].blue = (short)lut[i][2] << 8;
      xcols[i].flags = (DoRed | DoGreen | DoBlue);
    }
    XStoreColors(imgwin->display, imgwin->cmap, xcols, imgwin->display_colors);
  }
  else {
    if((imgwin->cmap = XCreateColormap(imgwin->display, imgwin->rootwindow, 
				       imgwin->visual, AllocNone)) == 0) {
      fprintf(stderr, "** Error creating 24 bit colormap\n");
      return NULL;
    }
  }

  imgwin->winattr.colormap = imgwin->cmap;
  imgwin->winattr.border_pixel = WhitePixel(imgwin->display, 
					    imgwin->screen);
  imgwin->winattr.background_pixel = BlackPixel(imgwin->display, 
						imgwin->screen);
  imgwin->winattr.backing_store = Always;
  
  /* For some reason, the only way to get around the "BAD MATCH"
     errors is to specify the "Border Pixel" attribute.  I can't
     believe this shit. */
  
  imgwin->imagewindow = 
    XCreateWindow(imgwin->display, imgwin->rootwindow,
		  xpos, ypos, xsize, ysize,
		  0, imgwin->display_bits, 
		  InputOutput, imgwin->visual,
		  CWColormap | CWBorderPixel | CWBackPixel | CWBackingStore, 
		  &imgwin->winattr);
  XFlush(imgwin->display);

  imgwin->size.flags = (USPosition | PPosition | PSize | 
			PMaxSize | PMinSize | PResizeInc);
  imgwin->size.width = xsize;
  imgwin->size.height = ysize;
  imgwin->size.x = xpos;
  imgwin->size.y = ypos;
  imgwin->size.min_width = 1;
  imgwin->size.min_height = 1;
  imgwin->size.max_width = max_xsize;
  imgwin->size.max_height = max_ysize;
  imgwin->size.width_inc = 1;
  imgwin->size.height_inc = 1;
  XSetStandardProperties(imgwin->display, imgwin->imagewindow, 
			 title, title, 
			 None, NULL, 0L, &imgwin->size);
  
  imgwin->xwmh.flags = (InputHint | StateHint);
  imgwin->xwmh.input = True;
  imgwin->xwmh.initial_state = NormalState;
  XSetWMHints(imgwin->display, imgwin->imagewindow, &imgwin->xwmh);

  XSetNormalHints(imgwin->display, imgwin->imagewindow, &imgwin->size);
  XMoveResizeWindow(imgwin->display, imgwin->imagewindow, 
		    xpos, ypos, xsize, ysize);

  imgwin->gc = XCreateGC(imgwin->display, imgwin->imagewindow, 0L, NULL);

  if(map_at_creation) XMapWindow(imgwin->display, imgwin->imagewindow);

  XFlush(imgwin->display);
  
  if(imgwin->display_bits == 24) imgwin->bufsize = max_xsize * max_ysize * 4;
  else imgwin->bufsize = max_xsize * max_ysize;
  
  if(imgwin->bufsize > outbufsize) expand_outbuf(imgwin->bufsize);
  
  imgwin->image = 
    XCreateImage(imgwin->display, imgwin->visual, 
		 imgwin->display_bits, ZPixmap, 0, 
		 (char *) outbuf,
		 max_xsize, max_ysize, 
		 (imgwin->display_bits == 24) ? 32 : 8, 
		 (imgwin->display_bits == 24) ? max_xsize * 4 : max_xsize);

  imgwin->tempimage = 
    XCreateImage(imgwin->display, imgwin->visual, 
		 imgwin->display_bits, ZPixmap, 0, 
		 (char *) outbuf,
		 max_xsize, max_ysize, 
		 (imgwin->display_bits == 24) ? 32 : 8, 
		 (imgwin->display_bits == 24) ? max_xsize * 4 : max_xsize);
  imgwin->tempimage->bitmap_unit = 32;
  imgwin->tempimage->bitmap_pad = 32;
  imgwin->tempimage->depth = 24;

  imgwin->num_inputs = NUM_INPUTS;
  imgwin->inputs = (int *) malloc(NUM_INPUTS * sizeof(int));
  imgwin->xeventmask = 0;

  return imgwin;
}

/*-------------------------------------------------------------*/
ImageWindow *new_child_image_window(ImageWindow *parwin, 
				    int xpos, int ypos,
				    int xsize, int ysize,
				    int map_at_creation)
{
  ImageWindow *imgwin;
  int i;
  
  if(parwin == NULL) return NULL;

  imgwin = (ImageWindow *) malloc(sizeof(ImageWindow));
  if(imgwin == NULL) {
    fprintf(stderr, "** Can't allocate ImageWindow structure.\n");
    return NULL;
  }

  imgwin->child = 1;

  /* If displayname is NULL, the default display defined in the
     DISPLAY environment variable is used */
  
  imgwin->display = parwin->display;
  imgwin->screen = parwin->screen;
  imgwin->defgc = parwin->defgc;
  imgwin->defcmap = parwin->defcmap;
  imgwin->rootwindow = parwin->rootwindow;
  imgwin->display_bits = parwin->display_bits;
  imgwin->gray = parwin->gray;

  imgwin->visual = parwin->visual;
  imgwin->display_colors = parwin->display_colors;
  
  imgwin->quality = parwin->quality;
  imgwin->cmap = parwin->cmap;

  imgwin->dith = parwin->dith;

  imgwin->winattr.colormap = imgwin->cmap;
  imgwin->winattr.border_pixel = WhitePixel(imgwin->display, 
					    imgwin->screen);
  imgwin->winattr.background_pixel = BlackPixel(imgwin->display, 
						imgwin->screen);
  imgwin->winattr.backing_store = Always;
  
  /* For some reason, the only way to get around the "BAD MATCH"
     errors is to specify the "Border Pixel" attribute.  I can't
     believe this shit. */
  
  imgwin->imagewindow = 
    XCreateWindow(imgwin->display, parwin->imagewindow,
		  xpos, ypos, xsize, ysize,
		  0, imgwin->display_bits, 
		  InputOutput, imgwin->visual,
		  CWColormap | CWBorderPixel | CWBackPixel | CWBackingStore, 
		  &imgwin->winattr);
  XFlush(imgwin->display);

  imgwin->size.flags = (USPosition | PPosition | PSize | 
			PMaxSize | PMinSize | PResizeInc);
  imgwin->size.width = xsize;
  imgwin->size.height = ysize;
  imgwin->size.x = xpos;
  imgwin->size.y = ypos;
  imgwin->size.min_width = 1;
  imgwin->size.min_height = 1;
  imgwin->size.max_width = parwin->size.max_width;
  imgwin->size.max_height = parwin->size.max_height;
  imgwin->size.width_inc = 1;
  imgwin->size.height_inc = 1;
  XSetStandardProperties(imgwin->display, imgwin->imagewindow, 
			 "child", "child", 
			 None, NULL, 0L, &imgwin->size);
  
  imgwin->xwmh.flags = (InputHint | StateHint);
  imgwin->xwmh.input = True;
  imgwin->xwmh.initial_state = NormalState;
  XSetWMHints(imgwin->display, imgwin->imagewindow, &imgwin->xwmh);

  XSetNormalHints(imgwin->display, imgwin->imagewindow, &imgwin->size);
  XMoveResizeWindow(imgwin->display, imgwin->imagewindow, 
		    xpos, ypos, xsize, ysize);

  imgwin->gc = parwin->gc;
  
  if(map_at_creation) XMapWindow(imgwin->display, imgwin->imagewindow);
  
  XFlush(imgwin->display);
  
  imgwin->bufsize = parwin->bufsize;
  
  imgwin->image = parwin->image;
  imgwin->tempimage = parwin->tempimage;

  imgwin->num_inputs = parwin->num_inputs;
  imgwin->inputs = parwin->inputs;
  imgwin->xeventmask = parwin->xeventmask;

  return imgwin;
}

/*-------------------------------------------------------------*/
void show_image_window(ImageWindow *imgwin)
{
  if(imgwin == NULL) return;
  XMapWindow(imgwin->display, imgwin->imagewindow);
  XFlush(imgwin->display);
}  

/*-------------------------------------------------------------*/
void hide_image_window(ImageWindow *imgwin)
{
  if(imgwin == NULL) return;
  XUnmapWindow(imgwin->display, imgwin->imagewindow);
  XFlush(imgwin->display);
}  
  
/*-------------------------------------------------------------*/
void move_image_window(ImageWindow *imgwin, int x, int y)
{
  if(imgwin == NULL) return;
  XMoveWindow(imgwin->display, imgwin->imagewindow, x, y);
  XFlush(imgwin->display);
}  
  
/*-------------------------------------------------------------*/
void resize_image_window(ImageWindow *imgwin, int x, int y)
{
  if(imgwin == NULL) return;
  XResizeWindow(imgwin->display, imgwin->imagewindow, x, y);
  XFlush(imgwin->display);
}  
  
/*-------------------------------------------------------------*/
void set_image_window_title(ImageWindow *imgwin, char *title)
{
  if(imgwin == NULL) return;
  XStoreName(imgwin->display, imgwin->imagewindow, title);
  XFlush(imgwin->display);
}  

/*-------------------------------------------------------------*/
void destroy_image_window(ImageWindow *imgwin)
{
  if(imgwin == NULL) return;
  XUnmapWindow(imgwin->display, imgwin->imagewindow);
  XDestroyWindow(imgwin->display, imgwin->imagewindow);
  if(!imgwin->child) {
    XFreeGC(imgwin->display, imgwin->gc);
    /* Must set image->data to null because XDestroyImage would free it */
    imgwin->image->data = NULL;
    XDestroyImage(imgwin->image);
    XUninstallColormap(imgwin->display, imgwin->cmap);
  }
  XFlush(imgwin->display);
}

/*-------------------------------------------------------------*/
void display_image(ImageWindow *imgwin,
		   int xpos, int ypos,
		   int xsize, int ysize, 
		   int skip, int stride,
		   unsigned char *r,
		   unsigned char *g, 
		   unsigned char *b)
{
  if(imgwin == NULL) return;

  /* need to check if image size is to big */
  
  if(xsize > imgwin->image->width) xsize = imgwin->image->width;
  if(ysize > imgwin->image->height) ysize = imgwin->image->height;

  if(imgwin->display_bits == 24)
    InterleaveData(xsize, ysize, 
		   r, g, b, 
		   skip, stride,
		   outbuf, 
		   4, imgwin->image->width * 4,
		   (imgwin->image->byte_order == MSBFirst),
		   (imgwin->image->red_mask & 0x1));  
  else DitherData(imgwin->dith, imgwin->quality, xsize, ysize,
		  r, g, b, 
		  skip, stride, 
		  outbuf, 
		  1, imgwin->image->width);

  imgwin->image->data = (char *) outbuf;  /* address might have changed */
  
  XPutImage(imgwin->display, imgwin->imagewindow, imgwin->gc, imgwin->image,
	    0, 0, xpos, ypos, xsize, ysize);
  XFlush(imgwin->display);
}

/*-------------------------------------------------------------*/
void display_32bit_interleaved_image(ImageWindow *imgwin,
				     int xpos, int ypos,
				     int xsize, int ysize, 
				     int skip, int stride,
				     unsigned char *buf)
{
  /* Note: skip is totally ignored, but included for future
     compatibility, compatibility that won't ever exist because X
     sucks my butt. */
  
  if(imgwin == NULL) return;
  
  imgwin->tempimage->width = xsize;
  imgwin->tempimage->height = ysize;
  imgwin->tempimage->data = (char *) buf;
  /* byte_order, bitmap_bit_order */
  imgwin->tempimage->bytes_per_line = stride;
  /* red, green, blue masks */
  
  XPutImage(imgwin->display, imgwin->imagewindow, imgwin->gc,
	    imgwin->tempimage, 0, 0, xpos, ypos, xsize, ysize);
  XFlush(imgwin->display);
}

/*-------------------------------------------------------------*/
int pointer_inside(ImageWindow *imgwin)
{
  XWindowAttributes winattr;
  Window rootwin, childwin;
  int rootx, rooty, winx, winy;
  Bool ret;
  unsigned int mask;
  
  if(imgwin == NULL) return 0;

  XGetWindowAttributes(imgwin->display, imgwin->imagewindow, &winattr);
  
  ret = XQueryPointer(imgwin->display, imgwin->imagewindow, 
		      &rootwin, &childwin, 
		      &rootx, &rooty,
		      &winx, &winy,
		      &mask);
  
  /* Due to window manager interactions, this is the only way this
     will work! */
  
  return (winx >= 0 && 
	  winx < winattr.width && 
	  winy >= 0 && 
	  winy < winattr.height);
}

/*-------------------------------------------------------------*/
int pointer_location(ImageWindow *imgwin, int *x, int *y)
{
  Window rootwin, childwin;
  int rootx, rooty, winx, winy;
  Bool ret;
  unsigned int mask;
  
  if(imgwin == NULL) return 0;

  ret = XQueryPointer(imgwin->display, imgwin->imagewindow, 
		      &rootwin, &childwin, 
		      &rootx, &rooty,
		      &winx, &winy,
		      &mask);
  
  if(ret) {
    *x = winx;
    *y = winy;
    return 1;  /* success */
  }
  else {
    *x = 0;
    *y = 0;
    return 0;  /* fail */
  }

}

/*-------------------------------------------------------------*/
int number_of_inputs(ImageWindow *imgwin)
{
  if(imgwin == NULL) return 0;
  return imgwin->num_inputs;
}

/*-------------------------------------------------------------*/
static void convert_input_mask(unsigned int state, int *inputs)
{
  inputs[0] = (state & Button1Mask) ? 1 : 0;
  inputs[1] = (state & Button2Mask) ? 1 : 0;
  inputs[2] = (state & Button3Mask) ? 1 : 0;
  inputs[3] = (state & ShiftMask) ? 1 : 0;
  inputs[4] = (state & ControlMask) ? 1 : 0;
  inputs[5] = (state & Mod1Mask) ? 1 : 0;
  inputs[6] = (state & LockMask) ? 1 : 0;
}

/*-------------------------------------------------------------*/
int *poll_all_inputs(ImageWindow *imgwin, int *num_inputs)
{
  Window rootwin, childwin;
  int rootx, rooty, winx, winy;
  Bool ret;
  unsigned int mask;
  
  if(imgwin == NULL) {
    *num_inputs = 0;
    return NULL;
  }
  ret = XQueryPointer(imgwin->display, imgwin->imagewindow, 
		      &rootwin, &childwin, 
		      &rootx, &rooty,
		      &winx, &winy,
		      &mask);

  convert_input_mask(mask, imgwin->inputs);
  
  *num_inputs = imgwin->num_inputs;
  return imgwin->inputs;
}

/*-------------------------------------------------------------*/
int poll_input(ImageWindow *imgwin, int input_number)
{
  int *inputs, num;

  if(imgwin == NULL) return 0;

  inputs = poll_all_inputs(imgwin, &num);

  if(input_number >= 0 && input_number < num) return inputs[input_number];
  else return 0;
}

/*-------------------------------------------------------------*/
void get_image_window_size(ImageWindow *imgwin, int *x, int *y)
{
  XWindowAttributes winattr;

  if(imgwin == NULL) return;

  XGetWindowAttributes(imgwin->display, imgwin->imagewindow, &winattr);

  *x = winattr.width;
  *y = winattr.height;

}

/*-------------------------------------------------------------*/
void get_image_window_location(ImageWindow *imgwin, int *x, int *y)
{
  XWindowAttributes winattr;

  if(imgwin == NULL) return;

  XGetWindowAttributes(imgwin->display, imgwin->imagewindow, &winattr);

  *x = winattr.x;
  *y = winattr.y;

}

/*-------------------------------------------------------------*/
static long convert_event_mask(int mask)
{
  long xmask = 0;

  if(mask & EVENT_POINTER_INSIDE)
    xmask |= EnterWindowMask | LeaveWindowMask;
  
  if(mask & EVENT_POINTER_LOCATION)
    xmask |= PointerMotionMask;
  
  if(mask & EVENT_WINDOW_INPUT)
    xmask |= ButtonPressMask | ButtonReleaseMask;
  
  if(mask & EVENT_WINDOW_GEOMETRY)
    xmask |= StructureNotifyMask;

  if(mask & EVENT_CHARACTER)
    xmask |= KeyPressMask;

  if(mask & EVENT_EXPOSE)
    xmask |= ExposureMask;

  return xmask;
}  

/*-------------------------------------------------------------*/
void select_events(ImageWindow *imgwin, int mask)
{
  long xmask;

  if(imgwin == NULL) return;
  xmask = convert_event_mask(mask);
  XSelectInput(imgwin->display, imgwin->imagewindow, xmask);
  imgwin->xeventmask = xmask;
}


/*-------------------------------------------------------------*/
void dump_events(ImageWindow *imgwin, int mask)
{
  XEvent xevent;
  long xmask;
  
  if(imgwin == NULL) return;
  if(mask == 0) while(XCheckWindowEvent(imgwin->display, imgwin->imagewindow,
					imgwin->xeventmask, &xevent));
  else {
    xmask = convert_event_mask(mask);
    while(XCheckWindowEvent(imgwin->display, imgwin->imagewindow,
			    mask, &xevent));
  }
}

/*-------------------------------------------------------------*/
int event_pending(ImageWindow *imgwin, int mask)
{
  XEvent xevent;
  long xmask;
  int gotone;

  if(imgwin == NULL) return 0;

  if(mask == 0) xmask = imgwin->xeventmask;
  else xmask = convert_event_mask(mask);

  gotone = (int) XCheckWindowEvent(imgwin->display, imgwin->imagewindow,
				   xmask, &xevent);
  if(gotone) {
    XPutBackEvent(imgwin->display, &xevent);
    return 1;
  }
  else return 0;

  /* Used to be simply: return XPending(imgwin->display); but was
     changed so that events in a particular window could be polled.
     (instead of in ANY window) */
}


/*-------------------------------------------------------------*/
int get_event(ImageWindow *imgwin, imgwin_event *event, 
	      int block, int mask)
{
  XEvent xevent;
  KeySym keysym_return;
  long xmask;
  int gotone, len;

  if(imgwin == NULL) {
    event->type = EVENT_NONE;
    return 0;
  }

  if(mask == 0) xmask = imgwin->xeventmask;
  else xmask = convert_event_mask(mask);

 get_another:
  
  if(block) {
    XWindowEvent(imgwin->display, imgwin->imagewindow,
		 xmask, &xevent);
    gotone = 1;
  } else {
    gotone = (int) XCheckWindowEvent(imgwin->display, imgwin->imagewindow,
				     xmask, &xevent);
  }
  
  if(!gotone) {
    event->type = EVENT_NONE;
    return 0;
  }
  
  switch(xevent.type) {

  case EnterNotify:
    event->type = EVENT_POINTER_INSIDE;
    event->pointer_inside.inside = 1;
    event->pointer_inside.x = xevent.xcrossing.x;
    event->pointer_inside.y = xevent.xcrossing.y;
    convert_input_mask(xevent.xcrossing.state, imgwin->inputs);
    event->pointer_inside.num_inputs = imgwin->num_inputs;
    event->pointer_inside.inputs = imgwin->inputs;
    return 1;

  case LeaveNotify:
    event->type = EVENT_POINTER_INSIDE;
    event->pointer_inside.inside = 0;
    event->pointer_inside.x = xevent.xcrossing.x;
    event->pointer_inside.y = xevent.xcrossing.y;
    convert_input_mask(xevent.xcrossing.state, imgwin->inputs);
    event->pointer_inside.num_inputs = imgwin->num_inputs;
    event->pointer_inside.inputs = imgwin->inputs;
    return 1;

  case MotionNotify:
    event->type = EVENT_POINTER_LOCATION;
    event->pointer_location.x = xevent.xmotion.x;
    event->pointer_location.y = xevent.xmotion.y;
    convert_input_mask(xevent.xmotion.state, imgwin->inputs);
    event->pointer_location.num_inputs = imgwin->num_inputs;
    event->pointer_location.inputs = imgwin->inputs;
    return 1;

  case ButtonPress:
    event->type = EVENT_WINDOW_INPUT;
    switch(xevent.xbutton.button) {
    case Button1:
      event->window_input.input_number = 0;
      break;
    case Button2:
      event->window_input.input_number = 1;
      break;
    case Button3:
      event->window_input.input_number = 2;
      break;
    }
    event->window_input.value = 1;
    event->window_input.x = xevent.xbutton.x;
    event->window_input.y = xevent.xbutton.y;
    convert_input_mask(xevent.xbutton.state, imgwin->inputs);
    event->window_input.num_inputs = imgwin->num_inputs;
    event->window_input.inputs = imgwin->inputs;
    return 1;

  case ButtonRelease:
    event->type = EVENT_WINDOW_INPUT;
    switch(xevent.xbutton.button) {
    case Button1:
      event->window_input.input_number = 0;
      break;
    case Button2:
      event->window_input.input_number = 1;
      break;
    case Button3:
      event->window_input.input_number = 2;
      break;
    }
    event->window_input.value = 0;
    event->window_input.x = xevent.xbutton.x;
    event->window_input.y = xevent.xbutton.y;
    convert_input_mask(xevent.xbutton.state, imgwin->inputs);
    event->window_input.num_inputs = imgwin->num_inputs;
    event->window_input.inputs = imgwin->inputs;
    return 1;

  case ConfigureNotify:
    event->type = EVENT_WINDOW_GEOMETRY;
    event->window_geometry.xloc = xevent.xconfigure.x;
    event->window_geometry.yloc = xevent.xconfigure.y;
    event->window_geometry.xsize = xevent.xconfigure.width;
    event->window_geometry.ysize = xevent.xconfigure.height;
    return 1;

  case KeyPress: 
    event->type = EVENT_CHARACTER;
    event->character.key = 0;
    len = XLookupString((XKeyEvent*)&xevent,
			&event->character.key, 1,
			&keysym_return, NULL);
    if(event->character.key == 0) goto get_another;  /* go ahead and sue me */
    event->character.x = xevent.xkey.x;
    event->character.y = xevent.xkey.y;
    convert_input_mask(xevent.xkey.state, imgwin->inputs);
    event->character.num_inputs = imgwin->num_inputs;
    event->character.inputs = imgwin->inputs;
    return 1;
  
  case Expose: 
    event->type = EVENT_EXPOSE;
    event->expose.x = xevent.xexpose.x;
    event->expose.y = xevent.xexpose.y;
    event->expose.w = xevent.xexpose.width;
    event->expose.h = xevent.xexpose.height;
    return 1;

  }

  /* default case */
  event->type = EVENT_NONE;
  return 0;
}


