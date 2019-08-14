//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_MODULE_H
#define M_BACK_MODULE_H

#include "http.h"
struct module{
    char* module_name;
    void* module_handle;
};

struct http_module_api{
    char* path;
    char* request_method;
    void (*function)(struct http_request* request,struct http_response* response);

};
void load_and_init_module();
#endif //M_BACK_MODULE_H
