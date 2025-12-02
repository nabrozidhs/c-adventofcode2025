#ifndef INPUT_H
#define INPUT_H

typedef struct
{
    U8 *content;
    U64 size;
    U64 position;
} DayInput;

static inline DayInput day_input_from_file(char *filename, MemoryArena *arena)
{
    DayInput result = {0};

    FILE *f = fopen(filename, "r");
    assert(f != NULL);

    fseek(f, 0, SEEK_END);
    U64 size = ftell(f);
    fseek(f, 0, SEEK_SET);

    result.content = memory_arena_alloc(arena, size);
    U64 bytes_written = fread(result.content, sizeof(char), size, f);
    assert(bytes_written == size);
    result.size = bytes_written;

    return result;
}

static inline void day_input_reset(DayInput *input)
{
    input->position = 0;
}

static inline B32 is_digit(U8 c)
{
    return ((c >= '0') && (c <= '9'));
}

static inline B32 is_whitespace(U8 c)
{
    return ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t'));
}

static inline void day_input_skip_whitespace(DayInput *input)
{
    while ((input->position < input->size) && is_whitespace(input->content[input->position]))
    {
        ++input->position;
    }
}

typedef struct
{
    I64 value;
    B32 is_valid;
} DayInputNextI64;

static inline DayInputNextI64 day_input_read_next_i64(DayInput *input)
{
    DayInputNextI64 result = {0};

    day_input_skip_whitespace(input);

    if (is_digit(input->content[input->position]))
    {
        result.is_valid = true;
        // TODO handle '-' sign
        while ((input->position < input->size) && is_digit(input->content[input->position]))
        {
            result.value = result.value * 10 + (input->content[input->position] - '0');
            ++input->position;
        }
    }

    return result;
}

typedef struct
{
    U64 value;
    B32 is_valid;
    String8 string;
} DayInputNextU64;

static inline DayInputNextU64 day_input_read_next_u64(DayInput *input)
{
    DayInputNextU64 result = {0};

    day_input_skip_whitespace(input);

    U64 start_position = input->position;
    if (is_digit(input->content[input->position]))
    {
        result.is_valid = true;
        while ((input->position < input->size) && is_digit(input->content[input->position]))
        {
            result.value = result.value * 10 + (input->content[input->position] - '0');
            ++input->position;
        }
    }

    if (result.is_valid)
    {
        U64 size = (input->position - start_position);
        String8 string = string8_from_c_string((input->content + start_position), size);
        result.string = string;
    }

    return result;
}

typedef struct
{
    U8 value;
    B32 is_valid;
} DayInputNextU8;

static inline DayInputNextU8 day_input_read_next_char(DayInput *input)
{
    DayInputNextU8 result = {0};
    day_input_skip_whitespace(input);

    if (input->position < input->size)
    {
        result.value = input->content[input->position++];
        result.is_valid = true;
    }

    return result;
}

#endif