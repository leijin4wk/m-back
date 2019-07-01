//
// Created by oyo on 2019-06-28.
//
#include <sys/socket.h>
#include "http_parser.h"
#include "http.h"
#include "dbg.h"
//http://ju.outofmemory.cn/entry/111438
// http_cb      on_message_begin;
//  http_data_cb on_url;
//  http_data_cb on_status;
//  http_data_cb on_header_field;
//  http_data_cb on_header_value;
//  http_cb      on_headers_complete;
//  http_data_cb on_body;
//  http_cb      on_message_complete;
//  /* When on_chunk_header is called, the
//  current chunk length is stored
//   * in parser->content_length.
//   */
//  http_cb      on_chunk_header;
//  http_cb      on_chunk_complete;

int my_on_url(http_parser* parser, const char *at, size_t length) {
    log_info("my_on_url: %s\n",*at);
    return 0;
}

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
    http_parser_settings settings;
    settings.on_url=my_on_url;
    http_parser *parser = malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    int nparsed = http_parser_execute(parser, &settings, buf, recved);
    log_info("%s\n",buf);
    log_info("size %d\n",recved);
    log_info("nparsed %d\n",nparsed);
    log_info("aaaaa\n");
    char* a="aa";
}
