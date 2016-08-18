

/* ---------------------------------------------------------------------- */
Value *isis_sleep(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  struct timeval tv;
  real rsec;
  
  if(call_data == 0) {    /* regular sleep */
    
    checkCount(1, "number of seconds");
    checkNumber(0, "number of seconds");
    
    rsec = getRealfromNumber(argv[0]);
    
    tv.tv_sec = (int) rsec;
    tv.tv_usec = (int) (1000000.0 * (rsec - ((real) tv.tv_sec)));
  }
  else if(call_data == 1) {   /* microsleep */
    
    checkCount(1, "number of microseconds");
    checkNumber(0, "number of microseconds");

    tv.tv_usec = (int) getRealfromNumber(argv[0]);
    tv.tv_sec = tv.tv_usec / 1000000;
    tv.tv_usec = tv.tv_usec % 1000000;
  }    

  return newBool( select(0, NULL, NULL, NULL, &tv) == 0);
}


