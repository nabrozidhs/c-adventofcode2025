#ifndef BUFFER_H
#define BUFFER_H

typedef struct
{
    void *data;
    U64 size;
    U64 capacity;
} Buffer;

static inline Buffer *buffer_init(MemoryArena *arena, U64 element_size, U64 capacity)
{
    Buffer *result = memory_arena_push_struct(arena, Buffer);
    result->data = memory_arena_alloc(arena, element_size * capacity);
    result->size = 0;
    result->capacity = capacity;
    return result;
}

#define buffer_add(buffer, type, value)                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        assert((buffer)->size < (buffer)->capacity);                                                                   \
        ((type *)(buffer)->data)[(buffer)->size++] = (value);                                                          \
    } while (0)

#define buffer_get(buffer, type, index) (((type *)(buffer)->data)[(index)])

#endif
