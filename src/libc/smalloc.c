#include "smalloc.h"

/*
  "smalloc" - simple memory allocator
  Multiple sources were used for educational purposes:
   glibc malloc:
   - https://github.com/lattera/glibc/blob/master/malloc/malloc.c
   dlmalloc:
   - http://gee.cs.oswego.edu/pub/misc/malloc.c
  
  Code is original but concepts were informed from the previous links.
  Thank you, giants, for allowing me to stand on your shoulders!

- Heap starts at specific address, HEAP_TOP, and grows downward.
- Heap grows in BLOCK units. Each block provides memory and
  tracks freed memory out of itself.
- Blocks grow in PAGESIZE units by default, but a block can
  be allocated to an arbitrarily large size.
- Blocks are a doubly-linked list.
- Freed (released) memory is enqueued as a CHUNK structure that can be
  searched on subsequent allocations for reuse.
- We try to find a freed chunk of memory first, then a
  block with enough remaining space, and then finally,
  allocate a new block that can accomodate the requested size.
- If we cannot allocate memory within the boundaries (HEAP_BOTTOM, HEAP_TOP)
  we return NULL -- no more memory.
- There is no sbrk.


TODO:
 - Recupurate freed large blocks -- maybe not so important?
 - Coalesce freed memory to form larger available chunks for reallocations
 - Reasonably split freed memory chunks to form new utilized chunks
 - Debug or general memory testing to detect bad memory frees
 - Simplify / reduce the overhead
*/

#ifndef NULL
#define NULL 0
#endif

#undef SIZE_T
#define SIZE_T unsigned long

// heap starts at HEAP_TOP and grows downward by PAGESIZE, NEVER past HEAP_BOTTOM
static void* HEAP_TOP = (void*)0x07ffff;
static void* HEAP_BOTTOM = (void*)0x050000;
static SIZE_T PAGESIZE = 8192;

#define ALLOCD 1

// struct CHUNK does two things: it's a general header for freed and non-freed memory.
// for freed memory it maintains a doubly-linked list of freed chunks within its block.
struct CHUNK {
    void *block;
    SIZE_T size; // total size, NOT program requested size
    unsigned flags; // TODO: optimization opportunity here
    struct {
        struct CHUNK* next;
        struct CHUNK* prev;
    } freed; // begining of CHUNK.freed is actually where non-freed program useable memory is
};
// TODO: min allocation size is 19 bytes -- some improvement is possible here.

// struct BLOCK maintains a doubly-linked list of BLOCKs
// It also maintains a pointer, top, to the next memory space to allocate (upward).
// It maintains a pointer to the head of a doubly-linked list of freed chunks.
// It maintains some size information to make new allocations and reporting easy.
struct BLOCK {
    struct BLOCK* prev;             // previous block, i.e., towards __first_block
    struct BLOCK* next;             // next block, i.e., towards __last_block
    SIZE_T size;                    // size of the entire block, header and actual space
    SIZE_T remaining;               // remaining bytes that can be allocated by smalloc in block
    void*  top;                     // where to start allocating new smalloc requests
    struct CHUNK* free;             // recycling is important
};
// 24-byte header on 8K default allocation size is 0.3% overhead

#define BLOCK_HEADER_SZ            (sizeof(struct BLOCK))

struct BLOCK* __first_block = NULL;
struct BLOCK* __last_block = NULL;

struct CHUNK* __use_freed_chunk(SIZE_T minSize, SIZE_T maxSize);
struct BLOCK* __block_with_free_space(SIZE_T size);
struct BLOCK* __new_block(SIZE_T requestedSize);

// tuning - must be called at the very beginning, IF at all
void __smalloc_init(SIZE_T mem, SIZE_T size, SIZE_T bound) {
    HEAP_TOP = (void*)mem;
    PAGESIZE = size;
    HEAP_BOTTOM = (void*)bound;
    __first_block = NULL;
    __last_block = NULL;
}

// Returns the total space taken by smalloc structures - utilized or not
// blocks will hold the number of blocks allocated
// freeChunkSz will hold the number of bytes in freed memory
SIZE_T __smalloc_stats(unsigned short *blocks, unsigned long *freeChunkSz) {
    SIZE_T size = 0;
    struct BLOCK* block = __first_block;
    *blocks = 0;
    *freeChunkSz = 0;
    while(block) {
        struct CHUNK* chunk = block->free;
        while(chunk) {
            *freeChunkSz += chunk->size;
            chunk = chunk->freed.next;
        }
        *blocks++;
        size += block->size + BLOCK_HEADER_SZ;
        block = block->next;
    }
    return size;
}

void* smalloc(SIZE_T n) {
    // Make sure we are allocating at least the minimum size to support the ability to
    // free and reallocate it later: we need the CHUNK header in addition to requested size
    SIZE_T allocSize = n + sizeof(struct CHUNK);
    // TODO: alignment! (we are generating unaligned pointers all over the place)

    // reallocate previously freed memory, if possible
    // we allow chunks of allocSize, up to allocSize*2 -- this could be very wasteful :-)
    SIZE_T doubleSize = allocSize << 1;
    struct CHUNK* chunk = __use_freed_chunk(allocSize, doubleSize > allocSize ? doubleSize : allocSize);
    if(chunk) return chunk;

    // Find a block with enough unused space for the requested size
    struct BLOCK* block = __block_with_free_space(allocSize);
    if(!block) {
        // No existing block exists, so create a new block with enough size.
        block = __new_block(allocSize);
    }

    if(!block) return NULL; // not enough memory

    // now allocate out of the block
    chunk = block->top;
    block->top += allocSize; // in the block, we grow upwards
    block->remaining -= allocSize;
    chunk->block = block;
    chunk->size = allocSize;
    chunk->flags = (ALLOCD);
    return &(chunk->freed); // look this is where the free memory is to use... I promise.
}

// Free a given pointer to smalloc'd space
// 1. Re-establish the struct CHUNK data
// 2. Verify that we are allocated (TODO: add magic word to provide additional test?)
// 3. Free and enqueue the chunk as free within the block
// 4. Mark it as free (~ALLOCD)
void sfree(void *ptr) {
    struct CHUNK* chunk = ptr - sizeof(struct CHUNK); // reestablish the metadata
    if(!(chunk->flags & ALLOCD)) return; // ruh-roh
    struct BLOCK* block = chunk->block;
    chunk->freed.prev = NULL;
    chunk->freed.next = block->free;
    if(!block->free) {
        block->free = chunk;
    } else {
        block->free->freed.prev = chunk;
        block->free = chunk; 
    }
    chunk->flags &= ~ALLOCD; // mark it as freed
}

// Find a freed chunk that meets the given size requirements
struct CHUNK* __use_freed_chunk(SIZE_T minSize, SIZE_T maxSize) {
    struct BLOCK* block = __first_block;
    while(block) {
        struct CHUNK* chunk = block->free;
        while(chunk) {
            if(chunk->size >= minSize && chunk->size <= maxSize) {
                // found one - dequeue it and return it
                if(chunk == block->free) block->free = chunk->freed.next;
                if(chunk->freed.next) chunk->freed.next->freed.prev = chunk->freed.prev;
                if(chunk->freed.prev) chunk->freed.prev->freed.next = chunk->freed.next;
                return chunk;
            }
            chunk = chunk->freed.next;
        }
        block = block->prev;
    }
    return NULL;
}

// Find a block with enough free space for the requested size
struct BLOCK* __block_with_free_space(SIZE_T size) {
    struct BLOCK* block = __first_block;
    while(block) {
        if(block->remaining >= size) return block;
        block = block->next;
    }
    return NULL;
}

// Create a new block that minimally satisfies the requested size + overhead
// Apply minimum PAGESIZE to ensure we aren't underallocating a block.
struct BLOCK* __new_block(SIZE_T requestedSize) {
    // pick an appropriate block size - this is the actual allocation size of the BLOCK
    // which needs to take into account the minimum block size (PAGESIZE), the requested
    // allocation size, and the size of the block header
    SIZE_T size = requestedSize + BLOCK_HEADER_SZ;
    if(size < PAGESIZE) size = PAGESIZE;

    void *start = __last_block == NULL ? HEAP_TOP : __last_block;
    if(start - PAGESIZE < HEAP_BOTTOM) return NULL; // hit the memory limit

    // this is where the magic happens
    struct BLOCK* block = start - PAGESIZE; // grow downward
    // oh so magical

    // book-keeping to make insertions and CHUNK allocations easy
    block->next = NULL;
    block->size = size;
    block->remaining = block->size - BLOCK_HEADER_SZ;
    block->top = block + BLOCK_HEADER_SZ; // upwards inside the block
    if(__first_block == NULL) __first_block = block;
    block->prev = __last_block;
    if(__last_block) __last_block->next = block;
    __last_block = block;
    return block;
}
