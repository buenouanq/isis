/* *********************************************************************
   Template source file for using C functions in Isis
   
   This file holds the definitions of C functions you want to be able
   to call from an Isis program.  These functions must follow a
   special prototype and must be written carefully to preserve the
   integrity of the Isis memory management system.  See the Isis web
   site under Development for more information.

   This file also holds a special function (called bind_cool_library()
   below) that will bind all of these C functions into the Isis
   interpreter when it is called.
   
   You must declare all of your C functions at the top of the file
   since are referred to inside of bind_cool_library().
   
   The header file "isis_cfunc_template.h" contains a single
   declaration of bind_cool_library().  Your interpreter's main.c
   should include this file and call this function to bind your
   symbols into Isis.  Be sure to edit both files appropriately if you
   change the names of functions.
   
   ********************************************************************* */

#include <stdio.h>                /* Should include before isis.h */
#include <isis.h>                 /* Holds all Isis interpreter info */

#include "isis_cfunc_template.h"  /* Holds a declaration bind_cool_library() */

/************************************************************************/
/* Function declarations */

Value *cool_func(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv);


/************************************************************************/

/*----------------------------------------------------------------------*/
void bind_cool_library(Script *script)
{
  /* Bind a variable to indicate this library is loaded */

  bindBool(script, "isis_cool", 1);
  
  /* Bind all the C functions and constants for this library */
  
  bindCfunc(script, "cool-function", cool_func, 0);  
  bindInt(script, "cool-constant", 42);
}


/************************************************************************/
/* Function definitions */

/*----------------------------------------------------------------------*/
Value *cool_func(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  printf("The Cool function has been called ! \n");
  return newBool(1);
}
