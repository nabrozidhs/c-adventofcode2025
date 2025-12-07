#ifndef COMMON_H
#define COMMON_H

#define internal static

#include <assert.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t U8;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int32_t I32;
typedef int64_t I64;

typedef uint32_t B32;

#define KILOBYTE(n) (n * 1024L)
#define MEGABYTE(n) (KILOBYTE(n) * 1024L)

#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))

#include "vector.h"
#include "string.h"
#include "memory_arena.h"
#include "day_input.h"
#include "buffer.h"

#define DAY_ARGS DayInput *day_input, MemoryArena *arena
#define DAY_CALL day_input, arena

static inline I64 common_abs_i64(I64 value)
{
    I64 result = __builtin_llabs(value);
    return result;
}

static inline I64 common_mod_i64(I64 a, I64 b)
{
    I64 result = (a % b + b) % b;
    return result;
}

static inline U64 common_log10_u64(U64 value)
{
    if (value < 10) return 1;
    if (value < 100) return 2;
    if (value < 1000) return 3;
    if (value < 10000) return 4;
    if (value < 100000) return 5;
    if (value < 1000000) return 6;
    if (value < 10000000) return 7;
    if (value < 100000000) return 8;
    if (value < 1000000000) return 9;
    if (value < 10000000000ULL) return 10;
    if (value < 100000000000ULL) return 11;
    if (value < 1000000000000ULL) return 12;
    if (value < 10000000000000ULL) return 13;
    if (value < 100000000000000ULL) return 14;
    if (value < 1000000000000000ULL) return 15;
    if (value < 10000000000000000ULL) return 16;
    if (value < 100000000000000000ULL) return 17;
    if (value < 1000000000000000000ULL) return 18;
    if (value < 10000000000000000000ULL) return 19;
    return 20;
}

static inline U64 common_powi_u64(U64 x, U64 y)
{
    U64 result = (U64)__builtin_powi((double)x, (int)y);
    return result;
}

static inline int common_memcmp(void *left, void *right, U64 n)
{
    return __builtin_memcmp(left, right, n);
}

static inline void *common_memcpy(void *dest, void *src, U64 n)
{
    return __builtin_memcpy(dest, src, n);
}

#endif
