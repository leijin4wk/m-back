//
// Created by oyo on 2019-06-27.
//

#ifndef M_BACK_SOCKET_H
#define M_BACK_SOCKET_H


#define LISTENQ     1024
/**
 * 创建socket 并绑定port,并监听队列
 * @param port
 * @return
 */
int create_and_bind (int port);

/**
 * 设置socket为非阻塞的
 * @param fd
 * @return
 */
int make_socket_non_blocking(int fd);
#endif //M_BACK_SOCKET_H
