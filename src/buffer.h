#ifndef buffer_h
#define buffer_h

#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <ev.h>
#define BUFFER_HAS_DATA(b)  ((b)->offset)
#define BUFFER_USED(b)      ((b)->data - (b)->orig + (b)->offset)
#define BUFFER_AVAILABLE(b) ((b)->length - BUFFER_USED(b))

struct Buffer {
    char *data;
    char *orig;
    size_t offset;
    size_t length;
    size_t capacity;
};

struct Buffer *new_buffer(size_t length, size_t capacity);
void free_buffer(struct Buffer *buf);
void buffer_reset(struct Buffer *buf);
int buffer_add(struct Buffer *buf, void *source, size_t length);
void buffer_drain(struct Buffer *buf, size_t length);
int buffer_read_fd(struct Buffer *buf, int fd);
int buffer_write_fd(struct Buffer *buf, int fd);
int buffer_expand(struct Buffer *buf, size_t need);

#endif