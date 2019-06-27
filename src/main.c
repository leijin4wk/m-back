#include <stdio.h>
#include "socket.h"
#include "dbg.h"

int main() {
    int flag,socket_fd,listenfd;
    socket_fd=create_and_bind(8888);
    listenfd=socket_listen(socket_fd);
    flag = make_socket_non_blocking(listenfd);
    check(flag == 0, "make_socket_non_blocking");
    printf("aaa\n");
    return 0;
}