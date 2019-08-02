#include <stdio.h>
#include "db_tool.h"
#include "config.h"
#include "event.h"
#include "ssl_tool.h"
#include "db_tool.h"
#include "socket_tool.h"
#include "module.h"
#include "dbg.h"
#include "thpool.h"
extern threadpool read_thread_pool;
extern threadpool write_thread_pool;
#define CONFIG_FILE_PATH "../config.json"
int main(){
    read_json_config(CONFIG_FILE_PATH);
    log_info("config_file load complete!");
    load_and_init_module();
    init_server_ctx();
    log_info("ssl init complete!");
    init_connection_pool();
    log_info("db connection_pool init complete!");
    int server_fd= init_server_socket();
    if(server_fd<0){
        log_err("server socket init fail!");
        exit(-1);
    }
    read_thread_pool=thpool_init(10);
    write_thread_pool=thpool_init(10);
    log_info("server_fd is %d init complete!",server_fd);
    ev_loop_init();
    ev_accept_start(server_fd);
    log_info("ev_loop init complete!");
    ev_loop_start();

    return 0;
}