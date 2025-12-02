#ifndef STRING_H
#define STRING_H
#include "common.h"

typedef struct {
    U8 *data;
    U64 size;
} String8;

static inline String8 string8_from_cstr(U8 *c_string, U64 size)
{
    String8 result = {
        .data = c_string,
        .size = size
    };

    return result;
}

#endif
