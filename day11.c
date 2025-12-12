#include "common/common.h"

typedef struct {
    Buffer *device_position_index_to_device_id;
    Buffer *device_position_index_to_connected_device_ids;
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

    result.device_position_index_to_device_id = buffer_init(arena, sizeof(String8), new_line_count);
    result.device_position_index_to_connected_device_ids = buffer_init(arena, sizeof(Buffer*), new_line_count);
    for (;;)
    {
        DayInputNextLine next_line = day_input_read_next_line(day_input);
        if (!next_line.is_valid)
        {
            break;
        }

        String8 line = next_line.line;
        buffer_add(result.device_position_index_to_device_id, String8, ((String8){.str = line.str, .size = 3}));
        Buffer *connected_device_ids = buffer_init(arena, sizeof(String8), (line.size - 4) / 4);

        U64 i = 5;
        while (i < line.size)
        {
            buffer_add(connected_device_ids, String8, ((String8){.str = line.str + i, .size = 3}));
            i += 4;
        }
        buffer_add(result.device_position_index_to_connected_device_ids, Buffer*, connected_device_ids);
    }

    return result;
}

internal inline U64 day11_device_to_device_position_index(Day11_Parsed parsed, String8 device_id)
{
    for (U64 i = 0; i < parsed.device_position_index_to_device_id->size; ++i)
    {
        if (string8_equals(buffer_get(parsed.device_position_index_to_device_id, String8, i), device_id))
        {
            return i;
        }
    }

    assert(false);
}

typedef struct {
    U64 device_position_index;
    U32 path_size;
    U32 visited_nodes;
} Day11_Path;

internal U64 day11_common(MemoryArena *arena, DayInput *day_input, String8 start, String8 end, B32 (* p) (Day11_Path))
{
    Day11_Parsed parsed = day11_parse(arena, day_input);
    String8 fft = string8_from_c_string((U8 *)"fft");
    String8 dac = string8_from_c_string((U8 *)"dac");
    Day11_Path current_path = {0};
    current_path.device_position_index = day11_device_to_device_position_index(parsed, start);
    current_path.path_size = 1;
    Buffer *bfs = buffer_init(arena, sizeof(Day11_Path), 100000);
    buffer_add(bfs, Day11_Path, current_path);

    U64 bfs_read_index = 0;
    U64 sum = 0;
    while (bfs_read_index < bfs->size)
    {
        current_path = buffer_get(bfs, Day11_Path, bfs_read_index++);

        Buffer *connected_device_ids = buffer_get(parsed.device_position_index_to_connected_device_ids, Buffer*, current_path.device_position_index);
        for (U64 i = 0; i < connected_device_ids->size; ++i)
        {
            String8 connected_device_id = buffer_get(connected_device_ids, String8, i);
            if (string8_equals(connected_device_id, end))
            {
                if (p(current_path))
                {
                    ++sum;
                }
                continue;
            }
            Day11_Path new_path = current_path;
            new_path.path_size += 1;
            if (new_path.path_size > parsed.device_position_index_to_device_id->size)
            {
                continue;
            }

            new_path.device_position_index = day11_device_to_device_position_index(parsed, connected_device_id);
            new_path.visited_nodes |= (string8_equals(connected_device_id, dac) ? (1 << 0) : 0);
            new_path.visited_nodes |= (string8_equals(connected_device_id, fft) ? (1 << 1) : 0);
            buffer_add(bfs, Day11_Path, new_path);
        }
    }

    return sum;
}

internal B32 day11_part1_condition(Day11_Path path) { return true; }

internal void day11_part1(DAY_ARGS)
{
    U64 sum = day11_common(
        arena,
        day_input,
        string8_from_c_string((U8 *)"you"),
        string8_from_c_string((U8 *)"out"),
        day11_part1_condition
    );
    printf("Part 1: %llu\n", sum);
}

internal B32 day11_part2_condition(Day11_Path path) { return (path.visited_nodes & 3) == 3; }

internal void day11_part2(DAY_ARGS)
{
    U64 sum = day11_common(
        arena,
        day_input,
        string8_from_c_string((U8 *)"svr"),
        string8_from_c_string((U8 *)"out"),
        day11_part2_condition
    );
    printf("Part 2: %llu\n", sum);
}

internal void day11(DAY_ARGS)
{
    day11_part1(DAY_CALL);
    day_input_reset(day_input);
    day11_part2(DAY_CALL);
}
