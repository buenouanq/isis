/*   color.c

     Color Processing Isis interface routines

     This file contains the color relaxation routines, and the binding code
     for the entire library.
     Other files in the library include:
     color_br.c
     color_const.c

     J. Watlington, 12/96

     Added isis_color_yiq.c dakss 12/1/98

     from template provided by Stefan Agamanolis.
*/

#include <stdio.h>                /* Must include before isis.h */
#include <isis.h>                 /* Holds all Isis interpreter info */

#include "isis_color.h"           /* Holds a declaration bind_my_funcs() */
#include "isis_color_private.h"


/* ------------------------------------------------------------ */
void bind_color_library(Script *script)
{
  bindBool(script, "isis_color", 1);
  
  bindCfunc( script, "image-color-relax-chan", 
	     image_color_relax_chan, 0 );
  bindCfunc( script, "image-color-relax-byte-chan",
	     image_color_relax_byte_chan, 0 );
  
  bindCfunc( script, "image-color-raw-rgb-to-br",
	     image_color_raw_rgb_to_br, 0 );
  bindCfunc( script, "image-color-raw-rgb-to-y",
	     image_color_raw_rgb_to_y, 0 );
  bindCfunc( script, "image-color-raw-ybr-to-rgb",
	     image_color_raw_ybr_to_rgb, 0 );
  bindCfunc( script, "image-color-raw-rgb-to-yiq",
	     image_color_raw_rgb_to_yiq, 0 );
  bindCfunc( script, "image-color-raw-yiq-to-rgb",
	     image_color_raw_yiq_to_rgb, 0 );
}


