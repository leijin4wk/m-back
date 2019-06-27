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
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;
    char portStr[10];
    sprintf(portStr, "%d", port);
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;     /* 设置为ipV4 */
    hints.ai_socktype = SOCK_STREAM; /* 设置为tcp模式 */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */
    /**
     * 完成主机名到地址解析
     * (1)hostname:一个主机名或者地址串(IPv4的点分十进制串或者IPv6的16进制串)
     * (2)service：服务名可以是十进制的端口号，也可以是已定义的服务名称，如ftp、http等
     * (3)hints：可以是一个空指针，也可以是一个指向某个addrinfo结构体的指针，调用者在这个结构中填入关于期望返回的信息类型的暗示。
     * 举例来说：如果指定的服务既支持TCP也支持UDP，那么调用者可以把hints结构中的ai_socktype成员设置成SOCK_DGRAM使得返回的仅仅是适用于数据报套接口的信息。
     * (4)result：本函数通过result指针参数返回一个指向addrinfo结构体链表的指针。
     */
    s = getaddrinfo(NULL, portStr, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
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
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;
        /**
         *
         （1）socket:套接字描述符，是函数socket的返回值
         （2）address：sockadrr是本地的地址（服务器的地址），是一个sockaddr结构指针，该结构中包含了要进行结合的IPD地址和端口号
         （3）address_len：是address缓冲区的长度
            成功：返回0
            失败：返回-1
         */

        s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0) {
            /* We managed to bind successfully! */
            break;
        }
        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }
    /**
     * 由getaddrinfo返回的存储空间，包括addrinfo结构、ai_addr结构和ai_canonname字符串，都是用malloc动态获取的。这些空间可调用 freeaddrinfo释放。
     */
    freeaddrinfo(result);
    return sfd;
}

int socket_listen(int socket_fd){
    /**
     * 功能 ：开始监听（服务器不知道需要与谁进行连接，因此，她不会主动的要求与某个进程连接，
     * 只是一直监听是否有其他的用户进程与之连接，然后响应该连接请求，并对他做出处理（一个服务器可同时处理多个客户机的请求连接） ）
     *
     *  (1)socket:是socket函数的返回值（此参数用于标识一个已经捆绑但是未连接的套接字描述符
     *  (2)backlog：同一时刻，操作系统允许的最大的可进行连接的客户端数（提示内核监听队列的最大长度
     *  ps：监听队列的长度如果超过backlog，服务器将不再受理新的客户连接，客户端也将收到ECONNREFUSED错误信息，而且一般可以连接的客户端数为backlog+1.
     */
    if (listen(socket_fd, LISTENQ) < 0) {
        fprintf(stderr, "Could not listen socket\n");
        return -1;
    }
    return socket_fd;
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