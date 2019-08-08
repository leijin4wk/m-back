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
struct http_request *new_http_request();
void delete_http_request(struct http_request *request);
struct http_response *new_http_response();
void delete_http_response(struct http_response * response);
int check_http_request_header_value(struct http_request *http_request,char * name,char* value);
void get_error_status_body(struct http_response *http_response,int code);
#endif //M_BACK_HTTP_BUFFER_H
