
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
