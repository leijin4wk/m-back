//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_SSL_TOOL_H
#define M_BACK_SSL_TOOL_H
#include <openssl/ssl.h>

void init_server_ctx(void);


SSL*  create_ssl(int socket_in);

int ssl_read(SSL* ssl, char* buffer, int len);

int ssl_write(SSL* ssl, const char* buffer, int len);

#endif //M_BACK_SSL_TOOL_H
