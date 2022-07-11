#ifndef HEHEPIG_SOCKET___
#define HEHEPIG_SOCKET___



//======================================================================
// Socket 相关函数
// 临时的公用函数，以后成熟了可以整到一个文件里面放common里，
// 现在先写在每个小题的程序里，不过也应该是可以直接复制的
// 大体统一一下，const char *ip 是 NULL 时代表 INADDR_ANY
//              port 是 负数时代表不绑定端口
/*
#include <stdio.h>
#include <unistd.h>         // for sleep ...
#include <sys/socket.h>     // for ...
#include <fcntl.h>          // for fcntl
#include <arpa/inet.h>      // for htonl
#include <errno.h>          // for errno
#include <netdb.h>          // for getnameinfo
#include <string.h>         // for strcpy ...
#include "../../include/my_tools.h"
#include "../../include/my_getopt.h"
*/
//======================================================================


int tmpsocket_socket();
int tmpsocket_socket_nonblock();
int tmpsocket_bind(int sockfd, const char *ip, int port);
int tmpsocket_listen(int sockfd, int backlog);
int tmpsocket_accept(int sockfd, char *new_ip, int *new_port);
int tmpsocket_connect(int sockfd, const char *ip, int port);

int tmpsocket_set_nonblock(int sockfd);

int open_listen_sockfd(const char *ip, int port, int backlog);
int open_connect_sockfd(const char *ip, int port, int myport);

int open_listen_sockfd_noblock(const char *ip, int port, int backlog);
int open_connect_sockfd_noblock(const char *ip, int port, int myport);
int open_accept_sockfd_noblock(int sockfd, char *new_ip, int *new_port);
int tmpsocket_connect_noblock(int sockfd, const char *ip, int port);

#endif