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
#define DAY10_BUTTON_INDICES_CAPACITY 10

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
    U32 *presses;
    I32 last_button_index;
} Day10_Find_Button_Presses_For_Light_Mask;

internal Buffer* day10_find_all_button_presses(MemoryArena *arena, Buffer *buttons, U32 light_mask)
{
    Buffer *button_presses = buffer_init(arena, sizeof(U32*), 1 << buttons->size);
    U64 num_combinations = (1ULL << buttons->size);

    for (U64 i = 0; i < num_combinations; ++i)
    {
        U32 current_mask = 0;
        for (U64 j = 0; j < buttons->size; ++j)
        {
            if ((i >> j) & 1)
            {
                current_mask ^= buffer_get(buttons, Day10_Buttons, j).button_mask;
            }
        }

        if (current_mask == light_mask)
        {
            U32 *presses = memory_arena_push_array(arena, U32, buttons->size);
            for (U64 j = 0; j < buttons->size; ++j)
            {
                presses[j] = (U32)((i >> j) & 1);
            }
            buffer_add(button_presses, U32*, presses);
        }
    }
    return button_presses;
}

internal U32 day10_find_button_presses_for_light_mask(MemoryArena *arena, Day10_Parsed_Line parsed_line)
{
    Buffer *valid_button_presses = day10_find_all_button_presses(arena, parsed_line.buttons, parsed_line.output);
    U32 smallest = U32_MAX;
    for (U64 i = 0; i < valid_button_presses->size; ++i)
    {
        U32 *presses = buffer_get(valid_button_presses, U32*, i);
        U32 count = 0;
        for (U64 j = 0; j < parsed_line.buttons->size; ++j)
        {
            if (presses[j])
            {
                ++count;
            }
        }

        if (count < smallest)
        {
            smallest = count;
        }
    }
    return smallest;
}

internal U64 day10_cache_hash(const void *key, U64 key_size)
{
    Buffer *counters = (Buffer*)key;
    U64 h = 14695981039346656037ull;
    for (U64 i = 0; i < counters->size; ++i)
    {
        h ^= buffer_get(counters, U32, i);
        h *= 1099511628211ull;
    }
    return h;
}

internal B32 day10_cache_eq(const void *a, const void *b, U64 key_size)
{
    Buffer *counters_a = (Buffer*)a;
    Buffer *counters_b = (Buffer*)b;
    return (common_memcmp(counters_a->data, counters_b->data, counters_a->size * sizeof(U32)) == 0);
}

internal inline B32 day10_counters_all_zero(Buffer *counters)
{
    for (U64 i = 0; i < counters->size; ++i)
    {
        if (buffer_get(counters, U32, i))
        {
            return false;
        }
    }

    return true;
}

internal U32 day10_find_button_presses_for_joltage_recursive(MemoryArena *arena, HashMap *cache, Buffer *buttons,
                                                             Buffer *counters)
{
    HashMapEntry *entry = hash_map_get(cache, counters);
    if (entry)
    {
        return *((U32*)entry->value);
    }

    if (day10_counters_all_zero(counters))
    {
        return 0;
    }

    U32 light_mask = 0;
    for (U64 i = 0; i < counters->size; ++i)
    {
        if (buffer_get(counters, U32, i) % 2)
        {
            light_mask |= (1 << i);
        }
    }

    Buffer *valid_button_presses = day10_find_all_button_presses(arena, buttons, light_mask);

    U32 smallest = U32_MAX;
    for (U64 i = 0; i < valid_button_presses->size; ++i)
    {
        U32 *valid_buttons = buffer_get(valid_button_presses, U32*, i);
        Buffer *new_counters = buffer_init(arena, sizeof(U32), counters->size);
        for (U64 j = 0; j < counters->size; ++j)
        {
            buffer_add(new_counters, U32, buffer_get(counters, U32, j));
        }

        U32 button_presses_count = 0;
        for (U64 j = 0; j < buttons->size; ++j)
        {
            if (!valid_buttons[j])
            {
                continue;
            }
            button_presses_count++;

            Day10_Buttons pressed_button = buffer_get(buttons, Day10_Buttons, j);
            for (U64 k = 0; k < pressed_button.button_indices->size; ++k)
            {
                U32 index = buffer_get(pressed_button.button_indices, U32, k);
                U32 *val = &((U32 *)new_counters->data)[index];
                if (*val == 0)
                {
                    goto next_iteration;
                }
                *val -= 1;
            }
        }

        for (U64 j = 0; j < new_counters->size; ++j)
        {
            ((U32 *)new_counters->data)[j] /= 2;
        }

        U32 sub_result = day10_find_button_presses_for_joltage_recursive(arena, cache, buttons, new_counters);
        if (sub_result != U32_MAX)
        {
            U32 result = 2 * sub_result + button_presses_count;
            if (result < smallest)
            {
                smallest = result;
            }
        }

    next_iteration:;
    }

    U32 *hash_map_value = memory_arena_alloc(arena, sizeof(U32));
    *hash_map_value = smallest;
    hash_map_set(cache, counters, hash_map_value);

    return smallest;
}

internal U32 day10_find_button_presses_for_joltage(MemoryArena *arena, Day10_Parsed_Line parsed_line)
{
    HashMapOptions options = {0};
    options.hash_fn = day10_cache_hash;
    options.eq_fn = day10_cache_eq;
    HashMap *cache = hash_map_init_with_options(arena, sizeof(Buffer *), sizeof(U32), 10000, options);

    return day10_find_button_presses_for_joltage_recursive(arena, cache, parsed_line.buttons, parsed_line.counters);
}

internal U64 day10_common(MemoryArena *arena, DayInput *day_input, U32 (*f)(MemoryArena *, Day10_Parsed_Line))
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
    day10_part2(DAY_CALL);
}
