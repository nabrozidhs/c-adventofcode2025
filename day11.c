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
        *key = string8_from_slice(line.str, 3);
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
    String8 device_id;
    U32 path_size;
} Day11_Path;

internal U64 day11_common(MemoryArena *arena, Day11_Parsed *parsed, String8 start, String8 end)
{
    Day11_Path current_path = {0};
    current_path.path_size = 1;
    current_path.device_id = start;
    Buffer *bfs = buffer_init(arena, sizeof(Day11_Path), 100000);
    buffer_add(bfs, Day11_Path, current_path);

    U64 bfs_read_index = 0;
    U64 sum = 0;
    while (bfs_read_index < bfs->size)
    {
        current_path = buffer_get(bfs, Day11_Path, bfs_read_index++);

        Buffer *connected_device_ids = hash_map_get(parsed->device_ids_to_connected_device_ids, current_path.device_id.str)->value;
        for (U64 i = 0; i < connected_device_ids->size; ++i)
        {
            String8 connected_device_id = buffer_get(connected_device_ids, String8, i);
            if (string8_equals(connected_device_id, end))
            {
                ++sum;
                continue;
            }
            Day11_Path new_path = current_path;
            new_path.path_size += 1;
            if (new_path.path_size > parsed->device_ids_to_connected_device_ids->size)
            {
                continue;
            }

            new_path.device_id = connected_device_id;
            buffer_add(bfs, Day11_Path, new_path);
        }
    }

    return sum;
}

internal void day11_part1(DAY_ARGS)
{
    Day11_Parsed parsed = day11_parse(arena, day_input);
    U64 sum = day11_common(arena, &parsed, string8_from_c_string("you"), string8_from_c_string("out"));
    printf("Part 1: %" PRIu64 "\n", sum);
}

internal void day11_part2(DAY_ARGS)
{
    Day11_Parsed parsed = day11_parse(arena, day_input);
    U64 sum = (day11_common(arena, &parsed, string8_from_c_string("svr"), string8_from_c_string("fft")) *
            day11_common(arena, &parsed, string8_from_c_string("fft"), string8_from_c_string("dac")) *
            day11_common(arena, &parsed, string8_from_c_string("dac"), string8_from_c_string("out"))) +
        (day11_common(arena, &parsed, string8_from_c_string("svr"), string8_from_c_string("dac")) *
            day11_common(arena, &parsed, string8_from_c_string("dac"), string8_from_c_string("fft")) *
            day11_common(arena, &parsed, string8_from_c_string("fft"), string8_from_c_string("out")));
    printf("Part 2: %" PRIu64 "\n", sum);
}

internal void day11(DAY_ARGS)
{
    day11_part1(DAY_CALL);
    day_input_reset(day_input);
    day11_part2(DAY_CALL);
}
