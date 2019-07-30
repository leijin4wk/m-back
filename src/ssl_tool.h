//
// Created by oyo on 2019-07-11.
//

#ifndef M_BACK_SSL_TOOL_H
#define M_BACK_SSL_TOOL_H
#include <openssl/ssl.h>
#include "buffer.h"
void init_server_ctx(void);

SSL *create_ssl(int event_fd);

int ssl_read_buffer(SSL *ssl,struct Buffer *read_buff);

int ssl_write_buffer(SSL *ssl,struct Buffer *write_buff);

int ssl_write_file(SSL *ssl,char* file_name,size_t file_size);

#endif //M_BACK_SSL_TOOL_H
