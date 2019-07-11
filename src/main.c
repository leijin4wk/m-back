#include <stdio.h>
#include <ev.h>
#include "config.h"
#include "event.h"
#include "ssl_tool.h"
#include "socket.h"
#include "dbg.h"
#define CONFIG_FILE_PATH "../config.ini"
int main(){
    init_config(CONFIG_FILE_PATH);
    debug("config_file load complete!");
    init_server_ctx();
    debug("ssl init complete!");
    int server_fd= init_server_socket();
    check_exit(server_fd<0,"server socket init fail!")
    debug("server_fd is %d init complete!",server_fd);
    ev_loop_init();
    ev_accept_start(server_fd);
    debug("ev_loop init complete!");
    ev_loop_start();
    return 0;
}