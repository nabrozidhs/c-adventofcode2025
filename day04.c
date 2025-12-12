typedef struct {
    U8 *content;
    Vector2i size;
} Day04_Find_Map_Size;

internal Day04_Find_Map_Size day04_find_map_size(MemoryArena *arena, DayInput *day_input)
{
    Day04_Find_Map_Size result = {0};

    result.content = memory_arena_push_array(arena, U8, day_input->size);
    for (;;)
    {
        DayInputNextLine next_line = day_input_read_next_line(day_input);
        if (!next_line.is_valid || next_line.line.size == 0)
        {
            break;
        }

        common_memcpy(result.content + result.size.y * result.size.x, next_line.line.str, next_line.line.size);

        result.size.x = MAXIMUM(result.size.x, next_line.line.size);
        ++result.size.y;
    }

    return result;
}

internal B32 has_paper(Day04_Find_Map_Size *map, Vector2i position)
{
    if (position.x < 0 || position.y < 0 || position.x >= map->size.x || position.y >= map->size.y)
    {
        return false;
    }

    U8 value = map->content[position.x + position.y * map->size.x];
    return value == '@';
}

internal B32 can_access(Day04_Find_Map_Size *map, Vector2i position)
{
    U32 count = 0;

    for (I32 y = position.y - 1; y <= position.y + 1; ++y)
    {
        for (I32 x = position.x - 1; x <= position.x + 1; ++x)
        {
            if (y == position.y && x == position.x)
            {
                continue;
            }

            Vector2i position_to_check = {x, y};
            if (has_paper(map, position_to_check))
            {
                ++count;
                if (count == 4)
                {
                    return false;
                }
            }
        }
    }


    return true;
}

internal void day04_part1(Day04_Find_Map_Size *map)
{
    U64 sum = 0;

    for (U64 y = 0; y < map->size.y; ++y)
    {
        for (U64 x = 0; x < map->size.x; ++x)
        {
            Vector2i position = {x, y};
            if (!has_paper(map, position))
            {
                continue;
            }

            if (can_access(map, position))
            {
                ++sum;
            }
        }
    }

    printf("Part 1: %llu\n", sum);
}

internal void day04_part2(Day04_Find_Map_Size *map)
{
    U64 removed = 0;
    for (;;)
    {
        B32 found_at_least_one = false;
        for (U64 y = 0; y < map->size.y; ++y)
        {
            for (U64 x = 0; x < map->size.x; ++x)
            {
                Vector2i position = {x, y};
                if (!has_paper(map, position))
                {
                    continue;
                }

                if (can_access(map, position))
                {
                    found_at_least_one = true;
                    map->content[position.x + position.y * map->size.x] = '.';
                    ++removed;
                }
            }
        }

        if (!found_at_least_one)
        {
            break;
        }
    }

    printf("Part 2: %llu\n", removed);
}

internal void day04(DAY_ARGS)
{
    Day04_Find_Map_Size map = day04_find_map_size(arena, day_input);
    day04_part1(&map);
    day04_part2(&map);
}
