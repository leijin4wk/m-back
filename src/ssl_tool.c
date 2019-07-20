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
    /* 将连接用户的 socket 加入到 SSL */
    SSL_set_fd(ssl, socket_in);
    /* 建立 SSL 连接 */
    while (true) {
        if (SSL_accept(ssl) != 1) {
            int ret = SSL_get_error(ssl, -1);
            if (ret == SSL_ERROR_WANT_READ) {
                continue;
            } else {
                SSL_free(ssl);
                return NULL;
            }
        }else{
            break;
        }
    }
    return ssl;
}

 int ssl_read(struct http_client* client) {
     char buff[MAX_LINE];
     int res = 0;
     int i=0;
     while (true) {
         i++;
         res = SSL_read(client->ssl, buff, MAX_LINE);
         int err_res = SSL_get_error(client->ssl, res);
         if(res > 0)
         {
             if (buffer_add(client->request_data,buff,res)==0)
             {
                 log_err("buff add fail!");
                 return -1;
             }
             continue;
         }else if(res==0){
             if(err_res==SSL_ERROR_NONE) {
                 log_info("client_fd is %d read complete ,loop times %d",client->event_fd,i);
                 return 1;
             }else{
                 log_err("ret is 0 but err is %d,loop times %d",err_res,i);
                 return -1;
             }
         }else{
             if(err_res == SSL_ERROR_WANT_READ) {
                 log_info("client_fd is %d,ret is %d,err is SSL_ERROR_WANT_READ ,loop times %d",client->event_fd,res,i);
                 return 0;
             }else{
                 log_err("ret < 0");
                 return -1;
             }
         }
     }
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