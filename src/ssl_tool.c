//
// Created by oyo on 2019-07-11.
//
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdbool.h>
#include "dictionary.h"
#include "iniparser.h"
#include "ssl_tool.h"
#include "dbg.h"

#include "http.h"

SSL_CTX* ssl_ctx;
extern  dictionary* ini_file;
void  init_server_ctx(void)
{
    const char *cert_file = iniparser_getstring(ini_file,"server:cert_file","null");
    debug("cert_file path is : %s", cert_file);
    const char *key_file =iniparser_getstring(ini_file,"server:key_file","null");
    debug("key_file path is : %s", key_file);
    /* init algorithms library */
    SSL_library_init();
    /* load & register all cryptos, etc. */
    OpenSSL_add_all_algorithms();
    /* load all error messages */
    SSL_load_error_strings();
    /* create new server-method instance and create new context from method */
    ssl_ctx = SSL_CTX_new(SSLv23_server_method());
    if ( ssl_ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ssl_ctx, cert_file, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ssl_ctx, key_file, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ssl_ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}
SSL* create_ssl(int socket_in){
    /* 基于 ctx 产生一个新的 SSL */
    SSL* ssl= SSL_new(ssl_ctx);
    if (!ssl){
        log_err("SSL_new error!\n");
        return NULL;
    }
    /* 将连接用户的 socket 加入到 SSL */
    SSL_set_fd(ssl, socket_in);
    /* 建立 SSL 连接 */
    bool is_continue = true;
    while(is_continue)
    {
        is_continue = false;
        if(SSL_accept(ssl) != 1)
        {
            int code = -1;
            int ret = SSL_get_error(ssl, code);
            if (ret == SSL_ERROR_WANT_READ)
            {
                is_continue = true;
            }
            else
            {
                SSL_free(ssl);
                ssl_ctx = NULL;
                ssl = NULL;
            }
        }
        else {
            break;
        }
    }
    return ssl;
}
struct Buffer* ssl_read(SSL* ssl){
    struct Buffer* read_buffer=new_buffer(MAX_LINE,MAX_REQUEST_SIZE);
    char buff[MAX_LINE];
    int res = 0;
    while (true)
    {
        res = SSL_read(ssl, buff, MAX_LINE);
        int err_res = SSL_get_error(ssl, res);
        if(err_res == SSL_ERROR_NONE)
        {
            if(res > 0)
            {
                if (buffer_add(read_buffer,buff,res)==0)
                {
                    log_err("buff add fail!");
                    free(read_buffer);
                    return NULL;
                }
                continue;
            }
        }
        else
        {
            break;
        }
    }
    return read_buffer;
}
int ssl_write(SSL* ssl, const struct Buffer* write_buffer){
    int res = 0, count = 0;
    while (true)
    {
        res = SSL_write(ssl, write_buffer->orig + count, write_buffer->offset - count);
        int err_res = SSL_get_error(ssl, res);
        if(err_res == SSL_ERROR_NONE)
        {
            if(res > 0)
            {
                if (count >= write_buffer->offset)
                {
                    break;
                }
                count += res;
                continue;
            }
        }
        else if (err_res == SSL_ERROR_WANT_READ)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    return count;
}