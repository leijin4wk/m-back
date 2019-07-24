//
// Created by oyo on 2019-07-11.
//

#include <dlfcn.h>
#include "cJSON.h"
#include "map.h"
#include "dbg.h"
#include "module.h"

extern cJSON *json_config;
map_void_t dispatcher_map;
map_void_t module_map;
typedef int(* test_handler)(int, int); // 定义函数指针类型的别名
static void load_api_by_json(char* module_name,char* module_path,cJSON* api_arr, int arr_size);
void load_and_init_module(){
    map_init(&dispatcher_map);
    map_init(&module_map);
    cJSON *module_array=cJSON_GetObjectItem(json_config,"module");
    int arr_size = cJSON_GetArraySize(module_array);
    cJSON* arr_item = module_array->child;//子对象
    for(int i=0;i<arr_size;i++){
        for(int i = 0;i <=arr_size-1;i++) {
            char* module_name= cJSON_GetObjectItem(arr_item, "module_name")->valuestring;
            char* module_path=cJSON_GetObjectItem(arr_item, "module_path")->valuestring;
            cJSON *api_array=cJSON_GetObjectItem(arr_item,"api");
            int api_size = cJSON_GetArraySize(api_array);
            load_api_by_json(module_name,module_path,api_array,api_size);
            log_info("%s module load success!", module_name);
            arr_item = arr_item->next;//下一个子对象
        }
    }
}
static void load_api_by_json(char* module_name,char* module_path,cJSON* api_arr, int arr_size){
    struct module* module=malloc(sizeof(struct module));
    if(module==NULL){
        log_err("%s module malloc fail!",module_name);
        exit(EXIT_FAILURE);
    }
    module->module_name=module_name;
    void* handle = dlopen( module_path, RTLD_LAZY );
    if( !handle )
    {
        log_err("%s dlopen get error!",module_path);
        exit( EXIT_FAILURE );
    }
    module->module_handle=handle;
    int res=map_set(&module_map,module_name,module);
    if (res<0){
        log_err("%s add to map error!",module_name);
        exit( EXIT_FAILURE );
    }

    cJSON* arr_item = api_arr->child;
    for(int i = 0;i <=arr_size-1;i++){
        struct http_module_api* api=malloc(sizeof(struct http_module_api));
        if(api==NULL){
            log_err("%s api malloc fail!",module_name);
            exit(EXIT_FAILURE);
        }
        char * path=cJSON_GetObjectItem(arr_item,"path")->valuestring;
        api->path=path;
        api->request_method=cJSON_GetObjectItem(arr_item,"request_method")->valuestring;
        api->consumes=cJSON_GetObjectItem(arr_item,"consumes")->valuestring;
        api->produces=cJSON_GetObjectItem(arr_item,"produces")->valuestring;
        api->function=dlsym(handle,cJSON_GetObjectItem(arr_item,"function")->valuestring);
        int res=map_set(&dispatcher_map,path,api);
        if (res<0){
            log_err("%s add to map error!",module_name);
            exit( EXIT_FAILURE );
        }
        arr_item = arr_item->next;
    }
}




