//
// Created by oyo on 2019-07-23.
//

#ifndef M_BACK_HTTP_BUFFER_H
#define M_BACK_HTTP_BUFFER_H
#include "http.h"
#define alloc_cpy(dest, src, len) \
    dest = malloc(len + 1);\
    memcpy(dest, src, len);\
    dest[len] = '\0';
struct http_parser* parser_http_request_buffer(struct Buffer *buf);
struct Buffer *create_http_response_buffer(struct http_response *http_response);
#endif //M_BACK_HTTP_BUFFER_H
