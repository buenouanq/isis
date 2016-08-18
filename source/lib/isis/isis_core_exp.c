/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* isis_core_exp.c */

/* This file contains functions to form and manipulate expressions. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "isis.h"

/* The following arrays define which functions are called for
   printing, evaluating, and freeing each type of expression.  The
   corresponding functions must be specified in the same order as in
   the exp_kind enumeration in isis.h */

void (*printExpFuncs[])(Script *script, Value *, Expression *, int) = { 
  printInvalidExpression, 
  printNullExpression, 
  printConstExpression,
  printVarExpression,
  printProcExpression,
  printAppExpression,
  printListExpression,
  printIfExpression,
  printCondExpression,
  printSwitchExpression,
  printBeginExpression,
  printLogicExpression,
  printSetExpression,
  printLocalExpression,
  printWhileExpression,
  printVaropExpression };

void (*evalExpFuncs[])(Script *, Environment *, Expression *, Value **) = { 
  evalInvalidExpression, 
  evalNullExpression, 
  evalConstExpression,
  evalVarExpression,
  evalProcExpression,
  evalAppExpression,
  evalListExpression,
  evalIfExpression,
  evalCondExpression,
  evalSwitchExpression,
  evalBeginExpression,
  evalLogicExpression,
  evalSetExpression,
  evalLocalExpression,
  evalWhileExpression,
  evalVaropExpression };

void (*freeExpFuncs[])(Expression *) = { 
  freeInvalidExpression, 
  freeNullExpression, 
  freeConstExpression,
  freeVarExpression,
  freeProcExpression,
  freeAppExpression,
  freeListExpression,
  freeIfExpression,
  freeCondExpression,
  freeSwitchExpression,
  freeBeginExpression,
  freeLogicExpression,
  freeSetExpression,
  freeLocalExpression,
  freeWhileExpression,
  freeVaropExpression };

/* The following macros are used only in this file, for nice error messages */

#define TOKEN_ERR(token) fprintf(stderr, "* Error in %s, line %d, col %d:\n", lookupString(token->colsnum & 0xFFFF), token->line, (token->colsnum >> 16));

#define DEBUG_ERR(exp) fprintf(stderr, "*   Expression located in %s, line %d, col %d.\n", lookupString((exp->nullexp.colsnum & 0xFFFF)), exp->nullexp.line, (exp->nullexp.colsnum >> 16));

/*-------------------------------------------------------------*/
int formExpression(Script *script, Environment *env, 
		   Token **token, Expression **exp)
{
  /* Parses an expression at the place pointed to by *token, and
     returns the pointer to the result in *exp, and resets *token to
     the token following the last token used in the expression.
     Returns 0 if an error occured, 1 if successful */
  
  Token *curtoken;
  Expression *newexp = NullExpression;
  Value *newval = NullValue;
  char *tempstr;
  int i, num_exps, nonconst;
  unsigned int startline, colsnum, tempnum;
  ExpList *explist, *explist2;
  ExpNode *expnode, *expnode2;
  Expression *tempexp;
  Binding *binding;
  
  curtoken = *token;
  
  if(curtoken == NULL) {
    fprintf(stderr, 
	    "* Unable to form expression from NULL token.\n");
    *exp = NullExpression;
    return 0;
  }
  
  startline = curtoken->line;
  colsnum = curtoken->colsnum;
  
  switch(curtoken->kind) {
    
  case TOK_END:      /* 'End' token--should never be seen */
    TOKEN_ERR(curtoken);
    fprintf(stderr, 
	    "*   Unexpected end of item.\n");
    *exp = NullExpression;
    return 0;
    
  case TOK_INT:
    newexp = newExpression(E_CONST);
    newexp->constexp.val = newInt(atoi(curtoken->str));
    getnext(curtoken);
    break;
    
  case TOK_BINARY:
    tempnum = 0;
    for(i = 2; curtoken->str[i] != 0; i++) {
      tempnum = tempnum << 1;
      if(curtoken->str[i] == '1') tempnum += 1;
    }
    newexp = newExpression(E_CONST);
    newexp->constexp.val = newInt(tempnum);
    getnext(curtoken);
    break;
    
  case TOK_REAL:
    newexp = newExpression(E_CONST);
    newexp->constexp.val = newReal(atof(curtoken->str));
    getnext(curtoken);
    break;
    
  case TOK_ADDR:
    newexp = newExpression(E_CONST);
    newexp->constexp.val = newAddr((void *)strtoul(curtoken->str, NULL, 0));
    getnext(curtoken);
    break;
    
  case TOK_CHAR:
    newexp = newExpression(E_CONST);
    newexp->constexp.val = newChar(curtoken->str[0]);
    getnext(curtoken);
    break;
    
  case TOK_STRING:
    newexp = newExpression(E_CONST);
    newexp->constexp.val = newString(strlen(curtoken->str), curtoken->str);
    getnext(curtoken);
    break;
    
  case TOK_ATOM:
    if(strcmp(curtoken->str, FALSE_STR) == 0) {
      newexp = newExpression(E_CONST);
      newexp->constexp.val = newBool(0);
    }
    else if(strcmp(curtoken->str, TRUE_STR) == 0) {
      newexp = newExpression(E_CONST);
      newexp->constexp.val = newBool(1);
    }
    else if(strcmp(curtoken->str, NULLVAL_STR) == 0) {
      newexp = newExpression(E_CONST);
      newexp->constexp.val = NullValue;
    }
    
    /* VARIABLE REFERENCE */
    
    else {
      newexp = newExpression(E_VAR);
      newexp->varexp.binding = NULL;
      binding = searchEnvironment(curtoken->str, env, 
				  &(newexp->varexp.fnum),
				  &(newexp->varexp.bnum));
      if(binding) {   /* found var in environment */
	newexp->varexp.vname = referVarname(binding->vname);
	if(newexp->varexp.bnum < 0) newexp->varexp.binding = binding;  /* at top level */
      }
      else {          /* var not found */
	newexp->varexp.vname = newVarname(curtoken->str);
	curtoken->str = NULL;
      }
    }
    getnext(curtoken);
    break;
    
    /* LIST FORMATION */
    
  case TOK_OPEN_LIST:
    getnext(curtoken);
    num_exps = 0;
    explist = newExpList();
    nonconst = 0;
    while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
      formExpression(script, env, &curtoken, &tempexp);
      if(tempexp != NullExpression && 
	 tempexp->nullexp.kind != E_CONST) nonconst = 1;
      insertExpression(tempexp, explist);
      num_exps++;
    }
    getnext(curtoken);
    if(nonconst) {
      newexp = newExpression(E_LIST);
      newexp->listexp.num_exps = num_exps;
      newexp->listexp.exps = (Expression **)newPtrArray(num_exps);
      expnode = explist->head;
      for(i = 0; i < num_exps; i++) {
	newexp->listexp.exps[i] = (Expression *)expnode->item;
	expnode = expnode->next;
      }
    }
    else {   /* All list items were constants, so make the list a constant */
      newexp = newExpression(E_CONST);
      newval = newList(num_exps, NULL);
      expnode = explist->head;
      for(i = 0; i < num_exps; i++) {
	Listitem(newval, i) = referValue(((Expression *)expnode->item)->constexp.val);
	freeExpression((Expression *)expnode->item);
	expnode = expnode->next;
      }
      newexp->constexp.val = newval;
    }
    freeExpList(explist);
    break;
    
  case TOK_OPEN_PAREN:
    getnext(curtoken);
    if(curtoken->kind == TOK_ATOM) {
      
      /* BOUND? EXPRESSION */
      
      if(strcmp(curtoken->str, BOUND_STR) == 0) {
	getnext(curtoken);
	if(curtoken->kind != TOK_ATOM) {
	  TOKEN_ERR(curtoken);
	  fprintf(stderr, 
		  "*   Error in variable name.\n");
	}
	else {
	  newexp = newExpression(E_VAROP);
	  newexp->varopexp.op_kind = V_BOUND;
	  newexp->varopexp.binding = NULL;
	  binding = searchEnvironment(curtoken->str, env, 
				      &(newexp->varopexp.fnum),
				      &(newexp->varopexp.bnum));
	  if(binding) {   /* found var in environment */
	    newexp->varopexp.vname = referVarname(binding->vname);
	    if(newexp->varopexp.bnum < 0) newexp->varopexp.binding = binding;  /* at top level */
	  }
	  else {          /* var not found */
	    newexp->varopexp.vname = newVarname(curtoken->str);
	    curtoken->str = NULL;
	  }
	  getnext(curtoken);
	  if(curtoken->kind != TOK_CLOSE) {
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Expected close paren in bound? expression.\n");
	    while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) 
	      getnext(curtoken);
	  }
	  getnext(curtoken);
	}
      }
      
      /* BEGIN EXPRESSION */
      
      else if(strcmp(curtoken->str, BEGIN_STR) == 0) {
	getnext(curtoken);
	num_exps = 0;
	explist = newExpList();
	while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	  formExpression(script, env, &curtoken, &tempexp);
	  insertExpression(tempexp, explist);
	  num_exps++;
	}
	getnext(curtoken);
	newexp = newExpression(E_BEGIN);
	newexp->beginexp.num_exps = num_exps;
	newexp->beginexp.exps = (Expression **)newPtrArray(num_exps);
	expnode = explist->head;
	for(i = 0; i < num_exps; i++) {
	  newexp->beginexp.exps[i] = (Expression *)expnode->item;
	  expnode = expnode->next;
	}
	freeExpList(explist);
      }
      
      /* LOGIC EXPRESSION */
      
      else if((strcmp(curtoken->str, AND_STR) == 0) ||
	      (strcmp(curtoken->str, OR_STR) == 0) ||
	      (strcmp(curtoken->str, NAND_STR) == 0) ||
	      (strcmp(curtoken->str, NOR_STR) == 0)) {
	/* Determine logic operator */
	if(curtoken->str[0] == 'a') i = L_AND;
	else if(curtoken->str[0] == 'o') i = L_OR;
	else if(curtoken->str[1] == 'a') i = L_NAND;
	else i = L_NOR;
	getnext(curtoken);
	num_exps = 0;
	explist = newExpList();
	while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	  formExpression(script, env, &curtoken, &tempexp);
	  insertExpression(tempexp, explist);
	  num_exps++;
	}
	getnext(curtoken);
	newexp = newExpression(E_LOGIC);
	newexp->logicexp.logic_kind = i;
	newexp->logicexp.num_exps = num_exps;
	newexp->logicexp.exps = (Expression **)newPtrArray(num_exps);
	expnode = explist->head;
	for(i = 0; i < num_exps; i++) {
	  newexp->logicexp.exps[i] = (Expression *)expnode->item;
	  expnode = expnode->next;
	}
	freeExpList(explist);
      }
      
      /* IF EXPRESSION */
      
      else if(strcmp(curtoken->str, IF_STR) == 0) {
	newexp = newExpression(E_IF);
	newexp->ifexp.ifpart = NullExpression;
	newexp->ifexp.thenpart = NullExpression;
	newexp->ifexp.elsepart = NullExpression;
	getnext(curtoken);
	if(curtoken->kind == TOK_CLOSE) {
	  TOKEN_ERR(curtoken);
	  fprintf(stderr, 
		  "*   Unexpected close paren in if expression.\n");
	}
	else {
	  formExpression(script, env, &curtoken, &(newexp->ifexp.ifpart));
	  if(curtoken->kind == TOK_CLOSE) {
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Unexpected close paren in if expression.\n");
	  }
	  else {
	    formExpression(script, env, &curtoken, &(newexp->ifexp.thenpart));
	    if(curtoken->kind == TOK_CLOSE) {
	      getnext(curtoken);   /* To allow omission of 'else' part */
	    }
	    else {
	      formExpression(script, env, &curtoken, 
			     &(newexp->ifexp.elsepart));
	      if(curtoken->kind != TOK_CLOSE) {
		TOKEN_ERR(curtoken);
		fprintf(stderr, 
			"*   Expected close paren in if expression.\n");
	      }
	      else getnext(curtoken);
	    }
	  }
	}
      }
      
      /* WHILE EXPRESSION */
      
      else if(strcmp(curtoken->str, WHILE_STR) == 0) {
	newexp = newExpression(E_WHILE);
	newexp->whileexp.cond = NullExpression;
	newexp->whileexp.body = NullExpression;
	getnext(curtoken);
	if(curtoken->kind == TOK_CLOSE) {
	  TOKEN_ERR(curtoken);
	  fprintf(stderr, 
		  "*   Unexpected close paren in while expression.\n");
	}
	else {
	  formExpression(script, env, &curtoken, &(newexp->whileexp.cond));
	  if(curtoken->kind == TOK_CLOSE) {
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Unexpected close paren in while expression.\n");
	  }
	  else {
	    formExpression(script, env, &curtoken, &(newexp->whileexp.body));
	    if(curtoken->kind != TOK_CLOSE) {
	      TOKEN_ERR(curtoken);
	      fprintf(stderr, 
		      "*   Expected close paren in while expression.\n");
	    }
	    else getnext(curtoken);
	  }
	}
      }
      
      /* COND EXPRESSION */
      
      else if(strcmp(curtoken->str, COND_STR) == 0) {
	getnext(curtoken);
	explist = newExpList();
	explist2 = newExpList();
	num_exps = 0;
	while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	  if(curtoken->kind != TOK_OPEN_PAREN) { 
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Expected open paren for cond case.\n");
	  }
	  getnext(curtoken);
	  formExpression(script, env, &curtoken, &tempexp);
	  insertExpression(tempexp, explist);
	  if(curtoken->kind == TOK_CLOSE) { 
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Unexpected close paren in cond case.\n");
	  }
	  formExpression(script, env, &curtoken, &tempexp);
	  insertExpression(tempexp, explist2);
	  if(curtoken->kind != TOK_CLOSE) { 
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Expected close paren in cond case.\n");
	  }
	  getnext(curtoken);
	  num_exps++;
	}
	if(curtoken->kind != TOK_CLOSE) {
	  TOKEN_ERR(curtoken);
	  fprintf(stderr, 
		  "*   Expected close paren for cond expression.\n");
	}
	else getnext(curtoken);
	newexp = newExpression(E_COND);
	newexp->condexp.num_conds = num_exps;
	newexp->condexp.conds = (Expression **)newPtrArray(num_exps);
	newexp->condexp.vals = (Expression **)newPtrArray(num_exps);
	expnode = explist->head;
	expnode2 = explist2->head;
	for(i = 0; i < num_exps; i++) {
	  newexp->condexp.conds[i] = (Expression *)expnode->item;
	  newexp->condexp.vals[i] = (Expression *)expnode2->item;
	  expnode = expnode->next;
	  expnode2 = expnode2->next;
	}
	freeExpList(explist);
	freeExpList(explist2);
      }
      
      /* SWITCH EXPRESSION */
      
      else if(strcmp(curtoken->str, SWITCH_STR) == 0) {
	newexp = newExpression(E_SWITCH);
	newexp->switchexp.num_cases = 0;
	newexp->switchexp.last_default = 0;
	newexp->switchexp.switchexp = NullExpression;
	newexp->switchexp.cases = NULL;
	newexp->switchexp.vals = NULL;
	getnext(curtoken);
	if(curtoken->kind == TOK_CLOSE) {
	  TOKEN_ERR(curtoken);
	  fprintf(stderr,
		  "*   Unexpected close paren in switch expression.\n");
	}
	else {
	  formExpression(script, env, &curtoken, 
			 &(newexp->switchexp.switchexp));
	  explist = newExpList();
	  explist2 = newExpList();
	  num_exps = 0;
	  while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	    if(curtoken->kind != TOK_OPEN_PAREN) { 
	      TOKEN_ERR(curtoken);
	      fprintf(stderr, 
		      "*   Expected open paren for switch case.\n");
	    }
	    getnext(curtoken);
	    if(curtoken->kind == TOK_ATOM && 
	       (strcmp(curtoken->str, ELSE_STR) == 0)) {
	      getnext(curtoken);
	      newexp->switchexp.last_default = 1;
	      tempexp = NullExpression;
	    }
	    else formExpression(script, env, &curtoken, &tempexp);
	    insertExpression(tempexp, explist);
	    if(curtoken->kind == TOK_CLOSE) {
	      TOKEN_ERR(curtoken);
	      fprintf(stderr, 
		      "*   Unexpected close paren in switch case.\n");
	    }
	    formExpression(script, env, &curtoken, &tempexp);
	    insertExpression(tempexp, explist2);
	    if(curtoken->kind != TOK_CLOSE) { 
	      TOKEN_ERR(curtoken);
	      fprintf(stderr, 
		      "*   Expected close paren in switch case.\n");
	    }
	    getnext(curtoken);
	    num_exps++;
	  }
	  if(curtoken->kind != TOK_CLOSE) {
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Expected close paren at end of switch expr.\n");
	  }
	  else getnext(curtoken);
	  newexp->switchexp.num_cases = num_exps;
	  newexp->switchexp.cases = (Expression **)newPtrArray(num_exps);
	  newexp->switchexp.vals = (Expression **)newPtrArray(num_exps);
	  expnode = explist->head;
	  expnode2 = explist2->head;
	  for(i = 0; i < num_exps; i++) {
	    newexp->switchexp.cases[i] = (Expression *)expnode->item;
	    newexp->switchexp.vals[i] = (Expression *)expnode2->item;
	    expnode = expnode->next;
	    expnode2 = expnode2->next;
	  }
	  freeExpList(explist);
	  freeExpList(explist2);
	}
      }

      /* VARIABLE SET */
      
      else if(strcmp(curtoken->str, SET_STR) == 0) {
	getnext(curtoken);
	if(curtoken->kind != TOK_ATOM) {
	  TOKEN_ERR(curtoken);
	  fprintf(stderr, 
		  "*   Error in variable name.\n");
	}
	else {
	  newexp = newExpression(E_SET);
	  newexp->setexp.binding = NULL;
	  binding = searchEnvironment(curtoken->str, env, 
				      &(newexp->setexp.fnum),
				      &(newexp->setexp.bnum));
	  if(binding) {   /* found var in environment */
	    newexp->setexp.vname = referVarname(binding->vname);
	    if(newexp->setexp.bnum < 0) newexp->setexp.binding = binding;  /* at top level */
	  }
	  else {          /* var not found */
	    newexp->setexp.vname = newVarname(curtoken->str);
	    curtoken->str = NULL;
	  }
	  newexp->setexp.exp = NullExpression;
	  getnext(curtoken);
	  if(curtoken->kind == TOK_CLOSE) {
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Unexpected close paren in set.\n");
	  }
	  else {
	    formExpression(script, env, &curtoken, &newexp->setexp.exp);
	    if(curtoken->kind != TOK_CLOSE) {
	      TOKEN_ERR(curtoken);
	      fprintf(stderr, 
		      "*   Expected close paren in set expression.\n");
	      while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) 
		getnext(curtoken);
	    }
	    getnext(curtoken);
	  }
	}
      }

      /* LOCAL BLOCK */

      else if(strcmp(curtoken->str, LOCAL_STR) == 0) {
	newexp = newExpression(E_LOCAL);
	newexp->localexp.num_vars = 0;
	newexp->localexp.vnames = NULL;
	newexp->localexp.bodyexp = NullExpression;
	getnext(curtoken);
	if(curtoken->kind != TOK_OPEN_PAREN) {
	  TOKEN_ERR(curtoken);
	  fprintf(stderr, 
		  "*   Expected open paren for local variable list.\n");
	}
	else {
	  getnext(curtoken);
	  explist = newExpList();
	  while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	    if(curtoken->kind == TOK_ATOM) {
	      tempstr = curtoken->str;
	      curtoken->str = NULL;
	      insertExpression(tempstr, explist);
	      newexp->localexp.num_vars += 1;
	    }
	    else {
	      TOKEN_ERR(curtoken);
	      fprintf(stderr, "*   Expected local variable name.\n");
	    }
	    getnext(curtoken);
	  }
	  getnext(curtoken);
	  newexp->localexp.vnames = 
	    (Varname **)newPtrArray(newexp->localexp.num_vars);
	  expnode = explist->head;
	  for(i = 0; i < newexp->localexp.num_vars; i++) {
	    newexp->localexp.vnames[i] = newVarname((char *)expnode->item);
	    expnode = expnode->next;
	  }
	  env = arrayEnvironment(env, newexp->localexp.num_vars);
	  for(i = 0; i < newexp->localexp.num_vars; i++)
	    arrayBind(env, i, newexp->localexp.vnames[i], NullValue);
	  formExpression(script, env, &curtoken, &(newexp->localexp.bodyexp));
	  env = freeEnvironment(env);
	  if(curtoken->kind != TOK_CLOSE) {
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Expected close paren after local body.\n");
	  }
	  else getnext(curtoken);
	  freeExpList(explist);
	}
      }

      /* PROCEDURE DEFINITION */

      else if(strcmp(curtoken->str, PROC_STR) == 0) { 
	newexp = newExpression(E_PROC);
	newexp->procexp.num_formals = 0;
	newexp->procexp.var_num_args = 0;
	newexp->procexp.formals = NULL;
	newexp->procexp.bodyexp = NullExpression;
	getnext(curtoken);
	if(curtoken->kind != TOK_OPEN_PAREN && curtoken->kind != TOK_ATOM) {
	  TOKEN_ERR(curtoken);
          fprintf(stderr, 
		  "*   Expected variable declarations in proc def.\n");
	}
	else {
	  if(curtoken->kind == TOK_OPEN_PAREN) {
	    getnext(curtoken);
	    explist = newExpList();
	    while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	      if(curtoken->kind != TOK_ATOM) {
		TOKEN_ERR(curtoken);
		fprintf(stderr, "*   Expected argument name in proc def\n");
	      }
	      else {
		tempstr = curtoken->str;
		curtoken->str = NULL;
		insertExpression(tempstr, explist);
		newexp->procexp.num_formals += 1;
	      }
	      getnext(curtoken);
	    }
	    getnext(curtoken);
	    newexp->procexp.formals = 
	      (Varname **)newPtrArray(newexp->procexp.num_formals);
	    expnode = explist->head;
	    for(i = 0; i < newexp->procexp.num_formals; i++) {
	      newexp->procexp.formals[i] = newVarname((char *)expnode->item);
	      expnode = expnode->next;
	    }
	    env = arrayEnvironment(env, newexp->procexp.num_formals);
	    for(i = 0; i < newexp->procexp.num_formals; i++)
	      arrayBind(env, i, newexp->procexp.formals[i], NullValue);
	    freeExpList(explist);
	  }
	  else {  /* Token was an atom--means variable number of arguments */
	    newexp->procexp.var_num_args = 1;
	    newexp->procexp.formals = (Varname **) newPtrArray(1);
	    newexp->procexp.formals[0] = newVarname(curtoken->str);
	    curtoken->str = NULL;
	    newexp->procexp.num_formals = 1;
	    getnext(curtoken);
	    env = arrayEnvironment(env, 1);
	    arrayBind(env, 0, newexp->procexp.formals[0], NullValue);
	  }
	  formExpression(script, env, &curtoken, &(newexp->procexp.bodyexp));
	  env = freeEnvironment(env);
	  if(curtoken->kind != TOK_CLOSE) {
	    TOKEN_ERR(curtoken);
	    fprintf(stderr, 
		    "*   Expected close paren after procedure body.\n");
	  }
	  else getnext(curtoken);
	}
      }

      /* NAMED APPLICATION */
      
      else {
	num_exps = 0;
	explist = newExpList();
	while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	  formExpression(script, env, &curtoken, &tempexp);
	  insertExpression(tempexp, explist);
	  num_exps++;
	}
	getnext(curtoken);
	newexp = newExpression(E_APP);
	newexp->appexp.num_exps = num_exps;
	newexp->appexp.exps = (Expression **)newPtrArray(num_exps);
	expnode = explist->head;
	for(i = 0; i < num_exps; i++) {
	  newexp->appexp.exps[i] = (Expression *)expnode->item;
	  expnode = expnode->next;
	}
	freeExpList(explist);
      }
    } /* End of 'if first token is an atom' */

    else { 

      /* UNNAMED APPLICATION */

      num_exps = 0;
      explist = newExpList();
      while(curtoken->kind != TOK_CLOSE && curtoken->kind != TOK_END) {
	formExpression(script, env, &curtoken, &tempexp);
	insertExpression(tempexp, explist);
	num_exps++;
      }
      getnext(curtoken);
      newexp = newExpression(E_APP);
      newexp->appexp.num_exps = num_exps;
      newexp->appexp.exps = (Expression **)newPtrArray(num_exps);
      expnode = explist->head;
      for(i = 0; i < num_exps; i++) {
	newexp->appexp.exps[i] = (Expression *)expnode->item;
	expnode = expnode->next;
      }
      freeExpList(explist);

    }
    break;  /* End of 'token was an open paren' case */

  case TOK_CLOSE:
    /* Unexpected close paren -- error message already printed by parser */
    getnext(curtoken);
    break;

  default:
    TOKEN_ERR(curtoken);
    fprintf(stderr, "*   Invalid token %d while forming expr.\n", 
	    curtoken->kind);
    getnext(curtoken);
    break;
  }
  
  if(newexp != NullExpression) {
    newexp->nullexp.line = startline;
    newexp->nullexp.colsnum = colsnum;
  }

  *token = curtoken;
  *exp = newexp;  
  return 1;  /* success */
}

/* ************************************************************************ */
/* The print*Expression functions print the internal form of the
   expression.  Should NOT put a newline before leaving.  Should
   ALWAYS put a newline before calling */

/*-------------------------------------------------------------*/
void printInvalidExpression(Script *script, Value *port, 
			    Expression *exp, int indent)
{
  printSpace(script, port, indent);
  printStrn(script, port, "INVALID\n", 8);
}

/*-------------------------------------------------------------*/
void printNullExpression(Script *script, Value *port, 
			 Expression *exp, int indent)
{
  printSpace(script, port, indent);
  printStrn(script, port, "NULL\n", 5);
}

/*-------------------------------------------------------------*/
void printConstExpression(Script *script, Value *port, 
			  Expression *exp, int indent)
{
  printSpace(script, port, indent);
  printStrn(script, port, "CONST ", 6);
  printValue(script, port, exp->constexp.val);
  printStrn(script, port, "\n", 1);
}

/*-------------------------------------------------------------*/
void printVaropExpression(Script *script, Value *port, 
			  Expression *exp, int indent)
{
  printSpace(script, port, indent);
  switch(exp->varopexp.op_kind) {
  case V_BOUND:
    printStrn(script, port, "BOUND? ", 7);
    printStr(script, port, exp->varopexp.vname->str);
    printStrn(script, port, "\n", 1);
    break;
  }
}

/*-------------------------------------------------------------*/
void printVarExpression(Script *script, Value *port, 
			Expression *exp, int indent)
{
  printSpace(script, port, indent);
  printStrn(script, port, "VAR ", 4);
  printStr(script, port, exp->varexp.vname->str);
  printStrn(script, port, "\n", 1);
}

/*-------------------------------------------------------------*/
void printProcExpression(Script *script, Value *port, 
			 Expression *exp, int indent)
{
  int i;

  printSpace(script, port, indent);
  printStrn(script, port, "PROC ", 5);
  if(exp->procexp.var_num_args) {
    printStr(script, port, exp->procexp.formals[0]->str);
    printStrn(script, port, "\n", 1);
  }
  else {
    printStrn(script, port, "( ", 2);
    for(i = 0; i < exp->procexp.num_formals; i++) {
      printStr(script, port, exp->procexp.formals[i]->str);
      printStrn(script, port, " ", 1);
    }
    printStrn(script, port, ")\n", 2);
  }
  printExpression(script, port, exp->procexp.bodyexp, indent + 2);
}

/*-------------------------------------------------------------*/
void printLocalExpression(Script *script, Value *port, 
			  Expression *exp, int indent)
{
  int i;

  printSpace(script, port, indent);
  printStrn(script, port, "LOCAL ( ", 8);
  for(i = 0; i < exp->localexp.num_vars; i++) {
    printStr(script, port, exp->localexp.vnames[i]->str);
    printStrn(script, port, " ", 1);
  }
  printStrn(script, port, ")\n", 2);
  printExpression(script, port, exp->localexp.bodyexp, indent + 2);
}

/*-------------------------------------------------------------*/
void printAppExpression(Script *script, Value *port, 
			Expression *exp, int indent)
{
  int i;

  printSpace(script, port, indent);
  printStrn(script, port, "APP\n", 4);
  for(i = 0; i < exp->appexp.num_exps; i++)
    printExpression(script, port, exp->appexp.exps[i], indent + 2);
}

/*-------------------------------------------------------------*/
void printListExpression(Script *script, Value *port, 
			 Expression *exp, int indent)
{
  int i;

  printSpace(script, port, indent);
  printStrn(script, port, "LIST\n", 5);
  for(i = 0; i < exp->listexp.num_exps; i++)
    printExpression(script, port, exp->listexp.exps[i], indent + 2);
}

/*-------------------------------------------------------------*/
void printIfExpression(Script *script, Value *port, 
		       Expression *exp, int indent)
{
  printSpace(script, port, indent);
  printStrn(script, port, "IF\n", 3);
  printExpression(script, port, exp->ifexp.ifpart, indent + 2);
  printExpression(script, port, exp->ifexp.thenpart, indent + 2);
  printExpression(script, port, exp->ifexp.elsepart, indent + 2);
}

/*-------------------------------------------------------------*/
void printWhileExpression(Script *script, Value *port, 
			  Expression *exp, int indent)
{
  printSpace(script, port, indent);
  printStrn(script, port, "WHILE\n", 6);
  printExpression(script, port, exp->whileexp.cond, indent + 2);
  printExpression(script, port, exp->whileexp.body, indent + 2);
}

/*-------------------------------------------------------------*/
void printCondExpression(Script *script, Value *port, 
			 Expression *exp, int indent)
{
  int i;
  
  printSpace(script, port, indent);
  printStrn(script, port, "COND\n", 5);
  for(i = 0; i < exp->condexp.num_conds; i++) {
    printExpression(script, port, exp->condexp.conds[i], indent + 1);
    printExpression(script, port, exp->condexp.vals[i], indent + 2);
  }
}

/*-------------------------------------------------------------*/
void printSwitchExpression(Script *script, Value *port, 
			   Expression *exp, int indent)
{
  int i;

  printSpace(script, port, indent);
  printStrn(script, port, "SWITCH\n", 7);
  printExpression(script, port, exp->switchexp.switchexp, indent + 1);
  for(i = 0; i < exp->switchexp.num_cases; i++) {
    if(exp->switchexp.last_default && i == exp->switchexp.num_cases - 1) {
      printSpace(script, port, indent + 2);
      printStrn(script, port, "ELSE\n", 5);
    }
    else printExpression(script, port, exp->switchexp.cases[i], indent + 2);
    printExpression(script, port, exp->switchexp.vals[i], indent + 3);
  }
}

/*-------------------------------------------------------------*/
void printBeginExpression(Script *script, Value *port, 
			  Expression *exp, int indent)
{
  int i;

  printSpace(script, port, indent);
  printStrn(script, port, "BEGIN\n", 6);
  for(i = 0; i < exp->beginexp.num_exps; i++)
    printExpression(script, port, exp->beginexp.exps[i], indent + 2);
}

/*-------------------------------------------------------------*/
void printLogicExpression(Script *script, Value *port, 
			  Expression *exp, int indent)
{
  int i;

  printSpace(script, port, indent);
  printStr(script, port, (exp->logicexp.logic_kind == L_OR ? "OR\n" :
		  (exp->logicexp.logic_kind == L_AND ? "AND\n" :
		   (exp->logicexp.logic_kind == L_NOR ? "NOR\n" : "NAND\n"))));
  for(i = 0; i < exp->logicexp.num_exps; i++)
    printExpression(script, port, exp->logicexp.exps[i], indent + 2);
}

/*-------------------------------------------------------------*/
void printSetExpression(Script *script, Value *port, 
			Expression *exp, int indent)
{
  printSpace(script, port, indent);
  printStrn(script, port, "SET ", 4);
  printStr(script, port, exp->setexp.vname->str);
  printStrn(script, port, "\n", 1);
  printExpression(script, port, exp->setexp.exp, indent + 2);
}

/* ************************************************************************ */
/* The eval*Expression functions evaluate the expression, in the scope
   of the given script and environment, and returns the pointer to the
   value through 'val'. */

/*-------------------------------------------------------------*/
void evalInvalidExpression(Script *script, Environment *env, 
			   Expression *exp, Value **val)
{
  /* if this was called, there was a problem, so print an error message */
  
  fprintf(stderr, "* Tried to evaluate an invalid expression.\n");
  
  *val = NullValue;
}

/*-------------------------------------------------------------*/
void evalNullExpression(Script *script, Environment *env, 
			Expression *exp, Value **val)
{
  *val = NullValue;
}

/*-------------------------------------------------------------*/
void evalConstExpression(Script *script, Environment *env, 
			 Expression *exp, Value **val)
{
  *val = referValue(exp->constexp.val);
}

/*-------------------------------------------------------------*/
void evalVaropExpression(Script *script, Environment *env, 
			 Expression *exp, Value **val)
{
  /* get binding and perform operation.  if searching top level, it is
     not necessary to lock the mutex, as varopexp.binding is updated
     atomically and a simultaneous call will produce the same result.
     however, a mutex lock is needed to update the vname field. */

  Binding *binding;

  *val = NullValue;

  /* first find the binding in the environment */

  if(exp->varopexp.binding)  /* exists at the top level */   
    binding = exp->varopexp.binding;
  else if(exp->varopexp.fnum >= 0)  /* exists at a different level */
    binding = lookupBinding(env, exp->varopexp.fnum, exp->varopexp.bnum);
  else exp->varopexp.binding = binding =   /* not resolved, search top level */
	 searchTop(exp->varopexp.vname->str, script->env);
  
  /* then perform the desired operation */

  switch(exp->varopexp.op_kind) {
      
  case V_BOUND:
    *val = newBool(binding != NULL);
    break;
    
  }
}

/*-------------------------------------------------------------*/
void evalVarExpression(Script *script, Environment *env, 
		       Expression *exp, Value **val)
{
  /* must lock the bind mutex, because another thread could be
     starting to set this variable, and it might be possible for us to
     grab the value pointer before calling referValue on it, then the
     other thread executes and resets it and frees the value, and then
     we start again and our value pointer would be invalid if there
     was only one reference to the value previously.  */

  Binding *binding;

  if(exp->varexp.binding) { /* exists at the top level */   
    isisMutexLock(bindMutex);
    *val = referValue(exp->varexp.binding->val);
    isisMutexUnlock(bindMutex);
  }
  else if(exp->varexp.fnum >= 0) {  /* exists at a different level */
    binding = lookupBinding(env, exp->varexp.fnum, exp->varexp.bnum);
    isisMutexLock(bindMutex);
    *val = referValue(binding->val);
    isisMutexUnlock(bindMutex);
  }
  else if(exp->varexp.binding =   /* not resolved, search top level */
	  searchTop(exp->varexp.vname->str, script->env)) {
    isisMutexLock(bindMutex);
    *val = referValue(exp->varexp.binding->val);    
    isisMutexUnlock(bindMutex);
  }
  else {  /* could not find at top level */
    *val = NullValue;
    fprintf(stderr, "* Variable '%s' has not been defined.\n", 
	    exp->varexp.vname->str);
    if(script->debug) DEBUG_ERR(exp);
  }
}

/*-------------------------------------------------------------*/
void evalProcExpression(Script *script, Environment *env, 
			Expression *exp, Value **val)
{
  Procedure *proc;
  
  proc = newProcedure(P_SCRIPT);
  proc->scriptfunc.num_formals = exp->procexp.num_formals;
  proc->scriptfunc.var_num_args = exp->procexp.var_num_args;
  proc->scriptfunc.formals = 
    copyVarnameArray(exp->procexp.formals, exp->procexp.num_formals);
  proc->scriptfunc.env = referEnvironment(env);
  proc->scriptfunc.body = referExpression(exp->procexp.bodyexp);

  *val = newProc(proc);   /* creates its own reference */
  freeProcedure(proc);
}

/*-------------------------------------------------------------*/
void evalLocalExpression(Script *script, Environment *env, 
			 Expression *exp, Value **val)
{
  int i;

  env = arrayEnvironment(env, exp->localexp.num_vars);
  for(i = 0; i < exp->localexp.num_vars; i++) 
    arrayBind(env, i, exp->localexp.vnames[i], NullValue);
  evalExpression(script, env, exp->localexp.bodyexp, val);
  env = freeEnvironment(env);
}

/*-------------------------------------------------------------*/
void evalAppExpression(Script *script, Environment *env, 
		       Expression *exp, Value **val)
{
  /* Takes care of procedure call and list index operation */

  Value *firstval, *indexval, *pokeval, **argv;
  Procedure *proc;
  int i, argc;

  *val = firstval = NullValue;
  
  if(exp->appexp.num_exps != 0) 
    evalExpression(script, env, exp->appexp.exps[0], &firstval);
  
  if(isProc(firstval)) {
    proc = getProc(firstval);
    argc = exp->appexp.num_exps - 1;
    /* Evaluate arguments and put into argv */
    argv = (Value **) newPtrArray(argc);
    for(i = 0; i < argc; i++)
      evalExpression(script, env, exp->appexp.exps[i + 1], &argv[i]);
    *val = applyProcedure(script, proc, argc, argv);
    for(i = 0; i < argc; i++) freeValue(argv[i]);
    freePtrArray((void **) argv);
  }
  else if(isList(firstval)) {
    if(exp->appexp.num_exps <= 1) {
      fprintf(stderr, "* No list index specified.\n");
      if(script->debug) DEBUG_ERR(exp);
    }
    else {
      evalExpression(script, env, exp->appexp.exps[1], &indexval);
      if(!isInt(indexval)) {
	fprintf(stderr, "* List index was not an integer.\n");
	if(script->debug) DEBUG_ERR(exp);
      }
      else if(getInt(indexval) < 0 || getInt(indexval) >= Listsize(firstval)) {
	fprintf(stderr, "* Index %d out of range.\n", getInt(indexval));
	if(script->debug) DEBUG_ERR(exp);
      }
      else *val = referValue(Listitem(firstval, getInt(indexval)));
      freeValue(indexval);
    }
  }
  else if(isAddr(firstval)) {
    if(exp->appexp.num_exps == 1) *val = memPeek(getAddr(firstval), CTYPE_UCHAR);
    else if(exp->appexp.num_exps == 2) {    /* PEEK operation */
      evalExpression(script, env, exp->appexp.exps[1], &indexval);
      if(isInt(indexval)) *val = memPeek(getAddr(firstval), getInt(indexval));
      else if(isIntList(indexval) && Listsize(indexval) >= 2) {
	int ctype, quant, offset;
	ctype = getInt(Listitem(indexval, 0));
	quant = getInt(Listitem(indexval, 1));
	offset = (Listsize(indexval) > 2) ? (getInt(Listitem(indexval, 2))) : 0;
	if(quant) *val = memPeekArray(((char *) getAddr(firstval)) + offset, ctype, quant);
	else *val = memPeek(((char *) getAddr(firstval)) + offset, ctype);
      }
      else {
	fprintf(stderr, "* Improper memory access.\n");
	if(script->debug) DEBUG_ERR(exp);
      }
      freeValue(indexval);
    }
    else if(exp->appexp.num_exps == 3) {    /* POKE OPERATION */
      evalExpression(script, env, exp->appexp.exps[1], &indexval);
      evalExpression(script, env, exp->appexp.exps[2], &pokeval);
      if(isInt(indexval)) {
	if(isList(pokeval)) *val = memPokeArray(getAddr(firstval), getInt(indexval), Listsize(pokeval), pokeval);
	else *val = memPoke(getAddr(firstval), getInt(indexval), pokeval);
      }
      else if(isIntList(indexval) && Listsize(indexval) >= 2) {
	int ctype, quant, offset;
	ctype = getInt(Listitem(indexval, 0));
	quant = 0;
	if(isList(pokeval) && !(quant = getInt(Listitem(indexval, 1)))) quant = Listsize(pokeval);
	offset = (Listsize(indexval) > 2) ? (getInt(Listitem(indexval, 2))) : 0;
	if(quant) *val = memPokeArray(((char *) getAddr(firstval)) + offset, ctype, quant, pokeval);
	else *val = memPoke(((char *) getAddr(firstval)) + offset, ctype, pokeval);
      }
      else {
	fprintf(stderr, "* Improper memory access.\n");
	if(script->debug) DEBUG_ERR(exp);
      }
      freeValue(indexval);
      freeValue(pokeval);
    }
    else {  /* should not be more than 3 expressions */
      fprintf(stderr, "* Improper memory access.\n");
      if(script->debug) DEBUG_ERR(exp);
    }
  }
  else {
    fprintf(stderr, "* Improper application.\n");
    if(script->debug) DEBUG_ERR(exp);
  }
  freeValue(firstval);
}

/*-------------------------------------------------------------*/
void evalListExpression(Script *script, Environment *env, 
			Expression *exp, Value **val)
{
  int i;

  *val = newList(exp->listexp.num_exps, NULL);
  for(i = 0; i < exp->listexp.num_exps; i++)
    evalExpression(script, env, exp->listexp.exps[i], &Listitem(*val, i));
}

/*-------------------------------------------------------------*/
void evalIfExpression(Script *script, Environment *env, 
		      Expression *exp, Value **val)
{
  Value *condval;

  evalExpression(script, env, exp->ifexp.ifpart, &condval);
  if(trueValue(condval)) evalExpression(script, env, exp->ifexp.thenpart, val);
  else evalExpression(script, env, exp->ifexp.elsepart, val);
  freeValue(condval);    
}

/*-------------------------------------------------------------*/
void evalWhileExpression(Script *script, Environment *env, 
			 Expression *exp, Value **val)
{
  Value *condval;

  *val = NullValue;
  evalExpression(script, env, exp->whileexp.cond, &condval);
  while(trueValue(condval)) {
    freeValue(condval);
    freeValue(*val);
    evalExpression(script, env, exp->whileexp.body, val);
    evalExpression(script, env, exp->whileexp.cond, &condval);
  }
  freeValue(condval);
}

/*-------------------------------------------------------------*/
void evalCondExpression(Script *script, Environment *env, 
			Expression *exp, Value **val)
{
  Value *condval;
  int i;

  *val = NullValue;
  for(i = 0; i < exp->condexp.num_conds; i++) {
    evalExpression(script, env, exp->condexp.conds[i], &condval);
    if(trueValue(condval)) {
      freeValue(condval);
      evalExpression(script, env, exp->condexp.vals[i], val);
      break;
    }
    else freeValue(condval);
  }
}

/*-------------------------------------------------------------*/
void evalSwitchExpression(Script *script, Environment *env, 
			  Expression *exp, Value **val)
{
  Value *switchval, *caseval;
  int i;

  *val = NullValue;
  evalExpression(script, env, exp->switchexp.switchexp, &switchval);
  for(i = 0; i < exp->switchexp.num_cases; i++) {
    evalExpression(script, env, exp->switchexp.cases[i], &caseval);
    if(equalValue(switchval, caseval)) {
      freeValue(caseval);
      evalExpression(script, env, exp->switchexp.vals[i], val);
      break;
    }
    else freeValue(caseval);
  }
  freeValue(switchval);

  if(i == exp->switchexp.num_cases && exp->switchexp.last_default)
    evalExpression(script, env, exp->switchexp.vals[i - 1], val);
}

/*-------------------------------------------------------------*/
void evalBeginExpression(Script *script, Environment *env, 
			 Expression *exp, Value **val)
{
  int i;

  *val = NullValue;
  for(i = 0; i < exp->beginexp.num_exps; i++) {
    freeValue(*val);
    evalExpression(script, env, exp->beginexp.exps[i], val);
  }
}

/*-------------------------------------------------------------*/
void evalLogicExpression(Script *script, Environment *env, 
			 Expression *exp, Value **val)
{
  int res, i;
  Value *clauseval;

  switch(exp->logicexp.logic_kind) {
      
  case L_AND:
  case L_NAND:
    res = 1;
    for(i = 0; res == 1 && i < exp->logicexp.num_exps; i++) {
      evalExpression(script, env, exp->logicexp.exps[i], &clauseval);
      if(!trueValue(clauseval)) res = 0; 
      freeValue(clauseval);
    }
    if(exp->logicexp.logic_kind == L_NAND) res = !res;
    *val = newBool(res);
    break;

  case L_OR:
  case L_NOR:
    res = (!exp->logicexp.num_exps);
    for(i = 0; res == 0 && i < exp->logicexp.num_exps; i++) {
      evalExpression(script, env, exp->logicexp.exps[i], &clauseval);
      if(trueValue(clauseval)) res = 1; 
      freeValue(clauseval);
    }
    if(exp->logicexp.logic_kind == L_NOR) res = !res;
    *val = newBool(res);
    break;
      
  default:
    fprintf(stderr, "* Bad logic operator kind %d.\n", 
	    exp->logicexp.logic_kind);
    *val = NullValue;
    break;

  }
}

/*-------------------------------------------------------------*/
void evalSetExpression(Script *script, Environment *env, 
		       Expression *exp, Value **val)
{
  /* set variable to new value, or define new variable at top level.
     must lock the bind mutex when searching the top level since
     another thread might be searching at the same time. */

  Binding *binding; 

  /* get the variable's value */
  
  evalExpression(script, env, exp->setexp.exp, val);

  /* find the binding in the current environment */

  if(exp->setexp.binding) { /* exists at the top level */   
    isisMutexLock(bindMutex);
    freeValue(exp->setexp.binding->val);   /* no other accesses to binding->val must occur */
    exp->setexp.binding->val = referValue(*val);
    isisMutexUnlock(bindMutex);
  }
  else if(exp->setexp.fnum >= 0) { /* exists at a different level */
    binding = lookupBinding(env, exp->setexp.fnum, exp->setexp.bnum);
    isisMutexLock(bindMutex);
    freeValue(binding->val);   /* no other accesses to binding->val must occur */
    binding->val = referValue(*val);
    isisMutexUnlock(bindMutex);
  }
  else {
    isisMutexLock(bindMutex);
    if(exp->setexp.binding =   /* not resolved, search top level */
       searchTop(exp->setexp.vname->str, script->env)) {
      freeValue(exp->setexp.binding->val);   /* no other accesses to binding->val must occur */
      exp->setexp.binding->val = referValue(*val);
    }
    else hashBind(script->env, exp->setexp.vname, *val);  /* add to top level */
    isisMutexUnlock(bindMutex);
  }
}

/* ************************************************************************ */
/* The free*Expression functions free the memory associated with an
   expression (or just decrease the reference count) */

/*-------------------------------------------------------------*/
void freeInvalidExpression(Expression *exp)
{
  /* if this was called, there was a problem, so print an error message */

  fprintf(stderr, "* Tried to free an invalid expression.\n");
  
  return;
}

/*-------------------------------------------------------------*/
void freeNullExpression(Expression *exp)
{
  /* there will only ever be one and only one Null expression, and it
     must not be freed at any time */

  return;
}

/*-------------------------------------------------------------*/
void freeConstExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->constexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeValue(exp->constexp.val);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeVaropExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->varopexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeVarname(exp->varopexp.vname);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeVarExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->varexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeVarname(exp->varexp.vname);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeProcExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->procexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeVarnameArray(exp->procexp.formals, exp->procexp.num_formals);
  freeExpression(exp->procexp.bodyexp);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeLocalExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->localexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeVarnameArray(exp->localexp.vnames, exp->localexp.num_vars);
  freeExpression(exp->localexp.bodyexp);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeAppExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->appexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpressionArray(exp->appexp.exps, exp->appexp.num_exps);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeListExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->listexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpressionArray(exp->listexp.exps, exp->listexp.num_exps);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeIfExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->ifexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpression(exp->ifexp.ifpart);
  freeExpression(exp->ifexp.thenpart);
  freeExpression(exp->ifexp.elsepart);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeWhileExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->whileexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpression(exp->whileexp.cond);
  freeExpression(exp->whileexp.body);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeCondExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->condexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpressionArray(exp->condexp.conds, exp->condexp.num_conds);
  freeExpressionArray(exp->condexp.vals, exp->condexp.num_conds);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeSwitchExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->switchexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpression(exp->switchexp.switchexp);
  freeExpressionArray(exp->switchexp.cases, exp->switchexp.num_cases);
  freeExpressionArray(exp->switchexp.vals, exp->switchexp.num_cases);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeBeginExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->beginexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpressionArray(exp->beginexp.exps, exp->beginexp.num_exps);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeLogicExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->logicexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeExpressionArray(exp->logicexp.exps, exp->logicexp.num_exps);
  mem_freeItem(Expression_mem, exp);
}

/*-------------------------------------------------------------*/
void freeSetExpression(Expression *exp)
{
  isisMutexLock(expMutex);
  if(--(exp->setexp.ref_count) > 0) {
    isisMutexUnlock(expMutex);
    return;
  }
  isisMutexUnlock(expMutex);
  freeVarname(exp->setexp.vname);
  freeExpression(exp->setexp.exp);
  mem_freeItem(Expression_mem, exp);
}


/*-------------------------------------------------------------*/
Expression *newExpression(int kind)
{
  Expression *exp;
  
  exp = mem_alloc_item(Expression_mem);
  exp->nullexp.ref_count = 1;
  exp->nullexp.kind = kind;

  return exp;
}

/*-------------------------------------------------------------*/
Expression *referExpression(Expression *exp)
{
  /* Increases the number of references to this expression. */

  isisMutexLock(expMutex);

  exp->nullexp.ref_count++;

  isisMutexUnlock(expMutex);

  return exp;
}

/*-------------------------------------------------------------*/
int insertExpression(void *item, ExpList *explist)
{
  ExpNode *newnode;
  
  newnode = newExpNode(item);
  
  if(explist->tail == NULL) {
    explist->head = newnode;
    explist->tail = newnode;
    explist->num_items = 1;
  }
  else {
    explist->tail->next = newnode;
    explist->tail = newnode;
    explist->num_items += 1;
  }
  
  return explist->num_items;
}

/*-------------------------------------------------------------*/
ExpList *newExpList(void)
{
  ExpList *explist;
  
  explist = mem_alloc_item(ExpList_mem);
  
  explist->head = NULL;
  explist->tail = NULL;
  explist->num_items = 0;

  return explist;
}

/*-------------------------------------------------------------*/
ExpNode *newExpNode(void *item)
{
  ExpNode *expnode;
  
  expnode = mem_alloc_item(ExpNode_mem);
  
  expnode->item = item;
  expnode->next = NULL;

  return expnode;
}

/*-------------------------------------------------------------*/
void freeExpList(ExpList *explist)
{
  ExpNode *expnode, *next;
  
  expnode = explist->head;
  
  while(expnode != NULL) {
    next = expnode->next;
    mem_freeItem(ExpNode_mem, expnode);
    expnode = next;
  }
  
  mem_freeItem(ExpList_mem, explist);
}

/*-------------------------------------------------------------*/
void freeExpressionArray(Expression **exp_array, int num)
{
  int i;

  for(i = 0; i < num; i++) freeExpression(exp_array[i]);
  freePtrArray((void **) exp_array);
}
