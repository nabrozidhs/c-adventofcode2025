#include "common/common.h"
#include "days.c"

int main(void)
{
    U64 memory_buffer_size = MEGABYTE(32);
    void *memory_buffer = malloc(memory_buffer_size);
    MemoryArena arena = memory_arena_init(memory_buffer, memory_buffer_size);

    run_all(&arena);
}
