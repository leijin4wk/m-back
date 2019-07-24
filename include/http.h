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


// 保存HTTP报文头部的字段的链表
struct http_header {
    char *name;
    char *value;
    struct http_header *next;
};
struct http_param{
    char *name;
    char *value;
    struct http_param *next;
};
// HTTP请求的结构提
struct http_request {

    char *url;
    int method;
    char *path;
    char *query_str;
    struct http_param *query_param;
    char *body;
    unsigned int flags;
    unsigned short http_major, http_minor;
    struct http_header *headers;
    ConnectionPool_T data_pool;
};

struct http_response {
    unsigned int code;
    char* status;
    unsigned short http_major, http_minor;
    struct http_header *headers;
    char *body;
};
#endif //M_BACK_HTTP_H
