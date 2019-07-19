//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_SSL_TOOL_H
#define M_BACK_SSL_TOOL_H
#include <openssl/ssl.h>
#include "buffer.h"
void init_server_ctx(void);


SSL*  create_ssl(int socket_in);

struct Buffer* ssl_read(SSL* ssl);

int ssl_write(SSL* ssl, const struct Buffer* write_buffer);

#endif //M_BACK_SSL_TOOL_H
