#include "common/common.h"

#define GIFT_SHAPE_COUNT 6

typedef struct {
    Vector2i size;
    U32 counts[GIFT_SHAPE_COUNT];
} Day12_Parsed_Line;

internal Buffer *day12_parse_line(MemoryArena *arena, DayInput *day_input)
{
    Buffer *parsed_lines = buffer_init(arena, sizeof(Day12_Parsed_Line), 1000);
    for (U64 i = 0; i < 30;)
    {
        if (day_input->content[day_input->position++] == '\n')
        {
            i++;
        }
    }

    for (;;)
    {
        Day12_Parsed_Line parsed_line = {0};
        DayInputNextU64 next_u64 = day_input_read_next_u64(day_input);
        if (!next_u64.is_valid)
        {
            break;
        }
        I32 width = next_u64.value;
        ++day_input->position;
        I32 height = day_input_read_next_u64(day_input).value;
        ++day_input->position;
        parsed_line.size = (Vector2i){.x = width / 3, .y = height / 3};

        for (U64 i = 0; i < GIFT_SHAPE_COUNT; ++i)
        {
            parsed_line.counts[i] = day_input_read_next_u64(day_input).value;
        }

        buffer_add(parsed_lines, Day12_Parsed_Line, parsed_line);
    }

    return parsed_lines;
}

internal void day12_part1(DAY_ARGS)
{
    Buffer *parsed_lines = day12_parse_line(arena, day_input);
    U64 count = 0;
    for (U64 i = 0; i < parsed_lines->size; ++i)
    {
        Day12_Parsed_Line parsed_line = buffer_get(parsed_lines, Day12_Parsed_Line, i);
        U64 total_shapes = 0;
        for (U64 j = 0; j < GIFT_SHAPE_COUNT; ++j)
        {
            total_shapes += parsed_line.counts[j];
        }

        if (total_shapes <= ((U64)parsed_line.size.x * (U64)parsed_line.size.y))
        {
            ++count;
        }
    }
    printf("Part 1: %" PRIu64 "\n", count);
}

internal void day12(DAY_ARGS)
{
    day12_part1(DAY_CALL);
}
