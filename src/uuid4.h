#ifndef UUID4_H
#define UUID4_H
#define UUID4_LEN 37

enum {
  UUID4_ESUCCESS =  0,
  UUID4_EFAILURE = -1
};

int  uuid4_init(void);
void uuid4_generate(char *dst);

#endif
