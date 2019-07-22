//
// Created by oyo on 2019-06-28.
//
#include <sys/socket.h>
#include <http_parser.h>
#include "../include/http.h"
#include "dbg.h"
#include "buffer.h"
#include "ssl_tool.h"
static struct http_request *new_http_request();
static void delete_http_request(struct http_request *request);
static struct http_header *new_http_header();
static void delete_http_header(struct http_header *header);
static inline struct http_header *add_http_header(struct http_request *request);
static int on_message_begin(http_parser* parser);
static int on_url(http_parser* parser, const char* at, size_t length);
static int on_header_field(http_parser* parser, const char* at, size_t length);
static  int on_headers_complete(http_parser* parser);
static int on_header_value(http_parser* parser, const char* at, size_t length);
static int on_message_complete(http_parser* parser);
static int on_body(http_parser* parser, const char* at, size_t length);

http_parser_settings parser_set= {
        .on_message_begin = on_message_begin,
        .on_header_field = on_header_field,
        .on_header_value = on_header_value,
        .on_url = on_url,
        .on_body = on_body,
        .on_headers_complete = on_headers_complete,
        .on_message_complete = on_message_complete
};


// 初始化一个新的HTTP请求
struct http_request *new_http_request() {
    struct http_request *request = malloc(sizeof(struct http_request));
    request->headers = NULL;
    request->url = NULL;
    request->body = NULL;
    request->flags = 0;
    request->http_major = 0;
    request->http_minor = 0;
    return request;
}
// 删除一个HTTP请求
static  void delete_http_request(struct http_request *request) {
    if (request->url != NULL) free(request->url);
    if (request->body != NULL) free(request->body);
    struct http_header *header = request->headers;
    while (header != NULL) {
        struct http_header *to_delete = header;
        header = header->next;
        delete_http_header(to_delete);
    }
    free(request);
}
// 初始化一个新的头结点
static struct http_header *new_http_header() {
    struct http_header *header = malloc(sizeof(struct http_header));
    header->name = NULL;
    header->value = NULL;
    header->next = NULL;
    return header;
}
//释放一个头结点内存
static void delete_http_header(struct http_header *header) {
    if (header->name != NULL) free(header->name);
    if (header->value != NULL) free(header->value);
    free(header);
}
// 将一个空的HTTP头部字段附件到字段链表的尾部
// 返回值为创建的新节点
static struct http_header *add_http_header(struct http_request *request) {
    struct http_header *header = request->headers;
    while (header != NULL) {
        if (header->next == NULL) {
            header->next = new_http_header();
            return header->next;
        }
        header = header->next;
    }
    request->headers = new_http_header();
    return request->headers;
}

static int on_message_begin(http_parser* parser) {
    log_info("***MESSAGE BEGIN***");
    parser->data=new_http_request();
    return 0;
}

static int on_url(http_parser* parser, const char* at, size_t length) {
    printf("Url: %.*s", (int)length, at);
    struct http_request *request = (struct http_request *) parser->data;
    request->method = (char *)http_method_str(parser->method);
    request->http_major = parser->http_major;
    request->http_minor = parser->http_minor;
    alloc_cpy(request->url, at, length)
    return 0;
}

static int on_header_field(http_parser* parser, const char* at, size_t length) {
    struct http_request *request = (struct http_request *) parser->data;
    struct http_header *header = add_http_header(request);
    alloc_cpy(header->name, at, length)
    return 0;
}

static int on_header_value(http_parser* parser, const char* at, size_t length) {
    struct http_request *request = (struct http_request *) parser->data;
    struct http_header *header = request->headers;
    while (header->next != NULL) {
        header = header->next;
    }
    alloc_cpy(header->value, at, length)
   return 0;
}
static int on_headers_complete(http_parser* parser) {
    log_info("***HEADERS COMPLETE***");
    return 0;
}
static int on_body(http_parser* parser, const char* at, size_t length) {
    struct http_request *request = (struct http_request *) parser->data;
    alloc_cpy(request->body, at, length)
    return 0;
}
static int on_message_complete(http_parser* parser) {
    log_info("***MESSAGE COMPLETE***");
    return 0;
}