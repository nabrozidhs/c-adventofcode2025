typedef struct {
    U64 start;
    U64 end;
} Day05_Range;

internal B32 is_fresh(Buffer *buffer, U64 ingredient_id)
{
    for (U64 i = 0; i < buffer->size; ++i)
    {
        Day05_Range *range = &buffer_get(buffer, Day05_Range, i);
        if (range->start <= ingredient_id && ingredient_id <= range->end)
        {
            return true;
        }
    }
    return false;
}

internal Buffer *day05_common(MemoryArena *arena, DayInput *day_input)
{
    Buffer *ranges = buffer_init(arena, sizeof(Day05_Range), day_input->size / 3);
    while (day_input->content[day_input->position] != '\n')
    {
        Day05_Range *current_range = &buffer_get(ranges, Day05_Range, ranges->size++);
        DayInputNextU64 next_u64 = day_input_read_next_u64(day_input);
        assert(next_u64.is_valid);
        current_range->start = next_u64.value;
        ++day_input->position;

        next_u64 = day_input_read_next_u64(day_input);
        assert(next_u64.is_valid);
        current_range->end = next_u64.value;
        ++day_input->position;
    }

    return ranges;
}

internal void day05_part1(DAY_ARGS)
{
    Buffer *ranges = day05_common(arena, day_input);

    U64 sum = 0;
    for (;;)
    {
        DayInputNextU64 next_u64 = day_input_read_next_u64(day_input);
        if (!next_u64.is_valid)
        {
            break;
        }

        if (is_fresh(ranges, next_u64.value))
        {
            ++sum;
        }
    }
    printf("Part 1: %" PRIu64 "\n", sum);
}

internal int sort_by_start(const void *a, const void *b)
{
    U64 left_start = ((const Day05_Range*)a)->start;
    U64 right_start = ((const Day05_Range*)b)->start;
    return (left_start > right_start) - (left_start < right_start);
}

internal int sort_by_end(const void *a, const void *b)
{
    U64 left_end = ((const Day05_Range*)a)->end;
    U64 right_end = ((const Day05_Range*)b)->end;
    return (left_end > right_end) - (left_end < right_end);
}

internal void day05_part2(DAY_ARGS)
{
    Buffer *ranges = day05_common(arena, day_input);
    qsort(ranges->data, ranges->size, sizeof(Day05_Range), sort_by_end);
    qsort(ranges->data, ranges->size, sizeof(Day05_Range), sort_by_start);

    U64 previous_end = 0;
    U64 sum = 0;
    for (U64 i = 0; i < ranges->size; ++i)
    {
        Day05_Range *range = &buffer_get(ranges, Day05_Range, i);
        if (previous_end >= range->end)
        {
            continue;
        }

        U64 start = (previous_end >= range->start) ? (previous_end + 1) : range->start;
        sum += (range->end - start + 1);
        previous_end = range->end;
    }

    printf("Part 2: %" PRIu64 "\n", sum);
}

internal void day05(DAY_ARGS)
{
    day05_part1(DAY_CALL);
    day_input_reset(day_input);
    day05_part2(DAY_CALL);
}
