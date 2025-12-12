#ifndef STRING_H
#define STRING_H

static inline B32 char_is_digit(U8 c)
{
    return ((c >= '0') && (c <= '9'));
}

typedef struct {
    U8 *str;
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
        .str = c_string,
        .size = size
    };

    return result;
}

static inline String8 string8_from_slice(U8 *slice, U64 size)
{
    String8 result = {
        .str = slice,
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

    B32 result = (common_memcmp(a.str, b.str, a.size) == 0);
    return result;
}

#endif
