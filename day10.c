#include <math.h>

#include "common/common.h"

typedef struct {
    U32 button_mask;
    Buffer *button_indices;
} Day10_Buttons;

typedef struct {
    U32 output;
    Buffer *buttons;
    Buffer *counters;
} Day10_Parsed_Line;

#define DAY10_BUTTONS_CAPACITY 13
#define DAY10_BUTTON_INDICES_CAPACITY 9

internal Day10_Parsed_Line day10_parse_line(MemoryArena *arena, String8 next_line)
{
    Day10_Parsed_Line parsed_line = {0};

    U64 i = 1;
    assert(next_line.str[0] == '[');
    while (next_line.str[i] != ']')
    {
        if (next_line.str[i] == '#')
        {
            assert(i < 32);
            parsed_line.output |= 1 << (i - 1);
        }
        ++i;
    }
    i += 2;

    parsed_line.buttons = buffer_init(arena, sizeof(Day10_Buttons), DAY10_BUTTONS_CAPACITY);
    while (next_line.str[i] == '(')
    {
        Day10_Buttons buttons = {0};

        ++i;
        U32 button_mask = 0;
        Buffer *button_indices = buffer_init(arena, sizeof(U32), DAY10_BUTTON_INDICES_CAPACITY);
        while (next_line.str[i] != ')')
        {
            U32 number = 0;
            while (char_is_digit(next_line.str[i]))
            {
                number = number * 10 + (next_line.str[i++] - '0');
            }

            button_mask |= (1 << number);
            buffer_add(button_indices, U32, number);
            if (next_line.str[i] == ',')
            {
                ++i;
            }
        }
        buttons.button_mask = button_mask;
        buttons.button_indices = button_indices;
        buffer_add(parsed_line.buttons, Day10_Buttons, buttons);

        i += 2;
    }

    parsed_line.counters = buffer_init(arena, sizeof(U32), DAY10_BUTTON_INDICES_CAPACITY);
    assert(next_line.str[i++] == '{');
    while (next_line.str[i] != '}')
    {
        U32 number = 0;
        while (char_is_digit(next_line.str[i]))
        {
            number = number * 10 + (next_line.str[i++] - '0');
        }

        if (next_line.str[i] == ',')
        {
            ++i;
        }
        buffer_add(parsed_line.counters, U32, number);
    }

    return parsed_line;
}

typedef struct {
    U32 light_mask;
    U32 presses;
    I32 last_button_index;
} Day10_Find_Button_Presses_For_Light_Mask;

internal U32 day10_find_button_presses_for_light_mask(MemoryArena *arena, Day10_Parsed_Line parsed_line)
{
    U64 bfs_capacity = (1 << parsed_line.buttons->size);
    Buffer *bfs = buffer_init(arena, sizeof(Day10_Find_Button_Presses_For_Light_Mask), bfs_capacity);
    Day10_Find_Button_Presses_For_Light_Mask next = {0};
    next.last_button_index = -1;
    buffer_add(bfs, Day10_Find_Button_Presses_For_Light_Mask, next);
    U64 bfs_read_index = 0;
    for (;;)
    {
        next = buffer_get(bfs, Day10_Find_Button_Presses_For_Light_Mask, bfs_read_index++);
        if (next.light_mask == parsed_line.output)
        {
            return next.presses;
        }

        for (U64 i = (U64)(next.last_button_index + 1); i < parsed_line.buttons->size; ++i)
        {
            Day10_Find_Button_Presses_For_Light_Mask new = {
                .light_mask = next.light_mask ^ buffer_get(parsed_line.buttons, Day10_Buttons, i).button_mask,
                .presses = next.presses + 1,
                .last_button_index = (I32)i
            };
            buffer_add(bfs, Day10_Find_Button_Presses_For_Light_Mask, new);
        }
    }
}

typedef struct {
    U32 button_counts[DAY10_BUTTON_INDICES_CAPACITY];
    U32 presses;
    I32 last_button_index;
} Day10_Find_Button_Presses_For_Joltage;

internal inline B32 day10_button_counts_match(U32 *left_button_counts, U32 *right_button_counts)
{
    B32 result = (common_memcmp(left_button_counts, right_button_counts, sizeof(U32) * DAY10_BUTTON_INDICES_CAPACITY) == 0);
    return result;
}

internal U32 day10_find_button_presses_for_joltage(MemoryArena *arena, Day10_Parsed_Line parsed_line)
{
    U64 bfs_capacity = (1 << parsed_line.buttons->size);
    Buffer *bfs = buffer_init(arena, sizeof(Day10_Find_Button_Presses_For_Joltage), bfs_capacity);
    Day10_Find_Button_Presses_For_Joltage next = {0};
    next.last_button_index = -1;
    buffer_add(bfs, Day10_Find_Button_Presses_For_Joltage, next);
    U64 bfs_read_index = 0;
    for (;;)
    {
        next = buffer_get(bfs, Day10_Find_Button_Presses_For_Joltage, bfs_read_index++);
        if (day10_button_counts_match(next.button_counts, (U32 *)parsed_line.buttons->data))
        {
            return next.presses;
        }

        for (U64 i = (U64)(next.last_button_index + 1); i < parsed_line.buttons->size; ++i)
        {
            next.last_button_index = (I32)i;
            Day10_Find_Button_Presses_For_Joltage new = next;
            buffer_add(bfs, Day10_Find_Button_Presses_For_Joltage, new);
        }
    }
}

internal U64 day10_common(MemoryArena *arena, DayInput *day_input, U32 (* f) (MemoryArena *, Day10_Parsed_Line))
{
    U64 sum = 0;
    for (;;)
    {
        U64 memory_mark = memory_arena_mark(arena);
        DayInputNextLine next_line = day_input_read_next_line(day_input);
        if (!next_line.is_valid)
        {
            break;
        }

        Day10_Parsed_Line parsed_line = day10_parse_line(arena, next_line.line);

        U32 button_presses = f(arena, parsed_line);
        sum += button_presses;
        memory_arena_reset_to_mark(arena, memory_mark);
    }
    return sum;
}

internal void day10_part1(DAY_ARGS)
{
    U64 sum = day10_common(arena, day_input, day10_find_button_presses_for_light_mask);
    printf("Part 1: %" PRIu64 "\n", sum);
}

internal void day10_part2(DAY_ARGS)
{
    U64 sum = day10_common(arena, day_input, day10_find_button_presses_for_joltage);
    printf("Part 2: %" PRIu64 "\n", sum);
}

internal void day10(DAY_ARGS)
{
    day10_part1(DAY_CALL);
    day_input_reset(day_input);
    // day10_part2(DAY_CALL);
}
