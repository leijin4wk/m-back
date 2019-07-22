//
// Created by oyo on 2019-07-11.
//

#include <dlfcn.h>
#include "map.h"
#include "dictionary.h"
#include "iniparser.h"
#include "str_tool.h"
#include <regex.h>
extern dictionary* ini_file;

map_void_t dispatcher_map;


int load_and_init_module(){
    const char* modules =iniparser_getint(ini_file,"server:module","");

}

