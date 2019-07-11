//
// Created by Administrator on 2019/7/11.
//
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include "event.h"
#include "socket.h"

#define BUFFER_SIZE 1024
int total_clients=0;

struct ev_loop *loop;
struct ev_io socket_accept;
static void ev_accept_cb(struct ev_loop *loop,struct ev_io *watcher,int event);
void ev_loop_init(){
    loop=ev_default_loop(0);
}
void ev_accept_start(int server_fd){
    ev_io_init(&socket_accept,ev_accept_cb,server_fd,EV_READ);
    ev_io_start(loop,&socket_accept);
}

static void ev_accept_cb(struct ev_loop *loop,struct ev_io *watcher,int event)
{
    //struct sockaddr_in client_addr;
    int client_sd;
    struct ev_io *w_client=(struct ev_io*)malloc(sizeof(struct ev_io));
    if(EV_ERROR & event){
        printf("error event in accept");
        return ;
    }
    //client_sd=accept(watcher->fd,(struct sockaddr *)&client_addr,&client_len);
    client_sd=accept(watcher->fd,NULL,NULL);
    if(client_sd<0){
        printf("accept error");
        return;
    }
    if (set_nonblock(client_sd) < 0) {
        perror("failed to set client socket to nonblock");
        return;
    }
    total_clients++;
    printf("successfully connected with client.\n");
    printf("%d client connected .\n",total_clients);
    ev_io_init(w_client,ev_read_cb,client_sd,EV_READ);
    ev_io_start(loop,w_client);
}
void ev_read_cb(struct ev_loop *loop,struct ev_io *watcher,int event)
{
    char buffer[BUFFER_SIZE];
    int read;
    if(EV_ERROR & event){
        printf("error event in read");
        return;
    }
    read=recv(watcher->fd,buffer,BUFFER_SIZE,0);
    if(read==0){
        ev_io_stop(loop,watcher);
        free(watcher);
        perror("peer might closing");
        total_clients--;
        printf("%d client connected .\n",total_clients);
        return;
    }
    else{
        printf("#######################\n");
        printf("read is %d\n",read);
        printf("#######################\n");
        buffer[read]='\0';
        printf("get the message:\n %s\n",buffer);
    }

     send(watcher->fd,buffer,read,0);
     bzero(buffer,read);
}
void ev_loop_start(){
    while(1){
        ev_loop(loop,0);
    }
}
