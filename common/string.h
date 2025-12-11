#ifndef STRING_H
#define STRING_H

typedef struct {
    U8 *data;
    U64 size;
} String8;

static inline String8 string8_from_c_string(U8 *c_string)
{
    U64 size = 0;
    while (c_string[size] != '\0')
    {
        ++size;
    }

    String8 result = {
        .data = c_string,
        .size = size
    };

    return result;
}

static inline String8 string8_from_slice(U8 *slice, U64 size)
{
    String8 result = {
        .data = slice,
        .size = size
    };
    return result;
}

static inline B32 string8_equals(String8 a, String8 b)
{
    if (a.size != b.size)
    {
        return false;
    }

    B32 result = (common_memcmp(a.data, b.data, a.size) == 0);
    return result;
}

#endif
