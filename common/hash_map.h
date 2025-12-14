#ifndef GENERIC_MAP_H
#define GENERIC_MAP_H

typedef struct {
    void *key;
    void *value;
    U8 state; // 0 unused, 1 used, 2 for a WIP tombstone
} HashMapEntry;

typedef struct {
    HashMapEntry *data;
    U64 key_size;
    U64 value_size;
    U64 size;
    U64 capacity;
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
    return result;
}

// Taken from https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
internal U64 hash_fnv1a(const U8 *data, U64 len)
{
    U64 h = 14695981039346656037ull;
    for (U64 i = 0; i < len; ++i)
    {
        h ^= data[i];
        h *= 1099511628211ull;
    }
    return h;
}

internal void hash_map_set(HashMap *map, void *key, void *value)
{
    U64 hash = hash_fnv1a(key, map->key_size);
    U64 starting_position = (hash % map->capacity);
    U64 next_position = starting_position;
    while (map->data[next_position].state == 1)
    {
        if (common_memcmp(map->data[next_position].key, key, map->key_size) == 0)
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
    U64 hash = hash_fnv1a(key, map->key_size);
    U64 starting_position = (hash % map->capacity);
    U64 next_position = starting_position;
    while (map->data[next_position].state != 0)
    {
        if ((map->data[next_position].state == 1) &&
            (common_memcmp(map->data[next_position].key, key, map->key_size) == 0))
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

typedef HashMap HashMapString8;
typedef HashMapEntry HashMapEntryString8;

internal HashMapString8* hash_map_string8_init(MemoryArena *arena, U64 value_size, U64 capacity)
{
    HashMapString8 *result = hash_map_init(arena, sizeof(String8), value_size, capacity);
    return result;
}

internal HashMapEntryString8* hash_map_string8_get(HashMapString8 *map, String8 *key)
{
    U64 hash = string8_hash_fnv1a(*key);
    U64 starting_position = (hash % map->capacity);
    U64 next_position = starting_position;
    while (map->data[next_position].state != 0)
    {
        if ((map->data[next_position].state == 1) && string8_equals(*(String8 *)map->data[next_position].key, *key)) {
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

internal void hash_map_string8_set(HashMapString8 *map, String8 *key, void *value)
{
    U64 hash = string8_hash_fnv1a(*key);
    U64 starting_position = (hash % map->capacity);
    U64 next_position = starting_position;
    while (map->data[next_position].state == 1)
    {
        if (string8_equals(*(String8 *)map->data[next_position].key, *key))
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

#endif
