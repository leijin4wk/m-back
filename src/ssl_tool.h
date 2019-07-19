//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_SSL_TOOL_H
#define M_BACK_SSL_TOOL_H
#include <openssl/ssl.h>
#include "http.h"
void init_server_ctx(void);


int create_ssl(int socket_in,SSL* ssl);

int ssl_read(struct http_client* client);

int ssl_write(struct http_client* client);

#endif //M_BACK_SSL_TOOL_H
