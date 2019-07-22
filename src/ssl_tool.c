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
#define ssl_errno_s		ERR_error_string(ERR_get_error(), NULL)
SSL_CTX* ssl_ctx;
extern  dictionary* ini_file;
static int buffer_read_tls(SSL *ssl,struct Buffer *read_buff);
static int buffer_write_tls(SSL *ssl,struct Buffer *write_buff);
void  init_server_ctx(void)
{
    const char *cert_file = iniparser_getstring(ini_file,"server:cert_file","null");
    log_info("cert_file path is : %s", cert_file);
    const char *key_file =iniparser_getstring(ini_file,"server:key_file","null");
    log_info("key_file path is : %s", key_file);
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
SSL * create_ssl(int event_fd){
    /* 基于 ctx 产生一个新的 SSL */
    SSL * ssl= SSL_new(ssl_ctx);
    if (ssl == NULL) {
        log_err("SSL_new(): %s",ssl_errno_s);
        return NULL;
    }
    /* 将连接用户的 socket 加入到 SSL */
    SSL_set_fd(ssl,event_fd);
    SSL_set_accept_state(ssl);
    SSL_set_app_data(ssl, event_fd);
    /* 建立 SSL 连接 */
    ERR_clear_error();
    int	r= SSL_accept(ssl);
    if (r <= 0) {
        r = SSL_get_error(ssl, r);
        switch (r) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return ssl;
            default:
                log_err("SSL_accept(): %s", ssl_errno_s);
                return NULL;
        }
    }
    return ssl;
}

 int ssl_read(SSL *ssl,struct Buffer *read_buff) {
     int res = 0;
     while (true) {
         res= buffer_read_tls(ssl,read_buff);
         if(res==0){
             continue;
         }else if(res>0){
             return 1;
         }else{
             return -1;
         }
     }
}
//返回零代表要继续循环，返回1代表结束
static int buffer_read_tls(SSL *ssl,struct Buffer *read_buff)
{
    int		r;
    char buff[MAX_LINE];
    ERR_clear_error();
    r = SSL_read(ssl, buff, MAX_LINE);
    if (r <= 0) {
        r = SSL_get_error(ssl, r);
        switch (r) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return 0;
            case SSL_ERROR_SYSCALL:
                switch (errno) {
                    case EINTR:
                        return 1;
                    case EAGAIN:
                        return 0;
                    default:
                        return -1;
                }
                /* FALLTHROUGH */
            default:
                log_err("SSL_read(): %s", ssl_errno_s);
                return -1;
        }
    }
    if (buffer_add(read_buff,buff,r)==0)
    {
        log_err("buff add fail!");
        return -1;
    }
    return 1;
}
int ssl_write(SSL *ssl,struct Buffer *write_buff){
    int res = 0;
    while (true) {
        res= buffer_write_tls(ssl,write_buff);
        if(res==0){
            continue;
        }else if(res>0){
            return 1;
        }else{
            return -1;
        }
    }
}
static int buffer_write_tls(SSL *ssl,struct Buffer *write_buff){
    int		r;
    ERR_clear_error();
    r = SSL_write(ssl, write_buff->sent, write_buff->offset-write_buff->sent_size);
    if (r <= 0) {
        r = SSL_get_error(ssl, r);
        switch (r) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return 0;
            case SSL_ERROR_SYSCALL:
                switch (errno) {
                    case EINTR:
                        break;
                    case EAGAIN:
                        break;
                    default:
                        return -1;
                }
                /* FALLTHROUGH */
            default:
                log_err("SSL_write(): %s", ssl_errno_s);
                return -1;
        }
    }
    buffer_drain(write_buff, r);
    if(write_buff->sent_size==write_buff->offset){
        return 1;
    }else{
        return 0;
    }
}