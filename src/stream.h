#ifndef __STREAM_H
#define __STREAM_H

#include <stdlib.h>

typedef int STREAMDIR;

#define STREAM_IN 1
#define STREAM_OUT 2
#define STREAM_INOUT (STREAM_IN | STREAM_OUT)

typedef struct STREAM {
    unsigned short flags;
    void* source;
    size_t pos;
} Stream, *StreamPtr;

StreamPtr infile_stream(const char* path);
StreamPtr instr_stream(const char* string);
StreamPtr outstr_stream();
StreamPtr outfile_stream(const char* path);

int getc_stream(StreamPtr);
int endof_stream(StreamPtr);
char *cstrfrom_stream(StreamPtr);
void free_stream(StreamPtr);


#endif
