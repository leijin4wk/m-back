//
// Created by oyo on 2019-05-16.
//
#include "db_tool.h"
#include <string.h>
#include "iniparser.h"
extern dictionary* ini_file;
ConnectionPool_T pool;
void init_connection_pool() {
    const char *url = iniparser_getstring(ini_file,"db:url","null");
    URL_T dbUrl = URL_new(url);
    pool = ConnectionPool_new(dbUrl);
    ConnectionPool_start(pool);
}