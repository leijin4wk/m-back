 //
// Created by Administrator on 2019/7/11.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "socket.h"
#include "dictionary.h"
#include "iniparser.h"
#include "dbg.h"

 extern  dictionary* ini_file;
 int init_server_socket(void){
     int server_fd;
     const int port =iniparser_getint(ini_file,"server:port","null");
     debug("port is : %d", port);
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
     debug("bind port :%d",port);
     if(listen(server_fd,0)<0){
         log_err("listen error");
         return -1;
     }
     debug("listen port :%d",port);
     return server_fd;
}
 int set_nonblock(int fd) {
     int flags = fcntl(fd, F_GETFL);
     if (flags < 0) return flags;
     flags |= O_NONBLOCK;
     if (fcntl(fd, F_SETFL, flags) < 0) return -1;
     return 0;
 }