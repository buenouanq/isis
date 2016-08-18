/* *********************************************************************
   Template header file for using C functions in Isis
  
   This file contains a declaration of the function that will bind all
   of your C functions into Isis.  Your main interpreter source file
   will include this file and call this function before any script
   expressions are evaluated. */


void bind_v4l2_library(Script *script);

