//
// Created by Administrator on 2019/7/12.
//
#include "cJSON.h"
#include "buffer.h"
#include "dbg.h"

cJSON *json_config;

void read_json_config(const char * json_config_name){
    struct Buffer* buffer=read_file_to_buffer(json_config_name);
    if(buffer==NULL){
        log_err("config file read fail!");
        exit(-1);
    }
    buffer_add(buffer,"\0",1);
    json_config = cJSON_Parse(buffer->orig);
    if (json_config == NULL) {
        log_err("config json Parse error...");
        exit(-1);
    }
}

