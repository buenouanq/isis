
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>    /* For wait */
#include <fcntl.h>
#include <dirent.h>       /* for scandir */
#include <unistd.h>
#include <termios.h>
#include <netinet/in.h>   /* for defs of ntohl and htonl */
#include <netinet/tcp.h>
#include <netdb.h> 
#include <signal.h>      /* for kill/signals */
#include <errno.h>	 /* errno and perror */
#include <stropts.h>     /* for ioctl */


/*----------------------------------------------------------------------*/
/* enumerations */

enum { FINFO_TYPE, FINFO_SIZE, FINFO_RU, FINFO_WU, FINFO_XU,
       FINFO_RG, FINFO_WG, FINFO_XG, FINFO_RO, FINFO_WO, FINFO_XO,
       FINFO_UID, FINFO_SUID, FINFO_GID, FINFO_SGID,
       FINFO_ATIME, FINFO_MTIME, FINFO_CTIME,
       /* FINFO_INO, FINFO_DEV, FINFO_RDEV, FINFO_BLOCKSIZE, FINFO_BLOCKS, */
       FINFO_LEN };

enum { OPEN_INPUT, OPEN_OUTPUT, OPEN_UPDATE };
enum { INPUT_PORT, OUTPUT_PORT };

enum { ISIS_SOCKTYPE_TCP, ISIS_SOCKTYPE_UDP };

enum { ISIS_SO_REUSEADDR, ISIS_SO_KEEPALIVE, ISIS_SO_BROADCAST,
       ISIS_SO_SNDBUF, ISIS_SO_RCVBUF, ISIS_SO_PRIORITY, 
       ISIS_SO_ERROR, ISIS_SO_TOS, ISIS_SO_TTL, ISIS_SO_MTU,
       ISIS_SO_MCTTL, ISIS_SO_MCLOOP, ISIS_SO_ADDMEM, ISIS_SO_DROPMEM,
       ISIS_SO_NODELAY, ISIS_SO_MAXSEG };

/*----------------------------------------------------------------------*/
/* globals */

extern Value *isis_unix_port_type_FD_val;
extern Value *prim_unix_close_FD_val;
extern Value *prim_unix_read_FD_val;
extern Value *prim_unix_readline_FD_val;
extern Value *prim_unix_write_FD_val;
extern Value *prim_unix_tell_FD_val;
extern Value *prim_unix_seek_FD_val;
extern Value *prim_unix_read_ready_FD_val;
extern Value *prim_unix_write_ready_FD_val;
extern Value *prim_unix_configure_FD_val;
extern Value *prim_unix_recv_FD_val;
extern Value *prim_unix_send_FD_val;

/*----------------------------------------------------------------------*/
/* port routines */

Value *prim_unix_fileno(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_unix_pipe(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_unix_dup(Script *script, char *proc_name, int call_data, 
		     int argc, Value **argv);
Value *prim_unix_open(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_unix_new_port_FD(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_unix_close_FD(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_unix_read_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_readline_FD(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_unix_write_FD(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_unix_tell_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_seek_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_ready_FD(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_unix_ready_list_FD(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *prim_unix_configure_FD(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv);
Value *prim_unix_recv_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_send_FD(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);

/*----------------------------------------------------------------------*/
/* file routines */

Value *prim_unix_rename_file(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_unix_remove_file(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_unix_create_directory(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv);
Value *prim_unix_read_directory(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv);
Value *prim_unix_file_info(Script *script, char *proc_name, int call_data, 
			   int argc, Value **argv);

/*----------------------------------------------------------------------*/
/* tcp routines */

Value *prim_unix_tcp_open(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_unix_tcp_listen(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *prim_unix_tcp_accept(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);

/*----------------------------------------------------------------------*/
/* terminal routines */

Value *prim_unix_terminal_getattr(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv);
Value *prim_unix_terminal_setattr(Script *script, char *proc_name, int call_data, 
				  int argc, Value **argv);
Value *prim_unix_terminal_sendbreak(Script *script, char *proc_name, int call_data, 
				    int argc, Value **argv);
Value *prim_unix_terminal_drain(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv);

/*----------------------------------------------------------------------*/
/* process routines */

Value *prim_unix_system(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_unix_get_env(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_set_env(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_get_time(Script *script, char *proc_name, int call_data, 
			  int argc, Value **argv);
Value *prim_unix_sleep(Script *script, char *proc_name, int call_data, 
		       int argc, Value **argv);
Value *prim_unix_microsleep(Script *script, char *proc_name, int call_data, 
			    int argc, Value **argv);
Value *prim_unix_fork(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_unix_exec(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_unix_execute(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_waitpid(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_send_signal(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_unix_get_process_id(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv);
Value *prim_unix_get_parent_id(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *prim_unix_get_user_id(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_unix_get_group_id(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv);

/*----------------------------------------------------------------------*/
/* threading routines */

Value *prim_unix_thread_spawn(Script *script, char *proc_name, int call_data, 
			      int argc, Value **argv);
Value *prim_unix_thread_join(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);
Value *prim_unix_thread_self(Script *script, char *proc_name, int call_data, 
			     int argc, Value **argv);

/*----------------------------------------------------------------------*/
/* network routines */

Value *prim_unix_socket(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_unix_socket_option(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv);
Value *prim_unix_lookup_hostname(Script *script, char *proc_name, int call_data, 
				 int argc, Value **argv);
Value *prim_unix_lookup_address(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv);
Value *prim_unix_connect(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv);
Value *prim_unix_bind(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv);
Value *prim_unix_listen(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);
Value *prim_unix_accept(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv);

