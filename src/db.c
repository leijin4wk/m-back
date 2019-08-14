//
// Created by oyo on 2019-08-14.
//

//
// Created by oyo on 2019-05-16.
//
#include "db.h"
#include <string.h>
#include "cJSON.h"
#include "dbg.h"
extern cJSON *json_config;
ConnectionPool_T db_connect_pool;
void init_connection_pool() {
    cJSON *db_url_item=cJSON_GetObjectItem(json_config,"db_url");
    if(db_url_item==NULL){
        log_err("db_url is not config!");
        exit(1);
    }
    char* url=db_url_item->valuestring;
    URL_T dbUrl = URL_new(url);
    pool = ConnectionPool_new(dbUrl);
    ConnectionPool_start(pool);
}