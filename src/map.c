#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_NUM_BUCKETS         13

/*
 * Source: http://www.cse.yorku.ca/~oz/hash.html
 * djb2
 * "We stand on the shoulders of giants."
 */
unsigned long djb2_hash(const char *s) {
    unsigned long h = 5381;
    while(*s) h = ((h << 5) + h) + *s++;
    return h;
}

MapKeyValuePtr MakeKeyValue(const char* key, void* value) {
    MapKeyValuePtr kv = (MapKeyValuePtr)malloc(sizeof(MapKeyValue));
    if(!kv) return NULL;
    if(key) {
        size_t len = strlen(key);
        kv->key = (char *)malloc(len+1);
        memcpy(kv->key, key, len+1);
    } else {
        kv->key = NULL;
    }
    kv->value = value;
    kv->next = NULL;
    return kv;
}

void FreeKeyValue(MapKeyValuePtr kv) {
    if(!kv) return;
    if(kv->key) free(kv->key);
    free(kv);
}

MapPtr MakeMap(void) {
    MapPtr map = (MapPtr)malloc(sizeof(Map));
    if(!map) return NULL;
    map->numBuckets = MAP_NUM_BUCKETS;
    map->buckets = (MapKeyValuePtr *)malloc(sizeof(MapKeyValuePtr) * map->numBuckets);
    for(unsigned i=0; i<map->numBuckets; i++) map->buckets[i] = NULL;
    return map;
}

void FreeMap(MapPtr map) {
    if(!map) return;
    for(unsigned i=0; i<map->numBuckets; i++) {
        MapKeyValuePtr kv = map->buckets[i];
        while(kv) {
            MapKeyValuePtr next = kv->next;
            FreeKeyValue(kv);
            kv = next;
        }
    }
    free(map->buckets);
    free(map);
}

int MapContains(MapPtr map, const char* key) {
    if(!map) return 0;
    unsigned bucket = djb2_hash(key) % map->numBuckets;
    MapKeyValuePtr kv = map->buckets[bucket];
    while(kv) {
        if(kv->key && strcmp(key, kv->key) == 0) return 1;
        kv = kv->next;
    }
    return 0;
}

void *MapGet(MapPtr map, const char* key, void* defaultValue) {
    if(!map) return defaultValue;
    unsigned bucket = djb2_hash(key) % map->numBuckets;
    MapKeyValuePtr kv = map->buckets[bucket];
    while(kv) {
        if(kv->key && strcmp(key, kv->key) == 0) return kv->value;
        kv = kv->next;
    }
    return defaultValue;
}


void *MapSet(MapPtr map, const char* key, void* value) {
    if(!map) return NULL;
    unsigned bucket = djb2_hash(key) % map->numBuckets;
    MapKeyValuePtr kv = map->buckets[bucket];
    if(!kv) {
        map->buckets[bucket] = MakeKeyValue(key, value);
        return NULL;
    }
    while(kv) {
        if(kv->key && strcmp(key, kv->key) == 0) {
            void *old = kv->value;
            kv->value = value;
            return old;
        }
        if(kv->next == NULL) {
            kv->next = MakeKeyValue(key, value);
            return NULL;
        }
    }
    return NULL;
}

void *MapUnset(MapPtr map, const char* key) {
    if(!map) return NULL;
    unsigned bucket = djb2_hash(key) % map->numBuckets;
    MapKeyValuePtr kv = map->buckets[bucket];
    MapKeyValuePtr prev = NULL;
    while(kv) {
        if(kv->key && strcmp(key, kv->key) == 0) {
            if(!prev && kv->next) {
                map->buckets[bucket] = kv->next;
            } else if(!prev) {
                map->buckets[bucket] = MakeKeyValue(NULL, NULL);                
            }
            void *value = kv->value;
            FreeKeyValue(kv);
            return value;
        }
        prev = kv;
        kv = kv->next;
    }
    return NULL;
}

void print_map(MapPtr map) {
    if(!map) return;
    printf("Map:\n");
    for(unsigned i=0; i<map->numBuckets; i++) {
        printf("  Bucket #%u:\n", i);
        MapKeyValuePtr kv = map->buckets[i];
        while(kv) {
            if(kv->key) printf("    %s = %p\n", kv->key, kv->value);
            kv = kv->next;
        }
    }
}
