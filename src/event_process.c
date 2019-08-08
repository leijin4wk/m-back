//
// Created by Administrator on 2019/7/25.
//
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "event_process.h"
#include "ssl_tool.h"
#include "socket_tool.h"
#include "dbg.h"
#include "map.h"
#include "module.h"
#include "http_buffer.h"
#include "str_tool.h"

int total_clients = 1;


extern map_void_t dispatcher_map;
extern char *root;
extern char *index_page;
static void add_timer_call_back(void *client, struct timer_node_t *node);
static void delete_timer_call_back(void *client);
static struct http_response *new_http_response();

static void process_http(struct http_client *client);

static void add_timer_call_back(void *client, struct timer_node_t *node) {
    struct http_client *http_client = (struct http_client *) client;
    http_client->last_update_time = current_time_millis;
    node->value = client;
    node->deleted = 0;
    node->pri = current_time_millis + TIMEOUT_DEFAULT;
    http_client->timer = node;
}

static void delete_timer_call_back(void *client) {
    struct http_client *http_client = (struct http_client *) client;
    struct timer_node_t *node = (struct timer_node_t *) http_client->timer;
    node->deleted = 1;
}
void ev_accept_callback(struct m_event *watcher) {
    struct sockaddr_in in_addr;
    socklen_t in_len;
    memset(&in_addr, 0, sizeof(struct sockaddr_in));
    int in_fd;
    in_len = sizeof(struct sockaddr_in);
    in_fd = accept(watcher->event_fd, (struct sockaddr *) &in_addr, &in_len);
    if (in_fd < 0) {
        return;
    }
    int flag = set_nonblock(in_fd);
    if (flag < 0) {
        log_err("make_socket_non_blocking fail!");
        close(in_fd);
        return;
    }
    struct epoll_event event;
    struct http_client *client = new_http_client();
    //更新最后更新时间
    time_update();
    client->last_update_time = current_time_millis;
    add_timer(client,add_timer_call_back);
    char *ip = inet_ntoa(in_addr.sin_addr);
    alloc_cpy(client->client_ip, ip, strlen(ip))
    client->event_fd = in_fd;
    client->e_pool_fd = watcher->e_pool_fd;
    SSL *ssl = create_ssl(client->event_fd);
    if (ssl == NULL) {
        log_err("create_ssl fail!");
        free_http_client(client);
        return;
    }
    client->ssl = ssl;
    event.data.ptr = (void *) client;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    int rc = epoll_ctl(watcher->e_pool_fd, EPOLL_CTL_ADD, in_fd, &event);
    if (rc != 0) {
        log_err("epoll_read add fail!");
        free_http_client(client);
        return;
    }
}

void ev_read_callback(void *watcher) {
    struct http_client *client = (struct http_client *) watcher;
    //更新最后更新时间
    client->last_update_time = current_time_millis;
    if (client->ssl_connect_flag == 0) {
        struct epoll_event event;
        int res = accept_ssl(client->ssl);
        if (res < 0) {
            log_err("create_ssl fail!");
            free_http_client(client);
            return;
        } else if (res == 0) {
            client->ssl_connect_flag = 0;
        } else {
            client->ssl_connect_flag = 1;
            log_info("添加SSL连接:%d ,ip:%s , 当前连接人数%d", client->event_fd, client->client_ip, total_clients++);
        }
        //删除老节点
        delete_timer(client,delete_timer_call_back);
        log_info("delete_timer success!");
        //创建新节点
        add_timer(client,add_timer_call_back);
        log_info("add_timer success!");
        event.data.ptr = (void *) client;
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        int rc = epoll_ctl(client->e_pool_fd, EPOLL_CTL_ADD, client->event_fd, &event);
        if (rc != 0) {
            rc = epoll_ctl(client->e_pool_fd, EPOLL_CTL_MOD, client->event_fd, &event);
            if (rc != 0) {
                log_err("add epool fail!");
                free_http_client(client);
            }
        }
        log_info("add event success!");
        return;
    }
    log_info("当前读取fd为：%d", client->event_fd);
    struct Buffer *read_buff = new_buffer(MAX_LINE, MAX_REQUEST_SIZE);
    int res = ssl_read_buffer(client->ssl, read_buff);
    if (res < 0) {
        log_err("当前出错fd为：%d", client->event_fd);
        free_http_client(client);
        return;
    }
    client->request_data = read_buff;
    client->request = parser_http_request_buffer(client->request_data);
    //这个是关键方法
    process_http(client);
    //删除老节点
    delete_timer(client,delete_timer_call_back);
    //创建新节点
    add_timer(client,add_timer_call_back);
    struct epoll_event event;
    event.data.ptr = (void *) watcher;
    event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
    int rc = epoll_ctl(client->e_pool_fd, EPOLL_CTL_ADD, client->event_fd, &event);
    if (rc != 0) {
        rc = epoll_ctl(client->e_pool_fd, EPOLL_CTL_MOD, client->event_fd, &event);
        if (rc != 0) {
            log_err("add epool fail!");
            free_http_client(client);
        }
    }
    log_info("fd %d request read complete!", client->event_fd);
}

void ev_write_callback(void *watcher) {
    int res = 0;
    struct http_client *client = (struct http_client *) watcher;
    //更新最后更新时间
    client->last_update_time = current_time_millis;

    log_info("当前写fd为：%d", client->event_fd);
    struct http_header *header = add_http_response_header(client->response);
    header->name = strdup("Content-Length");
    if (client->response->data_type == DYNAMIC_DATA) {
        char *length = NULL;
        int_to_str(strlen(client->response->body), &length);
        header->value = length;
    } else {
        char *length = NULL;
        int_to_str(client->response->real_file_size, &length);
        header->value = length;
    }
    struct Buffer *read_buff = create_http_response_buffer(client->response);
    res = ssl_write_buffer(client->ssl, read_buff);
    if (res < 0) {
        log_err("ssl_write_buffer fail!");
        free_http_client(client);
        return;
    }
    if (client->response->data_type == STATIC_DATA) {
        res = ssl_write_file(client->ssl, client->response->real_file_path, client->response->real_file_size);
        if (res < 0) {
            log_err("ssl_write_file fail!");
            free_http_client(client);
            return;
        }
    }
    //删除老节点
    delete_timer(client,delete_timer_call_back);
    //创建新节点
    add_timer(client,add_timer_call_back);

    struct epoll_event event;
    event.data.ptr = (void *) client;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    int rc = epoll_ctl(client->e_pool_fd, EPOLL_CTL_ADD, client->event_fd, &event);
    if (rc != 0) {
        rc = epoll_ctl(client->e_pool_fd, EPOLL_CTL_MOD, client->event_fd, &event);
        if (rc != 0) {
            log_err("epoll_write MOD fail!");
            free_http_client(client);
        }
    }
    log_info("fd %d response write complete!", client->event_fd);
}

struct http_client *new_http_client() {
    struct http_client *client = malloc(sizeof(struct http_client));
    if (client == NULL) {
        log_err("new http client fail!");
        return NULL;
    }
    client->response = NULL;
    client->request = NULL;
    client->request_data = NULL;
    client->ssl_connect_flag = 0;
    return client;
}

void free_http_client(struct http_client *client) {
    //todo
}

static struct http_response *new_http_response(struct http_request *request) {
    struct http_response *response = malloc(sizeof(struct http_response));
    response->body = NULL;
    response->http_major = request->http_major;
    response->http_minor = request->http_minor;
    response->code = 200;
    response->headers = NULL;
    response->real_file_path = NULL;
    response->data_type = -1;
    struct http_header *header = add_http_response_header(response);
    header->name = strdup("Server");
    header->value = strdup("LeiJin/m_back");
    return response;
}

static void process_http(struct http_client *client) {
    struct stat sbuf;
    struct Buffer *filename = new_buffer(SHORTLINE, SHORTLINE);
    void (*function)(struct http_request *, struct http_response *);
    int flag = 0;
    buffer_add(filename, root, strlen(root));
    client->response = new_http_response(client->request);
    if (check_http_request_header_value(client->request, "Upgrade-Insecure-Requests", "1")) {
        struct http_header *header = add_http_response_header(client->response);
        header->name = strdup("Content-Security-Policy");
        header->value = strdup("upgrade-insecure-requests");
    }
    if (check_http_request_header_value(client->request, "Connection", "keep-alive")) {
        struct http_header *header = add_http_response_header(client->response);
        header->name = strdup("Connection");
        header->value = strdup("Keep-Alive");
    }

    void **fun = map_get(&dispatcher_map, client->request->path);
    if (fun == NULL) {
        buffer_add(filename, client->request->path, strlen(client->request->path));
        int res = stat(buffer_to_string(filename), &sbuf);
        if (res < 0) {
            client->response->data_type = DYNAMIC_DATA;
            client->response->code = 404;
        } else {
            if (S_ISREG(sbuf.st_mode)) {
                client->response->data_type = STATIC_DATA;
                client->response->real_file_path = buffer_to_string(filename);
                client->response->real_file_size = sbuf.st_size;
            } else if (S_ISDIR(sbuf.st_mode) && strcmp("/", client->request->path) == 0) {
                buffer_add(filename, index_page, strlen(index_page));
                flag = stat(buffer_to_string(filename), &sbuf);
                if (flag < 0) {
                    client->response->data_type = DYNAMIC_DATA;
                    client->response->code = 404;
                } else {
                    client->response->data_type = STATIC_DATA;
                    client->response->real_file_size = sbuf.st_size;
                    client->response->real_file_path = buffer_to_string(filename);
                }
            } else {
                client->response->data_type = DYNAMIC_DATA;
                client->response->code = 404;
            }
        }
    } else {
        client->response->data_type = DYNAMIC_DATA;
        struct http_module_api *api = (struct http_module_api *) (*fun);
        function = api->function;
        function(client->request, client->response);
    }
    if (client->response->code != 200) {
        get_error_status_body(client->response, client->response->code);
    }
    free_buffer(filename);
}

