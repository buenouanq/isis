/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "isis.h"
#include "isis_core_prim.h"

/* local functions */

static void linear_interpolate(Script *script, 
			       Value *val1, Value *val2, 
			       real frac, Value **newval);
static void cubic_interpolate(Script *script, 
			      Value *val1, Value *val2, 
			      Value *val3, Value *val4,
			      real frac, Value **newval);

/*-------------------------------------------------------------*/
Value *prim_add(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  /* Primitive math operators.  Allows multiple arguments of mixed
     types.  The result is the same type as the first argument. */

  int i;
  
  /* Answer is Null if there are no arguments*/
  if(argc == 0) return NullValue;
  
  switch(typeof(argv[0])) {

  case IsisList:     /* result will be list */
    {
      int k, len = Listsize(argv[0]);
      Value **newargv = (Value **) newPtrArray(argc);
      Value *result = newList(len, NULL);;
      
      for(k = 0; k < len; k++) {
	for(i = 0; i < argc; i++) {
	  if(isList(argv[i]) && k < Listsize(argv[i])) 
	    newargv[i] = Listitem(argv[i], k);
	  else newargv[i] = NullValue;
	}
	Listitem(result, k) = 
	  prim_add(script, proc_name, call_data, argc, newargv);
      }
      
      freePtrArray((void **) newargv);
      return result;
    }

  case IsisInt:    /* result will be int */
    { 
      int intres = getInt(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: intres += getInt(argv[i]); break;
	case IsisReal: intres += (int) getReal(argv[i]); break;
	case IsisChar: intres += (int) getChar(argv[i]); break;
	default: 
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newInt(intres);
    }

  case IsisReal:    /* result will be real */
    {
      real realres = getReal(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisReal: realres += getReal(argv[i]); break;
	case IsisInt: realres += (real) getInt(argv[i]); break;
	case IsisChar: realres += (real) getChar(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newReal(realres);
    }
    
  case IsisChar:    /* result will be char */
    { 
      char charres = getChar(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: charres += (char) getInt(argv[i]); break;
	case IsisReal: charres += (char) getReal(argv[i]); break;
	case IsisChar: charres += getChar(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newChar(charres);
    }

  case IsisAddr:    /* result will be address */
    {
      unsigned long addrres = (unsigned long) getAddr(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: addrres += getInt(argv[i]); break;
	case IsisReal: addrres += (int) getReal(argv[i]); break;
	case IsisChar: addrres += (int) getChar(argv[i]); break;
	case IsisAddr: addrres += (unsigned long) getAddr(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newAddr((void *)addrres);
    }

  default: 
    fprintf(stderr, "** %s: Type of first argument was invalid.\n", proc_name);
    return NullValue;
  }
}

/*-------------------------------------------------------------*/
Value *prim_sub(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  /* Primitive math operators.  Allows multiple arguments of mixed
     types.  The result is the same type as the first argument. */

  int i;
  
  /* Answer is Null if there are no arguments*/
  if(argc == 0) return NullValue;
  
  switch(typeof(argv[0])) {

  case IsisList:     /* result will be list */
    {
      int k, len = Listsize(argv[0]);
      Value **newargv = (Value **) newPtrArray(argc);
      Value *result = newList(len, NULL);;
      
      for(k = 0; k < len; k++) {
	for(i = 0; i < argc; i++) {
	  if(isList(argv[i]) && k < Listsize(argv[i])) 
	    newargv[i] = Listitem(argv[i], k);
	  else newargv[i] = NullValue;
	}
	Listitem(result, k) = 
	  prim_sub(script, proc_name, call_data, argc, newargv);
      }
      
      freePtrArray((void **) newargv);
      return result;
    }

  case IsisInt:    /* result will be int */
    { 
      int intres = getInt(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: intres -= getInt(argv[i]); break;
	case IsisReal: intres -= (int) getReal(argv[i]); break;
	case IsisChar: intres -= (int) getChar(argv[i]); break;
	default: 
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newInt(intres);
    }

  case IsisReal:    /* result will be real */
    {
      real realres = getReal(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisReal: realres -= getReal(argv[i]); break;
	case IsisInt: realres -= (real) getInt(argv[i]); break;
	case IsisChar: realres -= (real) getChar(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newReal(realres);
    }
    
  case IsisChar:    /* result will be char */
    { 
      char charres = getChar(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: charres -= (char) getInt(argv[i]); break;
	case IsisReal: charres -= (char) getReal(argv[i]); break;
	case IsisChar: charres -= getChar(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newChar(charres);
    }

  case IsisAddr:    /* result will be address */
    {
      unsigned long addrres = (unsigned long) getAddr(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: addrres -= getInt(argv[i]); break;
	case IsisReal: addrres -= (int) getReal(argv[i]); break;
	case IsisChar: addrres -= (int) getChar(argv[i]); break;
	case IsisAddr: addrres -= (unsigned long) getAddr(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newAddr((void *)addrres);
    }

  default: 
    fprintf(stderr, "** %s: Type of first argument was invalid.\n", proc_name);
    return NullValue;
  }
}

/*-------------------------------------------------------------*/
Value *prim_mul(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  /* Primitive math operators.  Allows multiple arguments of mixed
     types.  The result is the same type as the first argument. */

  int i;
  
  /* Answer is Null if there are no arguments*/
  if(argc == 0) return NullValue;
  
  switch(typeof(argv[0])) {

  case IsisList:     /* result will be list */
    {
      int k, len = Listsize(argv[0]);
      Value **newargv = (Value **) newPtrArray(argc);
      Value *result = newList(len, NULL);;
      
      for(k = 0; k < len; k++) {
	for(i = 0; i < argc; i++) {
	  if(isList(argv[i]) && k < Listsize(argv[i])) 
	    newargv[i] = Listitem(argv[i], k);
	  else newargv[i] = NullValue;
	}
	Listitem(result, k) = 
	  prim_mul(script, proc_name, call_data, argc, newargv);
      }
      
      freePtrArray((void **) newargv);
      return result;
    }

  case IsisInt:    /* result will be int */
    { 
      int intres = getInt(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: intres *= getInt(argv[i]); break;
	case IsisReal: intres *= (int) getReal(argv[i]); break;
	default: 
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newInt(intres);
    }

  case IsisReal:    /* result will be real */
    {
      real realres = getReal(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisReal: realres *= getReal(argv[i]); break;
	case IsisInt: realres *= (real) getInt(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newReal(realres);
    }
    
  default: 
    fprintf(stderr, "** %s: Type of first argument was invalid.\n", proc_name);
    return NullValue;
  }
}

/*-------------------------------------------------------------*/
Value *prim_div(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  /* Primitive math operators.  Allows multiple arguments of mixed
     types.  The result is the same type as the first argument. */

  int i;
  
  /* Answer is Null if there are no arguments*/
  if(argc == 0) return NullValue;
  
  switch(typeof(argv[0])) {

  case IsisList:     /* result will be list */
    {
      int k, len = Listsize(argv[0]);
      Value **newargv = (Value **) newPtrArray(argc);
      Value *result = newList(len, NULL);;
      
      for(k = 0; k < len; k++) {
	for(i = 0; i < argc; i++) {
	  if(isList(argv[i]) && k < Listsize(argv[i])) 
	    newargv[i] = Listitem(argv[i], k);
	  else newargv[i] = NullValue;
	}
	Listitem(result, k) = 
	  prim_div(script, proc_name, call_data, argc, newargv);
      }
      
      freePtrArray((void **) newargv);
      return result;
    }

  case IsisInt:    /* result will be int */
    { 
      int intres = getInt(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: intres /= getInt(argv[i]); break;
	case IsisReal: intres /= (int) getReal(argv[i]); break;
	default: 
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newInt(intres);
    }

  case IsisReal:    /* result will be real */
    {
      real realres = getReal(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisReal: realres /= getReal(argv[i]); break;
	case IsisInt: realres /= (real) getInt(argv[i]); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newReal(realres);
    }
    
  default: 
    fprintf(stderr, "** %s: Type of first argument was invalid.\n", proc_name);
    return NullValue;
  }
}

/*-------------------------------------------------------------*/
Value *prim_mod(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  /* Primitive math operators.  Allows multiple arguments of mixed
     types.  The result is the same type as the first argument. */

  int i;
  
  /* Answer is Null if there are no arguments*/
  if(argc == 0) return NullValue;
  
  switch(typeof(argv[0])) {

  case IsisList:     /* result will be list */
    {
      int k, len = Listsize(argv[0]);
      Value **newargv = (Value **) newPtrArray(argc);
      Value *result = newList(len, NULL);;
      
      for(k = 0; k < len; k++) {
	for(i = 0; i < argc; i++) {
	  if(isList(argv[i]) && k < Listsize(argv[i])) 
	    newargv[i] = Listitem(argv[i], k);
	  else newargv[i] = NullValue;
	}
	Listitem(result, k) = 
	  prim_mod(script, proc_name, call_data, argc, newargv);
      }
      
      freePtrArray((void **) newargv);
      return result;
    }

  case IsisInt:    /* result will be int */
    { 
      int intres = getInt(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisInt: intres %= getInt(argv[i]); break;
	case IsisReal: intres %= (int) getReal(argv[i]); break;
	default: 
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newInt(intres);
    }

  case IsisReal:    /* result will be real */
    {
      real realres = getReal(argv[0]);
      for(i = 1; i < argc; i++) {
	switch(typeof(argv[i])) {
	case IsisReal: realres = fmod(realres, getReal(argv[i])); break;
	case IsisInt: realres = fmod(realres, (real) getInt(argv[i])); break;
	default:
	  fprintf(stderr, "** %s: Type mismatch.\n", proc_name);
	  break;
	}
      }
      return newReal(realres);
    }
    
  default: 
    fprintf(stderr, "** %s: Type of first argument was invalid.\n", proc_name);
    return NullValue;
  }
}

/*-------------------------------------------------------------*/
Value *prim_abs(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  /* Primitive math operators.  Allows multiple arguments of mixed
     types.  The result is the same type as the first argument. */

  checkCount(1, NULL);
  
  switch(typeof(argv[0])) {

  case IsisList:     /* result will be list */
    {
      int k, len = Listsize(argv[0]);
      Value **newargv = (Value **) newPtrArray(1);
      Value *result = newList(len, NULL);;
      
      for(k = 0; k < len; k++) {
	newargv[0] = Listitem(argv[0], k);
	Listitem(result, k) = 
	  prim_abs(script, proc_name, call_data, argc, newargv);
      }
      
      freePtrArray((void **) newargv);
      return result;
    }

  case IsisInt:  return newInt(abs(getInt(argv[0])));
  case IsisReal:  return newReal(fabs(getReal(argv[0])));
  case IsisChar:  return newChar((char)abs((int)getChar(argv[0])));
    
  default: 
    fprintf(stderr, "** %s: Type of first argument was invalid.\n", proc_name);
    return NullValue;
  }
}

/*-------------------------------------------------------------*/
Value *prim_clamp(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  int imin, imax, ival;
  real rmin, rmax, rval;

  checkCount(3, "minimum, maximum, and value");
  checkNumber(0, "minimum");
  checkNumber(1, "maximum");
  checkNumber(2, "value");
  
  if(isReal(argv[2])) {
    rmin = getRealfromNumber(argv[0]);
    rmax = getRealfromNumber(argv[1]);
    rval = getReal(argv[2]);
    
    if(rval < rmin) return newReal(rmin); 
    if(rval > rmax) return newReal(rmax);
    return referValue(argv[2]);
  }
  else {
    imin = getIntfromNumber(argv[0]);
    imax = getIntfromNumber(argv[1]);
    ival = getInt(argv[2]);
    
    if(ival < imin) return newInt(imin); 
    if(ival > imax) return newInt(imax);
    return referValue(argv[2]);
  }
}

/*-------------------------------------------------------------*/
Value *prim_sin(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(sin(getRealfromNumber(argv[0])));
}

/*-------------------------------------------------------------*/
Value *prim_cos(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(cos(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_tan(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(tan(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_asin(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(asin(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_acos(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(acos(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_atan(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(atan(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_atan2(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  checkCount(2, "y followed by x");
  checkNumber(0, "y");
  checkNumber(1, "x");
  
  return newReal(atan2(getRealfromNumber(argv[0]), 
		       getRealfromNumber(argv[1])));
}


/*-------------------------------------------------------------*/
Value *prim_sinh(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(sinh(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_cosh(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(cosh(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_tanh(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(tanh(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_exp(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(exp(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_log(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(log(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_log10(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(log10(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_pow(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  checkCount(2, "base and exponent");
  checkNumber(0, "base");
  checkNumber(1, "exponent");

  return newReal(pow(getRealfromNumber(argv[0]), 
		     getRealfromNumber(argv[1])));
}


/*-------------------------------------------------------------*/
Value *prim_sqrt(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(sqrt(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_ceil(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(ceil(getRealfromNumber(argv[0])));
}


/*-------------------------------------------------------------*/
Value *prim_floor(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(floor(getRealfromNumber(argv[0])));
}

/*-------------------------------------------------------------*/
Value *prim_degrad(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(getRealfromNumber(argv[0]) * ISIS_PI / 180.0);
}

/*-------------------------------------------------------------*/
Value *prim_raddeg(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  checkCount(1, NULL);
  checkNumber(0, NULL);

  return newReal(getRealfromNumber(argv[0]) * 180.0 / ISIS_PI);
}

/*-------------------------------------------------------------*/
Value *prim_bit_and(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  int result, i;
  
  checkCountLeast(1, NULL);
  for(i = 0; i < argc; i++) checkInt(i, NULL);
  
  result = getInt(argv[0]);
  
  for(i = 1; i < argc; i++) result = result & getInt(argv[i]);
  
  return newInt(result);
}

/*-------------------------------------------------------------*/
Value *prim_bit_or(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  int result, i;
  
  checkCountLeast(1, NULL);
  for(i = 0; i < argc; i++) checkInt(i, NULL);
  
  result = getInt(argv[0]);
  
  for(i = 1; i < argc; i++) result = result | getInt(argv[i]);
  
  return newInt(result);
}

/*-------------------------------------------------------------*/
Value *prim_bit_xor(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  int result, i;
  
  checkCountLeast(1, NULL);
  for(i = 0; i < argc; i++) checkInt(i, NULL);
  
  result = getInt(argv[0]);
  
  for(i = 1; i < argc; i++) result = result ^ getInt(argv[i]);
  
  return newInt(result);
}

/*-------------------------------------------------------------*/
Value *prim_bit_left(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  checkCount(2, "value and shift amount");
  checkInt(0, NULL);
  checkInt(1, "shift amount");
  
  return newInt( getInt(argv[0]) << getInt(argv[1]) );
}


/*-------------------------------------------------------------*/
Value *prim_bit_right(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  checkCount(2, "value and shift amount");
  checkInt(0, NULL);
  checkInt(1, "shift amount");
  
  return newInt( getInt(argv[0]) >> getInt(argv[1]) );
}


/*-------------------------------------------------------------*/
Value *prim_bit_not(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  checkCount(1, NULL);
  checkInt(0, NULL);
  
  return newInt( ~getInt(argv[0]) );
}

/*-------------------------------------------------------------*/
Value *prim_rand(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  double drand;
  int irand, i;
  int low, high;
 
  for(i = 0; i < argc; i++) checkNumber(i, NULL);
  
  if(argc == 0) {
    low = 0;
    high = RAND_MAX;
  }
  else if(argc == 1) {
    low = 0;
    high = getIntfromNumber(argv[0]);
  }
  else {
    low = getIntfromNumber(argv[0]);
    high = getIntfromNumber(argv[1]);
  }
  drand = ((double)rand() / (double)RAND_MAX) * ((double) (high - low + 1));
  irand = ((int) drand) + low;

  return newInt(irand);
}

/*-------------------------------------------------------------*/
Value *prim_srand(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  int irand;
  
  checkCount(1, NULL);
  checkNumber(0, NULL);
  
  irand = getIntfromNumber(argv[0]);
  srand((unsigned int) irand);
  
  return referValue(argv[0]);
}

/*-------------------------------------------------------------*/
Value *prim_linear_interpolate(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  real frac;
  Value *newval;

  checkCount(3, "a fraction between 0.0 and 1.0 and two values");
  checkNumber(0, "fraction");
    
  frac = getRealfromNumber(argv[0]);
  
  if(frac < 0.0 || frac > 1.0)
    fprintf(stderr, "** %s: WARNING: Fraction not between 0 and 1.\n", 
	    proc_name);
  
  linear_interpolate(script, argv[1], argv[2], frac, &newval);
  
  return newval;
}

/*-------------------------------------------------------------*/
Value *prim_cubic_interpolate(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv)
{
  real frac;
  Value *newval;

  checkCount(5, "a fraction between 0.0 and 1.0 and four values");
  checkNumber(0, "fraction");
  
  frac = getRealfromNumber(argv[0]);
  
  if(frac < 0.0 || frac > 1.0)
    fprintf(stderr, "** %s: WARNING: Fraction not between 0 and 1.\n", 
	    proc_name);
  
  cubic_interpolate(script, argv[1], argv[2], argv[3], argv[4], frac, &newval);
  
  return newval;
}

/*-------------------------------------------------------------*/
static void linear_interpolate(Script *script, 
			       Value *val1, Value *val2, 
			       real frac, Value **newval)
{
  int i, num_items, int1, int2;
  real real1, real2;
  
  *newval = NullValue;
  
  if(isList(val1)) {
    if(!isList(val2)) return;
    
    /* find minimum number of items */
    num_items = minimum(Listsize(val1), Listsize(val2));

    *newval = newList(num_items, NULL);
    
    for(i = 0; i < num_items; i++) {
      linear_interpolate(script, Listitem(val1, i), Listitem(val2, i), 
			 frac, &(Listitem(*newval, i)));
    }
  }
  else {
    if(!isNumber(val1) || !isNumber(val2)) return;
    
    if(isInt(val1)) {
      int1 = getIntfromNumber(val1);
      int2 = getIntfromNumber(val2);      
      *newval = newInt(((int) (((real) (int2 - int1)) * frac)) + int1);
    }      
    else if(isReal(val1)) {
      real1 = getRealfromNumber(val1);
      real2 = getRealfromNumber(val2);      
      *newval = newReal(((real2 - real1) * frac) + real1);
    }
  }
}

/*-------------------------------------------------------------*/
static void cubic_interpolate(Script *script, 
			      Value *val1, Value *val2, 
			      Value *val3, Value *val4,
			      real frac, Value **newval)
{
  int i, num_items, lnum1, lnum2;
  real real1, real2, real3, real4, frac2, frac3, result;
  
  *newval = NullValue;
  
  if(isList(val2)) {
    /* bail if the other values are not lists */
    if(!isList(val1) || !isList(val3) || !isList(val4)) return;
    
    /* find minimum number of items in the list */
    lnum1 = minimum(Listsize(val1), Listsize(val2));
    lnum2 = minimum(Listsize(val3), Listsize(val4));
    num_items = minimum(lnum1, lnum2);
    
    *newval = newList(num_items, NULL);

    for(i = 0; i < num_items; i++) {
      cubic_interpolate(script, Listitem(val1, i), Listitem(val2, i), 
			Listitem(val3, i), Listitem(val4, i), 
			frac, &(Listitem(*newval, i)));
    }
  }
  else {
    if(!isNumber(val1) || !isNumber(val2) || 
       !isNumber(val3) || !isNumber(val4)) return;
    
    frac2 = frac * frac;
    frac3 = frac2 * frac;
    real1 = getRealfromNumber(val1);
    real2 = getRealfromNumber(val2);
    real3 = getRealfromNumber(val3);
    real4 = getRealfromNumber(val4);      
    
    result = (((        -frac3  + (2.0 * frac2) - frac       ) * real1) +
	      (( ( 3.0 * frac3) - (5.0 * frac2)        + 2.0 ) * real2) +
	      (( (-3.0 * frac3) + (4.0 * frac2) + frac       ) * real3) +
	      ((         frac3  -        frac2               ) * real4)) / 2.0;
    
    if(isInt(val2)) *newval = newInt((int) result);
    else *newval = newReal(result);
  }
}
