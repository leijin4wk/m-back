//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_SSL_TOOL_H
#define M_BACK_SSL_TOOL_H
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX* init_server_ctx(void);


void load_certificates(SSL_CTX* ctx, char* CertFile, char* KeyFile);


void show_certs_info(SSL* ssl);



#endif //M_BACK_SSL_TOOL_H
