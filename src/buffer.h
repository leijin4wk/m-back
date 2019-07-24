#ifndef buffer_h
#define buffer_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
struct Buffer {
    char *data;
    char *orig;
    char * sent;//用于记录已经发送的位置
    size_t sent_size;//用于记录已经字节数
    size_t offset;
    size_t length;
    size_t capacity;
};

struct Buffer *new_buffer(size_t length, size_t capacity);
void free_buffer(struct Buffer *buf);
void buffer_reset(struct Buffer *buf);
int buffer_add(struct Buffer *buf, void *source, size_t length);
void buffer_drain(struct Buffer *buf, size_t length);
int buffer_expand(struct Buffer *buf, size_t need);
struct Buffer *read_file_to_buffer(const char* file_name);
#endif
