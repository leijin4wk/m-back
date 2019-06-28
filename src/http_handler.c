//
// Created by oyo on 2019-06-28.
//
#include <sys/socket.h>
#include "http_handler.h"
#include "dbg.h"
#include "http_parser.h"

void handler_request(void *ptr) {
    int* fd=(int*)ptr;
    log_info("handler request fd %d", *fd);
    size_t len = 80*1024;
    char buf[len];
    ssize_t recved;
    recved = recv(*fd, buf, len, 0);
    if (recved < 0) {
        log_err("recv err!");
    }
    log_info("%s\n",buf);
}