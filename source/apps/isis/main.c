/* ****************************************************************
   A standard Isis interpreter application
   
   (includes support for reading global and local bootstrap files and
   for reading from filename arguments on the command line)
   
   by Stefan Agamanolis
   **************************************************************** */

#include <stdio.h>       /* Must include before isis.h */
#include <stdlib.h>
#include <string.h>
#include <isis.h>        /* Isis interpreter declarations */

/**** Include any external Isis library header files here ****/
#include <isis_unix.h>
#include <isis_improc.h>
#include <isis_xwin.h>
#include <isis_jpeg.h>
#include <isis_gif.h>
#include <isis_color.h>
#include <isis_stats.h>
#include <isis_seg.h>
#include <isis_readline.h>
#include <isis_gl12.h>
#include <isis_glu13.h> 
#include <isis_glut.h>
#include <isis_glx.h>
#include <isis_alsa.h>
#include <isis_v4l2.h>
/* #include <isis_sdl.h> */
/* #include <isis_cmuface.h> */

#define ISIS_ROOT "/usr/local/isis"

int main(int argc, char *argv[])
{
  Script *script;
  int boot = 1;
  char *root, *str;

  parse_command_line_options(argc, argv);    /* sets startup options */

  initialize_interpreter();    /* call after getting options */
  
  script = create_script("Isis", stdin, stdout);    /* create a script */

  /**** Add calls to bind external functions, libraries, etc., here: ****/

  bind_unix_library(script);
  bind_improc_library(script);
  bind_stats_library(script);
  bind_color_library(script);
  bind_seg_library(script);
  bind_xwin_library(script);
  bind_jpeg_library(script);
  bind_gif_library(script);
  bind_readline_library(script);
  bind_gl12_library(script);
  bind_glu13_library(script);
  bind_glut_library(script);
  bind_glx_library(script);
  bind_alsa_library(script);
  bind_v4l2_library(script);
  /* bind_sdl_library(script); */
  /* bind_cmuface_library(script); */
  
  /* Determine if bootstrap should be suppressed */

  if(argc > 1 && argv[1][0] == '-' && strchr(argv[1], 'b')) boot = 0;

  if(boot) {
    if (str = getenv("ISIS_BOOTSTRAP")) {
      interpret_filename(script, str);
    }
    else if (root = getenv("ISIS_ROOT")) {
      str = malloc(strlen(root) + strlen(ISIS_BOOTSTRAP) + 1);
      strcpy(str, root);
      strcat(str, ISIS_BOOTSTRAP);
      interpret_filename(script, str);
      free(str);
    }
    else {
      str = malloc(strlen(ISIS_ROOT) + strlen(ISIS_BOOTSTRAP) + 1);
      strcpy(str, ISIS_ROOT);
      strcat(str, ISIS_BOOTSTRAP);
      interpret_filename(script, str);
      free(str);
    }
  }

  /* Interpret all files on the command line */

  interpret_command_line_files(script, argc, argv);

  close_script(script);    /* Close the script cleanly at the end */

  return 0;
}

