#include "common/common.h"

typedef struct {
    U8 *content;
    Vector2i size;
} Day07_Find_Map_Size;

internal Day07_Find_Map_Size day07_find_map_size(MemoryArena *arena, DayInput *day_input)
{
    Day07_Find_Map_Size result = {0};

    result.content = memory_arena_push_array(arena, U8, day_input->size);
    for (;;)
    {
        DayInputNextLine next_line = day_input_read_next_line(day_input);
        if (!next_line.is_valid || next_line.line.size == 0)
        {
            break;
        }

        common_memcpy(result.content + result.size.y * result.size.x, next_line.line.data, next_line.line.size);

        result.size.x = MAXIMUM(result.size.x, next_line.line.size);
        ++result.size.y;
    }

    return result;
}

internal Vector2i find_start(Day07_Find_Map_Size *map)
{
    Vector2i result = {0};
    for (U64 x = 0; x < map->size.x; ++x)
    {
        if (map->content[x] == 'S')
        {
            result.x = x;
            return result;
        }
    }

    assert(false);
}

internal void day07_part1(DAY_ARGS)
{
    Day07_Find_Map_Size map = day07_find_map_size(arena, day_input);
    Buffer *beams_queue = buffer_init(arena, sizeof(Vector2i), map.size.x * map.size.y);

    Vector2i start = find_start(&map);
    Vector2i first_beam = (Vector2i){start.x, 1};
    map.content[first_beam.x + first_beam.y * map.size.x] = '|';
    buffer_add(beams_queue, Vector2i, first_beam);

    U64 total_splits = 0;
    U64 current_beam_index = 0;
    while (current_beam_index < beams_queue->size)
    {
        Vector2i beam = buffer_get(beams_queue, Vector2i, current_beam_index++);

        U64 y = beam.y + 1;
        while (y < map.size.y && map.content[beam.x + y * map.size.x] == '.')
        {
            map.content[beam.x + y * map.size.x] = '|';
            ++y;
        }

        if (y >= map.size.y || map.content[beam.x + y * map.size.x] != '^')
        {
            continue;
        }

        ++total_splits;
        if (map.content[beam.x - 1 + y * map.size.x] == '.')
        {
            Vector2i left_beam = (Vector2i){beam.x - 1, y};
            map.content[left_beam.x + left_beam.y * map.size.x] = '|';
            buffer_add(beams_queue, Vector2i, left_beam);
        }
        if (map.content[beam.x + 1 + y * map.size.x] == '.')
        {
            Vector2i right_beam = (Vector2i){beam.x + 1, y};
            map.content[right_beam.x + right_beam.y * map.size.x] = '|';
            buffer_add(beams_queue, Vector2i, right_beam);
        }
    }

    printf("Part 1: %llu\n", total_splits);
}

internal U64 find_timelines(Day07_Find_Map_Size *map, U64 *cache, Vector2i beam_position)
{
    U64 cache_value = cache[beam_position.x + beam_position.y * map->size.x];
    if (cache_value != 0)
    {
        return cache_value;
    }

    U64 y = beam_position.y + 1;
    while (y < map->size.y && map->content[beam_position.x + y * map->size.x] == '.')
    {
        ++y;
    }

    if (y >= map->size.y)
    {
        cache[beam_position.x + beam_position.y * map->size.x] = 1;
        return 1;
    }

    U64 left_timeline = find_timelines(map, cache, (Vector2i){beam_position.x - 1, y});
    U64 right_timeline = find_timelines(map, cache, (Vector2i){beam_position.x + 1, y});
    cache[beam_position.x + beam_position.y * map->size.x] = left_timeline + right_timeline;
    return left_timeline + right_timeline;
}

internal void day07_part2(DAY_ARGS)
{
    Day07_Find_Map_Size map = day07_find_map_size(arena, day_input);
    U64 *cache = memory_arena_push_array(arena, U64, map.size.x * map.size.y);
    for (U64 y = 0; y < map.size.y; ++y)
    {
        for (U64 x = 0; x < map.size.x; ++x)
        {
            cache[x + y * map.size.x] = 0;
        }
    }

    Vector2i start = find_start(&map);

    U64 total_timelines = find_timelines(&map, cache, start);

    printf("Part 2: %llu\n", total_timelines);
}

void day07(DAY_ARGS)
{
    day07_part1(DAY_CALL);
    day_input_reset(day_input);
    day07_part2(DAY_CALL);
}
