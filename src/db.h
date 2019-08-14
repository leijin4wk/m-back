//
// Created by oyo on 2019-08-14.
//

#ifndef M_BACK_DB_H
#define M_BACK_DB_H
#include <zdb.h>
extern ConnectionPool_T db_connect_pool;
void init_connection_pool();
#endif //M_BACK_DB_H
