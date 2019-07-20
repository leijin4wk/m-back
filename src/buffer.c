#include "buffer.h"
#include "dbg.h"

struct Buffer *new_buffer(size_t length, size_t capacity)
{
    struct Buffer *buf;
    buf = malloc(sizeof(struct Buffer));
    buf->orig = malloc(length);
    buf->sent=buf->orig;
    buf->data = buf->orig;
    buf->offset = 0;
    buf->length = length;
    buf->capacity = capacity;
    
    return buf;
}

void free_buffer(struct Buffer *buf)
{
    if (buf) {
        free(buf->orig);
        free(buf);
    }
}

void buffer_reset(struct Buffer *buf)
{
    buf->data = buf->orig;
    buf->offset = 0;
}

int buffer_expand(struct Buffer *buf, size_t need)
{
    size_t pos = buf->data - buf->orig;
    size_t expand = 0;
    size_t new_size = 0;

    log_info("%s: need %lu, pos %lu\n", __FUNCTION__, need, pos);
    
    if (need <= pos) {
        memmove(buf->orig, buf->data, buf->offset);
        buf->data = buf->orig;
        return 1;
    }
    
    expand = buf->length;
    while (expand < need) {
        expand = expand * 2;
    }

    log_info("%s: expanding by %lu\n", __FUNCTION__, expand);
    
    new_size = buf->length + expand;
    if (buf->capacity > 0 && new_size > buf->capacity) {
        return 0;
    }
    
    buf->orig = realloc(buf->orig, new_size);
    buf->data = buf->orig + pos;
    buf->length = new_size;
    
    return 1;
}

int buffer_add(struct Buffer *buf, void *source, size_t length)
{
    size_t used = buf->data - buf->orig + buf->offset;
    int32_t need = used + length - buf->length;

    log_info("%s: adding %d,used %d,length %d", __FUNCTION__, (int)length, (int)used, (int)buf->length);
    
    if (need > 0) {
        if (!buffer_expand(buf, need)) {
            return 0;
        }
    }
    
    memcpy(buf->data + buf->offset, source, length);
    buf->offset += length;
    
    return 1;
}

void buffer_drain(struct Buffer *buf, size_t length)
{
    if (length >= buf->offset) {
        buffer_reset(buf);
    } else {
        buf->sent += length;
        buf->offset -= length;
    }
}