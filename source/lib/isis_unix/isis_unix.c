/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* Unix functions library */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <isis.h>
#include "isis_unix.h"
#include "isis_unix_private.h"

/*----------------------------------------------------------------------*/
/* exported globals */

Value *isis_unix_port_type_FD_val = NULL;
Value *prim_unix_close_FD_val = NULL;
Value *prim_unix_read_FD_val = NULL;
Value *prim_unix_readline_FD_val = NULL;
Value *prim_unix_write_FD_val = NULL;
Value *prim_unix_tell_FD_val = NULL;
Value *prim_unix_seek_FD_val = NULL;
Value *prim_unix_read_ready_FD_val = NULL;
Value *prim_unix_write_ready_FD_val = NULL;
Value *prim_unix_configure_FD_val = NULL;
Value *prim_unix_recv_FD_val = NULL;
Value *prim_unix_send_FD_val = NULL;

/************************************************************************/

/*----------------------------------------------------------------------*/
void bind_unix_library(Script *script)
{
  Procedure *proc;

  bindBool(script, "isis_unix", 1);
  
  /* create a string value for FD port type */
  
  isis_unix_port_type_FD_val = newCharList(2, "FD");
  
  /* create procedure values for standard FD port routines */
  
  proc = newCfunc(prim_unix_close_FD, "unix-close-FD", 0);
  prim_unix_close_FD_val = newProc(proc);
  freeProcedure(proc);
  
  proc = newCfunc(prim_unix_read_FD, "unix-read-FD", 0);
  prim_unix_read_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_readline_FD, "unix-readline-FD", 0);
  prim_unix_readline_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_write_FD, "unix-write-FD", 0);
  prim_unix_write_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_tell_FD, "unix-tell-FD", 0);
  prim_unix_tell_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_seek_FD, "unix-seek-FD", 0);
  prim_unix_seek_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_ready_FD, "unix-read-ready-FD", 0);
  prim_unix_read_ready_FD_val = newProc(proc);
  freeProcedure(proc);
  
  proc = newCfunc(prim_unix_ready_FD, "unix-write-ready-FD", 1);
  prim_unix_write_ready_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_configure_FD, "unix-configure-FD", 0);
  prim_unix_configure_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_recv_FD, "unix-recv-FD", 0);
  prim_unix_recv_FD_val = newProc(proc);
  freeProcedure(proc);

  proc = newCfunc(prim_unix_send_FD, "unix-send-FD", 0);
  prim_unix_send_FD_val = newProc(proc);
  freeProcedure(proc);

  bindValue(script, "port-type-FD", isis_unix_port_type_FD_val);
  
  bindValue(script, "unix-close-FD", prim_unix_close_FD_val);
  bindValue(script, "unix-read-FD", prim_unix_read_FD_val);
  bindValue(script, "unix-readline-FD", prim_unix_readline_FD_val);
  bindValue(script, "unix-write-FD", prim_unix_write_FD_val);
  bindValue(script, "unix-tell-FD", prim_unix_tell_FD_val);
  bindValue(script, "unix-seek-FD", prim_unix_seek_FD_val);
  bindValue(script, "unix-read-ready-FD", prim_unix_read_ready_FD_val);
  bindValue(script, "unix-write-ready-FD", prim_unix_write_ready_FD_val);
  bindValue(script, "unix-configure-FD", prim_unix_configure_FD_val);

  bindValue(script, "unix-recv-FD", prim_unix_recv_FD_val);
  bindValue(script, "unix-send-FD", prim_unix_send_FD_val);

  /* port functions */

  bindCfunc(script, "unix-fileno", prim_unix_fileno, 0);
  bindCfunc(script, "unix-pipe", prim_unix_pipe, 0);
  bindCfunc(script, "unix-dup", prim_unix_dup, 0);
  bindCfunc(script, "unix-open", prim_unix_open, 0);
  bindCfunc(script, "unix-new-port-FD", prim_unix_new_port_FD, 0);
  bindCfunc(script, "unix-read-ready-list-FD", prim_unix_ready_list_FD, 0);
  bindCfunc(script, "unix-write-ready-list-FD", prim_unix_ready_list_FD, 1);
  
  bindCfunc(script, "unix-multipending-FD", prim_unix_ready_list_FD, 0);
  
  /* file functions */

  bindCfunc(script, "rename-file", prim_unix_rename_file, 0);  
  bindCfunc(script, "remove-file", prim_unix_remove_file, 0);  
  bindCfunc(script, "create-directory", prim_unix_create_directory, 0);
  bindCfunc(script, "read-directory", prim_unix_read_directory, 0);
  bindCfunc(script, "unix-file-info", prim_unix_file_info, 0);
  
  /* terminal functions */

  bindCfunc(script, "unix-terminal-getattr", prim_unix_terminal_getattr, 0);
  bindCfunc(script, "unix-terminal-setattr", prim_unix_terminal_setattr, 0);
  bindCfunc(script, "unix-terminal-sendbreak", prim_unix_terminal_sendbreak, 0);
  bindCfunc(script, "unix-terminal-drain", prim_unix_terminal_drain, 0);

  /* process functions */

  bindCfunc(script, "system", prim_unix_system, 0);
  bindCfunc(script, "get-env", prim_unix_get_env, 0);
  bindCfunc(script, "set-env", prim_unix_set_env, 0);
  bindCfunc(script, "get-time", prim_unix_get_time, 0);
  bindCfunc(script, "sleep", prim_unix_sleep, 0);
  bindCfunc(script, "microsleep", prim_unix_microsleep, 0);
  bindCfunc(script, "fork", prim_unix_fork, 0);
  bindCfunc(script, "exec", prim_unix_exec, 0);
  bindCfunc(script, "execute", prim_unix_execute, 0);
  bindCfunc(script, "get-status", prim_unix_waitpid, 0);  
  bindCfunc(script, "wait", prim_unix_waitpid, 1);
  bindCfunc(script, "send-signal", prim_unix_send_signal, 0);
  bindCfunc(script, "get-process-id", prim_unix_get_process_id, 0);
  bindCfunc(script, "get-parent-id", prim_unix_get_parent_id, 0);
  bindCfunc(script, "get-user-id", prim_unix_get_user_id, 0);
  bindCfunc(script, "get-group-id", prim_unix_get_group_id, 0);
  
  /* thread functions */

#ifdef ISIS_MULTITHREAD
  bindCfunc(script, "spawn", prim_unix_thread_spawn, 0);
  bindCfunc(script, "join", prim_unix_thread_join, 0);
  bindCfunc(script, "self", prim_unix_thread_self, 0);
#endif

  /* network */

  bindCfunc(script, "unix-socket", prim_unix_socket, 0);
  bindInt(script, "sock-type-tcp", ISIS_SOCKTYPE_TCP);
  bindInt(script, "sock-type-udp", ISIS_SOCKTYPE_UDP);

  bindCfunc(script, "unix-socket-option", prim_unix_socket_option, 0);
  bindInt(script, "sock-option-reuseaddr", ISIS_SO_REUSEADDR);
  bindInt(script, "sock-option-keepalive", ISIS_SO_KEEPALIVE);
  bindInt(script, "sock-option-broadcast", ISIS_SO_BROADCAST);
  bindInt(script, "sock-option-sndbuf", ISIS_SO_SNDBUF);
  bindInt(script, "sock-option-rcvbuf", ISIS_SO_RCVBUF);
  bindInt(script, "sock-option-priority", ISIS_SO_PRIORITY);
  bindInt(script, "sock-option-error", ISIS_SO_ERROR);
  bindInt(script, "sock-option-tos", ISIS_SO_TOS);
  bindInt(script, "sock-option-ttl", ISIS_SO_TTL);
  bindInt(script, "sock-option-mtu", ISIS_SO_MTU);
  bindInt(script, "sock-option-mcttl", ISIS_SO_MCTTL);
  bindInt(script, "sock-option-mcloop", ISIS_SO_MCLOOP);
  bindInt(script, "sock-option-addmem", ISIS_SO_ADDMEM);
  bindInt(script, "sock-option-dropmem", ISIS_SO_DROPMEM);
  bindInt(script, "sock-option-nodelay", ISIS_SO_NODELAY);
  bindInt(script, "sock-option-maxseg", ISIS_SO_MAXSEG);

  bindCfunc(script, "unix-connect", prim_unix_connect, 0);
  bindCfunc(script, "unix-bind", prim_unix_bind, 0);
  bindCfunc(script, "unix-listen", prim_unix_listen, 0);
  bindCfunc(script, "unix-accept", prim_unix_accept, 0);

  bindCfunc(script, "lookup-hostname", prim_unix_lookup_hostname, 0);
  bindCfunc(script, "lookup-address", prim_unix_lookup_address, 0);

  /* open flags */

  bindInt(script, "O_RDONLY", O_RDONLY);
  bindInt(script, "O_WRONLY", O_WRONLY);
  bindInt(script, "O_RDWR", O_RDWR);
  bindInt(script, "O_CREAT", O_CREAT);
  bindInt(script, "O_EXCL", O_EXCL);
  bindInt(script, "O_NOCTTY", O_NOCTTY);
  bindInt(script, "O_TRUNC", O_TRUNC);
  bindInt(script, "O_SYNC", O_SYNC);
  bindInt(script, "O_DSYNC", O_DSYNC);
  bindInt(script, "O_RSYNC", O_RSYNC);
  bindInt(script, "O_APPEND", O_TRUNC);
  bindInt(script, "O_NONBLOCK", O_NONBLOCK);
  bindInt(script, "O_NDELAY", O_NDELAY);

  /* mode flags */

  bindInt(script, "S_ISUID", S_ISUID);
  bindInt(script, "S_ISGID", S_ISGID);
  bindInt(script, "S_IRWXU", S_IRWXU);
  bindInt(script, "S_IRUSR", S_IRUSR);
  bindInt(script, "S_IWUSR", S_IWUSR);
  bindInt(script, "S_IXUSR", S_IXUSR);
  bindInt(script, "S_IRWXG", S_IRWXG);
  bindInt(script, "S_IRGRP", S_IRGRP);
  bindInt(script, "S_IWGRP", S_IWGRP);
  bindInt(script, "S_IXGRP", S_IXGRP);
  bindInt(script, "S_IRWXO", S_IRWXO);
  bindInt(script, "S_IROTH", S_IROTH);
  bindInt(script, "S_IWOTH", S_IWOTH);
  bindInt(script, "S_IXOTH", S_IXOTH);

  /* file info constants */

  bindInt(script, "file-info-type", FINFO_TYPE);
  bindInt(script, "file-info-size", FINFO_SIZE);
  bindInt(script, "file-info-user-read", FINFO_RU);
  bindInt(script, "file-info-user-write", FINFO_WU);
  bindInt(script, "file-info-user-execute", FINFO_XU);
  bindInt(script, "file-info-group-read", FINFO_RG);
  bindInt(script, "file-info-group-write", FINFO_WG);
  bindInt(script, "file-info-group-execute", FINFO_XG);
  bindInt(script, "file-info-other-read", FINFO_RO);
  bindInt(script, "file-info-other-write", FINFO_WO);
  bindInt(script, "file-info-other-execute", FINFO_XO);
  bindInt(script, "file-info-user-id", FINFO_UID);
  bindInt(script, "file-info-set-user-id", FINFO_SUID);
  bindInt(script, "file-info-group-id", FINFO_GID);
  bindInt(script, "file-info-set-group-id", FINFO_SGID);
  bindInt(script, "file-info-last-access", FINFO_ATIME);
  bindInt(script, "file-info-last-modification", FINFO_MTIME);
  bindInt(script, "file-info-last-status-change", FINFO_CTIME);

  /*
  bindInt(script, "file-info-inode", FINFO_INO);
  bindInt(script, "file-info-device", FINFO_DEV);
  bindInt(script, "file-info-remote-device", FINFO_RDEV);
  bindInt(script, "file-info-block-size", FINFO_BLOCKSIZE);
  bindInt(script, "file-info-blocks", FINFO_BLOCKS);
  */

  bindInt(script, "file-type-regular", S_IFREG);
  bindInt(script, "file-type-directory", S_IFDIR);
  bindInt(script, "file-type-block", S_IFBLK);
  bindInt(script, "file-type-character", S_IFCHR);
  bindInt(script, "file-type-fifo", S_IFIFO);

  /* signals constants */

  bindInt(script, "sig-hangup", SIGHUP);
  bindInt(script, "sig-interrupt", SIGINT);
  bindInt(script, "sig-quit", SIGQUIT);
  bindInt(script, "sig-illegal-instruction", SIGILL);
  bindInt(script, "sig-trace-trap", SIGTRAP);
  bindInt(script, "sig-abort", SIGABRT);
  /*  bindInt(script, "sig-emt", SIGEMT); */
  bindInt(script, "sig-floating-exception", SIGFPE);
  bindInt(script, "sig-kill", SIGKILL);
  bindInt(script, "sig-bus", SIGBUS);
  bindInt(script, "sig-segmentation-fault", SIGSEGV);
  /*  bindInt(script, "sig-system", SIGSYS); */
  bindInt(script, "sig-pipe", SIGPIPE);
  bindInt(script, "sig-alarm", SIGALRM);
  bindInt(script, "sig-urgent", SIGURG);
  bindInt(script, "sig-stop", SIGSTOP);
  bindInt(script, "sig-interactive-stop", SIGTSTP);
  bindInt(script, "sig-continue", SIGCONT);
  bindInt(script, "sig-child", SIGCHLD);
  bindInt(script, "sig-tty-input", SIGTTIN);
  bindInt(script, "sig-tty-output", SIGTTOU);
  bindInt(script, "sig-io", SIGIO);
  bindInt(script, "sig-window-change", SIGWINCH);
  bindInt(script, "sig-user-1", SIGUSR1);
  bindInt(script, "sig-user-2", SIGUSR2);
  
  /* terminal attribute constants */

  bindInt(script, "tty-ignbrk", IGNBRK);
  bindInt(script, "tty-brkint", BRKINT);
  bindInt(script, "tty-ignpar", IGNPAR);
  bindInt(script, "tty-parmrk", PARMRK);
  bindInt(script, "tty-inpck", INPCK);
  bindInt(script, "tty-istrip", ISTRIP);
  bindInt(script, "tty-inlcr", INLCR);
  bindInt(script, "tty-igncr", IGNCR);
  bindInt(script, "tty-icrnl", ICRNL);
  bindInt(script, "tty-iuclc", IUCLC);
  bindInt(script, "tty-ixon", IXON);
  bindInt(script, "tty-ixany", IXANY);
  bindInt(script, "tty-ixoff", IXOFF);

  bindInt(script, "tty-opost", OPOST);
  bindInt(script, "tty-olcuc", OLCUC);
  bindInt(script, "tty-onlcr", ONLCR);
  bindInt(script, "tty-ocrnl", OCRNL);
  bindInt(script, "tty-onocr", ONOCR);
  bindInt(script, "tty-onlret", ONLRET);
  bindInt(script, "tty-ofill", OFILL);
  bindInt(script, "tty-ofdel", OFDEL);
  bindInt(script, "tty-nldly", NLDLY);
  bindInt(script, "tty-nl0", NL0);
  bindInt(script, "tty-nl1", NL1);
  bindInt(script, "tty-crdly", CRDLY);
  bindInt(script, "tty-cr0", CR0);
  bindInt(script, "tty-cr1", CR1);
  bindInt(script, "tty-cr2", CR2);
  bindInt(script, "tty-cr3", CR3);
  bindInt(script, "tty-tabdly", TABDLY);
  bindInt(script, "tty-tab0", TAB0);
  bindInt(script, "tty-tab1", TAB1);
  bindInt(script, "tty-tab2", TAB2);
  bindInt(script, "tty-tab3", TAB3);
  bindInt(script, "tty-bsdly", BSDLY);
  bindInt(script, "tty-bs0", BS0);
  bindInt(script, "tty-bs1", BS1);
  bindInt(script, "tty-vtdly", VTDLY);
  bindInt(script, "tty-vt0", VT0);
  bindInt(script, "tty-vt1", VT1);
  bindInt(script, "tty-ffdly", FFDLY);
  bindInt(script, "tty-ff0", FF0);
  bindInt(script, "tty-ff1", FF1);

  bindInt(script, "tty-crtscts", CRTSCTS);
  bindInt(script, "tty-csize", CSIZE);
  bindInt(script, "tty-cs5", CS5);
  bindInt(script, "tty-cs6", CS6);
  bindInt(script, "tty-cs7", CS7);
  bindInt(script, "tty-cs8", CS8);
  bindInt(script, "tty-cstopb", CSTOPB);
  bindInt(script, "tty-cread", CREAD);
  bindInt(script, "tty-parenb", PARENB);
  bindInt(script, "tty-parodd", PARODD);
  bindInt(script, "tty-hupcl", HUPCL);
  bindInt(script, "tty-clocal", CLOCAL);

  bindInt(script, "tty-isig", ISIG);
  bindInt(script, "tty-icanon", ICANON);
  bindInt(script, "tty-xcase", XCASE);
  bindInt(script, "tty-echo", ECHO);
  bindInt(script, "tty-echoe", ECHOE);
  bindInt(script, "tty-echok", ECHOK);
  bindInt(script, "tty-echonl", ECHONL);
  bindInt(script, "tty-noflsh", NOFLSH);
  /*  bindInt(script, "tty-echoctl", ECHOCTL);
      bindInt(script, "tty-echoprt", ECHOPRT);
      bindInt(script, "tty-echoke", ECHOKE);
      bindInt(script, "tty-flusho", FLUSHO);
      bindInt(script, "tty-pendin", PENDIN); */
  bindInt(script, "tty-iexten", IEXTEN);
  bindInt(script, "tty-tostop", TOSTOP);

  bindInt(script, "tty-vintr", VINTR);
  bindInt(script, "tty-vquit", VQUIT);
  bindInt(script, "tty-verase", VERASE);
  bindInt(script, "tty-vkill", VKILL);
  bindInt(script, "tty-veof", VEOF);
  bindInt(script, "tty-veol", VEOL);
  bindInt(script, "tty-veol2", VEOL2);
  bindInt(script, "tty-vstart", VSTART);
  bindInt(script, "tty-vsusp", VSUSP);
  /*  bindInt(script, "tty-vdsusp", VDSUSP);*/
  bindInt(script, "tty-vstop", VSTOP);
  /*  bindInt(script, "tty-vreprint", VREPRINT);
      bindInt(script, "tty-vdiscard", VDISCARD);
      bindInt(script, "tty-vwerase", VWERASE);  
      bindInt(script, "tty-vlnext", VLNEXT);  */
  bindInt(script, "tty-vmin", VMIN);
  bindInt(script, "tty-vtime", VTIME);
}
