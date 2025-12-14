#ifndef GENERIC_MAP_H
#define GENERIC_MAP_H

typedef struct {
    void *key;
    void *value;
    U8 state; // 0 unused, 1 used, 2 for a WIP tombstone
} HashMapEntry;

typedef U64 (*HashMapHashFn)(const void *key, U64 key_size);
typedef B32 (*HashMapEqFn)(const void *a, const void *b, U64 key_size);

typedef struct {
    HashMapHashFn hash_fn;
    HashMapEqFn eq_fn;
} HashMapOptions;

typedef struct {
    HashMapEntry *data;
    U64 key_size;
    U64 value_size;
    U64 size;
    U64 capacity;
    HashMapOptions options;
} HashMap;

internal HashMap* hash_map_init(MemoryArena *arena, U64 key_size, U64 value_size, U64 capacity)
{
    HashMap *result = memory_arena_push_struct(arena, HashMap);
    result->key_size = key_size;
    result->value_size = value_size;
    result->size = 0;
    result->capacity = capacity;
    result->data = memory_arena_push_array(arena, HashMapEntry, capacity);
    for (U64 i = 0; i < capacity; ++i)
    {
        result->data[i] = (HashMapEntry){0};
    }
    result->options = (HashMapOptions){0};

    return result;
}

internal HashMap* hash_map_init_with_options(MemoryArena *arena, U64 key_size, U64 value_size, U64 capacity, HashMapOptions options)
{
    HashMap *result = hash_map_init(arena, key_size, value_size, capacity);
    result->options = options;
    return result;
}

// Taken from https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
internal U64 hash_map_hash_key_default(const U8 *data, U64 len)
{
    U64 h = 14695981039346656037ull;
    for (U64 i = 0; i < len; ++i)
    {
        h ^= data[i];
        h *= 1099511628211ull;
    }
    return h;
}

internal inline U64 hash_map_hash_key(HashMap *map, void *key)
{
    if (map->options.hash_fn)
    {
        return map->options.hash_fn(key, map->key_size);
    }
    return hash_map_hash_key_default(key, map->key_size);
}

internal inline B32 hash_map_eq_key(HashMap *map, void *a, void *b, U64 key_size)
{
    if (map->options.eq_fn)
    {
        return map->options.eq_fn(a, b, key_size);
    }
    return (common_memcmp(a, b, key_size) == 0);
}

internal void hash_map_set(HashMap *map, void *key, void *value)
{
    U64 hash = hash_map_hash_key(map, key);
    U64 starting_position = (hash % map->capacity);
    U64 next_position = starting_position;
    while (map->data[next_position].state == 1)
    {
        if (hash_map_eq_key(map, map->data[next_position].key, key, map->key_size))
        {
            map->data[next_position].value = value;
            return;
        }

        next_position = (next_position + 1) % map->capacity;
        assert(next_position != starting_position);
    }

    map->data[next_position].key = key;
    map->data[next_position].value = value;
    map->data[next_position].state = 1;
    ++map->size;
}

internal HashMapEntry* hash_map_get(HashMap *map, void *key)
{
    U64 hash = hash_map_hash_key(map, key);
    U64 starting_position = (hash % map->capacity);
    U64 next_position = starting_position;
    while (map->data[next_position].state != 0)
    {
        if ((map->data[next_position].state == 1) &&
            hash_map_eq_key(map, map->data[next_position].key, key, map->key_size))
        {
            return &map->data[next_position];
        }
        next_position = (next_position + 1) % map->capacity;
        if (next_position == starting_position)
        {
            return 0;
        }
    }

    return 0;
}

internal U64 hash_map_string8_hash(const void *key, U64 key_size)
{
    return string8_hash_fnv1a(*(String8 *)key);
}

internal B32 hash_map_string8_eq(const void *a, const void *b, U64 key_size)
{
    return string8_equals(*(String8 *)a, *(String8 *)b);
}

internal HashMap* hash_map_string8_init(MemoryArena *arena, U64 value_size, U64 capacity)
{
    HashMapOptions options = {0};
    options.hash_fn = hash_map_string8_hash;
    options.eq_fn = hash_map_string8_eq;
    HashMap *result = hash_map_init_with_options(arena, sizeof(String8), value_size, capacity, options);
    return result;
}

#endif
