internal Buffer* day09_parse_positions(MemoryArena *arena, DayInput *day_input)
{
    Buffer *positions = buffer_init(arena, sizeof(Vector2i), day_input->size);
    for (;;)
    {
        DayInputNextI64 next_i64 = day_input_read_next_i64(day_input);
        if (!next_i64.is_valid)
        {
            break;
        }

        Vector2i position = {0};
        position.x = (I32)next_i64.value;
        DayInputNextU8 next_u8 = day_input_read_next_char(day_input);
        assert(next_u8.is_valid);
        next_i64 = day_input_read_next_i64(day_input);
        assert(next_i64.is_valid);
        position.y = (I32)next_i64.value;
        buffer_add(positions, Vector2i, position);
    }
    return positions;
}

internal void day09_part1(DAY_ARGS)
{
    Buffer *positions = day09_parse_positions(arena, day_input);
    U64 largest = 0;
    for (U64 i = 0; i < positions->size; ++i)
    {
        for (U64 j = i + 1; j < positions->size; ++j)
        {
            Vector2i vector_1 = buffer_get(positions, Vector2i, i);
            Vector2i vector_2 = buffer_get(positions, Vector2i, j);

            U64 area = (1 + common_abs_i64(vector_1.x - vector_2.x)) * (1 + common_abs_i64(vector_1.y - vector_2.y));
            if (area > largest)
            {
                largest = area;
            }
        }
    }

    printf("Part 1: %llu\n", largest);
}

typedef struct {
    Vector2i corner_1;
    Vector2i corner_2;
    U64 area;
} Day09_Corners_To_Area;

internal int day09_sort_by_area(const void *a, const void *b)
{
    U64 left = ((const Day09_Corners_To_Area*)a)->area;
    U64 right = ((const Day09_Corners_To_Area*)b)->area;

    return (left > right) - (left < right);
}

typedef struct {
    Vector2i edge_1;
    Vector2i edge_2;
} Day09_Edge;

internal Buffer* day09_initialise_edges(MemoryArena *arena, Buffer *positions)
{
    Buffer *edges = buffer_init(arena, sizeof(Day09_Edge), positions->size);
    for (U64 i = 0; i < positions->size; ++i)
    {
        Day09_Edge edge = {0};
        edge.edge_1 = buffer_get(positions, Vector2i, i);
        edge.edge_2 = buffer_get(positions, Vector2i, (i + 1) % positions->size);
        assert(edge.edge_1.x == edge.edge_2.x || edge.edge_1.y == edge.edge_2.y);
        buffer_add(edges, Day09_Edge, edge);
    }
    return edges;
}

internal B32 day09_point_inside_edges(Vector2i point, Buffer *edges)
{
    U64 crossings = 0;
    for (U64 i = 0; i < edges->size; ++i)
    {
        Day09_Edge edge = buffer_get(edges, Day09_Edge, i);

        if (edge.edge_1.y == edge.edge_2.y)
        {
            continue;
        }

        I32 min_y = MINIMUM(edge.edge_1.y, edge.edge_2.y);
        I32 max_y = MAXIMUM(edge.edge_1.y, edge.edge_2.y);

        if (min_y <= point.y && point.y < max_y && (edge.edge_1.x < point.x))
        {
            crossings += 1;
        }
    }
    return crossings % 2 == 1;
}

internal B32 day09_is_rectangle_inside(Day09_Edge rect, Buffer *edges)
{
    if (!day09_point_inside_edges(rect.edge_1, edges) || !day09_point_inside_edges(rect.edge_1, edges))
    {
        return false;
    }

    I32 min_x = MINIMUM(rect.edge_1.x, rect.edge_2.x);
    I32 max_x = MAXIMUM(rect.edge_1.x, rect.edge_2.x);
    I32 min_y = MINIMUM(rect.edge_1.y, rect.edge_2.y);
    I32 max_y = MAXIMUM(rect.edge_1.y, rect.edge_2.y);

    for (U64 i = 0; i < edges->size; ++i)
    {
        Day09_Edge edge = buffer_get(edges, Day09_Edge, i);

        // Horizontal line
        if (edge.edge_1.y == edge.edge_2.y)
        {
            I32 edge_min_x = MINIMUM(edge.edge_1.x, edge.edge_2.x);
            I32 edge_max_x = MAXIMUM(edge.edge_1.x, edge.edge_2.x);
            if (min_y < edge.edge_1.y && edge.edge_1.y < max_y)
            {
                if (!(edge_max_x <= min_x || edge_min_x >= max_x))
                {
                    return false;
                }
            }
        }

        // Vertical line
        if (edge.edge_1.x == edge.edge_2.x)
        {
            I32 edge_min_y = MINIMUM(edge.edge_1.y, edge.edge_2.y);
            I32 edge_max_y = MAXIMUM(edge.edge_1.y, edge.edge_2.y);
            if (min_x < edge.edge_1.x && edge.edge_1.x < max_x)
            {
                if (!(edge_max_y <= min_y || edge_min_y >= max_y))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

internal void day09_part2(DAY_ARGS)
{
    Buffer *red_positions = day09_parse_positions(arena, day_input);
    Buffer *edges = day09_initialise_edges(arena, red_positions);
    Buffer *corners_to_area = buffer_init(arena, sizeof(Day09_Corners_To_Area),
                                          red_positions->size * (red_positions->size - 1) / 2);
    for (U64 i = 0; i < red_positions->size; ++i)
    {
        Vector2i corner_1 = buffer_get(red_positions, Vector2i, i);

        for (U64 j = i + 1; j < red_positions->size; ++j)
        {
            Day09_Corners_To_Area corner_to_area = {0};
            corner_to_area.corner_1 = corner_1;
            corner_to_area.corner_2 = buffer_get(red_positions, Vector2i, j);
            corner_to_area.area = (1 + common_abs_i64(corner_to_area.corner_1.x - corner_to_area.corner_2.x)) * (1 +
                common_abs_i64(corner_to_area.corner_1.y - corner_to_area.corner_2.y));
            buffer_add(corners_to_area, Day09_Corners_To_Area, corner_to_area);
        }
    }
    assert(corners_to_area->size == corners_to_area->capacity);
    qsort(corners_to_area->data, corners_to_area->size, sizeof(Day09_Corners_To_Area), day09_sort_by_area);

    U64 result = 0;
    for (U64 i = corners_to_area->size - 1; i > 0; --i)
    {
        Day09_Corners_To_Area corners = buffer_get(corners_to_area, Day09_Corners_To_Area, i);
        Day09_Edge rect = {
            .edge_1 = {.x = corners.corner_1.x, .y = corners.corner_2.y},
            .edge_2 = {.x = corners.corner_2.x, .y = corners.corner_1.y}
        };
        if (day09_is_rectangle_inside(rect, edges))
        {
            result = corners.area;
            break;
        }
    }

    printf("Part 2: %llu\n", result);
}

internal void day09(DAY_ARGS)
{
    day09_part1(DAY_CALL);
    day_input_reset(day_input);
    day09_part2(DAY_CALL);
}
