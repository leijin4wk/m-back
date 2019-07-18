#include <stdio.h>
#include "db_tool.h"
#include "config.h"
#include "event.h"
#include "ssl_tool.h"
#include "db_tool.h"
#include "socket_tool.h"
#include "dbg.h"
#define CONFIG_FILE_PATH "../config.ini"
int main(){
    init_config(CONFIG_FILE_PATH);
    log_info("config_file load complete!");
    init_server_ctx();
    log_info("ssl init complete!");
    init_connection_pool();
    log_info("db connection_pool init complete!");
    int server_fd= init_server_socket();
    check_exit(server_fd<0,"server socket init fail!")
    log_info("server_fd is %d init complete!",server_fd);
    ev_loop_init();
    ev_accept_start(server_fd);
    log_info("ev_loop init complete!");
    ev_loop_start();
    return 0;
}