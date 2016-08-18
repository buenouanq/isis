/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <isis.h>
#include "isis_unix_private.h"

#define MAX_PENDING_CONNECTIONS 5
#define MAX_INADDR_LEN 20

/*-------------------------------------------------------------*/
Value *prim_unix_socket(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int socktype, type, domain, protocol, sock;

  checkCount(1, "socket type");
  checkInt(0, "socket type");
  
  socktype = getInt(argv[0]);

  switch(socktype) {

  case ISIS_SOCKTYPE_TCP:
    domain = PF_INET;
    type = SOCK_STREAM;
    protocol = 0;
    break;

  case ISIS_SOCKTYPE_UDP:
    domain = PF_INET;
    type = SOCK_DGRAM;
    protocol = 0;
    break;

  default:
    fprintf(stderr, "** Unimplemented socket type\n");
    return NullValue;
  }

  if ((sock = socket(domain, type, protocol)) < 0) {
    fprintf(stderr, "** Error creating socket\n");
    return NullValue;
  }
  
  return newInt(sock);
}
    
/*-------------------------------------------------------------*/
Value *prim_unix_socket_option(Script *script, char *proc_name, int call_data, 
			       int argc, Value **argv)
{
  int sock, option, level, optname, optlen, i, len;
  struct ip_mreqn mreqn;
  void *optval;
  int flag, val;
  unsigned char hostnum[MAX_INADDR_LEN];

  checkCountLeast(2, "socket, option id, { value }");
  checkInt(0, "socket");
  checkInt(1, "option id");
  
  sock = getInt(argv[0]);
  option = getInt(argv[1]);
  
  switch(option) {
    
  case ISIS_SO_REUSEADDR:
    level = SOL_SOCKET;
    optname = SO_REUSEADDR;
    optval = &flag;
    optlen = sizeof(flag);
    if(argc > 2) flag = trueValue(argv[2]);
    break;

  case ISIS_SO_KEEPALIVE:
    level = SOL_SOCKET;
    optname = SO_KEEPALIVE;
    optval = &flag;
    optlen = sizeof(flag);
    if(argc > 2) flag = trueValue(argv[2]);
    break;

  case ISIS_SO_BROADCAST:
    level = SOL_SOCKET;
    optname = SO_BROADCAST;
    optval = &flag;
    optlen = sizeof(flag);
    if(argc > 2) flag = trueValue(argv[2]);
    break;

  case ISIS_SO_SNDBUF:
    level = SOL_SOCKET;
    optname = SO_SNDBUF;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) {
      checkInt(2, "send buffer size");
      val = getInt(argv[2]);
    }
    break;

  case ISIS_SO_RCVBUF:
    level = SOL_SOCKET;
    optname = SO_RCVBUF;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) {
      checkInt(2, "receive buffer size");
      val = getInt(argv[2]);
    }
    break;

  case ISIS_SO_PRIORITY:
    level = SOL_SOCKET;
    optname = SO_PRIORITY;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) {
      checkInt(2, "priority");
      val = getInt(argv[2]);
    }
    break;

  case ISIS_SO_ERROR:
    level = SOL_SOCKET;
    optname = SO_ERROR;
    optval = &val;
    optlen = sizeof(val);
    break;

  case ISIS_SO_TOS:
    level = SOL_IP;
    optname = IP_TOS;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) {
      checkInt(2, "type of service id");
      val = getInt(argv[2]);
    }
    break;

  case ISIS_SO_TTL:
    level = SOL_IP;
    optname = IP_TTL;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) {
      checkInt(2, "time to live");
      val = getInt(argv[2]);
    }
    break;

    /* 
  case ISIS_SO_MTU:
    level = SOL_IP;
    optname = IP_MTU;
    optval = &val;
    optlen = sizeof(val);
    break;
    */

  case ISIS_SO_MCTTL:
    level = SOL_IP;
    optname = IP_MULTICAST_TTL;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) {
      checkInt(2, "multicast time to live");
      val = getInt(argv[2]);
    }
    break;

  case ISIS_SO_MCLOOP:
    level = SOL_IP;
    optname = IP_MULTICAST_LOOP;
    optval = &flag;
    optlen = sizeof(flag);
    if(argc > 2) flag = trueValue(argv[2]);
    break;

  case ISIS_SO_ADDMEM:
    level = SOL_IP;
    optname = IP_ADD_MEMBERSHIP;
    optval = &mreqn;
    optlen = sizeof(mreqn);
    if(argc > 2) {
      checkIntList(2, "multicast address");
      len = Listsize(argv[2]);
      if(len > MAX_INADDR_LEN) len = MAX_INADDR_LEN;
      for(i = 0; i < len; i++)
	hostnum[i] = (unsigned char) getInt(Listitem(argv[2], i));
      memset(&mreqn, 0, sizeof(mreqn));
      memcpy(&mreqn.imr_multiaddr, hostnum, len);
    }
    break;

  case ISIS_SO_DROPMEM:
    level = SOL_IP;
    optname = IP_DROP_MEMBERSHIP;
    optval = &mreqn;
    optlen = sizeof(mreqn);
    if(argc > 2) {
      checkIntList(2, "multicast address");
      len = Listsize(argv[2]);
      if(len > MAX_INADDR_LEN) len = MAX_INADDR_LEN;
      for(i = 0; i < len; i++)
	hostnum[i] = (unsigned char) getInt(Listitem(argv[2], i));
      memset(&mreqn, 0, sizeof(mreqn));
      memcpy(&mreqn.imr_multiaddr, hostnum, len);
    }
    break;
    
  case ISIS_SO_NODELAY:
    level = SOL_TCP;
    optname = TCP_NODELAY;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) flag = trueValue(argv[2]);
    break;
    
  case ISIS_SO_MAXSEG:
    level = SOL_TCP;
    optname = TCP_MAXSEG;
    optval = &val;
    optlen = sizeof(val);
    if(argc > 2) {
      checkInt(2, "max tcp segment size");
      val = getInt(argv[2]);
    }
    break;

  default:
    fprintf(stderr, "** Unrecognized socket option %d\n", option);
    return NullValue;
  }
    
  if(argc > 2) {
    if( setsockopt(sock, level, optname, optval, optlen) < 0) {
      fprintf(stderr, "** Problem setting socket option %d on socket %d\n", option, sock);
      return NullValue;
    }
  }

  if((optname != IP_ADD_MEMBERSHIP) && (optname != IP_DROP_MEMBERSHIP)) { 
    if( getsockopt(sock, level, optname, optval, &optlen) < 0) {
      fprintf(stderr, "** Problem getting socket option %d on socket %d\n", option, sock);
      return NullValue;
    }
  }

  switch(option) {

  case ISIS_SO_REUSEADDR:
    return newBool(flag);

  case ISIS_SO_KEEPALIVE:
    return newBool(flag);

  case ISIS_SO_BROADCAST:
    return newBool(flag);

  case ISIS_SO_SNDBUF:
    return newInt(val);

  case ISIS_SO_RCVBUF:
    return newInt(val);

  case ISIS_SO_PRIORITY:
    return newInt(val);

  case ISIS_SO_ERROR:
    return newInt(val);

  case ISIS_SO_TOS:
    return newInt(val);

  case ISIS_SO_TTL:
    return newInt(val);

  case ISIS_SO_MTU:
    return newInt(val);

  case ISIS_SO_MCTTL:
    return newInt(val);
    
  case ISIS_SO_MCLOOP:
    return newBool(val);
    
  case ISIS_SO_ADDMEM:
    return newBool(1);
    
  case ISIS_SO_DROPMEM:
    return newBool(1);
    
  case ISIS_SO_NODELAY:
    return newBool(flag);

  case ISIS_SO_MAXSEG:
    return newInt(val);

  default:
    fprintf(stderr, "** Unrecognized socket option %d\n", option);
    return NullValue;
  }
}
    
/*-------------------------------------------------------------*/
Value *prim_unix_lookup_hostname(Script *script, char *proc_name, int call_data, 
				 int argc, Value **argv)
{
  struct hostent *hent;
  char *hostname;
  int len, i, *hostnum;
  Value *retval;

  checkCount(1, "hostname");
  checkString(0, "hostname");
  
  len = Listsize(argv[0]) + 1;
  hostname = malloc(len);
  getString(argv[0], hostname, len);
  
  hent = gethostbyname(hostname);
  free(hostname);
  if(hent == NULL) return NullValue;

  len = hent->h_length;
  hostnum = malloc(len * sizeof(int));

  for(i = 0; i < len; i++) hostnum[i] = hent->h_addr_list[0][i];
  retval = newIntList(len, hostnum);

  free(hostnum);

  return retval;
}


/*-------------------------------------------------------------*/
Value *prim_unix_lookup_address(Script *script, char *proc_name, int call_data, 
				int argc, Value **argv)
{
  struct hostent *hent;
  unsigned char *hostnum;
  int len, i;
  Value *retval;

  checkCount(1, "host address");
  checkIntList(0, "host address");
  
  len = Listsize(argv[0]);
  hostnum = malloc(len);
  for(i = 0; i < len; i++)
    hostnum[i] = (unsigned char) getInt(Listitem(argv[0], i));
  
  hent = gethostbyaddr(hostnum, len, AF_INET);
  free(hostnum);
  if(hent == NULL) return NullValue;
  
  return newString(strlen(hent->h_name), hent->h_name);
}


/*-------------------------------------------------------------*/
Value *prim_unix_connect(Script *script, char *proc_name, int call_data, 
			 int argc, Value **argv)
{
  struct sockaddr_in remote;
  unsigned char *hostnum;
  int len, i, sock, portnum;
  
  checkCount(3, "socket, host address, port number");
  checkInt(0, "socket");
  checkInt(2, "port number");
  
  sock = getInt(argv[0]);
  portnum = getInt(argv[2]);

  remote.sin_family = AF_INET;
  remote.sin_port = htons((unsigned short)portnum);  

  checkIntList(1, "host address");

  len = Listsize(argv[1]);
  hostnum = malloc(len);
  for(i = 0; i < len; i++)
    hostnum[i] = (unsigned char) getInt(Listitem(argv[1], i));
  memcpy(&remote.sin_addr, hostnum, len);
  free(hostnum);
  
  if( connect(sock, (struct sockaddr *)&remote, sizeof(remote)) < 0 ) return NullValue;
  return newBool(1);
}

/*-------------------------------------------------------------*/
Value *prim_unix_bind(Script *script, char *proc_name, int call_data, 
		      int argc, Value **argv)
{
  struct sockaddr_in local;
  unsigned char *hostnum;
  int len, i, sock, portnum;
  
  checkCount(3, "socket, host address, port number");
  checkInt(0, "socket");
  checkInt(2, "port number");
  
  sock = getInt(argv[0]);
  portnum = getInt(argv[2]);

  local.sin_family = AF_INET;
  local.sin_port = htons((unsigned short)portnum);  

  if(isNull(argv[1])) local.sin_addr.s_addr = INADDR_ANY;
  else {
    checkIntList(1, "host address");
    len = Listsize(argv[1]);
    hostnum = malloc(len);
    for(i = 0; i < len; i++)
      hostnum[i] = (unsigned char) getInt(Listitem(argv[1], i));
    memcpy(&local.sin_addr, hostnum, len);
    free(hostnum);
  }

  if( bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0 ) return NullValue;
  return newBool(1);
}

/*-------------------------------------------------------------*/
Value *prim_unix_listen(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  int sock, backlog;
  
  checkCountLeast(1, "socket, backlog");
  checkInt(0, "socket");
  if(argc > 1) checkInt(1, "backlog");
  
  sock = getInt(argv[0]);
  if(argc > 1) backlog = getInt(argv[1]);
  else backlog = MAX_PENDING_CONNECTIONS;
  
  if( listen(sock, backlog) < 0 ) return NullValue;
  return newBool(1);
}

/*-------------------------------------------------------------*/
Value *prim_unix_accept(Script *script, char *proc_name, int call_data, 
			int argc, Value **argv)
{
  struct sockaddr_in remote;
  int *hostnum;
  int len, i, remotelen, sock, newsock;
  Value *sockval, *addrval, *retval;
  
  checkCount(1, "socket");
  checkInt(0, "socket");
  sock = getInt(argv[0]);

  if( (newsock = accept(sock, (struct sockaddr *) &remote, &remotelen)) < 0 ) 
    return NullValue;
  
  len = 4;
  hostnum = malloc(len * sizeof(int));
  for(i = 0; i < len; i++) hostnum[i] = ((unsigned char *) &remote.sin_addr)[i];

  sockval = newInt(newsock);
  addrval = newIntList(len, hostnum);
  free(hostnum);
  
  retval = newListva(2, sockval, addrval);
  
  freeValue(sockval);
  freeValue(addrval);

  return retval;
}


