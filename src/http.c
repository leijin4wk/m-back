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

int on_message_begin(http_parser* parser) {
   (void)parser;
   printf("\n***MESSAGE BEGIN***\n\n");
   return 0;
}

int on_headers_complete(http_parser* parser) {
   (void)parser;
   printf("\n***HEADERS COMPLETE***\n\n");
   return 0;
}

int on_message_complete(http_parser* parser) {
   (void)parser;
   printf("\n***MESSAGE COMPLETE***\n\n");
   return 0;
}


int on_url(http_parser* parser, const char* at, size_t length) {
   (void)parser;
   printf("Url: %.*s\n", (int)length, at);


   return 0;
}

int on_header_field(http_parser* parser, const char* at, size_t length) {
   (void)parser;
   printf("Header field: %.*s\n", (int)length, at);
   return 0;
}

int on_header_value(http_parser* parser, const char* at, size_t length) {
   (void)parser;
   printf("Header value: %.*s\n", (int)length, at);
   return 0;
}

int on_body(http_parser* parser, const char* at, size_t length) {
   (void)parser;
   printf("Body: %.*s\n", (int)length, at);
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
    struct http_parser* parser = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST); // 初始化parser为Request类型
    http_parser_settings parser_set;
    parser_set.on_message_begin = on_message_begin;
    parser_set.on_header_field = on_header_field;
    parser_set.on_header_value = on_header_value;
    parser_set.on_url = on_url;
    parser_set.on_body = on_body;
    parser_set.on_headers_complete = on_headers_complete;
    parser_set.on_message_complete = on_message_complete;

    http_parser_execute(parser, &parser_set, buf, strlen(buf));
    log_info("size %d\n",recved);
    log_info("aaaaa\n");
    char* a="aa";
}
