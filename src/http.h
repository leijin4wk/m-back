//
// Created by oyo on 2019-06-28.
//

#ifndef M_BACK_HTTP_H
#define M_BACK_HTTP_H
#include <openssl/ssl.h>
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
    int event_type;
    char *client_ip;
    SSL *ssl;
    void (*handler)(struct http_request *request,struct http_request *response);
    struct http_request *request;
    char* request_data;
    int request_data_len;
    struct http_request *response;
    char* response_data;
    int response_data_len;
};


#define alloc_cpy(dest, src, len) \
    dest = malloc(len + 1);\
    memcpy(dest, src, len);\
    dest[len] = '\0';
void handler_request(void *ptr);

#endif //M_BACK_HTTP_H
