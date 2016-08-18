/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* Unix file functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <isis.h>
#include "isis_unix_private.h"

/*----------------------------------------------------------------------*/
Value *prim_unix_rename_file(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  char *str0, *str1;
  int len0, len1, res;
  
  checkCount(2, "old filename, new filename");
  checkString(0, "old filename");
  checkString(1, "new filename");
  
  len0 = Listsize(argv[0]) + 1;
  len1 = Listsize(argv[1]) + 1;
  
  str0 = malloc(len0);
  str1 = malloc(len1);

  getString(argv[0], str0, len0);
  getString(argv[1], str1, len1);
  
  res = rename(str0, str1);
  
  free(str0);
  free(str1);
  
  return newBool(res == 0);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_remove_file(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv)
{
  int len, res;
  char *str;

  checkCount(1, "filename of file to remove");
  checkString(0, "filename of file to remove");

  len = Listsize(argv[0]) + 1;
  str = malloc(len);
  getString(argv[0], str, len);
  res = remove(str);
  free(str);
  return newBool(res == 0);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_create_directory(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv)
{
  int len, res;
  char *str;

  checkCount(1, "directory pathname");
  checkString(0, "directory pathname");
    
  len = Listsize(argv[0]) + 1;
  str = malloc(len);
  getString(argv[0], str, len);
  
  res = mkdir(str, S_IRWXU | S_IRWXG | S_IRWXO);
  
  if(res != 0) {
    fprintf(stderr, "** %s: Unable to create directory: %s\n", proc_name, str);
    free(str);
    return NullValue;
  }

  free(str);
    
  return newBool(1);
}

/*----------------------------------------------------------------------*/
Value *prim_unix_read_directory(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv)
{
  int len, numentries, i;
  Value *newval;
  char *str;
  struct dirent **entries;
  
  if(argc == 0) {
    len = 2;
    str = malloc(len);
    strcpy(str, ".");
  }
  else {
    checkString(0, "directory pathname");    
    len = Listsize(argv[0]) + 1;
    str = malloc(len);
    getString(argv[0], str, len);
  }

  if((numentries = scandir(str, &entries, NULL, alphasort)) < 0) {
    fprintf(stderr, "** %s: Unable to scan directory: %s\n", proc_name, str);
    free(str);
    return NullValue;
  }
    
  newval = newList(numentries, NULL);
  for(i = 0; i < numentries; i++) {
    Listitem(newval, i) = newString(strlen(entries[i]->d_name), 
				    entries[i]->d_name);
    free(entries[i]);
  }
  free(entries);
  
  free(str);
  return newval;
}

/*----------------------------------------------------------------------*/
Value *prim_unix_file_info(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv)
{
  int len, ret;
  Value *newval;
  struct stat info;
  char *str;

  checkCount(1, "filename");
  checkString(0, "filename");
    
  len = Listsize(argv[0]) + 1;
  str = malloc(len);
  getString(argv[0], str, len);
  ret = stat(str, &info);
  if(ret < 0) {
    free(str);
    return NullValue;
  }
  free(str);

  newval = newList(FINFO_LEN, NULL);

  Listitem(newval, FINFO_TYPE) = newInt(info.st_mode & S_IFMT);
  Listitem(newval, FINFO_SIZE) = newInt(info.st_size);
  Listitem(newval, FINFO_RU) = newBool(info.st_mode & S_IRUSR);
  Listitem(newval, FINFO_WU) = newBool(info.st_mode & S_IWUSR);
  Listitem(newval, FINFO_XU) = newBool(info.st_mode & S_IXUSR);
  Listitem(newval, FINFO_RG) = newBool(info.st_mode & S_IRGRP);
  Listitem(newval, FINFO_WG) = newBool(info.st_mode & S_IWGRP);
  Listitem(newval, FINFO_XG) = newBool(info.st_mode & S_IXGRP);
  Listitem(newval, FINFO_RO) = newBool(info.st_mode & S_IROTH);
  Listitem(newval, FINFO_WO) = newBool(info.st_mode & S_IWOTH);
  Listitem(newval, FINFO_XO) = newBool(info.st_mode & S_IXOTH);
  Listitem(newval, FINFO_UID) = newInt(info.st_uid);
  Listitem(newval, FINFO_SUID) = newBool(info.st_mode & S_ISUID);
  Listitem(newval, FINFO_GID) = newInt(info.st_gid);
  Listitem(newval, FINFO_SGID) = newBool(info.st_mode & S_ISGID);
  Listitem(newval, FINFO_ATIME) = newInt(info.st_atime);
  Listitem(newval, FINFO_MTIME) = newInt(info.st_mtime);
  Listitem(newval, FINFO_CTIME) = newInt(info.st_ctime);
  /* 
  Listitem(newval, FINFO_INO) = newInt(info.st_ino);
  Listitem(newval, FINFO_DEV) = newInt(info.st_dev);
  Listitem(newval, FINFO_RDEV) = newInt(info.st_rdev);
  Listitem(newval, FINFO_BLOCKSIZE) = newInt(info.st_blksize);
  Listitem(newval, FINFO_BLOCKS) = newInt(info.st_blocks);
  */
  return newval;
}


