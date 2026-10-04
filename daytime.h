#ifndef DAYTIME_H
#define DAYTIME_H

/* Server address/port used by the client, port the server binds to. */
#define SERVER_IP    "172.233.157.24"
#define SERVER_PORT  13013          /* real Daytime is 13, which needs admin/root */
#define BUFFER_SIZE  256
#define RECV_TIMEOUT_SEC 5          /* give up if no valid reply arrives */

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef int socklen_t_;
  #define CLOSESOCK closesocket
  #define SOCK_INIT() do { WSADATA w; if (WSAStartup(MAKEWORD(2,2), &w)) { fprintf(stderr, "WSAStartup failed\n"); exit(1); } } while (0)
  #define SOCK_CLEANUP() WSACleanup()
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  typedef socklen_t socklen_t_;
  typedef int SOCKET;
  #define INVALID_SOCKET (-1)
  #define CLOSESOCK close
  #define SOCK_INIT() ((void)0)
  #define SOCK_CLEANUP() ((void)0)
#endif

#endif
