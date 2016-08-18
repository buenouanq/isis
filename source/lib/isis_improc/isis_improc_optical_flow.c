#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "isis_improc_private.h"

struct ps {
  int        begin;
  int        end;
  double     *coeff;
};

typedef struct ps PHASE_SAMPLE;
typedef struct ps FILTER;

struct pp {
  int              nphase;
  PHASE_SAMPLE    *phase;
};

typedef struct pp POLYPHASE;


/* declarations of functions, added by stefan: */

static void decimate_mvf (int *outbuf, int *inbuf, int height, int width);
static void filt_init (FILTER *filter);
static void hdecim (int *srcline, int width, int M, double *outline, FILTER *filter);
static void vdecim (double *inbuf, int height, int width, int M, int *outbuf, FILTER *filter);
static void mvf_update (double *mvfout, int *images, int height, int width, double dt, double *mvfin);
static void mvmult(double *A, double *B, double *C, int N, int M);
static double bilin(double **I, double y, double x);
static void horiz_filter(double *buf, int hdim, int vdim, double *hfilt, int hfdim);
static void vert_filter(double *buf, int hdim, int vdim, double *vfilt, int vfdim);
static double *vclr(register double *x, register int n);
static double vconv(register double *x, register double *y, register int n);
static double *vadd(register double *z, register double *x, double a, register double *y, register int N);
static double *vmove(register double *z, register double *x, register int N);
static double *vrev(register double *z, register double *x, register int N);
static double *vscal(register double *z, register double a, register double *x, register int N);
static char **arrayalloc (unsigned int imax, unsigned int jmax, unsigned int size);
static void arrayfree (double **v);
static void mat_invert (double *a, int n, double *y);
static int ludcmp (double *a, int n, int *indx, double *d);
static void lubksb (double *a, int n, int *indx, double *b);
static void eigenvalue (double *mat, double *eigenmax, double *eigenmin);
static void eigenvector (double *evect, double *mat, double eigenvalue);
static void mat_add (double *m1, double *m2, int ylen, int xlen);
static void mat_cross (double *vec1, double *vec2, int len, double *mat);
static void nrerror (char *error_text);
static double *new_vector(int nl, int nh);
static void free_vector(double *v, int nl, int nh);
static void subsample(register char *outptr, register char *inptr, int nbps, int h_factor, int h_samples);
static void upsample(register char *outptr, register char *inptr, int nbps, int h_factor, int h_size);





/*
 *   c a l c _ m v f _ b y t e _ C                                  Bill Butera
 *                                                                  MIT Media Lab
 *                                                                  Cambridge, MA
 *
 *   version 1.0                                                    December 1, 1997
 *
 *   Compute a 2D displacement field given two input images. The computed field is
 *   assumed to represent the function describing the warp from the first image
 *   to the second.
 *
 *   This is basically a two-stage hierarchical process. In the first stage, the 2-image
 *   input sequence is successively decimated by a factor of 2 for 5 iterations. In the
 *   second stage, the decimated images are used to successively refile the computed
 *   motion vector field.
 */

void calc_mvf_byte_C (unsigned char    *image, 
		      unsigned char    *model, 
		      int               height, 
		      int               width, 
		      float            *vy, 
		      float            *vx)
{
  int                             i;
  int                    *lev1,  *lev2,  *lev3,  *lev4,  *lev5;
  int                             h2,     h3,     h4,     h5;
  int                             w2,     w3,     w4,     w5;
  int                     fs1,    fs2,    fs3,    fs4,    fs5;
  int                    *ptrIm, *ptrMo;
  double                  *mvf1,  *mvf2,  *mvf3,  *mvf4,  *mvf5;
  double                  *ptrVx, *ptrVy;

  fs1 = height * width;
  if ( ((height % 16) != 0) || ((width % 16) != 0) )  {
    fprintf (stderr, "calc_mvf_C : Unexpected dimensions for anchor frames. Must be divisible by 16, continuing ...");
    for (i=0; i<fs1; ++i)
      vx[i] = vy[i] = (float) 0.0;
    return;  }

  h2 = height / 2;     w2 = width / 2;    fs2 = h2 * w2;
  h3 = h2 / 2;         w3 = w2 / 2;       fs3 = h3 * w3;
  h4 = h3 / 2;         w4 = w3 / 2;       fs4 = h4 * w4;
  h5 = h4 / 2;         w5 = w4 / 2;       fs5 = h5 * w5;


  /*** Stage 1 :  Create a 5 level pyramid of images  ***/

  if (((lev1 = (int *) calloc ((size_t)(fs1 * 2), sizeof(int))) == NULL) ||
      ((lev2 = (int *) calloc ((size_t)(fs2 * 2), sizeof(int))) == NULL) ||
      ((lev3 = (int *) calloc ((size_t)(fs3 * 2), sizeof(int))) == NULL) ||
      ((lev4 = (int *) calloc ((size_t)(fs4 * 2), sizeof(int))) == NULL) ||
      ((lev5 = (int *) calloc ((size_t)(fs5 * 2), sizeof(int))) == NULL) )   {
    fprintf (stderr, "Failed allocating buffer space for the decimated images") ;
    return;  }

  ptrMo = lev1;
  ptrIm = lev1 + fs1;
  for (i=0; i<fs1; ++i)  {
    *ptrIm++ = (int) image[i];
    *ptrMo++ = (int) model[i];  }


  decimate_mvf (lev2,      lev1,      height, width);
  decimate_mvf (lev2+fs2,  lev1+fs1,  height, width);
  decimate_mvf (lev3,      lev2,      h2,     w2);
  decimate_mvf (lev3+fs3,  lev2+fs2,  h2,     w2);
  decimate_mvf (lev4,      lev3,      h3,     w3);
  decimate_mvf (lev4+fs4,  lev3+fs3,  h3,     w3);
  decimate_mvf (lev5,      lev4,      h4,     w4);
  decimate_mvf (lev5+fs5,  lev4+fs4,  h4,     w4);



  /*** Stage 2 :  Generate motion estimates and successively refine
    them using the pyramid images                    ***/

  if (((mvf1 = (double *) calloc ((size_t)(fs1 * 2), sizeof(double))) == NULL)   ||
      ((mvf2 = (double *) calloc ((size_t)(fs2 * 2), sizeof(double))) == NULL)   ||
      ((mvf3 = (double *) calloc ((size_t)(fs3 * 2), sizeof(double))) == NULL)   ||
      ((mvf4 = (double *) calloc ((size_t)(fs4 * 2), sizeof(double))) == NULL)   ||
      ((mvf5 = (double *) calloc ((size_t)(fs5 * 2), sizeof(double))) == NULL) )  {
    fprintf (stderr, "Failed allocating buffer space for the displacement vectors");
    return;  }

  mvf_update (mvf5, lev5, h5,     w5,    1.0,  NULL);
  mvf_update (mvf4, lev4, h4,     w4,    1.0,  mvf5);
  mvf_update (mvf3, lev3, h3,     w3,    1.0,  mvf4);
  mvf_update (mvf2, lev2, h2,     w2,    1.0,  mvf3);
  mvf_update (mvf1, lev1, height, width, 1.0,  mvf2);


  /*** Clean Up :  Copy the displacement vectors off to the output buffers 
    and bail   ***/

  ptrVx = mvf1;
  ptrVy = mvf1 + fs1;
  for (i=0; i<fs1; ++i)  {
    vx[i] = (float) *ptrVx++;
    vy[i] = (float) *ptrVy++;  }


  free ((void *) mvf1);      free ((void *) mvf2);
  free ((void *) mvf3);      free ((void *) mvf4);
  free ((void *) mvf5);
  free ((void *) lev1);      free ((void *) lev2);
  free ((void *) lev3);      free ((void *) lev4);
  free ((void *) lev5);

  return;

}






























/*
 *   c a l c _ m v f _ i n t _ C                                    Bill Butera
 *                                                                  MIT Media Lab
 *                                                                  Cambridge, MA
 *
 *   version 1.0                                                    December 1, 1997
 *
 *   Compute a 2D displacement field given two input images. The computed field is
 *   assumed to represent the function describing the warp from the first image
 *   to the second.
 *
 *   This is basically a two-stage hierarchical process. In the first stage, the 2-image
 *   input sequence is successively decimated by a factor of 2 for 5 iterations. In the
 *   second stage, the decimated images are used to successively refile the computed
 *   motion vector field.
 */

void calc_mvf_int_C (int              *image, 
		     int              *model, 
		     int               height, 
		     int               width, 
		     float            *vy, 
		     float            *vx)
{
  int                             i;
  int                    *lev1,  *lev2,  *lev3,  *lev4,  *lev5;
  int                             h2,     h3,     h4,     h5;
  int                             w2,     w3,     w4,     w5;
  int                     fs1,    fs2,    fs3,    fs4,    fs5;
  int                    *ptrIm, *ptrMo;
  double                  *mvf1,  *mvf2,  *mvf3,  *mvf4,  *mvf5;
  double                  *ptrVx, *ptrVy;

  fs1 = height * width;
  if ( ((height % 16) != 0) || ((width % 16) != 0) )  {
    fprintf (stderr, "calc_mvf_C : Unexpected dimensions for anchor frames. Must be divisible by 16, continuing ...");
    for (i=0; i<fs1; ++i)
      vx[i] = vy[i] = (float) 0.0;
    return;  }

  h2 = height / 2;     w2 = width / 2;    fs2 = h2 * w2;
  h3 = h2 / 2;         w3 = w2 / 2;       fs3 = h3 * w3;
  h4 = h3 / 2;         w4 = w3 / 2;       fs4 = h4 * w4;
  h5 = h4 / 2;         w5 = w4 / 2;       fs5 = h5 * w5;


  /*** Stage 1 :  Create a 5 level pyramid of images  ***/

  if (((lev1 = (int *) calloc ((size_t)(fs1 * 2), sizeof(int))) == NULL) ||
      ((lev2 = (int *) calloc ((size_t)(fs2 * 2), sizeof(int))) == NULL) ||
      ((lev3 = (int *) calloc ((size_t)(fs3 * 2), sizeof(int))) == NULL) ||
      ((lev4 = (int *) calloc ((size_t)(fs4 * 2), sizeof(int))) == NULL) ||
      ((lev5 = (int *) calloc ((size_t)(fs5 * 2), sizeof(int))) == NULL) )   {
    fprintf (stderr, "Failed allocating buffer space for the decimated images") ;
    return;  }

  ptrMo = lev1;
  ptrIm = lev1 + fs1;
  for (i=0; i<fs1; ++i)  {
    *ptrIm++ = image[i];
    *ptrMo++ = model[i];  }


  decimate_mvf (lev2,      lev1,      height, width);
  decimate_mvf (lev2+fs2,  lev1+fs1,  height, width);
  decimate_mvf (lev3,      lev2,      h2,     w2);
  decimate_mvf (lev3+fs3,  lev2+fs2,  h2,     w2);
  decimate_mvf (lev4,      lev3,      h3,     w3);
  decimate_mvf (lev4+fs4,  lev3+fs3,  h3,     w3);
  decimate_mvf (lev5,      lev4,      h4,     w4);
  decimate_mvf (lev5+fs5,  lev4+fs4,  h4,     w4);



  /*** Stage 2 :  Generate motion estimates and successively refine
    them using the pyramid images                    ***/

  if (((mvf1 = (double *) calloc ((size_t)(fs1 * 2), sizeof(double))) == NULL)   ||
      ((mvf2 = (double *) calloc ((size_t)(fs2 * 2), sizeof(double))) == NULL)   ||
      ((mvf3 = (double *) calloc ((size_t)(fs3 * 2), sizeof(double))) == NULL)   ||
      ((mvf4 = (double *) calloc ((size_t)(fs4 * 2), sizeof(double))) == NULL)   ||
      ((mvf5 = (double *) calloc ((size_t)(fs5 * 2), sizeof(double))) == NULL) )  {
    fprintf (stderr, "Failed allocating buffer space for the displacement vectors");
    return;  }

  mvf_update (mvf5, lev5, h5,     w5,    1.0,  NULL);
  mvf_update (mvf4, lev4, h4,     w4,    1.0,  mvf5);
  mvf_update (mvf3, lev3, h3,     w3,    1.0,  mvf4);
  mvf_update (mvf2, lev2, h2,     w2,    1.0,  mvf3);
  mvf_update (mvf1, lev1, height, width, 1.0,  mvf2);


  /*** Clean Up :  Copy the displacement vectors off to the output buffers 
    and bail   ***/

  ptrVx = mvf1;
  ptrVy = mvf1 + fs1;
  for (i=0; i<fs1; ++i)  {
    vx[i] = (float) *ptrVx++;
    vy[i] = (float) *ptrVy++;  }


  free ((void *) mvf1);      free ((void *) mvf2);
  free ((void *) mvf3);      free ((void *) mvf4);
  free ((void *) mvf5);
  free ((void *) lev1);      free ((void *) lev2);
  free ((void *) lev3);      free ((void *) lev4);
  free ((void *) lev5);

  return;

}




















































/*
 *   decimate_mvf                    Local decimation function with parameters hard wired for 
 *                                   use with the Ed Krause's displacement estimator.
 *
 */

static void decimate_mvf (int *outbuf, int *inbuf, int height, int width)
{
  int                   oheight, owidth;
  int                  *ptrIn;
  int                   line;
  double                *ptrTemp, *tempbuf;
  FILTER                filter;

  filt_init (&filter);        /**    Read filter coefficients into filter structures   **/
  

  /*****    Malloc buffer space for the intermediary frame   *****/

  oheight = height / 2;
  owidth  = width / 2;
  if ((tempbuf = (double *) malloc ((unsigned)(height * owidth * sizeof(double)))) == NULL)  {
    fprintf (stderr, "Died trying to malloc space for intermediary frame storage. Bye", NULL);
    return;  }


  /*****    Perform the horizontal decimation   *****/

  ptrIn = inbuf;
  ptrTemp = tempbuf;
  for (line=0; line<height; line++, ptrIn+=width, ptrTemp+=owidth)
    hdecim (ptrIn, width, 2, ptrTemp, &filter); 

  /*****    Perform the vertical filtering   *****/

  vdecim (tempbuf, height, owidth, 2, outbuf, &filter);
  

  /*****    Close up shop   *****/

  free ((void *) filter.coeff);
  free ((void *) tempbuf);

  return;
}






/*
 *        f i l t _ i n i t                        Bill Butera
 *                                                 MIT Media Lab
 *                                                 Cambridge, MA
 *
 *        version 1.0                              December 1, 1997
 *
 */

static void filt_init (FILTER *filter)
{
  int            begin, end, center;
  int            filtlen;
  double          temp;
  
  filtlen = 9;
  if ((filter->coeff = (double *) calloc ((size_t)filtlen, sizeof(double))) == NULL)  {
    fprintf (stderr, "Failed allocating buffer space for the smoothing filter");
    return;  }

  filter->coeff[0]  = 0.00391;
  filter->coeff[1]  = 0.03125;
  filter->coeff[2]  = 0.10938;
  filter->coeff[3]  = 0.21875;
  filter->coeff[4]  = 0.27344;
  filter->coeff[5]  = 0.21875;
  filter->coeff[6]  = 0.10938;
  filter->coeff[7]  = 0.03125;
  filter->coeff[8]  = 0.00391;


  /*    mirror filter about center   ....   for convolution   */

  for (begin=0, end=(filtlen-1); begin<end; begin++, end--) {
    temp = filter->coeff[begin];
    filter->coeff[begin] = filter->coeff[end];
    filter->coeff[end] = temp;  }
  
  center = (filtlen - 1) / 2;
  filter->begin = -1 * center;
  filter->end = filtlen - 1 - center;
  return;
}













/*
 *        h d e c i m                 horizontal decimation.
 * 
 *        The boundary conditions are treated by padding with zeros
 */

static void hdecim (int *srcline, int width, int M, double *outline, FILTER *filter)
{

  int                   osam;
  register int          ftap, isam, index, tap;
  register double        sum;


  for (osam=0, isam=0; isam<width; isam+=M, osam++) {
    ftap=filter->begin;
    index=isam+ftap;

    if ((isam + filter->begin) < 0) {
      for (sum=0.0, tap=0; ftap<=filter->end; ftap++, index++, tap++)
	if (index < 0)
	  sum += filter->coeff[tap] * (double)srcline[0];
	else
	  sum += filter->coeff[tap] * (double)srcline[index]; }

    else if ((isam + filter->end) >= width) {
      for (sum=0.0, tap=0; ftap<=filter->end; ftap++, index++, tap++)
	if (index >= width)
	  sum += filter->coeff[tap] * (double)srcline[width-1];
	else
	  sum += filter->coeff[tap] * (double)srcline[index]; }
    else 
      for (sum=0.0, tap=0; ftap<=filter->end; ftap++, index++, tap++)
	sum += filter->coeff[tap] * (double)srcline[index];

    outline[osam] = sum;
  }
  return;
}

















/*
 *        v d e c i m             vertical decimation.
 *
 *        The boundary conditions are treated by padding with zeros.
 */

static void vdecim (double *inbuf, int height, int width, int M, int *outbuf, FILTER *filter)
{
  int                   srcline, ftap;
  register int          tap, pixel, line, point;
  register double        sum;
  int                  *ptrOut=outbuf;

  for (srcline=0; srcline<height; srcline+=M)
    for (pixel=0; pixel<width; pixel++) {
      line = srcline + filter->begin;
      point= (line * width) + pixel;

      if ((srcline + filter->begin) < 0) {
	for (sum=0.0, tap=0, ftap=filter->begin;   ftap<=filter->end;   ftap++, tap++, line++, point+=width)
	  if (line < 0)
	    sum += filter->coeff[tap] * inbuf[pixel];
	  else 
	    sum += filter->coeff[tap] * inbuf[point]; }

      else if ((srcline + filter->end) >= height) {
	for (sum=0.0, tap=0, ftap=filter->begin;   ftap<=filter->end;   ftap++, tap++, line++, point+=width)
	  if (line >= height)
	    sum += filter->coeff[tap] * inbuf[((height - 1) * width) + pixel];
	  else 
	    sum += filter->coeff[tap] * inbuf[point]; }

      else
	for (sum=0.0, tap=0, ftap=filter->begin;   ftap<=filter->end;   ftap++, tap++, line++, point+=width)
	  sum += filter->coeff[tap] * inbuf[point];

      *ptrOut++ = (int) floor(sum + 0.5);
    }

  return;
}


/*
   Copyright 1988 by the Massachusetts Institute of Technology
   All rights reserved.
   
   Permission to use, copy, and modify this software and its
   documentation for any purpose and without fee is hereby granted,
   provided that the above copyright notice appear in all copies and that
   both that copyright notice and this permission notice appear in
   supporting documentation, and that the name of M.I.T. not be used in
   advertising or publicity pertaining to distribution of the software
   without specific, written prior permission.  This software or any
   other copies thereof may not be provided or otherwise made available to
   others.  No title to or ownership of the software is hereby transferred.
   M.I.T. makes no representations about the suitability of this software for any
   purpose.  It is provided "as is" without express or implied warranty.
 */


#define NC              9
#define NB              6

#define C0  1.0
#define C1  x
#define C2  y
#define C3  x*y
#define C4  x*x
#define C5  y*y

#define abs(x)  ((x)<0 ? -(x) : (x))
#define fl_int(x)  ((x)<0 ? ((int)(x-0.5)) : ((int)(x+0.5)))
#define LRATIO          0.001
#define LARGE           1.E30
#define MEDT            0.5

#define         SUCCESS         1
#define         SMALL           1.0e-10
#define         TINY            1.0e-20
#define         SINGULAR       -1



static void mvf_update (double *mvfout, int *images, int height, int width, double dt, double *mvfin)
{
  double             A[NC][NC],B[NC],C[NC+1],**pin,**ptemp1,**ptemp2,**Vx,**Vy,
                     **Pmd,*pVx,*pVy,*vfilt,*uxfilt,*uyfilt,
                     *p1_ptr,*p2_ptr,*p3_ptr,bill_temp[NC][NC];
  double           **P,*p0,*p1,*p2,*p3,*p4,*p5;
  double           **A11,**A22,**A12,**B1,**B2,*a11,*a22,*a12,*b1,*b2;
  double             x,y,t,t1=0.0,t2,t3=1.0,mT=8.0,offset_y1=0.0,offset_y2=0.0;
  double             T0,lmax,tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
  double             SX1=0.0,SY1=0.0,SXX1=0.0,SYY1=0.0;
  long              dim[10],n_sets,vl1,
                    unsolved=0,decomposed=0,vclip=0,SN1=0;
  int               vsize,hsize,hdim,vdim,vfdim,uxfdim,uyfdim;
  int               subx=1,suby=1,subvx,subvy,dy;
  int               wx=3,wy=3,wt=2,bx=1,by=1,wx2,wy2;
  int               i,j,k,n,m,i0,i1,j0,j1;

  int              *ptrIm, ii, framesize;
  double            *ptrVx, *ptrVy, *ptrMvfIn;
  char            **arrayalloc (unsigned imax, unsigned jmax, unsigned size);


  t2 = dt;
  t1=(t1<0.0 ? 0.0 : t1);  t3=(t3>(wt-1.001) ? (wt-1.001) : t3);
  t2=(t2<t1 ? t1 : t2>t3 ? t3 : t2);

  n_sets = 2;
  dim[0] = (long) height;
  dim[1] = (long)width;
  vsize=height;  hsize=width;

  vfdim=5;
  if ((vfilt = (double *) calloc ((size_t)vfdim, sizeof(double))) == NULL) {
    fprintf (stderr, "Failure allocating buffer space for a box filter");
    return;  }
  vfilt[0] =  vfilt[1] =  vfilt[2] =  vfilt[3] = vfilt[4] = 0.2;

  uxfdim = 5;
  if ((uxfilt = (double *) calloc ((size_t)uxfdim, sizeof(double))) == NULL)  {
    fprintf (stderr,"Failure allocating buffer space for a triangular smoothing filter");
    return;  }
  uxfilt[0] = uxfilt[4] = 0.1;
  uxfilt[1] = uxfilt[3] = 0.5;
  uxfilt[2] = 0.8;
  uyfilt=uxfilt;  uyfdim=uxfdim;

  if(suby*(vsize/suby)!=vsize || subx*(hsize/subx)!=hsize)
    fprintf(stderr,"Warning: picture size (%d,%d) is not an integer\nmultiple of subsampling rate (%d,%d)\n",vsize,hsize,suby,subx);
  wx*=subx; wy*=suby;
  hdim=hsize+wx/2;  vdim=vsize+wy/2;  wt=(int)n_sets;
  if((pin=(double **)arrayalloc(wt*vdim+(wy-1)/2+1,hdim,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'pin'\n");
    exit(-1);
  }
	
  ptrIm = images;
  p1_ptr=pin[0];
  for(k=wt;k-->0;) {
    for(j=hdim*(wy/2);j-->0;)      *p1_ptr++ = 0.0;
    for(i=vsize;i-->0;) {
      for (j=wx/2;j-->0;)            *p1_ptr++ = 0.0;
      for (ii=0; ii<hsize; ++ii)      p1_ptr[ii] = (double) *ptrIm++;
      p1_ptr= &p1_ptr[hsize];
    }
  }


  for(j=hdim*(wy-1)/2+(wx-1)/2;j-->0;)*p1_ptr++ = 0;

  if(suby>1 || subx>1) {
    dy=vdim;  vdim/=suby;  hdim/=subx;
    vsize/=suby;  hsize/=subx;
    wy/=suby;  wx/=subx;
    if((ptemp1=(double **)arrayalloc(wt*vdim+(wy-1)/2+1,hdim,sizeof(double)))
       == NULL) {
      fprintf(stderr,"can't allocate memory for 'ptemp1'\n");
      exit(-1);
    }
    ptemp2=ptemp1;
    for(k=0;k<wt;++k)
      for(i=k*dy,n=vdim;n-->0;++ptemp2) {
	subsample((char *)*ptemp2,(char *)pin[i],sizeof(double),subx,hdim);
	i+=suby;
      }
    for(i=wt*dy,n=(wy-1)/2+1;n-->0;++ptemp2) {
      subsample((char *)*ptemp2,(char *)pin[i],sizeof(double),subx,hdim);
      i+=suby;
    }
    arrayfree(pin);
    pin=ptemp1;
  }
  wx2=wx/2;  wy2=wy/2;

  if((Vx=(double **)arrayalloc(vsize+by/2+(by-1)/2+1,hsize+bx/2,
			       sizeof(double))) == NULL) {
    fprintf(stderr,"can't allocate memory for 'Vx'\n");
    exit(-1);
  }
  if((Vy=(double **)arrayalloc(vsize+by/2+(by-1)/2+1,hsize+bx/2,
			       sizeof(double))) == NULL) {
    fprintf(stderr,"can't allocate memory for 'Vy'\n");
    exit(-1);
  }

  pVx=Vx[0];  pVy=Vy[0];
  for(vl1=(vsize+by/2+(by-1)/2+1)*(hsize+bx/2);vl1-->0;)*pVx++ = *pVy++ = 0.0;
  
  if (mvfin != NULL)  {

    subvy=2;  subvx=2;
    ptrMvfIn = mvfin;
    if(subvx>1 || subvy>1) {
      n=vsize/subvy;  m=hsize/subvx;
      if((pVx=(double *)malloc(((size_t)m)*sizeof(double))) == NULL) {
	fprintf(stderr,"can't allocate memory for 'pVx'\n");
	exit(-1);
      }
      for(k=0,i=n;i-->0;k+=subvy) {
	for (ii=0; ii<m; ++ii)      pVx[ii] = *ptrMvfIn++;
	upsample((char *)&Vx[by/2+k][bx/2],(char *)pVx,sizeof(double),subvx,m);
	horiz_filter(&Vx[by/2+k][bx/2],hsize,1,uxfilt,uxfdim);
      }
      vert_filter(Vx[0],hsize+bx/2,vsize+by/2,uyfilt,uyfdim);
      for(k=0,i=n;i-->0;k+=subvy) {
	for (ii=0; ii<m; ++ii)      pVx[ii] = *ptrMvfIn++;
	upsample((char *)&Vy[by/2+k][bx/2],(char *)pVx,sizeof(double),subvx,m);
	horiz_filter(&Vy[by/2+k][bx/2],hsize,1,uxfilt,uxfdim);
      }
      vert_filter(Vy[0],hsize+bx/2,vsize+by/2,uyfilt,uyfdim);
      free(pVx);
    }
  
    tmp1 = wt - t2 - 1.0;
    for(n=vsize-1,i=0;i<vsize;++i,--n) {
      pVx = &Vx[by/2+i][bx/2];  pVy = &Vy[by/2+i][bx/2];
      for(m=hsize-1,j=0;j<hsize;++j,--m) {

	tmp2= *pVx*subvx;
	tmp3=((t2*tmp2) >j ? (j/t2) : tmp2);
	tmp3=((tmp1*tmp3) > m ? (m/tmp1) : tmp3);
	tmp3=((-tmp1*tmp3) >j ? (-j/tmp1) : tmp3);
	tmp3=((-t2*tmp3) > m ? (-m/t2) : tmp3);

	tmp4= *pVy*subvy;
	tmp5=((t2*tmp4) >i ? (i/t2) : tmp4);
	tmp5=((tmp1*tmp5) > n ? (n/tmp1) : tmp5);
	tmp5=((-tmp1*tmp5) >i ? (-i/tmp1) : tmp5);
	tmp5=((-t2*tmp5) > n ? (-n/t2) : tmp5);

	if(tmp3!=tmp2 || tmp5!=tmp4)++vclip;
	*pVx++=tmp3; *pVy++=tmp5;
      }
    }
  }  
  
  if((Pmd=(double **)arrayalloc(vsize,hsize,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'Pmd'\n");
    exit(-1);
  }
  p3_ptr=Pmd[0];
  for(vl1=hsize*(long)vsize;vl1-->0;)*p3_ptr++=0.0;


  if((P=(double **)arrayalloc(wt*NB*vsize,hsize,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'P'\n");
    exit(-1);
  }
  
  t1-=t2;  t3-=t2;
  T0=t3-t1;
  tmp1=t1*t1; tmp3=t3*t3;
  tmp1*=t1;   tmp3*=t3;
  
  for(p2_ptr=bill_temp[0],i=NC*NC;i-->0;)*p2_ptr++ = 0.0;
  tmp1=offset_y1;  tmp2=offset_y2-offset_y1;
  for(t= -t2,k=wt;k-->0;t+=1.0,tmp1+=tmp2,tmp2= -tmp2)
    for(y=tmp1-wy2,i=wy;i-->0;y+=1.0)
      for(x= (double) -wx2,j=wx;j-->0;x+=1.0) {

	B[0] = C0;
	B[1] = C1;
	B[2] = C2;
	B[3] = C0 * t;
	B[4] = C4;
	B[5] = C5;
	B[6] = C3;
	B[7] = C1 * t;
	B[8] = C2 * t;

	for(n=NC;n-->0;)
	  for(m=NC;m-->0;)
	    bill_temp[m][n]+=B[m]*B[n];
      }
   
  /* stefan changed this from: mat_invert (bill_temp, 9, A); */
  mat_invert (&bill_temp[0][0], 9, &A[0][0]);
 
  p0=P[0];  p2=P[2*wt*vsize];  p5=P[5*wt*vsize];
  tmp1=offset_y1;  tmp2=offset_y2-offset_y1;
  for(i0=0;i0<wt;++i0,tmp1+=tmp2,tmp2= -tmp2) {
    for(n=0;n<vsize;++n) {
      i1=i0*vdim+n;
      *p0 = *p2 = *p5 = 0.0;
      for(y=tmp1-wy2,i=0;i<wy;++i,y+=1.0)
	for(x=0.0,j=wx2;j<wx;++j,x+=1.0) {
	  tmp0=pin[i1+i][j];
	  *p0 += C0 * tmp0;
	  *p2 += C2 * tmp0;
	  *p5 += C5 * tmp0;
	}
      for(k=m=0;++k<hsize;++m) {
	p0[1] = *p0;  p2[1] = *p2;  p5[1] = *p5;
	++p0; ++p2; ++p5;
	for(y=tmp1-wy2,i=0;i<wy;++i,y+=1.) {
	  tmp0=pin[i1+i][m+wx]-pin[i1+i][m];
	  *p0 += C0 * tmp0;
	  *p2 += C2 * tmp0;
	  *p5 += C5 * tmp0;
	}
      }
      ++p0; ++p2; ++p5;
    }
  }
  
  for(i0=0;i0<wt;++i0) {
    i1=i0*vdim;
    for(m=0;m<hsize;++m) {
      p1= &P[(wt+i0)*vsize][m];  p4= &P[(4*wt+i0)*vsize][m];
      *p1 = *p4 = 0.0;
      for(x= (double) -wx2,j=0;j<wx;++j,x+=1.0)
	for(i=wy2;i<wy;++i) {
	  tmp0=pin[i1+i][m+j];
	  *p1 += C1 * tmp0;
	  *p4 += C4 * tmp0;
	}
      for(k=n=0;++k<vsize;++n) {
	p1[hsize] = *p1;  p4[hsize] = *p4;
	p1+=hsize;  p4+=hsize;
	for(x= (double) -wx2,j=0;j<wx;++j,x+=1.0) {
	  tmp0=pin[i1+n+wy][m+j]-pin[i1+n][m+j];
	  *p1 += C1 * tmp0;
	  *p4 += C4 * tmp0;
	}
      }
    }
  }
  
  p3=P[3*wt*vsize];
  tmp1=offset_y1;  tmp2=offset_y2-offset_y1;
  for(i0=0;i0<wt;++i0,tmp1+=tmp2,tmp2= -tmp2) {
    for(n=0;n<vsize;++n) {
      i1=i0*vdim+n;
      for(m=0;m<hsize;++m,++p3) {
	*p3 = 0.0;
	for(y=tmp1-wy2,i=0;i<wy;++i,y+=1.0)
	  for(x= (double) -wx2,j=0;j<wx;++j,x+=1.0)*p3 += C3 * pin[i1+i][m+j];
      }
    }
  }
  
  if((A11=(double **)arrayalloc(vsize,hsize,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'A11'\n");
    exit(-1);
  }
  if((A22=(double **)arrayalloc(vsize,hsize,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'A22'\n");
    exit(-1);
  }
  if((A12=(double **)arrayalloc(vsize,hsize,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'A12'\n");
    exit(-1);
  }
  if((B1=(double **)arrayalloc(vsize,hsize,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'B1'\n");
    exit(-1);
  }
  if((B2=(double **)arrayalloc(vsize,hsize,sizeof(double)))
     == NULL) {
    fprintf(stderr,"can't allocate memory for 'B2'\n");
    exit(-1);
  }
  a11=A11[0];  a22=A22[0];  a12=A12[0];  b1=B1[0];  b2=B2[0];
  k=wt*vsize;
  for(n=0;n<vsize;++n) {
    p2_ptr=p1_ptr;
    pVx = &Vx[by/2+n][bx/2];  pVy = &Vy[by/2+n][bx/2];
    for(m=0;m<hsize;++m,++pVx,++pVy) {
      tmp5= *pVx;  tmp6= *pVy;
      *pVx=(double)fl_int(tmp5); *pVy=(double)fl_int(tmp6);
      j0 = (int) (((double)(m))+0.5-t2*tmp5); i0 = (int) (((double)(n))+0.5-t2*tmp6);
      for(p3_ptr=B,i=NC;i-->0;)*p3_ptr++ = 0.0;
      j1=j0;  i1=i0;
      for(t= -t2,i=0;i<wt;++i,t+=1.0) {

	B[0] += P[i1][j1];
	B[3] += t*P[i1][j1];
	i1+=k;
	B[1] += P[i1][j1];
	B[7] += t*P[i1][j1];
	i1+=k;
	B[2] += P[i1][j1];
	B[8] += t*P[i1][j1];
	i1+=k;
	B[6] += P[i1][j1];
	i1+=k;
	B[4] += P[i1][j1];
	i1+=k;
	B[5] += P[i1][j1];
	j1+= (int) *pVx;  i1+= vsize - 5 * k + (int) *pVy;
      }

      mvmult(A[4],B,C,5,NC);
      tmp0 = C[2]*C[2]*T0;  tmp1 = C[0]+C[1];
      tmp2 = C[3]*T0;  tmp3 = C[4]*T0;
      *a11++ = 4.0*C[0]*C[0]*T0 + tmp0;
      *a22++ = 4.0*C[1]*C[1]*T0 + tmp0;
      *a12++ = 2.0*C[2]*tmp1*T0;
      *b1++ = 2.0*C[0]*tmp2 + C[2]*tmp3;
      *b2++ = 2.0*C[1]*tmp3 + C[2]*tmp2;
    }
  }
  arrayfree(P);
  
  horiz_filter(A11[0],hsize,vsize,vfilt,vfdim);
  horiz_filter(A22[0],hsize,vsize,vfilt,vfdim);
  horiz_filter(A12[0],hsize,vsize,vfilt,vfdim);
  horiz_filter(B1[0],hsize,vsize,vfilt,vfdim);
  horiz_filter(B2[0],hsize,vsize,vfilt,vfdim);
  vert_filter(A11[0],hsize,vsize,vfilt,vfdim);
  vert_filter(A22[0],hsize,vsize,vfilt,vfdim);
  vert_filter(A12[0],hsize,vsize,vfilt,vfdim);
  vert_filter(B1[0],hsize,vsize,vfilt,vfdim);
  vert_filter(B2[0],hsize,vsize,vfilt,vfdim);

  if (mvfin != NULL) {
    horiz_filter(Vx[0],hsize+bx/2,vsize+by/2,vfilt,vfdim);
    vert_filter(Vx[0],hsize+bx/2,vsize+by/2,vfilt,vfdim);
    horiz_filter(Vy[0],hsize+bx/2,vsize+by/2,vfilt,vfdim);
    vert_filter(Vy[0],hsize+bx/2,vsize+by/2,vfilt,vfdim);
  }
  
  a11=A11[0];  a22=A22[0];  a12=A12[0];  b1=B1[0];  b2=B2[0];
  p3_ptr=Pmd[0];
  for(n=0;n<vsize;++n) {
    pVx = &Vx[by/2+n][bx/2];  pVy = &Vy[by/2+n][bx/2];
    for(m=0;m<hsize;++m,++pVx,++pVy,++a11,++a22,++a12,++b1,++b2) {
      if(!*p3_ptr++) {
	tmp1 = *a11;  tmp2 = *a22;  tmp3 = *a12;
	tmp4 = *b1;   tmp5 = *b2;
	tmp6 = tmp1 - tmp2;
	tmp6 = sqrt(tmp6*tmp6+4.*tmp3*tmp3);
	if((lmax=(tmp1+tmp2+tmp6))==0.0)++unsolved;
	else {
	  if((tmp1+tmp2-tmp6)/lmax < LRATIO) {
	    lmax/=2.;
	    tmp1-=lmax;
	    if(tmp1==0.0 && tmp3==0.0) {
	      tmp6 = -tmp4/lmax;
	      tmp7 = 0.0;
	    }
	    else {
	      tmp0 = (tmp4*tmp3 - tmp5*tmp1)/
		(lmax*(tmp1*tmp1+tmp3*tmp3));
	      tmp6 = -tmp0*tmp3;
	      tmp7 = tmp0*tmp1;
	    }
	    ++decomposed;
	  }
	  else {
	    tmp0 = tmp3*tmp3 - tmp1*tmp2;
	    tmp6 = (tmp2*tmp4 - tmp3*tmp5)/tmp0;
	    tmp7 = (tmp1*tmp5 - tmp3*tmp4)/tmp0;
	  }
	  *pVx += tmp6;
	  *pVy += tmp7;
	}
      }
    }
  }
  
  arrayfree(A11);  arrayfree(A22);  arrayfree(A12);
  arrayfree(B1);  arrayfree(B2);  arrayfree(Pmd);
  
  for(i=vsize-wy;i-->wy;)
    for(j=hsize-wx,pVx= &Vx[i][j],pVy= &Vy[i][j];j-->wx;++SN1) {
      tmp6 = *--pVx;  SX1+=tmp6;  SXX1+=tmp6*tmp6;
      tmp7 = *--pVy;  SY1+=tmp7;  SYY1+=tmp7*tmp7;
    }
  if(SN1>0) {
    SX1/=SN1;
    SXX1=sqrt(SXX1/SN1-SX1*SX1);
    SY1/=SN1;
    SYY1=sqrt(SYY1/SN1-SY1*SY1);
  }
  
  framesize = hsize * vsize;
  ptrVx = mvfout;
  ptrVy = mvfout + framesize;
  for (i=0; i<framesize; ++i) {
    *ptrVx++ = Vx[0][i];
    *ptrVy++ = Vy[0][i]; }

  free ((void *)vfilt);
  free ((void *)uxfilt);
  arrayfree(pin);
  arrayfree(Vx);  arrayfree(Vy);
  
  return;
}

/*  matrix * vector C = AB                     */
/*  A is N*M matrix, B is vector of length M   */
/*  C is vector of length N                    */

static void mvmult(double *A, double *B, double *C, int N, int M)
{
  register double *ptr1,*ptr2,sum;
  register int i,j;
  for(i=0;i<N;++i) {
    ptr1= &A[i*M]; ptr2=B;
    for(sum=0.0,j=M;j-->0;)sum+= *ptr1++ * *ptr2++;
    C[i]=sum;
  }
  return;
}


static double bilin(double **I, double y, double x)
{
  double f;
  int i,j;
  i=(int)y;  j=(int)x;
  f=(i+1-y)*((j+1-x)*I[i][j]+(x-j)*I[i][j+1])+
    (y-i)*((j+1-x)*I[i+1][j]+(x-j)*I[i+1][j+1]);
  return(f);
}


/* This subroutine filters the input sequence horizontally.
 * The operations are performed in double to take advantage of
 * the vector routines.
 * The data is not padded at the edges.
 */

static void horiz_filter(double *buf, int hdim, int vdim, double *hfilt, int hfdim)
{
  register double *buffer,*bufptr;
  register int i,j;

  buffer=(double *) malloc((unsigned)sizeof(double)*(hdim+hfdim-1));
  bufptr=buf;
  i=vdim; 

  while(i--){
    vclr(buffer,hdim+hfdim-1);
    vmove(buffer+(hfdim-1)/2,bufptr,hdim);
    for(j=0;j<hdim;j++){
      *bufptr++ = vconv(hfilt,buffer+(hfdim-1+j),hfdim);
    }
  }


  free(buffer);
  return;
}
    
    
/* This subroutine filters the input sequence vertically.
 * The operations are performed in double to take advantage of
 * the vector routines.
 */

static void vert_filter(double *buf, int hdim, int vdim, double *vfilt, int vfdim)
{
  double **buffer,*tmptr,*zeroptr;
  int i,j;

  buffer=(double **) arrayalloc(vfdim,hdim,sizeof(double));
  zeroptr=buffer[0];
  vrev(vfilt,vfilt,vfdim);

  for(i=0;i<(vfdim-1)/2;i++)
    vclr(buffer[i],hdim);

  for(i=0;i<(vfdim+1)/2;i++)
    vmove(buffer[i+(vfdim-1)/2],buf+i*hdim,hdim);

  for(i=0;i<vdim;i++){
    vscal(buffer[0],vfilt[0],buffer[0],hdim);
    for(j=1;j<vfdim;j++)
      vadd(buffer[0],buffer[0],vfilt[j],buffer[j],hdim);
    vmove(buf+i*hdim,buffer[0],hdim);/* copies from one to the other */
    tmptr=buffer[0];
    for(j=0;j<vfdim-1;j++)
      buffer[j]=buffer[j+1];
    buffer[vfdim-1]=tmptr;
    if(i<vdim-(vfdim+1)/2)
      vmove(buffer[vfdim-1],buf+hdim*(i+(vfdim+1)/2),hdim);
    else{
      vclr(buffer[vfdim-1],hdim);
    }
  }

  vrev(vfilt,vfilt,vfdim);
  buffer[0]=zeroptr;
  arrayfree(buffer);
  return;
}


/*	double *vclr(x,n)	Clears double vector: x[i]=0 for i=0,...,n-1
 *	double *x ;              Returns pointer to x[]
 *	int n ;
*/

static double *vclr(register double *x, register int n)
{
  double *xold = x ;

  if(n>0)
    {
      do {
	*x++ = 0 ;
      } while( --n>0) ;
    }
  return(xold) ;
}


/*	double vconv(x,y,n)	Convolves two double vectors quickly
 *	double *x,*y ;
 *	int n ;
 *
 * For example:
 *	vconv( x, y+k, n)
 * returns:
 *	x[0]*y[k] + x[1]*y[k-1] + ... + x[n-1]*y[k-n+1]
*/

static double vconv(register double *x, register double *y, register int n)
{
  register double sum = 0.0;
  if(n>0)
    {
      do {
	sum += *x++ * *y-- ;
      } while(--n > 0) ;
    }
  return(sum);
}


/*	double *vadd(z,x,a,y,N)	Adds scaled double vector to another vector:
 *	double *z,*x,a,*y ;	  z[] = x[] + a*y[]
 *	int N ;			where x,y,z are double arrays of length N, and
 *				a is a double (double) scalar
 *
 * Handles cases of a=1,-1 specially
 * Returns pointer to z[] array
*/

static double *vadd(register double *z, register double *x, double a, register double *y, register int N)
{
  double *zold = z;

  if(N>0)
    {
      if(a==1.0)
	{
	  do {
	    *z++ = *x++ + *y++ ;
	  } while(--N > 0) ;
	}
      else if(a== -1.0)
	{
	  do {
	    *z++ = *x++ - *y++ ;
	  } while(--N > 0) ;
	}
      else
	{
	  register double aa = a;
	  do {
	    *z++ = *x++ + aa * *y++ ;
	  } while(--N > 0) ;
	}
    }
  return(zold) ;
}


/*      double *vmove(z,x,n)	Moves vector x[] into z[] quickly
 *      double *z,*x ;		  z[i] = x[i] for i=0,...,n-1 if z<=x
 *	int n ;				      for i=n-1,...,0 if z>x
 *				Returns pointer to z[]
 *
 * Works correctly regardless of how x[] and z[] might overlap
*/


static double *vmove(register double *z, register double *x, register int N)
{
  double *zold = z;

  if(N>0)
    {
      if(z<=x)
	{
	  do {
	    *z++ = *x++ ;
	  } while( --N > 0) ;
	}
      else
	{
	  z += N ;
	  x += N ;
	  do {
	    *--z = *--x ;
	  } while( --N > 0) ;
	}
    }
  return(zold) ;
}


/*       double *vrev(z,x,n)	Reverses vector: z[i]=x[n-1-i] for i=0,...,n-1
 *       double *z,*x ;		Returns pointer to z[]
 *       int n ;
 *
 * Works correctly if x[] and z[] vectors do not overlap at all, or if they
 * overlap completely
*/


static double *vrev(register double *z, register double *x, register int N)
{
  double *zold = z;
  register double *ze = z+N ;
  register double *xe = x+N ;
  register double temp1,temp2;

  if(N>0)
    {
      N = (N+1)>>1 ;
      do {
	temp1 = *x++ ;
	temp2 = *--xe ;
	*z++ = temp2 ;
	*--ze = temp1 ;
      } while( --N > 0) ;
    }
  return(zold) ;
}


/*       double *vscal(z,a,x,n)	Scales double vector x[] by a giving z[]
 *	 double *z,a,*x ;	  z[i] = a*x[i] for i=0,...,n-1 if z<=x
 *	 int n ;				for i=n-1,...,0 if z>x
 *
 * Works correctly regardless of how x[] and z[] might overlap
*/


static double *vscal(register double *z, register double a, register double *x, register int N)
{
  double *zold = z;

  if(N>0)
    {
      register double aa = a;

      if(z<=x)
	{
	  do {
	    *z++ = aa * *x++ ;
	  } while( --N > 0) ;
	}
      else
	{
	  z += N ;
	  x += N ;
	  do {
	    *--z = aa * *--x ;
	  } while( --N > 0) ;
	}
    }
  return(zold) ;
}



/*
 * Arrayalloc () -- allocate an imax by jmax vectored array of "size" byte
 * elements.  If memory can't be allocated, either for the main array or for
 * the row address vector, we return NULL.  See accompanying documentation
 * for more details.
 */
static char **arrayalloc (unsigned int imax, unsigned int jmax, unsigned int size)
{
  register char **vector, *array;
  register unsigned int k, stride;

  /*
   * Get memory for main array.
   */
  if ((array = (char *) malloc (imax * jmax * size)) == NULL)
    return (NULL);

# ifdef DEBUG
  printf ("array = %x\n", array);
# endif

  /*
   * Get memory for intermediate row address vector.
   */
  if ((vector = (char **) malloc (imax * sizeof (char *))) == NULL)
    return (NULL);

  /*
   * Initialize the address vector so each element points to the
   * first element in the corresponding row in the main array.
   */
  for (k = 0; k < imax; k++)
    {
      stride = jmax * size;
      vector [k] = &array [k*stride];
# ifdef DEBUG
      printf ("vector [%d] = %x\n", k, vector[k]);
# endif
    }

  return (vector);
}

/*
 * Arrayfree () -- free the memory acquired from arrayalloc ().  No checks
 * are made to make sure things are as they should be, so it is the user's
 * responsibility to make sure that you don't arrayfree() anything that you
 * didn't arrayalloc() in the first place.  Eventually, checks will be added
 * to make sure the user hasn't screwed things up.  We have to first free the
 * real array memory, and then free the intermediate vector.  This sounds
 * more complicated than it really is.
 */
static void arrayfree (double **v)
{
  free (v[0]);
  free (v);
}





/*
 *        m a t _ i n v e r t         Invert a matirx using the LU decomposition
 *                                    described in the text: 'Numerical Recipes in C'
 *                                    For this function, the matrix must be square.
 */

static void mat_invert (double *a, int n, double *y)
{
  int                i, j, *indx;
  double             d, *col;

  if ((col = (double *) malloc ((unsigned)(sizeof(double) * n))) == NULL)
    nrerror ("Unable to malloc space for column buffer");
  if ((indx = (int *) malloc ((unsigned)(sizeof(int) * n))) == NULL)
    nrerror ("Unable to malloc space for index buffer");
  ludcmp (a, n, indx, &d);
  for (j=0; j<n; ++j) {
    for (i=0; i<n; ++i)     col[i] = 0.0;
    col[j] = 1.0;
    lubksb (a, n, indx, col);
    for (i=0; i<n; ++i)        y[i*n+j] = col[i];
  }
  return;
}




/*
 *        l u d c m p                 Perform a single LU decomposisition as described in 
 *                                    the text: 'Numerical Recipes in C' (page 43)
 *
 *       Parameters          n ->     dimensions of a square matrix
 *                           a ->     square matrix (overwritten on output)
 *                           indx->   output vector
 *                           d->      even/odd indicator for row interchanges
 */

static int ludcmp (double *a, int n, int *indx, double *d)
{

  int                      i, imax, j, k;
  double                   big, dum, sum, temp;
  double                  *vv; 

  vv = new_vector(1, n);
  *d = 1.0;
  for (i=0;i<n; i++) {
    big = 0.0;
    for (j=0; j<n; ++j)
      if ((temp = fabs(a[i*n+j])) > big)       big = temp;
    if (fabs(big) < SMALL)                     return (SINGULAR);
    vv[i] = 1.0/big;
  }

  for (j=0; j<n; j++) {
    for (i=0; i<j; i++) {
      sum = a [i*n+j];
      for (k=0; k<i; k++)            sum -= a[i*n+k] * a[k*n+j];
      a [i*n+j] = sum;
    }
    big = 0.0;
    for (i=j; i<n; i++) {
      sum = a [i*n+j];
      for (k=0; k<j; k++)            sum -= a[i*n+k] * a[k*n+j];
      a [i*n+j] = sum;
      if ((dum = vv[i] * fabs(sum)) >= big) {
	big = dum;
	imax = i;
      }
    }
    if (j != imax) {
      for (k=0; k<n; ++k) {
	dum = a[imax*n+k];
	a[imax*n+k] = a[j*n+k];
	a[j*n+k] = dum;
      }
      *d = -(*d);
      vv[imax] = vv[j];
    }
    indx[j] = imax;
    if (a[j*n+j] == 0.0)                a[j*n+j] = TINY;
    if (j != (n-1)) {
      dum = 1.0 / a[j*n+j];
      for (i=j+1; i<n; ++i)            a[i*n+j] *= dum;
    }
  }
  free_vector(vv, 1, n);
  return (SUCCESS);
}



/*
 *        l u b k s b                perform forward and backward substitution on the LU decomposition of an array
 */

static void lubksb (double *a, int n, int *indx, double *b)
{
  int                       i, ii=-1, ip, j;
  double                    sum;

  for (i=0; i<n; ++i) {
    ip=indx[i];
    sum = b[ip];
    b[ip] = b[i];
    if (ii != -1)
      for (j=ii; j<=i-1; ++j)          sum -= a[i*n+j] * b[j];
    else if (sum) 
      ii = i;
    b[i] = sum;
  }
  for (i=n-1; i>=0; --i) {
    sum = b[i];
    for (j=i+1; j<n; ++j)              sum -= a[i*n+j] * b[j];
    b[i] = sum / a[i*n+i];
  }
  return;
}


/*
 *         e i g e n v a l u e        Calulate the eigenvalues for a 2 by 2 matrix.
 *                                    This amounts to nothing more than solving the quadratic formula.
 */

static void eigenvalue (double *mat, double *eigenmax, double *eigenmin)
{
  double                 b, c, root;

  b = -1.0 * (mat[0] + mat[3]);
  c = (mat[0] * mat[3]) - (mat[1] * mat[2]);
  root = (b * b) - (4.0 * c);
  if (root < 0.0) {
    fprintf (stderr, "Warning: eigenvalues of gradient matrix are complex. Returning real part\n");
    *eigenmax = (b * b) / 2.0;
    *eigenmin = *eigenmax;
    return; }
  else {
    *eigenmax = ((-1.0 * b) + sqrt(root)) / 2.0;
    *eigenmin = ((-1.0 * b) - sqrt(root)) / 2.0;
    return; }
}



/*
 *         e i g e n v e c t o r      Calculate the eigenvector of a 2 x 2 matrix
 */

static void eigenvector (double *evect, double *mat, double eigenvalue)
{
  double                 y, magnitude;

  if (mat[1] == 0.0) {
    if (fabs(mat[0]) > fabs(mat[3])) {
      evect[0] = 1.0;
      evect[1] = 0.0;  }
    else {
      evect[0] = 0.0;
      evect[1] = 1.0;  } }
  else {
    y = (eigenvalue - mat[0]) / mat[1];
    magnitude = 1.0 + (y * y);
    magnitude = sqrt(magnitude);
    evect[0] = 1.0 / magnitude;
    evect[1] = y / magnitude; }

  return;
}



/*
 *         m a t _ a d d 
 */

static void mat_add (double *m1, double *m2, int ylen, int xlen)
{
  int       y, x, offset;

  for (y=0; y<ylen; ++y)
    for (x=0; x<xlen; ++x) {
      offset = y * xlen + x;
      m1[offset] =  m1[offset] + m2[offset];
    }
  return;
}



/*
 *         m a t _ c r o s s 
 */

static void mat_cross (double *vec1, double *vec2, int len, double *mat)
{
  int       y, x;

  for (y=0; y<len; ++y)
    for (x=0; x<len; ++x)
      mat[y * len + x] = vec1[y] * vec2[x];

  return;
}




/*
 *           n r e r r o r                 Standard error handler for Numerical Recipes package
 */

static void nrerror (char *error_text)
{
  fprintf (stderr, "Numerical Recipes run time error ...\n");
  fprintf (stderr, "%s \n", error_text);
  fprintf (stderr, ".... now exiting to system ...\n");
  exit (1);
}


/*
 *           v e c t o r                 Allocates a double vector with range (nl .... nh)
 */

static double *new_vector (int nl, int nh)
{
  double                 *v;

  v = (double *) malloc ((unsigned) (nh - nl + 1) * sizeof(double));
  if (!v)    nrerror ("allocation failure in vector()");
  return (v);
}

/*
 *           f r e e _ v e c t o r       Frees a double vector allocated by VECTOR();
 */

static void free_vector(double *v, int nl, int nh)
{
  free (v);
  return ;
}


/* horizontal subsampler; stolen from /unips/src/pixutil/subsample.c */

static void subsample(register char *outptr, register char *inptr, int nbps, int h_factor, int h_samples)
{
  register int i, pel, incr;

  incr = (h_factor-1) * nbps;
  for (pel = 0; pel < h_samples; pel++){
    for(i = 0; i < nbps; i++)
      *outptr++ = *inptr++;
    inptr += incr;
  }
}




/* horizontal upsampler; stolen from /unips/src/pixutil/upsample.c */

static void upsample(register char *outptr, register char *inptr, int nbps, int h_factor, int h_size)
{
  register int index, pel, incr;

  incr = (h_factor-1) * nbps;
  for (pel = 0; pel < h_size; pel++){
    for(index = 0; index < nbps; index++)
      *outptr++ = *inptr++;
    outptr += incr;
  }
}


