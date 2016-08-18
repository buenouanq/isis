/* Linux SDL interface library for Isis
   stefan agamanolis
   october 2000
*/

#include <stdio.h>
#include <isis.h>
#include <SDL/SDL.h>
#include "isis_sdl.h"

/**************************************************************************/
/* function prototypes */

static Value *isis_sdl_init(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_quit(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_setup(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv);
static Value *isis_sdl_create(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv);
static Value *isis_sdl_free(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_info(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_update(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv);
static Value *isis_sdl_flip(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_mustlock(Script *script, char *proc_name, int call_data,
				int argc, Value **argv);
static Value *isis_sdl_lock(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_unlock(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv);
static Value *isis_sdl_blit(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_fill(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv);
static Value *isis_sdl_cursor(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv);
static Value *isis_sdl_mouse(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv);
static Value *isis_sdl_enable(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv);
static Value *isis_sdl_event(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv);

/**************************************************************************/
/* bindings */

/* --------------------------------------------------------------------- */
void bind_sdl_library(Script *script)
{
  bindBool(script, "isis_sdl", 1);
  
  bindCfunc(script, "sdl-init", isis_sdl_init, 0);
  bindCfunc(script, "sdl-quit", isis_sdl_quit, 0);
  bindCfunc(script, "sdl-setup", isis_sdl_setup, 0);

  bindCfunc(script, "sdl-create", isis_sdl_create, 0);
  bindCfunc(script, "sdl-free", isis_sdl_free, 0);
  bindCfunc(script, "sdl-info", isis_sdl_info, 0);

  bindCfunc(script, "sdl-update", isis_sdl_update, 0);
  bindCfunc(script, "sdl-flip", isis_sdl_flip, 0);
  bindCfunc(script, "sdl-mustlock", isis_sdl_mustlock, 0);
  bindCfunc(script, "sdl-lock", isis_sdl_lock, 0);
  bindCfunc(script, "sdl-unlock", isis_sdl_unlock, 0);

  bindCfunc(script, "sdl-blit", isis_sdl_blit, 0);
  bindCfunc(script, "sdl-fill", isis_sdl_fill, 0);

  bindCfunc(script, "sdl-cursor", isis_sdl_cursor, 0);

  bindCfunc(script, "sdl-mouse", isis_sdl_mouse, 0);

  bindCfunc(script, "sdl-enable", isis_sdl_enable, 1);
  bindCfunc(script, "sdl-ignore", isis_sdl_enable, 0);
  
  bindCfunc(script, "sdl-poll", isis_sdl_event, 1);
  bindCfunc(script, "sdl-wait", isis_sdl_event, 0);

  bindInt(script, "sdl-event-active", SDL_ACTIVEEVENT);
  bindInt(script, "sdl-event-key-down", SDL_KEYDOWN);
  bindInt(script, "sdl-event-key-up", SDL_KEYUP);
  bindInt(script, "sdl-event-mouse-motion", SDL_MOUSEMOTION);
  bindInt(script, "sdl-event-mouse-down", SDL_MOUSEBUTTONDOWN);
  bindInt(script, "sdl-event-mouse-up", SDL_MOUSEBUTTONUP);
  bindInt(script, "sdl-event-quit", SDL_QUIT);
  bindInt(script, "sdl-event-resize", SDL_VIDEORESIZE);

  bindInt(script, "sdl-active-mouse", SDL_APPMOUSEFOCUS);
  bindInt(script, "sdl-active-input", SDL_APPINPUTFOCUS);
  bindInt(script, "sdl-active-app", SDL_APPACTIVE);
}

/**************************************************************************/
/* helper functions */

static SDL_Surface *screen = NULL;

/**************************************************************************/
/* function definitions */

/* --------------------------------------------------------------------- */
static Value *isis_sdl_init(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  int len;
  char *caption;
  
  checkCount(1, "caption");
  checkString(0, "caption");
  
  if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) return NullValue;
  
  len = Listsize(argv[0]) + 1;
  caption = (char *) malloc(len);
  getString(argv[0], caption, len);
  
  SDL_WM_SetCaption(caption, caption);

  SDL_EnableUNICODE(1);
  
  return newBool(1);
}

/* --------------------------------------------------------------------- */
static Value *isis_sdl_quit(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  checkCount(0, NULL);
  
  SDL_Quit();
  
  screen = NULL;

  return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_sdl_setup(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv)
{
  int width, height, flags;

  checkCount(4, "framesize, fullscreen flag, hardware flag, doublebuf flag");
  checkIntList(0, "framesize");
  checkBool(1, "fullscreen flag");
  checkBool(2, "hardware flag");
  checkBool(3, "doublebuf flag");
  
  width = getInt( Listitem(argv[0], 0) );
  height = getInt( Listitem(argv[0], 1) );

  flags = 0;
  flags |= getBool(argv[1]) ? SDL_FULLSCREEN : 0;
  flags |= getBool(argv[2]) ? SDL_HWSURFACE : 0;
  flags |= getBool(argv[3]) ? SDL_DOUBLEBUF : 0;

  screen = SDL_SetVideoMode(width, height, 0, flags);

  if(screen == NULL) return NullValue;
  return newAddr(screen);
}

/* --------------------------------------------------------------------- */
static Value *isis_sdl_create(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv)
{
  int width, height, flags;
  SDL_Surface *surface;

  checkCount(2, "size, hardware flag");
  checkIntList(0, "framesize");
  checkBool(1, "hardware flag");

  width = getInt( Listitem(argv[0], 0) );
  height = getInt( Listitem(argv[0], 1) );

  flags = 0;
  flags |= getBool(argv[1]) ? SDL_HWSURFACE : 0;
  
  surface = SDL_CreateRGBSurface(flags, width, height, 32, 
				 screen->format->Rmask,
				 screen->format->Gmask,
				 screen->format->Bmask,
				 screen->format->Amask);
  
  if(surface == NULL) return NullValue;
  return newAddr(surface);
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_free(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  checkCount(1, "surface");
  checkAddr(0, "surface");

  SDL_FreeSurface(getAddr(argv[0]));

  return NullValue;
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_info(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  SDL_Surface *surface;
  Value *size, *steps, *addrs, *flags, *retval;

  checkCount(1, "surface");
  checkAddr(0, "surface");

  surface = getAddr(argv[0]);

  size = newBasicListva(2,
			IsisInt, (int) surface->w,
			IsisInt, (int) surface->h);
  steps = newBasicListva(2,
			 IsisInt, (int) surface->format->BytesPerPixel,
			 IsisInt, (int) surface->pitch);
  addrs = newBasicListva(4, 
			 IsisAddr, (void *) (surface->pixels + (surface->format->Rshift / 8)),
			 IsisAddr, (void *) (surface->pixels + (surface->format->Gshift / 8)),
			 IsisAddr, (void *) (surface->pixels + (surface->format->Bshift / 8)),
			 IsisAddr, (void *) (surface->pixels + (surface->format->Ashift / 8)));
  
  flags = newBasicListva(3,
			 IsisBool, (int) (surface->flags & SDL_FULLSCREEN),
			 IsisBool, (int) (surface->flags & SDL_HWSURFACE),
			 IsisBool, (int) (surface->flags & SDL_DOUBLEBUF));
			 
  retval = newListva(4, size, steps, addrs, flags);

  freeValue(size);
  freeValue(steps);
  freeValue(addrs);
  freeValue(flags);
  
  return retval;
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_update(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv)
{
  checkCount(1, "surface");
  checkAddr(0, "surface");

  SDL_UpdateRect(getAddr(argv[0]), 0, 0, 0, 0);

  return NullValue;
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_flip(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  checkCount(1, "surface");
  checkAddr(0, "surface");

  return newBool( SDL_Flip(getAddr(argv[0])) == 0);
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_mustlock(Script *script, char *proc_name, int call_data,
				int argc, Value **argv)
{
  checkCount(1, "surface");
  checkAddr(0, "surface");

  return newBool( SDL_MUSTLOCK(((SDL_Surface *)getAddr(argv[0]))) );
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_lock(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  checkCount(1, "surface");
  checkAddr(0, "surface");

  return newBool( SDL_LockSurface(getAddr(argv[0])) == 0 );
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_unlock(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv)
{
  checkCount(1, "surface");
  checkAddr(0, "surface");
  
  SDL_UnlockSurface(getAddr(argv[0]));
  
  return NullValue;
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_blit(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  checkCount(2, "src surface, dst surface");
  checkAddr(0, "src surface");
  checkAddr(1, "dst surface");
  
  return newBool( SDL_BlitSurface(getAddr(argv[0]), NULL, getAddr(argv[1]), NULL) == 0);
}


/* --------------------------------------------------------------------- */
static Value *isis_sdl_fill(Script *script, char *proc_name, int call_data,
			    int argc, Value **argv)
{
  SDL_Surface *surface;

  checkCount(4, "surface, red, green, blue");  
  checkAddr(0, "surface");
  checkInt(1, "red");
  checkInt(2, "green");
  checkInt(3, "blue");

  surface = getAddr(argv[0]);

  return newBool( SDL_FillRect(surface, NULL, 
			       SDL_MapRGB(surface->format, 
					  getInt(argv[1]), 
					  getInt(argv[2]), 
					  getInt(argv[3]))) == 0 );
}

/* --------------------------------------------------------------------- */
static Value *isis_sdl_cursor(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv)
{
  checkCount(1, "show-cursor-flag");
  checkBool(0, "show-cursor-flag");
  
  SDL_ShowCursor( getBool(argv[0]) );
  
  return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_sdl_mouse(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv)
{
  int x, y, mask;
  Value *posval, *butval, *retval;

  checkCount(0, NULL);
  
  mask = SDL_GetMouseState( &x, &y );

  posval = newIntListva(2, x, y);
  butval = newBoolListva(3, 
			 mask & SDL_BUTTON_LEFT, 
			 mask & SDL_BUTTON_MIDDLE, 
			 mask & SDL_BUTTON_RIGHT);
  retval = newListva(2, posval, butval);
  freeValue(posval);
  freeValue(butval);
  return retval;
}

/* --------------------------------------------------------------------- */
static Value *isis_sdl_enable(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv)
{
  checkCount(1, "event type");
  checkInt(0, "event type");
  
  SDL_EventState( getInt(argv[0]), (call_data ? SDL_ENABLE : SDL_IGNORE) );
  
  return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_sdl_event(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv)
{
  SDL_Event event;
  int res;
  Value *retval, *typeval, *posval, *butval, *sizeval;

  checkCount(0, NULL);

  if(call_data) res = SDL_PollEvent(&event);
  else res = SDL_WaitEvent(&event);
  
  if(res == 0) return NullValue;  /* no event */

  switch(event.type) {
    
  case SDL_ACTIVEEVENT:
    return newBasicListva(3, 
			  IsisInt, event.type,
			  IsisInt, event.active.state,
			  IsisBool, event.active.gain);
    
  case SDL_KEYDOWN:
  case SDL_KEYUP:
    return newBasicListva(3, 
			  IsisInt, event.type,
			  IsisInt, event.key.keysym.sym,
			  IsisInt, event.key.keysym.unicode);
    
  case SDL_MOUSEMOTION:
    typeval = newInt(event.type);
    posval = newIntListva(2, event.motion.x, event.motion.y);
    retval = newListva(2, typeval, posval);
    freeValue(typeval);
    freeValue(posval);
    return retval;
    
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    typeval = newInt(event.type);
    butval = newInt(event.button.button);
    posval = newIntListva(2, event.button.x, event.button.y);
    retval = newListva(3, typeval, butval, posval);
    freeValue(typeval);
    freeValue(butval);
    freeValue(posval);
    return retval;
    
  case SDL_QUIT:
    return newBasicListva(1, IsisInt, SDL_QUIT);
    
  case SDL_VIDEORESIZE:
    typeval = newInt(event.type);
    sizeval = newIntListva(2, event.resize.w, event.resize.h);
    retval = newListva(2, typeval, sizeval);
    freeValue(typeval);
    freeValue(sizeval);
    return retval;
    
  }
  
  return NullValue;
}


