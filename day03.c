typedef struct {
    U8 value;
    U64 position;
} Day03FindLargest;

internal Day03FindLargest day03_find_largest(String8 line)
{
    assert(line.size > 0);
    Day03FindLargest largest = {0};
    for (U64 i = 0; i < line.size; ++i)
    {
        U64 value = line.data[i];
        if (value > largest.value)
        {
            largest.value = value;
            largest.position = i;
        }
    }

    return largest;
}

internal U64 day03_find_n_largest(String8 line, U32 n)
{
    U64 result = 0;
    String8 input = {
        .data = line.data,
        .size = line.size - (n - 1)
    };
    for (U32 i = 0; i < n; ++i)
    {
        Day03FindLargest largest = day03_find_largest(input);
        result = (10 * result) + (U64)(largest.value - '0');

        input.data += (largest.position + 1);
        input.size = line.size - (n - 1) + (i + 1) - (input.data - line.data);
    }

    return result;
}

internal U64 day03_common(DayInput *day_input, U32 n)
{
    U64 sum = 0;
    for (;;)
    {
        DayInputNextLine next_line = day_input_read_next_line(day_input);
        if (!next_line.is_valid)
        {
            break;
        }

        U64 result = day03_find_n_largest(next_line.line, n);
        sum += result;
    }
    return sum;
}

internal void day03_part1(DAY_ARGS)
{
    U64 sum = day03_common(day_input, 2);
    printf("Part 1: %llu\n", sum);
}

internal void day03_part2(DAY_ARGS)
{
    U64 sum = day03_common(day_input, 12);
    printf("Part 2: %llu\n", sum);
}

void day03(DAY_ARGS)
{
    day03_part1(DAY_CALL);
    day_input_reset(day_input);
    day03_part2(DAY_CALL);
}
