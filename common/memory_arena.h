#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

typedef struct
{
    void *base;
    U64 size;
    U64 used;
} MemoryArena;

static inline MemoryArena memory_arena_init(void *backing_buffer, U64 size)
{
    MemoryArena arena = {
        .base = backing_buffer,
        .size = size,
        .used = 0
    };

    return arena;
}

static inline U64 align_up(U64 n, U64 alignment)
{
    return (n + alignment - 1) & ~(alignment - 1);
}

static inline void *memory_arena_alloc_aligned(MemoryArena *arena, U64 size, U64 alignment)
{
    U64 aligned_up = align_up(arena->used, alignment);
    assert(aligned_up + size <= arena->size);

    void *ptr = (U8 *)arena->base + aligned_up;
    arena->used = (aligned_up + size);
    return ptr;
}

static inline void *memory_arena_alloc(MemoryArena *arena, U64 size)
{
    U64 alignment = alignof(max_align_t);
    void *ptr = memory_arena_alloc_aligned(arena, size, alignment);
    return ptr;
}

#define memory_arena_push_array(arena, type, count) (type *)memory_arena_alloc(arena, sizeof(type) * count)
#define memory_arena_push_struct(arena, type) memory_arena_push_array(arena, type, 1)

static inline void memory_arena_reset(MemoryArena *arena)
{
    arena->used = 0;
}

#endif