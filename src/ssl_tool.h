//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_SSL_TOOL_H
#define M_BACK_SSL_TOOL_H
#include <openssl/ssl.h>

void init_server_ctx(void);


int socket_add_ssl(int socket_in,SSL *ssl);


#endif //M_BACK_SSL_TOOL_H
