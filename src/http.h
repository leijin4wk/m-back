//
// Created by oyo on 2019-06-28.
//

#ifndef M_BACK_HTTP_H
#define M_BACK_HTTP_H
#include <openssl/ssl.h>
#include "buffer.h"
//http 请求最大长度 2M
#define MAX_REQUEST_SIZE 2*1024*1024
//每次读取数据最大 8K
#define MAX_LINE 8192

#define alloc_cpy(dest, src, len) \
    dest = malloc(len + 1);\
    memcpy(dest, src, len);\
    dest[len] = '\0';

// 保存HTTP报文头部的字段的链表
struct http_header {
    char *name;
    char *value;
    struct http_header *next;
};
// HTTP请求的结构提
struct http_request {
    char *method;
    char *url;
    char *body;
    unsigned int flags;
    unsigned short http_major, http_minor;
    struct http_header *headers;
};

struct http_response {
    struct http_header *headers;
    char *body;
};

struct http_client{
    int event_fd;
    char *client_ip;
    SSL *ssl;
    struct http_request *request;
    struct Buffer* request_data;
    struct http_response *response;
    struct Buffer* response_data;
    void (*handler)(struct http_request *request,struct http_request *response);
};


struct http_client* new_http_client();

void free_http_client(struct http_client* client);

int  parser_http_request_buffer(struct http_client *client);

int create_http_response_buffer(struct http_client* client);

int http_process(struct http_client *client);

#endif //M_BACK_HTTP_H
