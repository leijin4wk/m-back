//
// Created by oyo on 2019-07-23.
//

#ifndef M_BACK_HTTP_BUFFER_H
#define M_BACK_HTTP_BUFFER_H
#include "http.h"
#include "buffer.h"
#define alloc_cpy(dest, src, len) \
    dest = malloc(len + 1);\
    memcpy(dest, src, len);\
    dest[len] = '\0';
struct http_request* parser_http_request_buffer(struct Buffer *buf);
struct Buffer *create_http_response_buffer(struct http_response *http_response);
int check_http_request_header_value(struct http_request *http_request,char * name,char* value);
#endif //M_BACK_HTTP_BUFFER_H
