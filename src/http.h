//
// Created by oyo on 2019-06-28.
//

#ifndef M_BACK_HTTP_H
#define M_BACK_HTTP_H
#include <sys/types.h>
#include <sys/queue.h>
#include <openssl/sha.h>
struct netbuf{

};
struct http_request {
    u_int8_t			method;
    u_int8_t			fsm_state;
    u_int16_t			flags;
    u_int16_t			status;
    u_int64_t			ms;
    u_int64_t			start;
    u_int64_t			end;
    u_int64_t			total;
    const char			*path;
    const char			*host;
    const char			*agent;
    const char			*referer;
    struct connection		*owner;
    SHA256_CTX			hashctx;
    u_int8_t			*headers;
    struct kore_buf			*http_body;
    int				http_body_fd;
    char				*http_body_path;
    size_t				http_body_length;
    size_t				http_body_offset;
    size_t				content_length;
    void				*hdlr_extra;
    size_t				state_len;
    char				*query_string;
    struct kore_module_handle	*hdlr;
    struct http_runlock_queue	*runlock;
    void				(*onfree)(struct http_request *);

    TAILQ_HEAD(, http_cookie)	req_cookies;
    TAILQ_HEAD(, http_cookie)	resp_cookies;
    TAILQ_HEAD(, http_header)	req_headers;
    TAILQ_HEAD(, http_header)	resp_headers;
    TAILQ_HEAD(, http_arg)		arguments;
    TAILQ_HEAD(, http_file)		files;
    TAILQ_ENTRY(http_request)	list;
    TAILQ_ENTRY(http_request)	olist;
};


void handler_request(void *ptr);

#endif //M_BACK_HTTP_H
