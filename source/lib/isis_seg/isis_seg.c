/* Image Segmentation library */
/* John Watlington and Stefan Agamanolis */

#include <stdio.h>
#include <isis.h>
#include "isis_seg.h"
#include "isis_seg_private.h"

/* ------------------------------------------------------------ */
void bind_seg_library(Script *script)
{
  bindBool(script, "isis_seg", 1);
  
  bindCfunc(script, "seg-init-bkseg-yuv", isis_init_bkseg_yuv, 0);
  bindCfunc(script, "seg-bkseg-yuv", isis_bkseg_yuv, 0);
  
  bindCfunc(script, "seg-init-chromakey", isis_init_chromakey, 0);
  bindCfunc(script, "seg-chromakey", isis_chromakey, 0);
  
  bindCfunc(script, "seg-simple-bkseg", isis_simple_bkseg, 0);
  bindCfunc(script, "seg-simple-bkseg-yuv", isis_simple_bkseg_yuv, 0);

  bindCfunc(script, "seg-init-var-bkseg-table", isis_init_var_bkseg_table, 0);

  bindCfunc(script, "seg-var-bkseg-yuv", isis_var_bkseg_yuv, 0);
  bindCfunc(script, "seg-var-bkseg-rgb", isis_var_bkseg_rgb, 0);

  bindCfunc(script, "seg-fill-holes", isis_fill_holes, 0);
  bindCfunc(script, "seg-init-encode-image-yuv", isis_init_encode_image_yuv, 0);
  bindCfunc(script, "seg-init-encode-image-rgb", isis_init_encode_image_rgb,0);

  bindCfunc(script, "seg-encode-image-yuv", isis_encode_image_yuv, 0);
  bindCfunc(script, "seg-encode-image-rgb", isis_encode_image_rgb, 0);

  bindCfunc(script, "seg-decode-image-yuv", isis_decode_image_yuv, 0);
  bindCfunc(script, "seg-decode-image-rgb-planar", isis_decode_image_rgb_planar, 0);
  bindCfunc(script, "seg-decode-image-rgb-interleaved", isis_decode_image_rgb_interleaved, 0);

  bindCfunc(script, "seg-color-rgb", isis_seg_color_rgb, 0);
}

