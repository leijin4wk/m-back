
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str_tool.h"

int int_to_str(int i,char **out){
    int j=1;
    int tmp=i;
    while((tmp=tmp/10)>=1){
      j++;
    }
    char* res=malloc(j+1);
    sprintf(res,"%d",i);
    res[j]='\0';
    *out=res;
    return j;
}