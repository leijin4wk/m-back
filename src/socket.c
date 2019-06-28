//
// Created by oyo on 2019-06-27.
//
#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include "dbg.h"

//函数:
//功能:创建和绑定一个TCP socket
//参数:端口
//返回值:创建的socket
int create_and_bind(int port) {
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;

    /**
        *
        (1)domain（family）：表示套接字的通信域，不同的取值决定了socket的地址类型，其一般取值如下：
             AF_INET:IPV4因特网域
             AF_INET6：IPV6因特网域
             AF_UNIX:UNIX域
             AF_ROUTE:路由套接字

        (2)type:是socket的类型，常用如下：
            SOCK_STREAM:有序、可靠、双向的面向连接--->字节流套接字，例如TCP
            SOCK_DGRAM:长度固定的、无连接的不可靠的--->数据报套接字，例如UDP
            SOCK_RAW:原始套接字

         (3)protocol:指定协议，取值如下：
            0：表示选择type类型对应的默认协议
            IPPROROTO_TCP:TCP传输协议
            IPPROTO_UDP:UDP传输协议
            IPPRPTO_SCTP:SCTP传输协议
            IPPROTO_TIPC:TIPC传输协议
            成功：返回一个整数，是socket的文件描述符
            失败：返回-1
        */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* Listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)port);
    /**
         *
         （1）socket:套接字描述符，是函数socket的返回值
         （2）address：sockadrr是本地的地址（服务器的地址），是一个sockaddr结构指针，该结构中包含了要进行结合的IPD地址和端口号
         （3）address_len：是address缓冲区的长度
            成功：返回0
            失败：返回-1
         */
    if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /**
     * 功能 ：开始监听（服务器不知道需要与谁进行连接，因此，她不会主动的要求与某个进程连接，
     * 只是一直监听是否有其他的用户进程与之连接，然后响应该连接请求，并对他做出处理（一个服务器可同时处理多个客户机的请求连接） ）
     *
     *  (1)socket:是socket函数的返回值（此参数用于标识一个已经捆绑但是未连接的套接字描述符
     *  (2)backlog：同一时刻，操作系统允许的最大的可进行连接的客户端数（提示内核监听队列的最大长度
     *  ps：监听队列的长度如果超过backlog，服务器将不再受理新的客户连接，客户端也将收到ECONNREFUSED错误信息，而且一般可以连接的客户端数为backlog+1.
     */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;

    return listenfd;
}

int make_socket_non_blocking(int fd) {
    int flags, s;
    //得到文件状态标志
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        log_err("fcntl");
        return -1;
    }
    //设置文件状态标志
    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
        log_err("fcntl");
        return -1;
    }
    return 0;
}