//
// Created by oyo on 2019-07-22.
//
#include "buffer.h"
#include "dbg.h"
#include "http.h"
void home_index(struct http_request* request,struct http_response* response)
{
    log_info("this is home");
    struct http_header* header=add_http_response_header(response);
    header->name=strdup("Content-Type");
    header->value=strdup("text/html;charset=utf-8");
    struct Buffer* body=new_buffer(MAX_READLINE,10*MAX_READLINE);
    const char* a="<!DOCTYPE html><html lang=\34en\34";
    buffer_add(body,a,strlen(a));
    const char* b="<head><meta charset=\34UTF-8\34><title>Title</title></head";
    buffer_add(body,b,strlen(b));
    const char* c="<body><h1>这是首页</h1></body>";
    buffer_add(body,c,strlen(c));
    const char* d="</html>";
    buffer_add(body,d,strlen(d));
    response->body=buffer_to_string(body);
}
