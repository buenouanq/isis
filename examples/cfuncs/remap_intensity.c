
/*----------------------------------------------------------------------*/
void remap_intensity(int xsize,
		     int ysize,
		     unsigned char *iptr,	     
		     int inxstep,
		     int inystep,
		     unsigned char *optr,
		     int outxstep,
		     int outystep,
		     unsigned char *remap)
{
  int x, y, indiff, outdiff;
  
  indiff = inystep - (xsize * inxstep);
  outdiff = outystep - (xsize * outxstep);
  
  for(y = 0; y < ysize; y++) {
    for(x = 0; x < xsize; x++) {
      *optr = remap[*iptr];
      iptr += inxstep;
      optr += outxstep;
    }
    iptr += indiff;
    optr += outdiff;
  }
}

/* ------------------------------------------------------------ */
Value *isis_remap_intensity(Script *script, 
			    char *proc_name, int call_data, 
			    int argc, Value **argv)
{
  int i, chans;
  
  checkCount(6, NULL); 
  checkIntList(0, "size");
  checkAddrList(1, "inbufs");
  checkIntList(2, "instep");
  checkAddrList(3, "outbufs");
  checkIntList(4, "outstep");
  checkAddr(5, "remap");

  chans = Listsize(argv[1]);

  for(i = 0; i < chans; i++)
    remap_intensity(
		    getInt(Listitem(argv[0], 0)),
		    getInt(Listitem(argv[0], 1)),
		    
		    getAddr(Listitem(argv[1], i)),
		    getInt(Listitem(argv[2], 0)),
		    getInt(Listitem(argv[2], 1)),
		    
		    getAddr(Listitem(argv[3], i)),
		    getInt(Listitem(argv[4], 0)),
		    getInt(Listitem(argv[4], 1)),
		    
		    getAddr(argv[5])
		    );

  return NullValue;
}


/* An Isis image protocool wrapper for remap-intensity: */

(set image-remap-intensity
  (proc (remap buf1 buf2)
    (if (or (!= (buf1 buf-dims) (buf2 buf-dims))
	    (!= (buf1 buf-chans) (buf2 buf-chans)))
	(image-buf-error "image-remap-intensity" buf1 buf2)
	(begin 
	  (improc-remap-intensity (buf1 buf-dims)
				  (buf1 buf-addrs) 
				  (buf1 buf-steps) 
				  (buf2 buf-addrs) 
				  (buf2 buf-steps)
				  remap)
	  buf2))))

