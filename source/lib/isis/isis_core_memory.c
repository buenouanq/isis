/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* This file contains functions that handle memory managment for all
   structures used in the language. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "isis.h"

Memory *Token_mem = NULL;
Memory *Value_mem = NULL;
Memory *Parse_mem = NULL;
Memory *Script_mem = NULL;
Memory *Expression_mem = NULL;
Memory *ExpList_mem = NULL;
Memory *ExpNode_mem = NULL;
Memory *Varname_mem = NULL;
Memory *Binding_mem = NULL;
Memory *Environment_mem = NULL;
Memory *Procedure_mem = NULL;
Memory *StringLow_mem = NULL;
Memory *StringMed_mem = NULL;
Memory *StringHigh_mem = NULL;
Memory *PtrLow_mem = NULL;
Memory *PtrMed_mem = NULL;
Memory *PtrHigh_mem = NULL;

/*-------------------------------------------------------------*/
void initMemory(void)
{
  Token_mem = mem_new_memory("Token", sizeof(Token), 
			     2000, 2000);
  Value_mem = mem_new_memory("Value", sizeof(Value), 
			     10000, 20000);
  Parse_mem = mem_new_memory("Parse", sizeof(Parse), 
			     10, 10);
  Script_mem = mem_new_memory("Script", sizeof(Script), 
			      5, 5);
  Expression_mem = mem_new_memory("Expression", sizeof(Expression), 
				  10000, 10000);
  ExpList_mem = mem_new_memory("ExpList", sizeof(ExpList), 
			       100, 100);
  ExpNode_mem = mem_new_memory("ExpNode", sizeof(ExpNode), 
			       500, 500);
  Varname_mem = mem_new_memory("Varname", sizeof(Varname), 
			       2000, 1000);
  Binding_mem = mem_new_memory("Binding", sizeof(Binding), 
			       1000, 1000);
  Environment_mem = mem_new_memory("Environment", sizeof(Environment), 
				   150, 300);
  Procedure_mem = mem_new_memory("Procedure", sizeof(Procedure), 
				 1000, 1000);
  
  StringLow_mem = mem_new_memory("StringLow", 
				 sizeof(char) * (STRING_LOW_LEN + 1), 
				 1000, 1000);
  StringMed_mem = mem_new_memory("StringMed", 
				 sizeof(char) * (STRING_MED_LEN + 1), 
				 2000, 1000);
  StringHigh_mem = mem_new_memory("StringHigh", 
				  sizeof(char) * (STRING_HIGH_LEN + 1), 
				  125, 200);

  PtrLow_mem = mem_new_memory("PtrLow", 
			      sizeof(void *) * (PTR_LOW_LEN + 1), 
			      7000, 5000);
  PtrMed_mem = mem_new_memory("PtrMed", 
			      sizeof(void *) * (PTR_MED_LEN + 1), 
			      1000, 1000);
  PtrHigh_mem = mem_new_memory("PtrHigh", 
			       sizeof(void *) * (PTR_HIGH_LEN + 1), 
			       50, 100); 
}

/*-------------------------------------------------------------*/
char *newStr(int len)
{
  char *str;
  
  if(len == 0) return NULL;
  if(len <= STRING_LOW_LEN) {
    str = mem_alloc_item(StringLow_mem);
    str[0] = (char) STRING_LOW;
  }
  else if(len <= STRING_MED_LEN) {
    str = mem_alloc_item(StringMed_mem);
    str[0] = (char) STRING_MED;
  }
  else if(len <= STRING_HIGH_LEN) {
    str = mem_alloc_item(StringHigh_mem);
    str[0] = (char) STRING_HIGH;
  }
  else {
    str = mem_alloc((len + 1) * sizeof(char));
    str[0] = (char) STRING_EXTERN;
  }

  return str + 1;
}

/*-------------------------------------------------------------*/
void freeStr(char *str)
{
  char *realstr;

  if(str == NULL) return;

  realstr = str - 1;

  switch( (int) realstr[0] ) {

  case STRING_LOW:
    mem_freeItem(StringLow_mem, realstr);
    break;

  case STRING_MED:
    mem_freeItem(StringMed_mem, realstr);
    break;

  case STRING_HIGH:
    mem_freeItem(StringHigh_mem, realstr);
    break;

  case STRING_EXTERN:
    free(realstr);
    break;

  }
}

/*-------------------------------------------------------------*/
char *copyStr(char *str)
{
  int len;
  char *realstr, *newstr;

  if(str == NULL) return NULL;

  realstr = str - 1;
  
  switch( (int) realstr[0] ) {

  case STRING_LOW:
    len = STRING_LOW_LEN;
    break;

  case STRING_MED:
    len = STRING_MED_LEN;
    break;

  case STRING_HIGH:
    len = STRING_HIGH_LEN;
    break;

  default:
    len = strlen(str) + 1;
    break;

  }
  
  newstr = newStr(len);
  strcpy(newstr, str);

  return newstr;
}

/*-------------------------------------------------------------*/
char *createStr(char *str)
{
  /* Creates a new string exactly the same as the argument */
  
  char *newstr;
  
  if(str == NULL) return NULL;
  
  newstr = newStr(strlen(str) + 1);
  strcpy(newstr, str);
  
  return newstr;
}

/*-------------------------------------------------------------*/
char *createStrn(char *str, int len)
{
  /* Creates a new string exactly the same as the argument, but the
     array will be of length 'len'. */
  
  char *newstr;
  
  if(str == NULL) return NULL;
  
  newstr = newStr(len);
  strncpy(newstr, str, len);
  
  return newstr;
}

/*-------------------------------------------------------------*/
void **newPtrArray(int len)
{
  void **ptrarray;

  if(len == 0) return NULL;
  else if(len <= PTR_LOW_LEN) {
    ptrarray = mem_alloc_item(PtrLow_mem);
    ptrarray[0] = (void *) PTR_LOW;
  }
  else if(len <= PTR_MED_LEN) {
    ptrarray = mem_alloc_item(PtrMed_mem);
    ptrarray[0] = (void *) PTR_MED;
  }
  else if(len <= PTR_HIGH_LEN) {
    ptrarray = mem_alloc_item(PtrHigh_mem);
    ptrarray[0] = (void *) PTR_HIGH;
  }
  else {
    ptrarray = mem_alloc((len + 1) * sizeof(void *));
    ptrarray[0] = (void *) PTR_EXTERN;
  }

  return ptrarray + 1;
}

/*-------------------------------------------------------------*/
void freePtrArray(void **ptrarray)
{
  void **realptrarray;
  
  if(ptrarray == NULL) return;

  realptrarray = ptrarray - 1;

  switch( (int) realptrarray[0] ) {

  case PTR_LOW:
    mem_freeItem(PtrLow_mem, realptrarray);
    break;
    
  case PTR_MED:
    mem_freeItem(PtrMed_mem, realptrarray);
    break;

  case PTR_HIGH:
    mem_freeItem(PtrHigh_mem, realptrarray);
    break;
    
  case PTR_EXTERN:
    free(realptrarray);
    break;

  }
}

/*----------------------------------------------------------------------*/
void *allocBuffer(int size)
{
  void *buf;
  
  buf = malloc(size);
  
  if(buf == NULL) {
    fprintf(stderr, "* allocBuffer of %d bytes failed.\n", size);
    exit(1);
  }
  
  return buf;
}

/*----------------------------------------------------------------------*/
void *expandBuffer(void *oldbuf, int newsize)
{
  void *newbuf;
  
  if(oldbuf) newbuf = realloc(oldbuf, newsize);
  else newbuf = malloc(newsize);
  
  if(newbuf == NULL) {
    fprintf(stderr, "* expandBuffer to %d bytes failed.\n", newsize);
    exit(1);
  }
  
  return newbuf;
}

/*-------------------------------------------------------------*/
Value *memPeek(void *genptr, int ctype)
{
  switch(ctype) {

  case CTYPE_UCHAR: return newInt( *((unsigned char *)genptr) );
  case CTYPE_CHAR: return newChar( *((char *)genptr) );
  case CTYPE_SHORT: return newInt( *((short *)genptr) );
  case CTYPE_USHORT: return newInt( *((unsigned short *)genptr) );
  case CTYPE_INT: return newInt( *((int *)genptr) );
  case CTYPE_UINT: return newInt( *((unsigned int *)genptr) );
  case CTYPE_LONG: return newInt( *((long *)genptr) );
  case CTYPE_ULONG: return newInt( *((unsigned long *)genptr) );
#ifdef _LONGLONG
  case CTYPE_LONGLONG: return newInt( *((long long *)genptr) );
  case CTYPE_ULONGLONG: return newInt( *((unsigned long long *)genptr) );
#endif	
  case CTYPE_FLOAT: return newReal( *((float *)genptr) );
  case CTYPE_DOUBLE: return newReal( *((double *)genptr) );
  case CTYPE_POINTER: return newAddr( *((void **)genptr) );
  }
  
  fprintf(stderr, "* memPeek: %d is not a valid C type.\n", ctype);
  return NullValue;
}


/*-------------------------------------------------------------*/
Value *memPoke(void *genptr, int ctype, Value *val)
{
  int isistype;

  isistype = typeof(val);

  switch(ctype) {
    
  case CTYPE_UCHAR:
    switch(isistype) {
    case IsisInt: *((unsigned char *)genptr) = (unsigned char) getInt(val); return referValue(val);      
    case IsisReal: *((unsigned char *)genptr) = (unsigned char) getReal(val); return referValue(val);
    case IsisChar: *((unsigned char *)genptr) = (unsigned char) getChar(val); return referValue(val);
    case IsisBool: *((unsigned char *)genptr) = (unsigned char) getBool(val); return referValue(val);      
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }	

  case CTYPE_CHAR:
    switch(isistype) {
    case IsisInt: *((char *)genptr) = (char) getInt(val); return referValue(val);
    case IsisReal: *((char *)genptr) = (char) getReal(val); return referValue(val);
    case IsisChar: *((char *)genptr) = (char) getChar(val); return referValue(val);
    case IsisBool: *((char *)genptr) = (char) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }	
    
  case CTYPE_SHORT:
    switch(isistype) {
    case IsisInt: *((short *)genptr) = (short) getInt(val); return referValue(val);
    case IsisReal: *((short *)genptr) = (short) getReal(val); return referValue(val);
    case IsisChar: *((short *)genptr) = (short) getChar(val); return referValue(val);
    case IsisBool: *((short *)genptr) = (short) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_USHORT:
    switch(isistype) {
    case IsisInt: *((unsigned short *)genptr) = (unsigned short) getInt(val); return referValue(val);
    case IsisReal: *((unsigned short *)genptr) = (unsigned short) getReal(val); return referValue(val);
    case IsisChar: *((unsigned short *)genptr) = (unsigned short) getChar(val); return referValue(val);
    case IsisBool: *((unsigned short *)genptr) = (unsigned short) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_INT:
    switch(isistype) {
    case IsisInt: *((int *)genptr) = (int) getInt(val); return referValue(val);
    case IsisReal: *((int *)genptr) = (int) getReal(val); return referValue(val);
    case IsisChar: *((int *)genptr) = (int) getChar(val); return referValue(val);
    case IsisBool: *((int *)genptr) = (int) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_UINT:
    switch(isistype) {
    case IsisInt: *((unsigned int *)genptr) = (unsigned int) getInt(val); return referValue(val);
    case IsisReal: *((unsigned int *)genptr) = (unsigned int) getReal(val); return referValue(val);
    case IsisChar: *((unsigned int *)genptr) = (unsigned int) getChar(val); return referValue(val);
    case IsisBool: *((unsigned int *)genptr) = (unsigned int) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_LONG:
    switch(isistype) {
    case IsisInt: *((long *)genptr) = (long) getInt(val); return referValue(val);
    case IsisReal: *((long *)genptr) = (long) getReal(val); return referValue(val);
    case IsisChar: *((long *)genptr) = (long) getChar(val); return referValue(val);
    case IsisBool: *((long *)genptr) = (long) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_ULONG:
    switch(isistype) {
    case IsisInt: *((unsigned long *)genptr) = (unsigned long) getInt(val); return referValue(val);
    case IsisReal: *((unsigned long *)genptr) = (unsigned long) getReal(val); return referValue(val);
    case IsisChar: *((unsigned long *)genptr) = (unsigned long) getChar(val); return referValue(val);
    case IsisBool: *((unsigned long *)genptr) = (unsigned long) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
#ifdef _LONGLONG
  case CTYPE_LONGLONG:
    switch(isistype) {
    case IsisInt: *((long long *)genptr) = (long long) getInt(val); return referValue(val);
    case IsisReal: *((long long *)genptr) = (long long) getReal(val); return referValue(val);
    case IsisChar: *((long long *)genptr) = (long long) getChar(val); return referValue(val);
    case IsisBool: *((long long *)genptr) = (long long) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_ULONGLONG:
    switch(isistype) {
    case IsisInt: *((unsigned long long *)genptr) = (unsigned long long) getInt(val); return referValue(val);
    case IsisReal: *((unsigned long long *)genptr) = (unsigned long long) getReal(val); return referValue(val);
    case IsisChar: *((unsigned long long *)genptr) = (unsigned long long) getChar(val); return referValue(val);
    case IsisBool: *((unsigned long long *)genptr) = (unsigned long long) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
#endif
	
  case CTYPE_FLOAT:
    switch(isistype) {
    case IsisInt: *((float *)genptr) = (float) getInt(val); return referValue(val);
    case IsisReal: *((float *)genptr) = (float) getReal(val); return referValue(val);
    case IsisChar: *((float *)genptr) = (float) getChar(val); return referValue(val);
    case IsisBool: *((float *)genptr) = (float) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_DOUBLE:
    switch(isistype) {
    case IsisInt: *((double *)genptr) = (double) getInt(val); return referValue(val);
    case IsisReal: *((double *)genptr) = (double) getReal(val); return referValue(val);
    case IsisChar: *((double *)genptr) = (double) getChar(val); return referValue(val);
    case IsisBool: *((double *)genptr) = (double) getBool(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }
	
  case CTYPE_POINTER:
    switch(isistype) {
    case IsisAddr: *((void **)genptr) = getAddr(val); return referValue(val);
    default: fprintf(stderr, "* memPoke: Incompatible type.\n"); return NullValue;
    }	
    
  }

  fprintf(stderr, "* memPoke: %d is not a valid C type.\n", ctype); 
  return NullValue;
}

/*-------------------------------------------------------------*/
Value *memPeekArray(void *genptr, int arraytype, int size)
{
  int i;
  Value *val;
    
  switch(arraytype) {
    
  case CTYPE_UCHAR: {
    unsigned char *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
  
  case CTYPE_CHAR: {
    char *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newChar(*ptr++);
    return val;
  }
    
  case CTYPE_SHORT: {
    short *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
	
  case CTYPE_USHORT: {
    unsigned short *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
	
  case CTYPE_INT: {
    int *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
	
  case CTYPE_UINT: {
    unsigned int *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
	
  case CTYPE_LONG: {
    long *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
	
  case CTYPE_ULONG: {
    unsigned long *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
	
#ifdef _LONGLONG
  case CTYPE_LONGLONG: {
    long long *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
	
  case CTYPE_ULONGLONG: {
    unsigned long long *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newInt(*ptr++);
    return val;
  }
#endif
    
  case CTYPE_FLOAT: {
    float *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newReal(*ptr++);
    return val;
  }
	
  case CTYPE_DOUBLE: {
    double *ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newReal(*ptr++);
    return val;
  }
	
  case CTYPE_POINTER: {
    void **ptr = genptr;
    val = newList(size, NULL);
    for(i = 0; i < size; i++) Listitem(val, i) = newAddr(*ptr++);
    return val;
  }
  }
  
  fprintf(stderr, "* memPeekArray: %d is not a valid C type.\n", arraytype);
  return NullValue;  
}


/*-------------------------------------------------------------*/
Value *memPokeArray(void *genptr, int arraytype, int arraysize, Value *val)
{
  int listtype, listsize, i;
  
  if( !( listtype = homogeneousList(val, 0) ) ) {
    fprintf(stderr, "* memPokeArray: Value was not a homogeneous list.\n");
    return NullValue;
  }
  
  listsize = minimum(arraysize, Listsize(val));
  
  switch(arraytype) {
    
  case CTYPE_UCHAR: {
    unsigned char *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (unsigned char) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (unsigned char) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (unsigned char) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (unsigned char) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }

  case CTYPE_CHAR: {
    char *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (char) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (char) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (char) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (char) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
    
  case CTYPE_SHORT: {
    short *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (short) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (short) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (short) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (short) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }

  case CTYPE_USHORT: {
    unsigned short *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (unsigned short) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (unsigned short) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (unsigned short) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (unsigned short) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
  case CTYPE_INT: {
    int *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (int) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (int) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (int) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (int) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
  case CTYPE_UINT: {
    unsigned int *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (unsigned int) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (unsigned int) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (unsigned int) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (unsigned int) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
  case CTYPE_LONG: {
    long *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (long) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (long) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (long) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (long) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
  case CTYPE_ULONG: {
    unsigned long *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
#ifdef _LONGLONG
  case CTYPE_LONGLONG: {
    long long *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (long long) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (long long) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (long long) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (long long) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
  case CTYPE_ULONGLONG: {
    unsigned long long *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long long) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long long) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long long) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (unsigned long long) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
#endif
	
  case CTYPE_FLOAT: {
    float *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (float) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (float) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (float) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (float) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
  case CTYPE_DOUBLE: {
    double *ptr = genptr;
    switch(listtype) {
    case IsisInt: for(i = 0; i < listsize; i++) *ptr++ = (double) getInt(Listitem(val, i)); return referValue(val);
    case IsisReal: for(i = 0; i < listsize; i++) *ptr++ = (double) getReal(Listitem(val, i)); return referValue(val);
    case IsisChar: for(i = 0; i < listsize; i++) *ptr++ = (double) getChar(Listitem(val, i)); return referValue(val);
    case IsisBool: for(i = 0; i < listsize; i++) *ptr++ = (double) getBool(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
	
  case CTYPE_POINTER: {
    void **ptr = genptr;
    switch(listtype) {
    case IsisAddr: for(i = 0; i < listsize; i++) *ptr++ = getAddr(Listitem(val, i)); return referValue(val);
    default: fprintf(stderr, "* memPokeArray: Incompatible type.\n"); return NullValue;
    }	
  }
  }
  
  fprintf(stderr, "* memPokeArray: %d is not a valid C type.\n", arraytype);
  return NullValue;
}
