
/* ---------------------------------------------------------------------- */
Value *loop_string(Script *script, char *proc_name, int call_data, 
                   int argc, Value **argv)
{
  /* This function takes an integer and a string, and prints that
     string the number of times specified by the integer. */

  int len;
  char *str;

  /* First check that we actually received an integer and a string: */

  checkCount(2, "loop parameter and string");
  checkInt(0, "loop parameter");
  checkString(1, NULL);

  /* Extract the string and print it repeatedly: */

  len = Listsize(argv[1]) + 1;  /* add 1 for final '\0' character */
  str = malloc(len);
  getString(argv[1], str, len);
  for(i = 0; i < getInt(argv[0]); i++) printf("%s\n" , str);
  free(str);

  /* Return the original string that came in.  Must use referValue
     because we are creating a new reference to an argument: */

  return referValue(argv[1]);
}

