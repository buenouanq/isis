/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* Value coding/decoding routines */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isis.h"

#define CODE_INT 20
#define CODE_CHAR 21
#define CODE_REAL 22
#define CODE_BOOL 23
#define CODE_ADDR 24
#define CODE_PROC 25
#define CODE_LIST 26
#define CODE_HOMOGENEOUS_LIST 27
#define CODE_NULL 28

#define LEN_INT 4
#define LEN_CHAR 1
#define LEN_REAL 4
#define LEN_BOOL 1
#define LEN_TAG 1 

/*----------------------------------------------------------------------*/
unsigned int reverseInteger(unsigned int x)
{
  return ((x << 24) | 
	  ((x & 0xff00) << 8) | 
	  ((x >> 8) & 0xff00) | 
	  (x >> 24));
}

/*----------------------------------------------------------------------*/
unsigned short reverseShort(unsigned short x)
{
  return (x << 8) | (x >> 8);
}

/*----------------------------------------------------------------------*/
int precodeValue(Value *val)
{
  int listtype, i, tot;

  switch(typeof(val)) {
  case IsisNull:
    return LEN_TAG;

  case IsisInt:
    return LEN_TAG + LEN_INT;

  case IsisReal:
    return LEN_TAG + LEN_REAL;

  case IsisBool:
    return LEN_TAG + LEN_BOOL;

  case IsisChar:
    return LEN_TAG + LEN_CHAR;

  case IsisAddr:
    return LEN_TAG;   /* cannot send addresses */

  case IsisProc:
    return LEN_TAG;   /* cannot send procedures */

  case IsisList:
    listtype = homogeneousList(val, 0);
    switch(listtype) {
      
    case IsisNull:
      return (2 * LEN_TAG) + LEN_INT;
      
    case IsisInt:
      return (2 * LEN_TAG) + LEN_INT + (LEN_INT * Listsize(val));
      
    case IsisReal:
      return (2 * LEN_TAG) + LEN_INT + (LEN_REAL * Listsize(val));
      
    case IsisBool:
      return (2 * LEN_TAG) + LEN_INT + (LEN_BOOL * Listsize(val));
      
    case IsisChar:
      return (2 * LEN_TAG) + LEN_INT + (LEN_CHAR * Listsize(val));
      
    case IsisAddr:
      return (2 * LEN_TAG) + LEN_INT;   /* cannot send addresses */

    case IsisProc:
      return (2 * LEN_TAG) + LEN_INT;   /* cannot send procedures */
      
    default:   /* list of lists or non-homogeneous list */
      tot = LEN_TAG + LEN_INT;  /* size of list */
      for(i = 0; i < Listsize(val); i++) 
	tot += precodeValue(Listitem(val, i));
      return tot;
    }
  }

  return 0;   /* return 0 only if value is invalid */
}

/*----------------------------------------------------------------------*/
/* Macros for encodeValue() */

#ifdef ISIS_BIG_ENDIAN   /* BIG ENDIAN MACROS */

#define encFloat(val)  \
    tempfloat = (val);  temp = (char *) (void *) &tempfloat;  \
    *vp++ = *temp++; *vp++ = *temp++;  \
    *vp++ = *temp++; *vp++ = *temp++; 

#define encInt(val)  \
    tempint = (val);  temp = (char *) (void *) &tempint;  \
    *vp++ = *temp++; *vp++ = *temp++;  \
    *vp++ = *temp++; *vp++ = *temp++; 

#else  /* LITTLE ENDIAN MACROS */

#define encFloat(val)  \
    tempfloat = (val);  temp = (char *) (void *) &tempfloat;  temp += 3; \
    *vp++ = *temp--; *vp++ = *temp--; \
    *vp++ = *temp--; *vp++ = *temp--;

#define encInt(val)  \
    tempint = (val);  temp = (char *) (void *) &tempint;  temp += 3;  \
    *vp++ = *temp--; *vp++ = *temp--; \
    *vp++ = *temp--; *vp++ = *temp--;

#endif

/*----------------------------------------------------------------------*/
char *encodeValue(Value *val, char *vp)
{
  int listtype, i, tempint;
  float tempfloat;
  char *temp;

  switch(typeof(val)) {

  case IsisNull:
    *vp++ = CODE_NULL;
    break;

  case IsisInt:
    *vp++ = CODE_INT;
    encInt(getInt(val));
    break;

  case IsisReal:
    *vp++ = CODE_REAL;
    encFloat((float) getReal(val));
    break;

  case IsisBool:
    *vp++ = CODE_BOOL;
    *vp++ = (char) getBool(val);
    break;

  case IsisChar:
    *vp++ = CODE_CHAR;
    *vp++ = getChar(val);
    break;

  case IsisAddr:
    *vp++ = CODE_ADDR;
    break;

  case IsisProc:
    *vp++ = CODE_PROC;
    break;

  case IsisList:
    listtype = homogeneousList(val, 0);
    switch(listtype) {

    case IsisNull:
      *vp++ = CODE_HOMOGENEOUS_LIST;
      *vp++ = CODE_NULL;
      encInt(Listsize(val));
      break;
      
    case IsisInt:
      *vp++ = CODE_HOMOGENEOUS_LIST;
      *vp++ = CODE_INT;
      encInt(Listsize(val));
      for(i = 0; i < Listsize(val); i++) {
	encInt(getInt(Listitem(val, i)));
      }
      break;
      
    case IsisReal:
      *vp++ = CODE_HOMOGENEOUS_LIST;
      *vp++ = CODE_REAL;
      encInt(Listsize(val));
      for(i = 0; i < Listsize(val); i++) {
	encFloat((float) getReal(Listitem(val, i)));
      }
      break;
      
    case IsisBool:
      *vp++ = CODE_HOMOGENEOUS_LIST;
      *vp++ = CODE_BOOL;
      encInt(Listsize(val));
      for(i = 0; i < Listsize(val); i++)
	*vp++ = getBool(Listitem(val, i));
      break;
      
    case IsisChar:
      *vp++ = CODE_HOMOGENEOUS_LIST;
      *vp++ = CODE_CHAR;
      encInt(Listsize(val));
      for(i = 0; i < Listsize(val); i++)
	*vp++ = getChar(Listitem(val, i));
      break;
      
    case IsisAddr:
      *vp++ = CODE_HOMOGENEOUS_LIST;
      *vp++ = CODE_ADDR;
      encInt(Listsize(val));
      break;                  /* cannot send addresses */
      
    case IsisProc:
      *vp++ = CODE_HOMOGENEOUS_LIST;
      *vp++ = CODE_PROC;
      encInt(Listsize(val));
      break;                 /* cannot send procedures */
      
    default:   /* list of lists or non-homogeneous list */
      *vp++ = CODE_LIST;
      encInt(Listsize(val));
      for(i = 0; i < Listsize(val); i++)
	vp = encodeValue(Listitem(val, i), vp);
      break;
    }
  }

  return vp;
}


/*----------------------------------------------------------------------*/
/* Macros for decodeValue() */

#ifdef ISIS_BIG_ENDIAN   /* BIG ENDIAN MACROS */

#define decFloat()  \
    temp = (char *) (void *) &(val);  \
    *temp++ = *vp++; *temp++ = *vp++;  \
    *temp++ = *vp++; *temp++ = *vp++;

#define decInt()  \
    temp = (char *) (void *) &(val);  \
    *temp++ = *vp++; *temp++ = *vp++;  \
    *temp++ = *vp++; *temp++ = *vp++;

#else  /* LITTLE ENDIAN MACROS */

#define decFloat(val)  \
    temp = (char *) (void *) &(val);  temp += 3;  \
    *temp-- = *vp++; *temp-- = *vp++;  \
    *temp-- = *vp++; *temp-- = *vp++;

#define decInt(val)  \
    temp = (char *) (void *) &(val);  temp += 3;  \
    *temp-- = *vp++; *temp-- = *vp++;  \
    *temp-- = *vp++; *temp-- = *vp++;

#endif


/*----------------------------------------------------------------------*/
Value *decodeValue(char **vpptr)
{
  int i, len, tempint;
  float tempfloat;
  char *temp, *vp, tempchar;
  Value *listval;

  vp = *vpptr;
  
  if(vp == NULL) return NullValue;

  switch (*vp++) {

  case CODE_NULL:
    *vpptr = vp;
    return NullValue;

  case CODE_INT:
    decInt(tempint);
    *vpptr = vp;
    return newInt(tempint);

  case CODE_REAL:
    decFloat(tempfloat);
    *vpptr = vp;
    return newReal((double) tempfloat);
    
  case CODE_CHAR:
    tempchar = *vp++;
    *vpptr = vp;
    return newChar(tempchar);

  case CODE_BOOL:
    tempchar = *vp++;
    *vpptr = vp;
    return newBool(tempchar);

  case CODE_ADDR:
    *vpptr = vp;
    return newAddr(NULL);

  case CODE_PROC:
    *vpptr = vp;
    return newProc(NULL);

  case CODE_HOMOGENEOUS_LIST:
    switch(*vp++) {
      
    case CODE_NULL:
      decInt(len);
      listval = newList(len, NULL);
      for(i = 0; i < len; i++) Listitem(listval, i) = NullValue;
      *vpptr = vp;
      return listval;

    case CODE_INT:
      decInt(len);
      listval = newList(len, NULL);
      for(i = 0; i < len; i++) {
	decInt(tempint);
	Listitem(listval, i) = newInt(tempint);
      }
      *vpptr = vp;
      return listval;

    case CODE_REAL:
      decInt(len);
      listval = newList(len, NULL);
      for(i = 0; i < len; i++) {
	decFloat(tempfloat);
	Listitem(listval, i) = newReal((double) tempfloat);
      }
      *vpptr = vp;
      return listval;

    case CODE_CHAR:
      decInt(len);
      listval = newList(len, NULL);
      for(i = 0; i < len; i++) Listitem(listval, i) = newChar(*vp++);
      *vpptr = vp;
      return listval;
      
    case CODE_BOOL:
      decInt(len);
      listval = newList(len, NULL);
      for(i = 0; i < len; i++) Listitem(listval, i) = newBool(*vp++);
      *vpptr = vp;
      return listval;
      
    case CODE_ADDR:
      decInt(len);
      listval = newList(len, NULL);
      for(i = 0; i < len; i++) Listitem(listval, i) = NullValue;
      *vpptr = vp;
      return listval;
      
    case CODE_PROC:
      decInt(len);
      listval = newList(len, NULL);
      for(i = 0; i < len; i++) Listitem(listval, i) = NullValue;
      *vpptr = vp;
      return listval;

    }
    
  case CODE_LIST:
    decInt(len);
    listval = newList(len, NULL);
    for(i = 0; i < len; i++) Listitem(listval, i) = decodeValue(&vp);
    *vpptr = vp;
    return listval;

  default:
    *vpptr = vp;
    return NullValue; 
  }
}

/*----------------------------------------------------------------------*/
int encodeItem(Script *script, Value *port, Token *token)
{
  unsigned char clen;
  unsigned int nlen;
  char temp[256], ckind;
  int len, tot = 0;
  
  while(token != NULL) {
    
    ckind = token->kind;
    tot += printStrn(script, port, &ckind, 1);
    
    if(token->kind == TOK_STRING || token->kind == TOK_ATOM ||
       token->kind == TOK_INT || token->kind == TOK_REAL ||
       token->kind == TOK_CHAR || token->kind == TOK_ADDR ||
       token->kind == TOK_BINARY) {
      
      len = strlen(token->str);
      if(len < 252) {
	clen = len;
	tot += printStrn(script, port, (char *) &clen, 1);
	encodeString(len, len, token->str, temp);
	tot += printStrn(script, port, temp, len);
      }
      else {
	clen = 255;
	tot += printStrn(script, port, (char *) &clen, 1);
	nlen = encodeInteger(len);
	tot += printStrn(script, port, (char *) &nlen, 4);
	tot += printStrn(script, port, token->str, len);
      }
    }
    token = token->next;
  }

  return tot;
}

/*----------------------------------------------------------------------*/
int decodeItem(Script *script, Value *port, Token **token)
{
  Token *curtoken;
  unsigned char clen;
  unsigned int nlen;
  char *str, ckind;
  int len, kind, tokencount;

  *token = curtoken = newToken();
  curtoken->kind = TOK_END;
  tokencount = 1;
  
  while(1) {
    if( readStrn(script, port, &ckind, 1) != 1 ) break;
    kind = ckind;
    
    if(kind == TOK_STRING || kind == TOK_ATOM ||
       kind == TOK_INT || kind == TOK_REAL ||
       kind == TOK_CHAR || kind == TOK_ADDR ||
       kind == TOK_BINARY) {
      
      if( readStrn(script, port, (char *) &clen, 1) != 1 ) break;
      len = clen;
      if(len == 255) {
	if( readStrn(script, port, (char *) &nlen, 4) != 4 ) break;
	len = decodeInteger(nlen);
	str = newStr(len + 1);
	if( readStrn(script, port, str, len) != len) {
	  freeStr(str);
	  break;
	}
      }
      else {
	str = newStr(len + 1);
	if( readStrn(script, port, str, len) != len) {
	  freeStr(str);
	  break;
	}
	decodeString(len, len, str, str);
      }

      str[len] = 0;
      curtoken->str = str;
    }
    curtoken->kind = kind;
    curtoken->colsnum = 0;
    curtoken->line = 0;

    if(kind == TOK_END) break;
    
    curtoken->next = newToken();
    curtoken = curtoken->next;
    curtoken->kind = TOK_END;
    tokencount++;
  }

  return tokencount;
}

/*----------------------------------------------------------------------*/
void encodeString(int code, int len, char *instr, char *outstr)
{
  int i;
  for(i = 0; i < len; i++) 
    *outstr++ = (char) (((int) *instr++) + code) % 256;
}

/*----------------------------------------------------------------------*/
void decodeString(int code, int len, char *instr, char *outstr)
{
  int i;
  for(i = 0; i < len; i++) 
    *outstr++ = (char) (((int) *instr++) + 256 - code) % 256;
}
