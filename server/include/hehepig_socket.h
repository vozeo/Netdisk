#ifndef HEHEPIG_SOCKET___
#define HEHEPIG_SOCKET___



//======================================================================
// Socket ��غ���
// ��ʱ�Ĺ��ú������Ժ�����˿�������һ���ļ������common�
// ������д��ÿ��С��ĳ��������ҲӦ���ǿ���ֱ�Ӹ��Ƶ�
// ����ͳһһ�£�const char *ip �� NULL ʱ���� INADDR_ANY
//              port �� ����ʱ�����󶨶˿�
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