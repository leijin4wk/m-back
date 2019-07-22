//
// Created by oyo on 2019-07-11.
//

#include <dlfcn.h>
#include "map.h"
#include "dictionary.h"
#include "iniparser.h"
#include "str_tool.h"
#include "dbg.h"
extern dictionary* ini_file;

map_void_t dispatcher_map;

int load_and_init_module(){
    int n= iniparser_getsecnkeys(ini_file,"module");
    log_info("total module : %d",n);
    char** keys=malloc(sizeof(char*)*n);
    keys=iniparser_getseckeys(ini_file,"module",keys);
    for(int i=0;i<n;i++){
      log_info("%s",keys[i]) ;
    }
}

