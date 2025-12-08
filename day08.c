#include "common/common.h"

typedef struct {
    U64 position_index_1;
    U64 position_index_2;
    F32 distance;
} Day08_Position_Indices_And_Distance;

int day08_compare_by_distance(const void *a, const void *b)
{
    F32 distance_left = ((Day08_Position_Indices_And_Distance*)a)->distance;
    F32 distance_right = ((Day08_Position_Indices_And_Distance*)b)->distance;

    if (distance_left < distance_right)
    {
        return -1;
    }
    if (distance_left > distance_right)
    {
        return 1;
    }
    return 0;
}

int day08_compare_by_u64(const void *a, const void *b)
{
    U64 left = *(U64*)a;
    U64 right = *(U64*)b;

    if (left < right)
    {
        return -1;
    }
    if (left > right)
    {
        return 1;
    }
    return 0;
}

internal Buffer* parse_positions(MemoryArena *arena, DayInput *day_input)
{
    Buffer *positions = buffer_init(arena, sizeof(Vector3i), day_input->size / 3);
    for (;;)
    {
        DayInputNextI64 next_i64 = day_input_read_next_i64(day_input);
        if (!next_i64.is_valid)
        {
            break;
        }

        assert(positions->size < positions->capacity);
        Vector3i *v = &buffer_get(positions, Vector3i, positions->size++);
        v->x = (I32)next_i64.value;
        day_input_read_next_char(day_input);
        v->y = day_input_read_next_i64(day_input).value;
        day_input_read_next_char(day_input);
        v->z = day_input_read_next_i64(day_input).value;
    }
    return positions;
}

internal Buffer *day08_initialise_position_indices_and_distances(MemoryArena *arena, Buffer *positions)
{
    Buffer *position_indices_and_distance = buffer_init(
        arena,
        sizeof(Day08_Position_Indices_And_Distance),
        positions->size * (positions->size - 1) / 2
    );

    for (U64 i = 0; i < positions->size; ++i)
    {
        Vector3i vector_1 = buffer_get(positions, Vector3i, i);
        for (U64 j = i + 1; j < positions->size; ++j)
        {
            Day08_Position_Indices_And_Distance element = {0};
            element.position_index_1 = i;
            element.position_index_2 = j;
            element.distance = vector3i_euclidian_distance_squared(vector_1, buffer_get(positions, Vector3i, j));
            buffer_add(position_indices_and_distance, Day08_Position_Indices_And_Distance, element);
        }
    }

    assert(position_indices_and_distance->size == position_indices_and_distance->capacity);
    qsort(position_indices_and_distance->data,
          position_indices_and_distance->size,
          sizeof(Day08_Position_Indices_And_Distance),
          day08_compare_by_distance
    );

    return position_indices_and_distance;
}

internal U64 *initialise_position_index_to_circuit_id(MemoryArena *arena, Buffer *positions)
{
    U64 *position_index_to_circuit_id = memory_arena_push_array(arena, U64, positions->size);
    for (int i = 0; i < positions->size; ++i)
    {
        position_index_to_circuit_id[i] = i;
    }
    return position_index_to_circuit_id;
}

internal void day08_part1(DAY_ARGS)
{
    U64 n_iterations = 10; // NOTE: change this to 1000 for the real input

    Buffer *positions = parse_positions(arena, day_input);
    Buffer *position_indices_and_distance = day08_initialise_position_indices_and_distances(arena, positions);
    U64 *position_index_to_circuit_id = initialise_position_index_to_circuit_id(arena, positions);

    for (int iteration = 0; iteration < n_iterations; ++iteration)
    {
        Day08_Position_Indices_And_Distance position_indices = buffer_get(
            position_indices_and_distance, Day08_Position_Indices_And_Distance, iteration);
        U64 circuit_id_1 = position_index_to_circuit_id[position_indices.position_index_1];
        U64 circuit_id_2 = position_index_to_circuit_id[position_indices.position_index_2];
        if (circuit_id_1 == circuit_id_2)
        {
            continue;
        }

        for (int i = 0; i < positions->size; ++i)
        {
            if (position_index_to_circuit_id[i] == circuit_id_2)
            {
                position_index_to_circuit_id[i] = circuit_id_1;
            }
        }
    }

    U64 *counts = memory_arena_push_array(arena, U64, positions->size);
    for (U64 i = 0; i < positions->size; ++i)
    {
        counts[i] = 0;
    }

    for (U64 i = 0; i < positions->size; ++i)
    {
        U64 count = 0;
        for (U64 j = 0; j < positions->size; ++j)
        {
            if (position_index_to_circuit_id[j] == i)
            {
                ++count;
            }
        }
        counts[i] = count;
    }

    qsort(counts, positions->size, sizeof(U64), day08_compare_by_u64);
    U64 take = 3;
    U64 result = 1;
    for (U64 i = positions->size - 1; i > 0; --i)
    {
        U64 value = counts[i];
        if (value > 0)
        {
            result *= value;
            --take;
            if (take == 0)
            {
                break;
            }
        }
    }

    printf("Part 1: %llu\n", result);
}

internal void day08_part2(DAY_ARGS)
{
    Buffer *positions = parse_positions(arena, day_input);
    Buffer *position_indices_and_distance = day08_initialise_position_indices_and_distances(arena, positions);
    U64 *position_index_to_circuit_id = initialise_position_index_to_circuit_id(arena, positions);

    U64 iteration = 0;
    U64 left_position_index = 0;
    U64 right_position_index = 0;

    U64 connected = 1;
    while (connected < positions->size)
    {
        Day08_Position_Indices_And_Distance position_indices = buffer_get(
                    position_indices_and_distance,
                    Day08_Position_Indices_And_Distance,
                    iteration++
        );
        U64 circuit_id_1 = position_index_to_circuit_id[position_indices.position_index_1];
        U64 circuit_id_2 = position_index_to_circuit_id[position_indices.position_index_2];
        if (circuit_id_1 == circuit_id_2)
        {
            continue;
        }

        ++connected;
        for (int i = 0; i < positions->size; ++i)
        {
            if (position_index_to_circuit_id[i] == circuit_id_2)
            {
                position_index_to_circuit_id[i] = circuit_id_1;
            }
        }

        left_position_index = position_indices.position_index_1;
        right_position_index = position_indices.position_index_2;
    }

    U64 result = (U64)buffer_get(positions, Vector3i, left_position_index).x * (U64)buffer_get(positions, Vector3i, right_position_index).x;
    printf("Part 2: %llu\n", result);
}

void day08(DAY_ARGS)
{
    day08_part1(DAY_CALL);
    day_input_reset(day_input);
    day08_part2(DAY_CALL);
}
