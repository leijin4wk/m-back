//
// Created by oyo on 2019-07-11.
//
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "dictionary.h"
#include "iniparser.h"
#include "ssl_tool.h"
#include "dbg.h"
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
int socket_add_ssl(int socket_in,SSL *ssl){
    /* 基于 ctx 产生一个新的 SSL */
    ssl = SSL_new(ssl_ctx);
    /* 将连接用户的 socket 加入到 SSL */
    SSL_set_fd(ssl, socket_in);
    /* 建立 SSL 连接 */
    if (SSL_accept(ssl) == -1) {
        log_err("accept");
        return -1;
    }
    return 0;
}

