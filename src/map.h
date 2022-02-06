#ifndef __MAP_H
#define __MAP_H

typedef struct MAPKEYVALUE {
    struct MAPKEYVALUE* next;
    char *key;
    void *value;
} MapKeyValue, *MapKeyValuePtr;

typedef struct MAP {
    unsigned numBuckets;
    MapKeyValuePtr *buckets;
} Map, *MapPtr;

MapPtr MakeMap(void);
void FreeMap(MapPtr);
int MapContains(MapPtr, const char*);
void *MapGet(MapPtr, const char*, void*);
void *MapSet(MapPtr, const char*, void*);
void *MapUnset(MapPtr, const char*);
void print_map(MapPtr map);

#endif