//
// Created by oyo on 2019-06-27.
//

#ifndef M_BACK_SOCKET_H
#define M_BACK_SOCKET_H


#define LISTENQ     1024
/**
 * 创建socket 并绑定port
 * @param port
 * @return
 */
int create_and_bind (int port);

/**
 * socket绑定监听队列
 * @param socket_fd
 * @return
 */
int socket_listen(int socket_fd);

/**
 * 设置socket为非阻塞的
 * @param fd
 * @return
 */
int make_socket_non_blocking(int fd);
#endif //M_BACK_SOCKET_H
