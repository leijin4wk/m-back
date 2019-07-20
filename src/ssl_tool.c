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
static int
buffer_read_tls(struct http_client* client);
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
int create_ssl(struct http_client *client){
    /* 基于 ctx 产生一个新的 SSL */
    client->ssl= SSL_new(ssl_ctx);
    if (client->ssl == NULL) {
        log_err("SSL_new(): %s",ssl_errno_s);
        return -1;
    }
    /* 将连接用户的 socket 加入到 SSL */
    SSL_set_fd(client->ssl,client->event_fd);
    SSL_set_accept_state(client->ssl);
    SSL_set_app_data(client->ssl, client);
    /* 建立 SSL 连接 */
    ERR_clear_error();
    int	r= SSL_accept(client->ssl);
    if (r <= 0) {
        r = SSL_get_error(client->ssl, r);
        switch (r) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return 0;
            default:
                log_err("SSL_accept(): %s", ssl_errno_s);
                return -1;
        }
    }
    return 0;
}

 int ssl_read(struct http_client* client) {
     int res = 0;
     while (true) {
         res= buffer_read_tls(client);
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
static int
buffer_read_tls(struct http_client* client)
{
    int		r;
    char buff[MAX_LINE];
    ERR_clear_error();
    r = SSL_read(client->ssl, buff, MAX_LINE);
    if (r <= 0) {
        r = SSL_get_error(client->ssl, r);
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
    if (buffer_add(client->request_data,buff,r)==0)
    {
        log_err("buff add fail!");
        return -1;
    }
    return 1;
}
int ssl_write(struct http_client* client){
    int res = 0, count = 0;
    while (true) {
        res = SSL_write(client->ssl, client->response_data->sent + count, client->response_data->offset - count);
        int err_res = SSL_get_error(client->ssl, res);
        if (res > 0) {
            if (client->response_data->sent == client->response_data->data) {
                return 1;
            }
            count += count;
            buffer_drain(client->response_data, res);
            continue;
        } else {
            if (err_res == SSL_ERROR_WANT_WRITE) {
                return 0;
            } else {
                log_err("ret < 0");
                return -1;
            }

        }
    }
}