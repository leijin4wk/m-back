//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_MODULE_H
#define M_BACK_MODULE_H
struct module{
    char* path;
    void* handler;
    char *func;
};
void load_and_init_module();
#endif //M_BACK_MODULE_H
