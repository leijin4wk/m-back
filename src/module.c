//
// Created by oyo on 2019-07-11.
//

#include <dlfcn.h>
#include <cJSON.h>
#include "map.h"
#include "buffer.h"
#include "dbg.h"
#include "dictionary.h"
#include "iniparser.h"

extern dictionary* ini_file;

map_void_t dispatcher_map;
typedef int(* test_handler)(int, int); // 定义函数指针类型的别名
static void parser_buffer_to_json(struct Buffer* buffer);
static void load_module_by_json(char* module_path,cJSON* arr_item, int arr_size);

void load_and_init_module(){
    map_init(&dispatcher_map);
    int n= iniparser_getsecnkeys(ini_file,"module");
    const char** keys=malloc(sizeof(char*)*n);
    keys=iniparser_getseckeys(ini_file,"module",keys);
    for(int i=0;i<n;i++){
        const char* so_path= iniparser_getstring(ini_file,keys[i],NULL);
        if(so_path==NULL){
            log_info("%s not exist",keys[i]);
            continue;
        }
       struct Buffer* buffer=read_file_to_buffer(so_path);
       if(buffer==NULL){
           return;
       }
       parser_buffer_to_json(buffer);

    }
}
static void parser_buffer_to_json(struct Buffer* buffer) {
    cJSON *cjson = cJSON_Parse(buffer->orig);
    if (cjson == NULL) {
        log_err("json Parse error...");
        return;
    }
    cJSON *so_path=cJSON_GetObjectItem(cjson,"module_path");
    if (so_path==NULL){
        log_err("module config not module_path!");
        return;
    }
    char *module_path= so_path->valuestring;
    cJSON *api_array=cJSON_GetObjectItem(cjson,"api");
    if (api_array==NULL){
        log_err("module config not api_array!");
        return;
    }
    int arr_size = cJSON_GetArraySize(api_array);
    cJSON* arr_item = api_array->child;//子对象
    load_module_by_json(module_path,arr_item,arr_size);
}
static void load_module_by_json(char* module_path,cJSON* arr_item, int arr_size){

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

    for(int i = 0;i <=arr_size-1;i++){
        log_info("%s",cJSON_GetObjectItem(arr_item,"path")->valuestring);
        log_info("%s",cJSON_GetObjectItem(arr_item,"request_method")->valuestring);
        arr_item = arr_item->next;//下一个子对象
    }
}




