#ifndef __SMALLOC_H
#define __SMALLOC_H

void *smalloc(unsigned long);
void sfree(void*);

// some low level calls for testing and tuning

// set the heap memory boundaries and default block allocation size
// WARNING: this will reallocate initial structures and book-keeping
// values
void __smalloc_init(unsigned long heapTop, unsigned long size, unsigned long heapBottom);

// returns the total number of bytes used by smalloc internals (including smalloc'd data)
// *blocks = the number of blocks allocated
// *freeChunkSz = the total number of bytes sitting in free queues
unsigned long __smalloc_stats(unsigned short *blocks, unsigned long *freeChunkSz);

#endif