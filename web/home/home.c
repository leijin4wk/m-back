//
// Created by oyo on 2019-07-22.
//
#include "dbg.h"
#include "http.h"
void home_index(struct http_request* request,struct http_response* response)
{
    log_info("this is home");
    struct http_header* header=add_http_response_header(response);
    header->name="Content-Type";
    header->value="text/html;charset=utf-8";
    response->body="<!DOCTYPE html>\n"
                   "<html lang=\"en\">\n"
                   "<head>\n"
                   "    <meta charset=\"UTF-8\">\n"
                   "    <title>Title</title>\n"
                   "</head>\n"
                   "<body>\n"
                   "<h1>这是首页</h1>\n"
                   "</body>\n"
                   "</html>";
}
