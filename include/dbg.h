//
// Created by oyo on 2019-06-27.
//
/*
 * 开发中对日志，数据检查的封装
 */

#ifndef M_BACK_DBG_H
#define M_BACK_DBG_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#ifndef DEBUG
#define debug(M, ...)
#else
#define log_debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] %s:%d: errno: %s " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] pid:%ld  %s:%d " M "\n",pthread_self(), __FILE__, __LINE__, ##__VA_ARGS__)

#endif
