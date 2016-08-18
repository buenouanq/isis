/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* This file contains functions to parse tokens from an input stream. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "isis.h"

static char *allowed_symbols = "~!@#$%^&*_-+=|/?><.";

#define PARSE_ERR(parse) fprintf(stderr, "* Error in %s, line %d, col %d:\n", \
				 lookupString(parse->snum), \
				 parse->line, parse->col + parse->pos);

#define backchar(parse) if((parse)->pos > 0) ((parse)->pos)--;

/*-------------------------------------------------------------*/
static char nextchar(Parse *parse)
{
  /* should return a character or 0 if no more */

  if(parse->s[parse->pos]) return parse->s[parse->pos++];
  
  if(parse->pos > 0 && parse->s[parse->pos - 1] == NEWLINE_CHAR) { 
    parse->line++; 
    parse->col = -1; 
  }
  else parse->col += parse->pos;

  parse->pos = 0;
  
  if(isNull(parse->port) || 
     !readStr(parse->script, parse->port, parse->s, parse->maxlen)) {
    parse->s[0] = 0;
    return 0;
  }
  
  return parse->s[parse->pos++];
}

/*-------------------------------------------------------------*/
static void tokenchar(Parse *parse, char c)
{
  if(parse->tpos >= parse->maxtlen) {
    char *newstr;
    
    newstr = newStr(parse->maxtlen * 2);
    memcpy(newstr, parse->ts, parse->maxtlen);
    freeStr(parse->ts);
    parse->ts = newstr;
    parse->maxtlen *= 2;
  }
  
  parse->ts[parse->tpos++] = c;
}

/*-------------------------------------------------------------*/
Token *newToken(void)
{
  Token *token;
  
  token = mem_alloc_item(Token_mem);
  
  token->str = NULL;
  token->next = NULL;

  return token;
}

/*-------------------------------------------------------------*/
char skipComment(Parse *parse)
{
  char lastc, c = COMMENT_CHAR;
  int incomment = 1, inlongcomment = 0, instring = 0, inchar = 0;

  do {
    lastc = c;
    c = nextchar(parse);

    switch(c) {

    case ESCAPE_CHAR: 
      c = nextchar(parse); 
      break;

    case NEWLINE_CHAR: 
      incomment = 0; 
      break;

    case STRING_CHAR: 
      if(instring) instring = 0;
      else if(!inchar && !incomment) instring = 1;
      break;

    case CHAR_CHAR:
      if(inchar) inchar = 0;
      else if(!instring && !incomment) inchar = 1;
      break;

    case COMMENT_CHAR:
      if(!instring && !inchar) incomment = 1;
      break;

    case LONG_COMMENT_OPEN_CHAR:
      if(lastc == COMMENT_CHAR && incomment && !instring && !inchar) 
	inlongcomment = 1;
      break;

    case LONG_COMMENT_CLOSE_CHAR:
      if(inlongcomment && !instring && !inchar) {
	incomment = inlongcomment = 0;
	c = nextchar(parse);
      }
      break;
    }

  } while(c && (incomment || inlongcomment));
  
  return c;
}

/*-------------------------------------------------------------*/
int readItem(Parse *parse, Token **token)
{
  /* Get next item from the file or string and return a token list */
  
  char c;
  int nest, decimal, exponent, signok, ok, hex, binary, leadzero, digit;
  Token *first_token, *last_token;
  int tokencount;

  first_token = newToken();
  last_token = first_token;
  last_token->kind = TOK_END;
  nest = 0;
  tokencount = 1;
  *token = first_token;

  do {
    parse->tpos = 0;
    parse->ts[0] = 0;
    
    do {  /* skip over white space or comment */
      c = nextchar(parse);
      while(c == COMMENT_CHAR) c = skipComment(parse);
    } while(c && isspace(c));
      
    /* mark starting file, line, and column of token */
    last_token->colsnum = (parse->snum & 0xFFFF) | ((parse->col + parse->pos) << 16);
    last_token->line = parse->line;
    
    /* end of file */

    if(c == 0) break;

    /* open paren */

    else if(c == OPEN_CHAR) {
      last_token->kind = TOK_OPEN_PAREN;
      nest++;
    }

    /* open bracket */

    else if(c == LIST_OPEN_CHAR) {
      last_token->kind = TOK_OPEN_LIST;
      nest++;
    }

    /* close paren or bracket */

    else if(c == CLOSE_CHAR || c == LIST_CLOSE_CHAR) {
      last_token->kind = TOK_CLOSE;
      nest--;
      if(nest < 0) {
	nest = 0;
	PARSE_ERR(parse);
	fprintf(stderr, "*   Too many close parens/brackets.\n");
      }
    }

    /* possible number */

    else if(isdigit(c) || c == '-' || c == '+' || c == '.') {
      decimal = exponent = ok = hex = binary = leadzero = digit = 0;
      signok = 1;
      while(1) {
	if(binary) {
	  if(c != '0' && c != '1') break;
	}
	if(hex) {  /* hex */
	  if(!isxdigit(c)) break;
	}
	else {
	  ok = 0;
	  if(parse->tpos == 0 && c == '0') leadzero = 1;
	  if(parse->tpos == 1 && leadzero) {
	    if(c == 'x' || c == 'X') { hex = 1; ok = 1; }
	    if(c == 'b' || c == 'B') { binary = 2; ok = 1; }
	  }
	  if(signok && (c == '+' || c == '-')) ok = 1;
	  signok = 0;
	  if(c == '.') { ok = 1; decimal++; }
	  if(c == 'e') { ok = 1; exponent++; signok = 1; }
	  if(isdigit(c)) { ok = 1; digit = 1; }
	  if(!ok || decimal > 1 || exponent > 1) break;
	}
	tokenchar(parse, c);
	c = nextchar(parse);
      }
      backchar(parse);
      if(binary) last_token->kind = TOK_BINARY;
      else if(hex) last_token->kind = TOK_ADDR;
      else if(digit && (decimal || exponent)) last_token->kind = TOK_REAL;
      else if(digit) last_token->kind = TOK_INT;
      else last_token->kind = TOK_ATOM;
      tokenchar(parse, 0);
    }

    /* string enclosed in double quotes */

    else if(c == STRING_CHAR) {
      c = nextchar(parse);
      while(c != STRING_CHAR && c) {
	if(c == ESCAPE_CHAR) {
	  c = nextchar(parse);
	  c = controlChar(c);
	  if(c == 0) {
	    PARSE_ERR(parse);
	    fprintf(stderr, "*   Unrecognized escape character.\n");
	    c = nextchar(parse);
	    continue;
	  }
	}
	tokenchar(parse, c);
	c = nextchar(parse);
      }
      last_token->kind = TOK_STRING;
      tokenchar(parse, 0);
    }

    /* character constant */

    else if(c == CHAR_CHAR) {
      c = nextchar(parse);
      if(c == CHAR_CHAR) {
	PARSE_ERR(parse);
	fprintf(stderr, "*   Bad character constant.\n");
      }
      else {
	if(c == ESCAPE_CHAR) {
	  c = nextchar(parse);
	  c = controlChar(c);
	  if(c == 0) {
	    PARSE_ERR(parse);
	    fprintf(stderr, "*   Unrecognized escape character.\n");
	    c = ' ';
	  }
	}
	tokenchar(parse, c);
	tokenchar(parse, 0);
	c = nextchar(parse);
	if(c != CHAR_CHAR) {
	  PARSE_ERR(parse);
	  fprintf(stderr, "*   Bad character constant.\n");
	  while(c != CHAR_CHAR && c != NEWLINE_CHAR && c) c = nextchar(parse);
	}
	last_token->kind = TOK_CHAR;
      }
    }
    
    /* variable name */

    else if(isalpha(c) || allowedSymbol(c)) {
      while(isalnum(c) || allowedSymbol(c)) {
	tokenchar(parse, c);
	c = nextchar(parse);
      }
      backchar(parse);
      last_token->kind = TOK_ATOM;
      tokenchar(parse, 0);
    }

    /* illegal character */

    else {
      PARSE_ERR(parse);
      fprintf(stderr, "*   Unrecognized character: ascii %d, character '%c'\n",
	      (int) c, (unsigned char) c);
    }

    last_token->str = createStrn(parse->ts, parse->tpos);

    last_token->next = newToken();
    last_token = last_token->next;
    last_token->kind = TOK_END;
    tokencount++;

  } while(nest);

  /* mark ending file, line, and column of item */
  last_token->colsnum = (parse->snum & 0xFFFF) | ((parse->col + parse->pos) << 16);
  last_token->line = parse->line;

  /* skip over white space or comment until newline */

  do {
    c = nextchar(parse);
    while(c == COMMENT_CHAR) c = skipComment(parse);
  } while(c && c != NEWLINE_CHAR && isspace(c));

  if(!isspace(c)) backchar(parse);

  *token = first_token;  /* returning first token in list */
  return tokencount;     /* and total number of tokens in list */
}

/*-------------------------------------------------------------*/
void printItem(Script *script, Value *port, Token *token)
{
  /* Print a token list on the specified output file */
  
  while(token != NULL) {
    switch(token->kind) {

    case TOK_UNUSED:
      printStrn(script, port, "UNUSED\n", 7);
      break;

    case TOK_STRING:
      printStrn(script, port, "STRING = [", 10);
      printStr(script, port, token->str);
      printStrn(script, port, "]\n", 2);
      break;

    case TOK_ATOM:
      printStrn(script, port, "ATOM = [", 8);
      printStr(script, port, token->str);
      printStrn(script, port, "]\n", 2);
      break;

    case TOK_INT:
      printStrn(script, port, "INT = [", 7);
      printStr(script, port, token->str);
      printStrn(script, port, "]\n", 2);
      break;

    case TOK_REAL:
      printStrn(script, port, "REAL = [", 8);
      printStr(script, port, token->str);
      printStrn(script, port, "]\n", 2);
      break;

    case TOK_OPEN_PAREN:
      printStrn(script, port, "OPEN_PAREN\n", 11);
      break;

    case TOK_OPEN_LIST:
      printStrn(script, port, "OPEN_LIST\n", 10);
      break;

    case TOK_CLOSE:
      printStrn(script, port, "CLOSE\n", 6);
      break;

    case TOK_CHAR:
      printStrn(script, port, "CHAR = [", 8);
      printStr(script, port, token->str);
      printStrn(script, port, "]\n", 2);
      break;

    case TOK_ADDR:
      printStrn(script, port, "ADDR = [", 8);
      printStr(script, port, token->str);
      printStrn(script, port, "]\n", 2);
      break;

    case TOK_BINARY:
      printStrn(script, port, "BINARY = [", 10);
      printStr(script, port, token->str);
      printStrn(script, port, "]\n", 2);
      break;

    case TOK_END:
      printStrn(script, port, "END\n", 4);
      break;
    }
    token = token->next;
  }
}

/*-------------------------------------------------------------*/
void freeItem(Token *token)
{
  Token *next_token, *this_token;

  this_token = token;

  while(this_token != NULL) {
    next_token = this_token->next;
    freeStr(this_token->str);
    mem_freeItem(Token_mem, this_token);
    this_token = next_token;
  }
}

/*-------------------------------------------------------------*/
int allowedSymbol(char c)
{
  int i = 0;

  while(allowed_symbols[i] != 0) if(c == allowed_symbols[i++]) return 1;

  return 0;  /* illegal symbol */
}

/*-------------------------------------------------------------*/
char controlChar(char c)
{
  switch(c) {

  case '\\':
    return '\\';

  case 'a':
    return '\a';
    
  case 'b':
    return '\b';
    
  case 'f':
    return '\f';
    
  case 'n':
    return '\n';
    
  case 'r':
    return '\r';
    
  case 't':
    return '\t';
    
  case 'v':
    return '\v';
    
  case '"':
    return '"';
    
  case '\'':
    return '\'';

  default:
    return 0;

  }
}    
