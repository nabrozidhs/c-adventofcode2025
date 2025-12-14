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

static inline String8 string8_from_c_string(char *c_string)
{
    U64 size = 0;
    while (c_string[size] != '\0')
    {
        ++size;
    }

    String8 result = {
        .str = (U8 *)c_string,
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

// Taken from https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
internal inline U64 string8_hash_fnv1a(String8 s)
{
    U64 h = 14695981039346656037ull;
    for (U64 i = 0; i < s.size; ++i)
    {
        h ^= (U64)s.str[i];
        h *= 1099511628211ull;
    }
    return h;
}

#endif
