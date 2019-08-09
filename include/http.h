//
// Created by oyo on 2019-06-28.
//

#ifndef M_BACK_HTTP_H
#define M_BACK_HTTP_H
#include <openssl/ssl.h>
//动态数据
#define DYNAMIC_DATA 0
//静态数据
#define STATIC_DATA 1

//http 请求最大长度 2M
#define MAX_REQUEST_SIZE 2*1024*1024
#define MAX_RESPONSE_SIZE 2*1024*1024
//每次读取数据最大 8K
#define MAX_LINE 4096
typedef struct mime_type_s {
    const char *type;
    const char *value;
} mime_type_t;

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
    //Transfer-Encoding等判断
    unsigned int flags;
    unsigned short http_major, http_minor;
    struct http_header *headers;
};

struct http_response {
    unsigned short http_major, http_minor;
    unsigned int code;
    struct http_header *headers;
    int data_type;
    char* real_file_path;
    size_t real_file_size;
    char *body;
};
struct http_header *add_http_response_header(struct http_response *response);
#endif //M_BACK_HTTP_BUFFER_H
