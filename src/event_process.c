//
// Created by Administrator on 2019/7/25.
//
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "event_process.h"
#include "event.h"
#include "ssl_tool.h"
#include "socket_tool.h"
#include "dbg.h"
#include "map.h"
#include "module.h"
#include "http_buffer.h"
#include "str_tool.h"
int total_clients=0;

extern map_void_t dispatcher_map;

static struct http_response *new_http_response();
static void process_http(int e_pool_fd,struct http_client* client);
void ev_accept_callback(int e_pool_fd,struct m_event *watcher)
{
    struct sockaddr_in in_addr;
    socklen_t in_len;
    memset(&in_addr, 0, sizeof(struct sockaddr_in));
    int in_fd;
    while(1) {
        in_fd = accept(watcher->event_fd, (struct sockaddr*)&in_addr, &in_len);
        if (in_fd < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                /* we have processed all incoming connections */
                break;
            } else {
                log_err("accept error！");
                break;
            }
        }
        int flag = set_nonblock(in_fd);
        if (flag < 0) {
            log_err("make_socket_non_blocking fail!");
            continue;
        }
        struct epoll_event event;
        struct http_client *client = new_http_client();
        char *ip = inet_ntoa(in_addr.sin_addr);
        alloc_cpy(client->client_ip,ip,strlen(ip))
        client->event_fd = in_fd;
        SSL *ssl=create_ssl(client->event_fd);
        if (ssl == NULL) {
            log_err("create_ssl fail!");
            free_http_client(client);
            continue;
        }
        client->ssl=ssl;
        event.data.ptr = (void *) client;
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        int rc = epoll_ctl(e_pool_fd, EPOLL_CTL_ADD, in_fd, &event);
        if (rc != 0) {
            log_err("epoll_read add fail!");
            free_http_client(client);
            continue;
        }
        total_clients++;
        log_info("添加SSL连接:%d ,ip:%s , 当前连接人数%d", in_fd, ip, total_clients);
    }
}

void ev_read_callback(int e_pool_fd,struct m_event* watcher){
    struct http_client* client= (struct http_client *)watcher;
    struct Buffer* read_buff=new_buffer(MAX_LINE, MAX_REQUEST_SIZE);
    int res=ssl_read(client->ssl,read_buff);
    if(res<0){
        free_http_client(client);
        return;
    }
    log_info("http read size %d",(int)read_buff->offset);
    client->request_data=read_buff;
    client->request=parser_http_request_buffer(client->request_data);
    log_info("http parser complete!");
    //这个是关键方法
    process_http(e_pool_fd,client);

    struct epoll_event event;
    event.data.ptr = (void *) client;
    event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
    int rc = epoll_ctl(e_pool_fd, EPOLL_CTL_ADD, client->event_fd, &event);
    if (rc != 0) {
        log_info("fd exist in epool!");
        rc= epoll_ctl(e_pool_fd, EPOLL_CTL_MOD, client->event_fd, &event);
        if (rc != 0) {
            log_err("epoll_write MOD fail!");
            free_http_client(client);
        }
    }
}

void ev_write_callback(int e_pool_fd,struct m_event* watcher){
    int res=0;
    struct http_client* client= (struct http_client *)watcher;
    struct http_header* header=add_http_response_header(client->response);
    header->name=strdup("Content-Length");
    if(client->response->body!=NULL) {
        char *length = NULL;
        int_to_str(strlen(client->response->body), &length);
        header->value = length;
    }else{
        header->value =strdup("0");
    }
    log_info("http content size %s",header->value);
    struct Buffer* read_buff=create_http_response_buffer(client->response);
    res=ssl_write(client->ssl,read_buff);
    if(res<0){
        free_http_client(client);
        return;
    }
    log_info("http write size %d",(int)read_buff->offset);
    struct epoll_event event;
    event.data.ptr = (void *) client;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    int rc = epoll_ctl(e_pool_fd, EPOLL_CTL_ADD, client->event_fd, &event);
    if (rc != 0) {
        log_info("fd exist in epool!");
        rc= epoll_ctl(e_pool_fd, EPOLL_CTL_MOD, client->event_fd, &event);
        if (rc != 0) {
            log_err("epoll_write MOD fail!");
            free_http_client(client);
        }
    }
    log_info("epoll add read  success!");
}

struct http_client* new_http_client(){
    struct http_client *client = malloc(sizeof(struct http_client));
    if(client==NULL){
        log_err("new http client fail!");
        return NULL;
    }

    client->response=NULL;
    client->request=NULL;
    client->request_data=NULL;
    client->response_data=new_buffer(MAX_LINE, MAX_RESPONSE_SIZE);
    return client;
}

void free_http_client(struct http_client* client){
    //todo
}
static struct http_response *new_http_response(struct http_request* request){
    struct http_response* response=malloc(sizeof(struct http_response));
    response->body=NULL;
    response->http_major=request->http_major;
    response->http_minor=request->http_minor;
    response->code=200;
    response->headers=NULL;
    struct http_header* header= add_http_response_header(response);
    header->name=strdup("Server");
    header->value=strdup("LeiJin/m_back");
    return response;
}
static void process_http(int e_pool_fd,struct http_client* client){
    void (*function)(struct http_request*,struct http_response*);
    client->response=new_http_response(client->request);
    log_info("%s",client->request->path);
    void** fun=map_get(&dispatcher_map, client->request->path);
    if(fun==NULL){
        client->response->code=404;
    } else {
        struct http_module_api* api=(struct http_module_api*)(*fun);
        function = api->function;
        function(client->request, client->response);
    }

}

