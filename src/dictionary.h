#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
typedef struct _dictionary_ {
    int             n ;     /** Number of entries in dictionary */
    ssize_t         size ;  /** Storage size */
    char        **  val ;   /** List of string values */
    char        **  key ;   /** List of string keys */
    unsigned     *  hash ;  /** List of hash values for keys */
} dictionary ;


unsigned dictionary_hash(const char * key);

dictionary * dictionary_new(size_t size);

void dictionary_del(dictionary * vd);

const char * dictionary_get(const dictionary * d, const char * key, const char * def);

int dictionary_set(dictionary * vd, const char * key, const char * val);

void dictionary_unset(dictionary * d, const char * key);

void dictionary_dump(const dictionary * d, FILE * out);

#endif
