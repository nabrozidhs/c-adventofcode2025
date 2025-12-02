#include "common/common.h"

B32 is_invalid_id(U64 id, U64 id_length, U64 sequence_size)
{
    assert(id_length % sequence_size == 0);
    U64 base = common_powi_u64(10, sequence_size);
    U64 left = (id / common_powi_u64(10, (sequence_size * ((id_length - sequence_size) / sequence_size))));
    for (U64 i = 1; i < id_length / sequence_size; ++i)
    {
        U64 right = (id / common_powi_u64(10, (sequence_size * ((id_length - sequence_size * (i + 1)) / sequence_size)))) % base;
        if (left != right)
        {
            return false;
        }
    }
    return true;
}

B32 is_invalid_id_part1(U64 id)
{
    U64 length = common_log10_u64(id);
    if (length % 2 == 1)
    {
        return false;
    }
    return is_invalid_id(id, length, length / 2);
}

U64 day02_common(DayInput *day_input, B32 (* f) (U64))
{
    U64 sum = 0;

    for (;;)
    {
        DayInputNextU64 start = day_input_read_next_u64(day_input);
        assert(start.is_valid);
        DayInputNextU8 next_char = day_input_read_next_char(day_input);
        assert(next_char.is_valid);
        DayInputNextU64 end = day_input_read_next_u64(day_input);
        assert(end.is_valid);

        for (U64 i = start.value; i <= end.value; ++i)
        {
            if (f(i))
            {
                sum += i;
            }
        }

        next_char = day_input_read_next_char(day_input);
        if (!next_char.is_valid)
        {
            break;
        }
    }

    return sum;
}

void day02_part1(DAY_ARGS)
{
    U64 sum = day02_common(day_input, is_invalid_id_part1);
    printf("Part 1: %lu\n", sum);
}

B32 is_invalid_id_part2(U64 id)
{
    U64 length = common_log10_u64(id);
    for (int sequence_size = 1; sequence_size <= length / 2; ++sequence_size)
    {
        if ((length % sequence_size == 0) && is_invalid_id(id, length, sequence_size))
        {
            return true;
        }
    }

    return false;
}

void day02_part2(DAY_ARGS)
{
    U64 sum = day02_common(day_input, is_invalid_id_part2);
    printf("Part 2: %lu\n", sum);
}

void day02(DAY_ARGS)
{
    day02_part1(DAY_CALL);
    day_input_reset(day_input);
    day02_part2(DAY_CALL);
}
