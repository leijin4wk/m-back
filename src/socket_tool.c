 //
// Created by Administrator on 2019/7/11.
//

#include <fcntl.h>
#include <cJSON.h>
#include "socket_tool.h"
#include "dbg.h"
 extern cJSON *json_config;

 int init_server_socket(void){
     int server_fd;
     cJSON *port_item=cJSON_GetObjectItem(json_config,"port");
     if(port_item==NULL){
         log_err("port is not  config!");
         return -1;
     }
     int port=port_item->valueint;
     struct sockaddr_in addr;
     if((server_fd=socket(AF_INET,SOCK_STREAM,0))<0){
         log_err("socket error");
         return -1;
     }
     bzero(&addr,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_port=htons(port);
     addr.sin_addr.s_addr=INADDR_ANY;
     if(bind(server_fd,(struct sockaddr*)&addr,sizeof(addr))!=0){
         log_err("bind error");
         return -1;
     }
     if(listen(server_fd,0)<0){
         log_err("listen error");
         return -1;
     }
     log_info("listen port :%d",port);
     return server_fd;
}
 int set_nonblock(int fd) {
     int flags = fcntl(fd, F_GETFL);
     if (flags < 0) return flags;
     flags |= O_NONBLOCK;
     if (fcntl(fd, F_SETFL, flags) < 0) return -1;
     return 0;
 }