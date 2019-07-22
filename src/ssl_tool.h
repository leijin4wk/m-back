//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_SSL_TOOL_H
#define M_BACK_SSL_TOOL_H
#include <openssl/ssl.h>
#include "buffer.h"
void init_server_ctx(void);

SSL *create_ssl(int event_fd);

int ssl_read(SSL *ssl,struct Buffer *read_buff);

int ssl_write(SSL *ssl,struct Buffer *write_buff);

#endif //M_BACK_SSL_TOOL_H
