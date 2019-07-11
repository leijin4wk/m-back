//
// Created by Administrator on 2019/7/12.
//
#include "dictionary.h"
#include "iniparser.h"

dictionary* ini_file;

void init_config(const char * ininame){
    ini_file=iniparser_load(ininame);
}

