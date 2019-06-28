//
// Created by oyo on 2019-06-28.
//
#include "http_handler.h"
#include "dbg.h"

void handler_request(void *ptr) {
    int* fd=(int*)ptr;
    log_info("handler request fd %d", *fd);

}