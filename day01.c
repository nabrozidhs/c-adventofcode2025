internal void day01_part1(DAY_ARGS)
{
    U64 count = 0;
    I64 dial = 50;

    DayInputNextU8 next_char = day_input_read_next_char(day_input);
    while (next_char.is_valid)
    {
        DayInputNextI64 next_number = day_input_read_next_i64(day_input);
        assert(next_number.is_valid);
        if (next_char.value == 'L')
        {
            dial = (dial - next_number.value) % 100;
        }
        else
        {
            dial = (dial + next_number.value) % 100;
        }

        if (dial == 0)
        {
            ++count;
        }

        next_char = day_input_read_next_char(day_input);
    }

    printf("Part 1: %llu\n", count);
}

internal void day01_part2(DAY_ARGS)
{
    U64 count = 0;
    I64 dial = 50;

    DayInputNextU8 next_char = day_input_read_next_char(day_input);
    while (next_char.is_valid)
    {
        DayInputNextI64 next_number = day_input_read_next_i64(day_input);
        assert(next_number.is_valid);
        if (next_char.value == 'L')
        {
            dial = ((100 - dial) % 100) + next_number.value;
            count += (dial / 100);
            dial = ((100 - (dial % 100)) % 100);
        }
        else
        {
            dial += next_number.value;
            count += (dial / 100);
            dial %= 100;
        }

        next_char = day_input_read_next_char(day_input);
    }

    printf("Part 2: %llu\n", count);
}

void day01(DAY_ARGS)
{
    day01_part1(DAY_CALL);
    day_input_reset(day_input);
    day01_part2(DAY_CALL);
}
