//
// Created by oyo on 2019-07-11.
//
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "cJSON.h"
#include "ssl_tool.h"
#include "dbg.h"
#define ssl_errno_s		ERR_error_string(ERR_get_error(), NULL)
SSL_CTX* ssl_ctx;
extern cJSON *json_config;
static int buffer_read_tls(SSL *ssl,struct Buffer *read_buff);
static int buffer_write_tls(SSL *ssl,struct Buffer *write_buff);
void  init_server_ctx(void)
{

    cJSON *cert_file_item=cJSON_GetObjectItem(json_config,"cert_file");
    if(cert_file_item==NULL){
        log_err("config file read fail!");
        exit(-1);
    }
    cJSON *key_file_item=cJSON_GetObjectItem(json_config,"key_file");
    if(key_file_item==NULL){
        log_err("config file read fail!");
        exit(-1);
    }
    const char *cert_file = cert_file_item->valuestring;
    log_info("cert_file path is : %s", cert_file);
    const char *key_file =key_file_item->valuestring;
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
        log_info("%d fd:%d",r,event_fd);
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

 int ssl_read_buffer(SSL *ssl,struct Buffer *read_buff) {
     int res = 0;
     int i=0;
     while (1) {
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
    char buff[MAX_READLINE];
    ERR_clear_error();
    r = SSL_read(ssl, buff, MAX_READLINE);
    if (r <= 0) {
        r = SSL_get_error(ssl, r);
        switch (r) {
            case SSL_ERROR_WANT_READ:
                return 0;
            case SSL_ERROR_WANT_WRITE:
                return 0;
            case SSL_ERROR_SYSCALL:
                switch (errno) {
                    case EINTR:
                        return 1;
                    case EAGAIN:
                        log_err("EAGAIN");
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
int ssl_write_buffer(SSL *ssl,struct Buffer *write_buff){
    int res = 0;
    while (1) {
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
int ssl_write_file(SSL *ssl,char *file_name,size_t file_size){
    int src_fd = open(file_name, O_RDONLY, 0);
    char *src_addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
    if (src_addr == (void *) -1){
        log_err("%s mmap error",file_name);
    }
    int offset=0;
    int res = 0;
    while (1) {
        if (file_size==offset){
            break;
        }
        res = SSL_write(ssl, src_addr+offset, file_size-offset);
        if(res>0){
            offset+=res;
            continue;
        }else{
            res = SSL_get_error(ssl, res);
            switch (res) {
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                    continue;
                case SSL_ERROR_SYSCALL:
                    switch (errno) {
                        case EINTR:
                        case EAGAIN:
                            continue;
                        default:
                            close(src_fd);
                            munmap(src_addr, file_size);
                            return -1;
                    }
                    /* FALLTHROUGH */
                default:
                    log_err("SSL_file_write(): %s", ssl_errno_s);
                    close(src_fd);
                    munmap(src_addr, file_size);
                    return -1;
            }
        }
    }
    munmap(src_addr, file_size);
    close(src_fd);
    return 1;
}