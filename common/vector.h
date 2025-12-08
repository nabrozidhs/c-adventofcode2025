#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    I32 x;
    I32 y;
} Vector2i;

static inline F32 vector2i_euclidian_distance_squared(Vector2i a, Vector2i b)
{
    return common_powi_f32((F32)a.x - (F32)b.x, 2) + common_powi_f32((F32)a.y - (F32)b.y, 2);
}

typedef struct {
    I32 x;
    I32 y;
    I32 z;
} Vector3i;

static inline F32 vector3i_euclidian_distance_squared(Vector3i a, Vector3i b)
{
    return common_powi_f32((F32)a.x - (F32)b.x, 2) +
        common_powi_f32((F32)a.y - (F32)b.y, 2) +
        common_powi_f32((F32)a.z - (F32)b.z, 2);
}

#endif
