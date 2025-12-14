#include "common/common.h"

typedef struct {
    HashMap *device_ids_to_connected_device_ids;
} Day11_Parsed;

internal Day11_Parsed day11_parse(MemoryArena *arena, DayInput *day_input)
{
    Day11_Parsed result = {0};
    U64 new_line_count = 0;
    for (U64 i = 0; i < day_input->size; ++i)
    {
        if (day_input->content[i] == '\n')
        {
            ++new_line_count;
        }
    }

    result.device_ids_to_connected_device_ids = hash_map_init(arena, 3, sizeof(Buffer*), new_line_count * 3 / 2);
    for (;;)
    {
        DayInputNextLine next_line = day_input_read_next_line(day_input);
        if (!next_line.is_valid)
        {
            break;
        }

        String8 line = next_line.line;
        String8 *key = memory_arena_push_struct(arena, String8);
        *key = string8_from_bytes(line.str, 3);
        Buffer *connected_device_ids = buffer_init(arena, sizeof(String8), (line.size - 4) / 4);

        U64 i = 5;
        while (i < line.size)
        {
            buffer_add(connected_device_ids, String8, ((String8){.str = line.str + i, .size = 3}));
            i += 4;
        }

        hash_map_set(result.device_ids_to_connected_device_ids, key->str, connected_device_ids);
    }

    hash_map_set(result.device_ids_to_connected_device_ids, "out", buffer_init(arena, sizeof(String8), 0));

    return result;
}

typedef struct {
    U8 *from;
    U8 *to;
} Day11_Cache_Connection;

internal U64 day11_cache_hash(const void *key, U64 key_size)
{
    Day11_Cache_Connection *connection = (Day11_Cache_Connection*)key;
    U64 h = 14695981039346656037ull;
    for (U64 i = 0; i < 3; ++i)
    {
        h ^= connection->from[i];
        h *= 1099511628211ull;
    }
    for (U64 i = 0; i < 3; ++i)
    {
        h ^= connection->to[i];
        h *= 1099511628211ull;
    }
    return h;
}

internal B32 day11_cache_eq(const void *a, const void *b, U64 key_size)
{
    Day11_Cache_Connection *connection_a = (Day11_Cache_Connection*)a;
    Day11_Cache_Connection *connection_b = (Day11_Cache_Connection*)b;
    if (common_memcmp(connection_a->from, connection_b->from, 3) != 0)
    {
        return false;
    }
    if (common_memcmp(connection_a->to, connection_b->to, 3) != 0)
    {
        return false;
    }
    return true;
}

internal inline HashMap* day11_init_cache(MemoryArena *arena)
{
    HashMapOptions options = {0};
    options.hash_fn = day11_cache_hash;
    options.eq_fn = day11_cache_eq;
    HashMap *cache = hash_map_init_with_options(
        arena,
        sizeof(Day11_Cache_Connection),
        sizeof(U64),
        100000,
        options
    );
    return cache;
}

internal U64 day11_common(MemoryArena *arena, Day11_Parsed *parsed, HashMap *cache, String8 start, String8 end)
{
    if (string8_equals(start, end))
    {
        return 1;
    }

    Day11_Cache_Connection lookup = {0};
    lookup.from = start.str;
    lookup.to = end.str;

    HashMapEntry *cache_result = hash_map_get(cache, &lookup);
    if (cache_result)
    {
        return *(U64 *)cache_result->value;
    }

    U64 total_paths = 0;

    Buffer *connected_device_ids = hash_map_get(parsed->device_ids_to_connected_device_ids, start.str)->value;
    for (U64 i = 0; i < connected_device_ids->size; ++i)
    {
        String8 connected_device_id = buffer_get(connected_device_ids, String8, i);
        total_paths += day11_common(arena, parsed, cache, connected_device_id, end);
    }

    Day11_Cache_Connection *key = memory_arena_push_struct(arena, Day11_Cache_Connection);
    key->from = start.str;
    key->to = end.str;
    U64 *value = memory_arena_alloc(arena, sizeof(U64));
    *value = total_paths;
    hash_map_set(cache, key, value);

    return total_paths;
}

internal void day11_part1(DAY_ARGS)
{
    Day11_Parsed parsed = day11_parse(arena, day_input);
    HashMap *cache = day11_init_cache(arena);
    U64 sum = day11_common(arena, &parsed, cache, string8_from_c_string("you"), string8_from_c_string("out"));
    printf("Part 1: %" PRIu64 "\n", sum);
}

internal void day11_part2(DAY_ARGS)
{
    Day11_Parsed parsed = day11_parse(arena, day_input);
    HashMap *cache = day11_init_cache(arena);

    U64 sum = (day11_common(arena, &parsed, cache, string8_from_c_string("svr"), string8_from_c_string("fft")) *
            day11_common(arena, &parsed, cache, string8_from_c_string("fft"), string8_from_c_string("dac")) *
            day11_common(arena, &parsed, cache, string8_from_c_string("dac"), string8_from_c_string("out"))) +
        (day11_common(arena, &parsed, cache, string8_from_c_string("svr"), string8_from_c_string("dac")) *
            day11_common(arena, &parsed, cache, string8_from_c_string("dac"), string8_from_c_string("fft")) *
            day11_common(arena, &parsed, cache, string8_from_c_string("fft"), string8_from_c_string("out")));
    printf("Part 2: %" PRIu64 "\n", sum);
}

internal void day11(DAY_ARGS)
{
    day11_part1(DAY_CALL);
    day_input_reset(day_input);
    day11_part2(DAY_CALL);
}
