/* Simple background difference algorithms */
/* by stefan agamanolis */

#include <stdio.h>
#include <stdlib.h>
#include <isis.h>
#include "isis_seg_private.h"

/* ------------------------------------------------------------ */
Value *isis_simple_bkseg(Script *script, 
			 char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  checkCount(9, NULL); 
  checkIntList(0, "image size");
  checkAddrList(1, "fg bufs");
  checkIntList(2, "fg steps");
  checkAddrList(3, "bg bufs");
  checkIntList(4, "bg steps");
  checkIntList(5, "alpha size");
  checkAddrList(6, "alpha buf");
  checkIntList(7, "alpha steps");
  checkAddr(8, "diff to alpha map");
  
  switch(Listsize(argv[3])) {

  case 3:
    simple_bkseg_3ch(
		     getInt(Listitem(argv[0], 0)),
		     getInt(Listitem(argv[0], 1)),

		     getAddr(Listitem(argv[1], 0)),
		     getAddr(Listitem(argv[1], 1)),
		     getAddr(Listitem(argv[1], 2)),
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
		     getInt(Listitem(argv[7], 0)),
		     getInt(Listitem(argv[7], 1)),

		     getAddr(argv[8])
		     );
    break;

  case 1:
    simple_bkseg_1ch(
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

		     getAddr(argv[8])
		     );
    break;
  }

  return NullValue;
}

/* ------------------------------------------------------------ */
Value *isis_simple_bkseg_yuv(Script *script, 
			     char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  checkCount(13, NULL);
  checkIntList(0, "fbufsize");
  checkAddrList(1, "fyaddrs");
  checkIntList(2, "fysteps");
  checkAddrList(3, "fuvaddrs");
  checkIntList(4, "fuvsteps");
  checkAddrList(5, "byaddrs");
  checkIntList(6, "bysteps");
  checkAddrList(7, "buvaddrs");
  checkIntList(8, "buvsteps");
  checkIntList(9, "abufsize");
  checkAddrList(10, "abufaddrs");
  checkIntList(11, "asteps");
  checkAddrList(12, "gbmaps");

  simple_bkseg_yuv(
		   getInt(Listitem(argv[0],0)),
		   getInt(Listitem(argv[0],1)),
		   getAddr(Listitem(argv[1],0)),
		   getInt(Listitem(argv[2],0)),
		   getInt(Listitem(argv[2],1)),
		   getAddr(Listitem(argv[3],0)),
		   getAddr(Listitem(argv[3],1)),
		   getInt(Listitem(argv[4],0)),
		   getInt(Listitem(argv[4],1)),
		   getAddr(Listitem(argv[5],0)),
		   getInt(Listitem(argv[6],0)),
		   getInt(Listitem(argv[6],1)),
		   getAddr(Listitem(argv[7],0)),
		   getAddr(Listitem(argv[7],1)),
		   getInt(Listitem(argv[8],0)),
		   getInt(Listitem(argv[8],1)),
		   getInt(Listitem(argv[9],0)),
		   getInt(Listitem(argv[9],1)),	      
		   getAddr(Listitem(argv[10],0)),
		   getInt(Listitem(argv[11],0)),
		   getInt(Listitem(argv[11],1)),	      
		   getAddr(Listitem(argv[12],0)),
		   getAddr(Listitem(argv[12],1)),
		   getAddr(Listitem(argv[12],2))
		   );

  return NullValue;
}

/* ------------------------------------------------------------ */
void simple_bkseg_3ch(int xdim,
		      int ydim,
		      unsigned char *fbuf1, 
		      unsigned char *fbuf2, 
		      unsigned char *fbuf3, 
		      int fskip,
		      int fstride,
		      unsigned char *bbuf1, 
		      unsigned char *bbuf2, 
		      unsigned char *bbuf3,
		      int bskip,
		      int bstride,
		      int axdim,
		      int aydim,
		      unsigned char *abuf, 
		      int askip,
		      int astride, 
		      unsigned char *gbmap)
{
  int i, j;
  int fdiff, bdiff, adiff;
  register unsigned char a1, a2, a3;
  
  /*takes the difference between the fbuf and bbuf*/

  fdiff = fstride - (xdim*fskip);
  bdiff = bstride - (xdim*bskip);
  adiff = astride - (axdim*askip);

  for(j=0;j<ydim;j++){
    for(i=0;i<xdim;i++){
      a1 = abs(((int)*fbuf1) - ((int)*bbuf1));
      a2 = abs(((int)*fbuf2) - ((int)*bbuf2));
      a3 = abs(((int)*fbuf3) - ((int)*bbuf3));
      a1 = (a1 > a2) ? a1 : a2;
      *abuf = gbmap[(a1 > a3) ? a1 : a3];
      fbuf1+=fskip; fbuf2+=fskip; fbuf3+=fskip;
      bbuf1+=bskip; bbuf2+=bskip; bbuf3+=bskip;
      abuf+=askip;
    }
    fbuf1+=fdiff; fbuf2+=fdiff; fbuf3+=fdiff;
    bbuf1+=bdiff; bbuf2+=bdiff; bbuf3+=bdiff;
    abuf+=adiff;
  }
}


/* ------------------------------------------------------------ */
void simple_bkseg_1ch(int xdim,
		      int ydim,
		      unsigned char *fbuf1, 
		      int fskip,
		      int fstride,
		      unsigned char *bbuf1, 
		      int bskip,
		      int bstride,
		      int axdim,
		      int aydim,
		      unsigned char *abuf, 
		      int askip,
		      int astride, 
		      unsigned char *gbmap)
{
  int i, j;
  int fdiff, bdiff, adiff;
  
  /*takes the difference between the fbuf and bbuf*/

  fdiff = fstride - (xdim*fskip);
  bdiff = bstride - (xdim*bskip);
  adiff = astride - (axdim*askip);

  for(j=0;j<ydim;j++){
    for(i=0;i<xdim;i++){
      *abuf = gbmap[abs(((int)*fbuf1) - ((int)*bbuf1))];
      fbuf1+=fskip;
      bbuf1+=bskip;
      abuf+=askip;
    }
    fbuf1+=fdiff;
    bbuf1+=bdiff;
    abuf+=adiff;
  }
}

/* ------------------------------------------------------------ */
void simple_bkseg_yuv(int xdim,
		      int ydim,
		      unsigned char *fybuf, 
		      int fskip,
		      int fstride,
		      unsigned char *fubuf, 
		      unsigned char *fvbuf, 
		      int fuvskip,
		      int fuvstride,
		      unsigned char *bybuf, 
		      int bskip,
		      int bstride,
		      unsigned char *bubuf, 
		      unsigned char *bvbuf,
		      int buvskip,
		      int buvstride,
		      int axdim,
		      int aydim,
		      unsigned char *abuf, 
		      int askip,
		      int astride, 
		      unsigned char *ygbmap,
		      unsigned char *ugbmap,
		      unsigned char *vgbmap)
{
  int i, j;
  int fdiff, fuvdiff, bdiff, buvdiff, adiff;
  register unsigned char a1, a2, a3;
  
  /*takes the difference between the fbuf and bbuf*/
  /*different skip and stride for uv*/
  /*buffer is half as large for uv*/
  fdiff = fstride - (xdim*fskip);
  fuvdiff = fuvstride - (xdim*fuvskip)/2;
  bdiff = bstride - (xdim*bskip);
  buvdiff = buvstride - (xdim*buvskip)/2;
  adiff = astride - (axdim*askip);

  for(j=0;j<ydim;j++){
    for(i=0;i<xdim;i+=2){
      a1 = ygbmap[abs(((int)*fybuf) - ((int)*bybuf))];
      a2 = ugbmap[abs(((int)*fubuf) - ((int)*bubuf))];
      a3 = vgbmap[abs(((int)*fvbuf) - ((int)*bvbuf))];
      a1 = (a1 > a2) ? a1 : a2;
      *abuf = (a1 > a3) ? a1 : a3;
      fybuf+=fskip; bybuf+=bskip; abuf+=askip;

      a1 = ygbmap[abs(((int)*fybuf) - ((int)*bybuf))];
      a1 = (a1 > a2) ? a1 : a2;
      *abuf = (a1 > a3) ? a1 : a3;
      fybuf+=fskip; bybuf+=bskip; abuf+=askip;

      fubuf+=fuvskip; fvbuf+=fuvskip;
      bubuf+=buvskip; bvbuf+=buvskip;
    }
    fybuf+=fdiff; fubuf+=fuvdiff; fvbuf+=fuvdiff;
    bybuf+=bdiff; bubuf+=buvdiff; bvbuf+=buvdiff;
    abuf+=adiff;
  }
}


