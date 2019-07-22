//
// Created by oyo on 2019-07-11.
//

#include <dlfcn.h>
#include <yajl/yajl_tree.h>
#include "map.h"
#include "buffer.h"
#include "dbg.h"
#include "dictionary.h"
#include "iniparser.h"

extern dictionary* ini_file;

map_void_t dispatcher_map;

int load_and_init_module(){
    int n= iniparser_getsecnkeys(ini_file,"module");
    const char** keys=malloc(sizeof(char*)*n);
    keys=iniparser_getseckeys(ini_file,"module",keys);
    for(int i=0;i<n;i++){
       struct Buffer* buffer=read_file_to_buffer(keys[i]);
        printf("Url: %.*s", (int)buffer->offset, buffer->orig);
    }
}



