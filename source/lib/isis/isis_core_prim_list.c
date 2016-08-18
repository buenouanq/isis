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

/*-------------------------------------------------------------*/  
Value *prim_length(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  Value *newval;
  
  checkCount(1, NULL);
  checkList(0, NULL);
  
  newval = newInt(Listsize(argv[0]));

  return newval;
}

/*-------------------------------------------------------------*/  
Value *prim_head(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkList(0, NULL);

  if(Listsize(argv[0]) == 0) return NullValue;

  return referValue(Listitem(argv[0], 0));
}

/*-------------------------------------------------------------*/  
Value *prim_tail(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  checkCount(1, NULL);
  checkList(0, NULL);

  if(Listsize(argv[0]) == 0) return NullValue;

  return referValue(Listitem(argv[0], Listsize(argv[0]) - 1));
}

/*-------------------------------------------------------------*/  
Value *prim_first(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  int listlen, i, j;
  int first, last;
  Value *listval, *newval;

  checkCount(2, "number of items and list");
  checkInt(0, "number of items");
  checkList(1, NULL);

  listval = argv[1];
  listlen = Listsize(listval);
  first = 0;
  last = getInt(argv[0]);

  if(last > listlen) last = listlen;
  if(last < 0) last = 0;
  
  newval = newList(last - first, NULL);

  for(i = first, j = 0; i < last; i++, j++)
    Listitem(newval, j) = referValue(Listitem(listval, i));
  
  return newval;
}

/*-------------------------------------------------------------*/  
Value *prim_last(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  int listlen, i, j;
  int first, last;
  Value *listval, *newval;

  checkCount(2, "number of items and list");
  checkInt(0, "number of items");
  checkList(1, NULL);

  listval = argv[1];
  listlen = Listsize(listval);
  last = listlen;
  first = listlen - getInt(argv[0]);
  
  if(first > listlen) first = listlen;
  if(first < 0) first = 0;
  
  newval = newList(last - first, NULL);

  for(i = first, j = 0; i < last; i++, j++)
    Listitem(newval, j) = referValue(Listitem(listval, i));
  
  return newval;
}

/*-------------------------------------------------------------*/  
Value *prim_allbutfirst(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int listlen, i, j;
  int first, last;
  Value *listval, *newval;

  checkCount(2, "number of items and list");
  checkInt(0, "number of items");
  checkList(1, NULL);

  listval = argv[1];
  listlen = Listsize(listval);
  first = getInt(argv[0]);
  last = listlen;

  if(first > listlen) first = listlen;
  if(first < 0) first = 0;
  
  newval = newList(last - first, NULL);

  for(i = first, j = 0; i < last; i++, j++)
    Listitem(newval, j) = referValue(Listitem(listval, i));
  
  return newval;
}

/*-------------------------------------------------------------*/  
Value *prim_allbutlast(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv)
{
  int listlen, i, j;
  int first, last;
  Value *listval, *newval;

  checkCount(2, "number of items and list");
  checkInt(0, "number of items");
  checkList(1, NULL);

  listval = argv[1];
  listlen = Listsize(listval);
  first = 0;
  last = listlen - getInt(argv[0]);

  if(last > listlen) last = listlen;
  if(last < 0) last = 0;
  
  newval = newList(last - first, NULL);

  for(i = first, j = 0; i < last; i++, j++)
    Listitem(newval, j) = referValue(Listitem(listval, i));
  
  return newval;
}

/*-------------------------------------------------------------*/  
Value *prim_sublist(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  int listlen, i, j;
  int first, last;
  Value *listval, *newval;

  checkCount(3, "first and last item indices and list");
  checkInt(0, "index of first item");
  checkInt(1, "index of last item");
  checkList(2, NULL);

  listval = argv[2];
  listlen = Listsize(listval);
  first = getInt(argv[0]);
  last = getInt(argv[1]) + 1;

  if(first > listlen) first = listlen;
  if(first < 0) first = 0;
  if(last > listlen) last = listlen;
  if(last < 0) last = 0;
  
  newval = newList(last - first, NULL);

  for(i = first, j = 0; i < last; i++, j++)
    Listitem(newval, j) = referValue(Listitem(listval, i));
  
  return newval;
}

/*-------------------------------------------------------------*/  
Value *prim_head_insert(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int listlen, i, j;
  Value *listval, *newval;
  
  checkCount(2, "new item and list");
  checkList(1, NULL);
  
  listval = argv[1];
  listlen = Listsize(listval);
  
  newval = newList(listlen + 1, NULL);

  j = 0;
  Listitem(newval, j++) = referValue(argv[0]);
  for(i = 0; i < listlen; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));

  return newval;
}
  
/*-------------------------------------------------------------*/  
Value *prim_tail_insert(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int listlen, i, j;
  Value *listval, *newval;
  
  checkCount(2, "new item and list");
  checkList(1, NULL);
  
  listval = argv[1];
  listlen = Listsize(listval);
  
  newval = newList(listlen + 1, NULL);

  j = 0;
  for(i = 0; i < listlen; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));
  Listitem(newval, j++) = referValue(argv[0]);

  return newval;
}
  
/*-------------------------------------------------------------*/  
Value *prim_insert(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  /* call_data = 0 for insert_before, 1 for insert_after */

  int cutpoint, listlen, i, j;
  Value *listval, *newval;
  
  checkCount(3, "index, new item, and list");
  checkInt(0, "index");
  checkList(2, NULL);
  
  listval = argv[2];
  listlen = Listsize(listval);
  cutpoint = getInt(argv[0]) + call_data;
  
  if(cutpoint < 0) cutpoint = 0;
  if(cutpoint > listlen) cutpoint = listlen;
  
  newval = newList(listlen + 1, NULL);

  j = 0;
  for(i = 0; i < cutpoint; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));
  Listitem(newval, j++) = referValue(argv[1]);
  for(i = cutpoint; i < listlen; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));

  return newval;
}
  
/*-------------------------------------------------------------*/  
Value *prim_change_item(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int cutlow, cuthigh;
  int listlen, i, j;
  Value *listval, *newval;
  
  checkCount(3, "index, new item, and list");
  checkInt(0, "index");
  checkList(2, NULL);
  
  listval = argv[2];
  listlen = Listsize(listval);
  cutlow = getInt(argv[0]);
  if(cutlow < 0 || cutlow > listlen) {
    fprintf(stderr, "** %s: Index %d is out of range\n", proc_name, cutlow);
    return referValue(listval);
  }
  cuthigh = cutlow + 1;
  if(cutlow == listlen) newval = newList(listlen + 1, NULL);
  else newval = newList(listlen, NULL);
  
  j = 0;
  for(i = 0; i < cutlow; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));
  Listitem(newval, j++) = referValue(argv[1]);
  for(i = cuthigh; i < listlen; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));

  return newval;
}
  
/*-------------------------------------------------------------*/  
Value *prim_remove_item(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int cutlow, cuthigh;
  int listlen, i, j;
  Value *listval, *newval;
  
  checkCount(2, "index and list");
  checkInt(0, "list index");
  checkList(1, NULL);

  listval = argv[1];
  listlen = Listsize(listval);
  cutlow = getInt(argv[0]);
  if(cutlow < 0 || cutlow >= listlen) {
    fprintf(stderr, "** %s: Index %d is out of range\n", proc_name, cutlow);
    return referValue(listval);
  }
  cuthigh = cutlow + 1;
  
  newval = newList(listlen - 1, NULL);

  j = 0;
  for(i = 0; i < cutlow; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));
  for(i = cuthigh; i < listlen; i++)
    Listitem(newval, j++) = referValue(Listitem(listval, i));

  return newval;
}
  
/*-------------------------------------------------------------*/  
Value *prim_append(Script *script, char *proc_name, int call_data, 
		   int argc, Value **argv)
{
  int listlen, newlen, i, newpos, lpos;
  Value *listval, *newval;
  
  /* check they are all lists */
  for(i = 0; i < argc; i++) checkList(i, NULL);
  
  newlen = 0;
  for(i = 0; i < argc; i++) newlen += Listsize(argv[i]);
  
  newval = newList(newlen, NULL);

  newpos = 0;
  for(i = 0; i < argc; i++) {
    listval = argv[i];
    listlen = Listsize(listval);
    for(lpos = 0; lpos < listlen; lpos++)
      Listitem(newval, newpos++) = referValue(Listitem(listval, lpos));
  }
  
  return newval;
}
  
/*-------------------------------------------------------------*/
Value *prim_make_list(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  int listlen, i;
  Value *newval;

  checkCount(2, "number of items and value to fill list with");
  checkInt(0, "number of items");

  listlen = getInt(argv[0]);
  newval = newList(listlen, NULL);
  
  for(i = 0; i < listlen; i++) Listitem(newval, i) = referValue(argv[1]);

  return newval;
}

/*-------------------------------------------------------------*/  
Value *prim_reverse(Script *script, char *proc_name, int call_data, 
		    int argc, Value **argv)
{
  int i, j, len;
  Value *newval;
  
  checkList(0, NULL);
  
  len = Listsize(argv[0]);
  newval = newList(len, NULL);
  
  for(i = 0, j = len - 1; i < len; i++, j--)
    Listitem(newval, i) = referValue(Listitem(argv[0], j));
  
  return newval;
}
  
/*-------------------------------------------------------------*/
Value *prim_foremost(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv)
{
  Value *res;
  Procedure *proc;
  int winner, i, size;
  
  if(argc == 0) return NullValue;
  
  checkCount(2, "compare procedure, list of values");
  checkProc(0, NULL);
  checkList(1, NULL);
  
  proc = getProc(argv[0]);
  winner = 0;
  size = Listsize(argv[1]);
  
  for(i = 1; i < size; i++) {
    res = callProcedure(script, proc, 2, 
			Listitem(argv[1], i), 
			Listitem(argv[1], winner));
    if(trueValue(res) == call_data) winner = i;
    freeValue(res);
  }
  
  return newInt(winner);
}

/*-------------------------------------------------------------*/
Value *prim_sort(Script *script, char *proc_name, int call_data, 
		 int argc, Value **argv)
{
  Value *res, *list, *temp;
  Procedure *proc;
  int i, j, size;
  
  if(argc == 0) return NullValue;
  
  checkCount(2, "compare procedure, list of values");
  checkProc(0, NULL);
  checkList(1, NULL);
  
  proc = getProc(argv[0]);
  size = Listsize(argv[1]);

  list = newList(size, getList(argv[1]));
  
  for(i = 0; i < size - 1; i++) {
    for(j = i + 1; j < size; j++) {
      res = callProcedure(script, proc, 2, 
			  Listitem(list, j), 
			  Listitem(list, i));
      if(trueValue(res)) {
	temp = Listitem(list, i);
	Listitem(list, i) = Listitem(list, j);
	Listitem(list, j) = temp;
      }
      freeValue(res);
    }
  }
  
  return list;
}

/*-------------------------------------------------------------*/  
Value *prim_apply(Script *script, char *proc_name, int call_data, 
		  int argc, Value **argv)
{
  checkCount(2, "procedure and list of arguments");
  checkProc(0, NULL);
  checkList(1, NULL);
  
  return applyProcedure(script, getProc(argv[0]), 
			Listsize(argv[1]), getList(argv[1]));
}

/*-------------------------------------------------------------*/
Value *prim_map(Script *script, char *proc_name, int call_data, 
		int argc, Value **argv)
{
  int listlen, i, j, numargs;
  Procedure *proc;
  Value *newval;
  Value **newargv;

  checkCountLeast(2, "procedure and lists of arguments");
  checkProc(0, NULL);
  for(i = 1; i < argc; i++) checkList(i, NULL);

  proc = getProc(argv[0]);
  listlen = Listsize(argv[1]);

  for(i = 2; i < argc; i++) if(Listsize(argv[i]) != listlen) {
    fprintf(stderr, "** %s: Arg %d list is not same size.\n", 
	    proc_name, i + 1);
    return NullValue;
  }
  
  numargs = argc - 1;
  newargv = (Value **) newPtrArray(numargs);

  newval = newList(listlen, NULL);

  for(i = 0; i < listlen; i++) {
    for(j = 0; j < numargs; j++) newargv[j] = Listitem(argv[j + 1], i);
    Listitem(newval, i) = applyProcedure(script, proc, numargs, newargv);
  }
  
  freePtrArray((void **) newargv);
  return newval;
}

