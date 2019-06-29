//
// Created by oyo on 2019-06-28.
//
#include <sys/socket.h>
#include "http_parser.h"
#include "http.h"
#include "dbg.h"

void handler_request(void *ptr) {

    http_parser_settings settings;

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
    log_info("size %d\n",recved);
    log_info("aaaaa\n");
    char* a="aa";
}
