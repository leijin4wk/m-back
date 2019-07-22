//
// Created by oyo on 2019-06-28.
//

#ifndef M_BACK_HTTP_H
#define M_BACK_HTTP_H
#include <openssl/ssl.h>
#include <zdb.h>

//http 请求最大长度 2M
#define MAX_REQUEST_SIZE 2*1024*1024
#define MAX_RESPONSE_SIZE 2*1024*1024
//每次读取数据最大 8K
#define MAX_LINE 4096

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
    ConnectionPool_T data_pool;
};

struct http_response {
    struct http_header *headers;
    char *body;
};
#endif //M_BACK_HTTP_H
