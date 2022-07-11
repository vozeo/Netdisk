#include <stdio.h>
#include <unistd.h>         // for sleep ...
#include <sys/socket.h>     // for ...
#include <fcntl.h>          // for fcntl
#include <arpa/inet.h>      // for htonl
#include <errno.h>          // for errno
#include <netdb.h>          // for getnameinfo
#include <string.h>         // for strcpy ...
#include "my_tools.h"
#include "my_getopt.h"

int tmpsocket_socket()
{
    int flag=1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("err in socket(): %d\n", errno);
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    return sockfd;
}

int tmpsocket_socket_nonblock()
{
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd < 0) {
        printf("err in socket(): %d\n", errno);
        return -1;
    }
    return sockfd;
}

int tmpsocket_bind(int sockfd, const char *ip, int port)
{
    struct sockaddr_in my_addr;
    my_addr.sin_family      = AF_INET;
    my_addr.sin_port        = htons(port);
    my_addr.sin_addr.s_addr = (ip ? inet_addr(ip) : htonl(INADDR_ANY));
    if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) < 0) {
        printf("err in bind(): %d\n", errno);
        return -1;
    }
    return 0;
}

int tmpsocket_listen(int sockfd, int backlog)
{
    if (listen(sockfd, backlog) < 0) {
        printf("err in listen(): %d\n", errno);
        return -1;
    }
    return 0;
}

int tmpsocket_accept(int sockfd, char *new_ip, int *new_port)
{
    int new_sockfd;
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    new_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &sin_size);
    if (new_sockfd < 0) {
        printf("err in accept(): %d\n", errno);
        return -1;
    }
    char buf1[20], buf2[20];
    getnameinfo((struct sockaddr*)&client_addr, sin_size,
                buf1, 20,
                buf2, 20, NI_NUMERICHOST|NI_NUMERICSERV);
    if (new_ip) strcpy(new_ip, buf1);
    if (new_port) string_to_int(buf2, new_port, 0, 2147483647);
    return new_sockfd;
}

int tmpsocket_connect(int sockfd, const char *ip, int port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("err in connect(): %d\n", errno);
        return -1;
    }
    return 0;
}

int tmpsocket_connect_noblock(int sockfd, const char *ip, int port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    printf("%s:%d\n", ip, port);
    for (;;) {
        int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
        if (ret == 0) {
            close(sockfd);
            printf("err in connect: return 0\n");
            return -1;
        }
        else if (ret == -1) {
            if (errno == EINTR) {   // connect 被信号中断
                printf("connecting interruptted by signal, try again.\n");
                continue;
            }
            else if (errno == EINPROGRESS) {    // 正常，连接正在进行中
                break;
            }
            else {  // 真的出错了
                printf("error in connect(), errno %d\n", errno);
                return -1;
            }
        }
    }

    // 调用 select 检查 socket 是否出错
    fd_set read_fds;
    fd_set write_fds;
    fd_set err_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&err_fds);
    FD_SET(sockfd, &read_fds);
    FD_SET(sockfd, &write_fds);
    FD_SET(sockfd, &err_fds);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (select(sockfd+1, NULL, &write_fds, NULL, &tv) != 1 || !FD_ISSET(sockfd, &write_fds)) {
        printf("err in connect, select.\n");
        return -1;
    }

    // 调用 getsockopt 检测此时 socket 是否出错
    int err;
    socklen_t len = sizeof(err);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
        printf("err in connect, getsockopt.\n");
        close(sockfd);
        return -1;
    }

    return 0;
}

int tmpsocket_set_nonblock(int sockfd)
{
    int old_flags = fcntl(sockfd, F_GETFL, 0);
    int new_flags = old_flags | O_NONBLOCK;
    return fcntl(sockfd, F_SETFL, new_flags);
}

int open_listen_sockfd(const char *ip, int port, int backlog)
{
    int sockfd;
    if ((sockfd = tmpsocket_socket()) < 0) return -1;
    if (tmpsocket_bind(sockfd, ip, port) < 0) {
        close(sockfd);
        return -1;
    }
    if (tmpsocket_listen(sockfd, backlog) < 0) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int open_connect_sockfd(const char *ip, int port, int myport)
{
    int sockfd;
    if ((sockfd = tmpsocket_socket()) < 0) return -1;
    if (myport>=0 && tmpsocket_bind(sockfd, NULL, myport) < 0) {
        close(sockfd);
        return -1;
    }
    if (tmpsocket_connect(sockfd, ip, port) < 0) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int open_listen_sockfd_noblock(const char *ip, int port, int backlog)
{
    int sockfd;
    if ((sockfd = tmpsocket_socket()) < 0) return -1;
    tmpsocket_set_nonblock(sockfd);
    if (tmpsocket_bind(sockfd, ip, port) < 0) {
        close(sockfd);
        return -1;
    }
    if (tmpsocket_listen(sockfd, backlog) < 0) {
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int open_connect_sockfd_noblock(const char *ip, int port, int myport)
{
    int sockfd;
    if ((sockfd = tmpsocket_socket()) < 0) return -1;
    tmpsocket_set_nonblock(sockfd);
    if (myport>=0 && tmpsocket_bind(sockfd, NULL, myport) < 0) {
        close(sockfd);
        return -1;
    }
    if (tmpsocket_connect_noblock(sockfd, ip, port) < 0) {
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

int open_accept_sockfd_noblock(int sockfd, char *new_ip, int *new_port)
{
    int new_sockfd;
    int tmp;
    fd_set read_fds;
    fd_set write_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_SET(sockfd, &read_fds);
    tmp = select(sockfd+1, &read_fds, &write_fds, NULL, NULL);
    if (tmp <= 0) {
        printf("err in select before accept.\n");
        return -1;
    }
    if ((new_sockfd = tmpsocket_accept(sockfd, new_ip, new_port)) < 0) return -1;
    tmpsocket_set_nonblock(new_sockfd);
    return new_sockfd;
}