#include "common/common.h"

internal void day06_part1(DAY_ARGS)
{
    U64 line_size = day_input_read_next_line(day_input).line.size;
    U64 lines = day_input->size / line_size;
    day_input_reset(day_input);

    Buffer* numbers = buffer_init(arena, sizeof(U64), (lines - 1) * (day_input->size / lines));
    U64 rows = lines - 1;
    for (;;)
    {
        DayInputNextU64 next_number = day_input_read_next_u64(day_input);
        if (!next_number.is_valid)
        {
            break;
        }
        buffer_add(numbers, U64, next_number.value);
    }

    U64 numbers_per_line = (numbers->size / rows);
    U64 total = 0;

    U64 current_column = 0;
    for (;;)
    {
        DayInputNextU8 next_char = day_input_read_next_char(day_input);
        if (!next_char.is_valid)
        {
            break;
        }

        U64 sum = buffer_get(numbers, U64, current_column);

        for (U64 i = 1; i < rows; ++i)
        {
            U64 value = buffer_get(numbers, U64, (i * numbers_per_line) + current_column);
            if (next_char.value == '+')
            {
                sum += value;
            }
            else
            {
                sum *= value;
            }
        }
        ++current_column;
        total += sum;
    }

    printf("Part 1: %" PRIu64 "\n", total);
}

internal void day06_part2(DAY_ARGS)
{
    DayInputNextLine operations_line = day_input_read_next_line(day_input);
    assert(operations_line.is_valid);
    U64 rows = 0;
    while (operations_line.line.str[0] != '+' && operations_line.line.str[0] != '*')
    {
        operations_line = day_input_read_next_line(day_input);
        assert(operations_line.is_valid);
        ++rows;
    }

    U64 total = 0;
    U64 group_start = 0;
    for (;;)
    {
        U64 group_end = group_start + 1;
        if (group_end >= operations_line.line.size)
        {
            break;
        }

        while (group_end < operations_line.line.size && operations_line.line.str[group_end] == ' ')
        {
            ++group_end;
        }

        U8 operation = operations_line.line.str[group_start];
        U64 columns_sum = (operation == '*') ? 1 : 0;
        for (U64 column = group_start; column < group_end; ++column)
        {
            U64 value = 0;
            for (U64 row = 0; row < rows; ++row)
            {
                U8 c = day_input->content[column + row * (operations_line.line.size + 1)]; // +1 because of newlines
                if (c != ' ')
                {
                    value = value * 10 + (c - '0');
                }
            }

            if (value > 0)
            {
                if (operation == '+')
                {
                    columns_sum += value;
                }
                else
                {
                    columns_sum *= value;
                }
            }
        }

        total += columns_sum;
        group_start = group_end;
    }

    printf("Part 2: %" PRIu64 "\n", total);
}

internal void day06(DAY_ARGS)
{
    day06_part1(DAY_CALL);
    day_input_reset(day_input);
    day06_part2(DAY_CALL);
}
