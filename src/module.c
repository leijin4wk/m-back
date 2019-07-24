//
// Created by oyo on 2019-07-11.
//

#include <dlfcn.h>
#include "cJSON.h"
#include "map.h"
#include "dbg.h"

extern cJSON *json_config;
map_void_t dispatcher_map;
typedef int(* test_handler)(int, int); // 定义函数指针类型的别名
static void load_api_by_json(char* module_path,cJSON* api_arr, int arr_size);
void load_and_init_module(){
    map_init(&dispatcher_map);
    cJSON *module_array=cJSON_GetObjectItem(json_config,"module");
    int arr_size = cJSON_GetArraySize(module_array);
    cJSON* arr_item = module_array->child;//子对象
    for(int i=0;i<arr_size;i++){
        for(int i = 0;i <=arr_size-1;i++) {
            char* module_name= cJSON_GetObjectItem(arr_item, "module_name")->valuestring;
            char* module_path=cJSON_GetObjectItem(arr_item, "module_path")->valuestring;
            cJSON *api_array=cJSON_GetObjectItem(arr_item,"api");
            int api_size = cJSON_GetArraySize(api_array);
            load_api_by_json(module_path,api_array,api_size);
            log_info("%s module load success!", module_name);
            arr_item = arr_item->next;//下一个子对象
        }
    }
}
static void load_api_by_json(char* module_path,cJSON* api_arr, int arr_size){
    cJSON* arr_item = api_arr->child;//子对象
    for(int i = 0;i <=arr_size-1;i++){
        log_info("%s",cJSON_GetObjectItem(arr_item,"path")->valuestring);
        log_info("%s",cJSON_GetObjectItem(arr_item,"request_method")->valuestring);
        arr_item = arr_item->next;//下一个子对象
    }
    void* handle = dlopen( module_path, RTLD_LAZY );
    if( !handle )
    {
        log_err("%s dlopen get error!",module_path);
        exit( EXIT_FAILURE );
    }
    //todo 添加http模块的处理
    do{ // for resource handle
        test_handler add_func = (test_handler)dlsym( handle, "add" );
        printf( "1 add 2 is %d \n", add_func(1,2) );
    }while(0); // for resource handle

    dlclose( handle );


}




