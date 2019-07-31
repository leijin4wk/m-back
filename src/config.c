//
// Created by Administrator on 2019/7/12.
//
#include "cJSON.h"
#include "buffer.h"
#include "dbg.h"

cJSON *json_config;
char* root;
char* index_page;
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
    cJSON *root_item=cJSON_GetObjectItem(json_config,"root");
    if(root_item==NULL){
        log_err("root is not  config!");
        exit(-1);
    }
    root=root_item->valuestring;
    cJSON *index_item=cJSON_GetObjectItem(json_config,"index");
    if(index_item==NULL){
        log_err("index is not  config!");
        exit(-1);
    }
    index_page=index_item->valuestring;
}

