//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_MODULE_H
#define M_BACK_MODULE_H

#include "http.h"

struct http_module_api{
    char* path;
    void* module_handle;
    char* request_method;
    void (*function)(struct http_request* request,struct http_response* response);

};
void load_and_init_module();
#endif //M_BACK_MODULE_H
