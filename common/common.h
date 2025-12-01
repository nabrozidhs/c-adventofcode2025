#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t U8;
typedef uint64_t U64;

typedef int64_t I64;

typedef uint32_t B32;

#define KILOBYTE(n) (n * 1024)
#define MEGABYTE(n) (KILOBYTE(n) * 1024)

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

#endif