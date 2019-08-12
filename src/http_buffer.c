//
// Created by oyo on 2019-06-28.
//
#include <sys/socket.h>
#include <stdlib.h>
#include "http_parser.h"
#include "dbg.h"
#include "ssl_tool.h"
#include "http_buffer.h"
#include "str_tool.h"
static const char* get_file_type(const char *type);
static struct http_header *new_http_header();
static void delete_http_header(struct http_header *header);
static void delete_http_param(struct http_param *param);
static inline struct http_header *add_http_request_header(struct http_request *request);
static int on_message_begin(http_parser* parser);
static int on_url(http_parser* parser, const char* at, size_t length);
static int on_header_request_field(http_parser* parser, const char* at, size_t length);
static  int on_headers_complete(http_parser* parser);
static int on_header_request_value(http_parser* parser, const char* at, size_t length);
static int on_message_complete(http_parser* parser);
static int on_body(http_parser* parser, const char* at, size_t length);
static int parser_query_param(struct http_request *request,const char *buf, size_t buf_len) ;
static int set_param_field(struct http_request *request, const char* at, size_t length);
static int set_param_value(struct http_request *request, const char* at, size_t length);
http_parser_settings parser_set= {
        .on_message_begin = on_message_begin,
        .on_header_field = on_header_request_field,
        .on_header_value = on_header_request_value,
        .on_url = on_url,
        .on_body = on_body,
        .on_headers_complete = on_headers_complete,
        .on_message_complete = on_message_complete
};
//媒体类型
mime_type_t mime_type[] =
        {
                {".html", "text/html"},
                {".xml", "text/xml"},
                {".xhtml", "application/xhtml+xml"},
                {".txt", "text/plain"},
                {".rtf", "application/rtf"},
                {".pdf", "application/pdf"},
                {".word", "application/msword"},
                {".png", "image/png"},
                {".gif", "image/gif"},
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {".au", "audio/basic"},
                {".mpeg", "video/mpeg"},
                {".mpg", "video/mpeg"},
                {".avi", "video/x-msvideo"},
                {".gz", "application/x-gzip"},
                {".tar", "application/x-tar"},
                {".css", "text/css"},
                {NULL, "text/plain"}
        };
//获取媒体类型
static const char* get_file_type(const char *type){
    if (type == NULL) {
        return "text/plain";
    }

    int i;
    for (i = 0; mime_type[i].type != NULL; ++i) {
        if (strcmp(type, mime_type[i].type) == 0)
            return mime_type[i].value;
    }
    return mime_type[i].value;
}
// 初始化一个新的HTTP请求
struct http_request *new_http_request() {
    struct http_request *request = malloc(sizeof(struct http_request));
    request->headers = NULL;
    request->url = NULL;
    request->mime_type=NULL;
    request->method=-1;
    request->body = NULL;
    request->query_str= NULL;
    request->query_param=NULL;
    request->flags = 0;
    request->http_major = 0;
    request->http_minor = 0;
    return request;
}
// 删除一个HTTP请求
void delete_http_request(struct http_request *request) {
    if (request->url != NULL) free(request->url);
    if (request->body != NULL) free(request->body);
    if (request->query_str!=NULL) free(request->query_str);
    if (request->mime_type!=NULL) free(request->mime_type);
    struct http_header *header = request->headers;
    while (header != NULL) {
        struct http_header *to_delete = header;
        header = header->next;
        delete_http_header(to_delete);
    }
    struct http_param *param = request->query_param;
    while (param != NULL) {
        struct http_param *to_delete = param;
        param = param->next;
        delete_http_param(to_delete);
    }
    free(request);
    request=NULL;
}
//初始化一个response相应
struct http_response *new_http_response() {
    struct http_response *response = malloc(sizeof(struct http_response));
    response->http_major = 1;
    response->http_minor = 1;
    response->code = 200;
    response->headers = NULL;
    response->body = NULL;
    response->real_file_path = NULL;
    response->data_type = -1;
    struct http_header *header = add_http_response_header(response);
    header->name = strdup("Server");
    header->value = strdup("LeiJin/m_back");
    return response;
}
//删除一个response
void delete_http_response(struct http_response * response){
    if (response->body != NULL) free(response->body);
    if (response->real_file_path != NULL) free(response->real_file_path);
    struct http_header *header = response->headers;
    while (header != NULL) {
        struct http_header *to_delete = header;
        header = header->next;
        delete_http_header(to_delete);
    }
    free(response);
    response=NULL;
}
// 初始化一个新的头结点
struct http_header *new_http_header() {
    struct http_header *header = malloc(sizeof(struct http_header));
    header->name = NULL;
    header->value = NULL;
    header->next = NULL;
    return header;
}
//初始化一个http请求param
static struct http_param *new_http_param() {
    struct http_param *param = malloc(sizeof(struct http_param));
    param->name = NULL;
    param->value = NULL;
    param->next = NULL;
    return param;
}
//释放一个头结点内存
static void delete_http_header(struct http_header *header) {
    if (header->name != NULL) free(header->name);
    if (header->value != NULL) free(header->value);
    free(header);
}
//释放一个param结点内存
static void delete_http_param(struct http_param *param) {
    if (param->name != NULL) free(param->name);
    if (param->value != NULL) free(param->value);
    free(param);
}
// 将一个空的HTTP头部字段附件到字段链表的尾部
// 返回值为创建的新节点
static struct http_header *add_http_request_header(struct http_request *request) {
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

// 将一个空的HTTP param字段附件到字段链表的尾部
// 返回值为创建的新节点
static struct http_param *add_http_param(struct http_request *request) {
    struct http_param *param = request->query_param;
    while (param != NULL) {
        if (param->next == NULL) {
            param->next = new_http_param();
            return param->next;
        }
        param = param->next;
    }
    request->query_param = new_http_param();
    return request->query_param;
}

static int on_message_begin(http_parser* parser) {
    parser->data=new_http_request();
    return 0;
}

static int on_url(http_parser* parser, const char* at, size_t length) {
    struct http_parser_url* http_parser_url=malloc(sizeof(struct http_parser_url));
    http_parser_url_init(http_parser_url);
    http_parser_parse_url(at, length,0,http_parser_url);
    struct http_request *request = (struct http_request *) parser->data;
    request->method = parser->method;
    alloc_cpy(request->url, at, length)
    alloc_cpy(request->path, request->url+http_parser_url->field_data[3].off, http_parser_url->field_data[3].len)
    alloc_cpy(request->query_str, request->url+http_parser_url->field_data[4].off, http_parser_url->field_data[4].len)
    parser_query_param(request,request->url+http_parser_url->field_data[4].off,http_parser_url->field_data[4].len);
    free(http_parser_url);
    return 0;
}

static int on_header_request_field(http_parser* parser, const char* at, size_t length) {
    struct http_request *request = (struct http_request *) parser->data;
    struct http_header *header = add_http_request_header(request);
    alloc_cpy(header->name, at, length)
    return 0;
}

static int set_param_field(struct http_request *request, const char* at, size_t length) {
    struct http_param *param = add_http_param(request);
    alloc_cpy(param->name, at, length)
    return 0;
}
static int on_header_request_value(http_parser* parser, const char* at, size_t length) {
    struct http_request *request = (struct http_request *) parser->data;
    struct http_header *header = request->headers;
    while (header->next != NULL) {
        header = header->next;
    }
    alloc_cpy(header->value, at, length)
   return 0;
}
static int set_param_value(struct http_request *request, const char* at, size_t length) {
    struct http_param *param = request->query_param;
    while (param->next != NULL) {
        param = param->next;
    }
    alloc_cpy(param->value, at, length)
    return 0;
}
static int on_headers_complete(http_parser* parser) {
    struct http_request *request = (struct http_request *) parser->data;
    request->http_major=parser->http_major;
    request->http_minor=parser->http_minor;
    return 0;
}
static int on_body(http_parser* parser, const char* at, size_t length) {
    struct http_request *request = (struct http_request *) parser->data;
    alloc_cpy(request->body, at, length)
    return 0;
}
static int on_message_complete(http_parser* parser) {
    return 0;
}
static int parser_query_param(struct http_request *request,const char *buf, size_t buflen) {
    char* p=malloc(buflen + 1);
    if(p==NULL){
        log_err("query_param tmp malloc fail!");
        return -1;
    }
    char *tmp=p;
    memcpy(p, "&", 1);
    memcpy(p+1,buf,buflen);
    size_t found_at = 0;
    int is_value=0,i=0;
    while (i<buflen) {
        if (*p == '=' && is_value == 0) {
            set_param_field(request, p - found_at + 1, found_at - 1);
            found_at = 0;
            is_value = 1;
        }
        if (*p == '&' && is_value == 1) {
            set_param_value(request, p - found_at + 1, found_at - 1);
            found_at = 0;
            is_value = 0;
        }
        i++;
        found_at++;
        p++;
    }
    if(i==buflen&&buflen>0&&is_value==1){
        set_param_value(request, p-found_at+1,found_at);
    }
    free(tmp);
    return 0;
}
struct http_request* parser_http_request_buffer(struct Buffer *buf){
    struct http_parser* parser = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST); // 初始化parser为Request类型
    http_parser_execute(parser, &parser_set, buf->orig, buf->offset);
    return parser->data;
}
struct Buffer * create_http_response_buffer(struct http_response *http_response){
    struct Buffer * buffer= new_buffer(MAX_LINE, MAX_RESPONSE_SIZE);
    buffer_add(buffer,"HTTP/",5);
    char*str_major;
    int res=0;
    res=int_to_str(http_response->http_major,&str_major);
    buffer_add(buffer,str_major,res);
    free(str_major);
    buffer_add(buffer,".",1);
    char*str_minor;
    res=int_to_str(http_response->http_minor,&str_minor);
    buffer_add(buffer,str_minor,res);
    free(str_minor);
    buffer_add(buffer," ",1);
    char*str_code;
    res=int_to_str(http_response->code,&str_code);
    buffer_add(buffer,str_code,res);
    free(str_code);
    buffer_add(buffer," ",1);
    enum http_status status;
    status =(enum http_status)http_response->code;
    const char *str_status=http_status_str(status);
    buffer_add(buffer,str_status,strlen(str_status));
    buffer_add(buffer,"\r\n",2);
    struct http_header *header = http_response->headers;
    while (header!= NULL) {
        buffer_add(buffer,header->name,strlen(header->name));
        buffer_add(buffer,": ",2);
        buffer_add(buffer,header->value,strlen(header->value));
        buffer_add(buffer,"\r\n",2);
        header = header->next;
    }
    buffer_add(buffer,"\r\n",2);
    if(http_response->body!=NULL) {
        buffer_add(buffer, http_response->body, strlen(http_response->body));
    }
    return buffer;
}
struct http_header *add_http_response_header(struct http_response *response){
    struct http_header *header = response->headers;
    while (header != NULL) {
        if (header->next == NULL) {
            header->next = new_http_header();
            return header->next;
        }
        header = header->next;
    }
    response->headers = new_http_header();
    return response->headers;
}
int check_http_request_header_value(struct http_request *http_request,char * name,char* value){
    struct http_header *header = http_request->headers;
    if(header==NULL){
        return 0;
    }
    while (header!= NULL) {
        if(strcmp(name,header->name)==0){
            if(strcmp(value,header->value)==0){
                return 1;
            }
        }
        header = header->next;
    }
    return 0;
}
void get_error_status_body(struct http_response *http_response,int code){
    struct Buffer* buffer=new_buffer(1024,1024);
    char *message;
    switch (code){
        case 404:
            message="resource not found!";
            buffer_add(buffer,message,strlen(message));
            break;
        default:
            message="unknown status code!";
            buffer_add(buffer,message,strlen(message));
            break;
    }
    http_response->body=buffer_to_string(buffer);
    free_buffer(buffer);
}